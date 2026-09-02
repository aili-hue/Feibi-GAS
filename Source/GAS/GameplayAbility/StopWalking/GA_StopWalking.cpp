// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_StopWalking.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/Character/MyCharacter.h"
#include "GAS/DataAsset/Montage/MontageDataAsset.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GAS/AttributeSet/AttributeSet/General/AttributeSet_General.h"

UGA_StopWalking::UGA_StopWalking()
{
	InstancingPolicy=EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_StopWalking::PlayMontage(UAnimMontage* Montage)
{
	
	if (UAbilityTask_PlayMontageAndWait* PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,Montage,1.f,NAME_None))
	{
		PlayMontageAndWait->OnCompleted.AddDynamic(this,&ThisClass::K2_EndAbility);
		PlayMontageAndWait->OnBlendOut.AddDynamic(this,&ThisClass::K2_EndAbility);
		PlayMontageAndWait->OnCancelled.AddDynamic(this,&ThisClass::K2_EndAbility);
		PlayMontageAndWait->OnInterrupted.AddDynamic(this,&ThisClass::K2_EndAbility);
		PlayMontageAndWait->ReadyForActivation();
	}
}

void UGA_StopWalking::OnMovingTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount>0)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UGA_StopWalking::JumpTagChanged(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount>0)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}


void UGA_StopWalking::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	float Speed = ASC->GetNumericAttribute(UAttributeSet_General::GetMovingSpeedAttribute());
	AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetAvatarActorFromActorInfo());

	// 结束 GA_Move → 移除 Data.Moving 标签
	FGameplayTagContainer TargetTags;
	TargetTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Move")));
	ASC->CancelAbilities(&TargetTags);

	// 冲刺状态需要降速（无论地面还是空中）
	if (Speed >= 400.f && Effect)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(Effect, 1, ContextHandle);
		
		if (EffectSpecHandle.IsValid())
		{
			FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Data.MovingSpeed"));
			EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(Tag, 400.f);
			ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		}
		
	}

	// 空中不播停止蒙太奇，直接结束
	if (!MyCharacter || MyCharacter->GetMovementComponent()->IsFalling())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 地面播放停止蒙太奇
	if (Speed >= 400.f)
	{
		if (!Effect) return;
		PlayMontage(MyCharacter->SprintMontage->GetMontageByTag(StopRunningTag));
	}
	else
	{
		PlayMontage(MyCharacter->SprintMontage->GetMontageByTag(StopWalkingTag));
	}
	
	FGameplayTag MovingTag=FGameplayTag::RequestGameplayTag(FName("Data.Moving"));
	MovingTagDelegateHandle=ASC->RegisterGameplayTagEvent(MovingTag).AddUObject(this,&ThisClass::OnMovingTagChanged);
	
	FGameplayTag JumpTag=FGameplayTag::RequestGameplayTag(FName("Data.Jump.Jumping"));
	JumpTagDelegateHandle=ASC->RegisterGameplayTagEvent(JumpTag).AddUObject(this, &ThisClass::JumpTagChanged);
	
}

void UGA_StopWalking::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	if (UAbilitySystemComponent* ASC=GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayTag MovingTag = FGameplayTag::RequestGameplayTag(FName("Data.Moving"));
		ASC->RegisterGameplayTagEvent(MovingTag).Remove(MovingTagDelegateHandle);
		FGameplayTag JumpTag = FGameplayTag::RequestGameplayTag(FName("Data.Jump.Jumping"));
		ASC->RegisterGameplayTagEvent(JumpTag).Remove(JumpTagDelegateHandle);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
