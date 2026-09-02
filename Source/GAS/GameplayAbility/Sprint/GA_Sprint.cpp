// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Sprint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/Character/MyCharacter.h"
#include "GAS/DataAsset/Montage/MontageDataAsset.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/GameplayEffect/Speed/GE_Speed.h"

UGA_Sprint::UGA_Sprint()
{
	InstancingPolicy=EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Sprint::PlayMontage(UAnimMontage* Montage)
{
	if (UAbilityTask_PlayMontageAndWait* AbilityTask_PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,Montage,1.f,NAME_None))
	{
		AbilityTask_PlayMontageAndWait->OnCompleted.AddDynamic(this,&ThisClass::K2_EndAbility);
		AbilityTask_PlayMontageAndWait->OnCancelled.AddDynamic(this,&ThisClass::K2_EndAbility);
		AbilityTask_PlayMontageAndWait->OnInterrupted.AddDynamic(this,&ThisClass::K2_EndAbility);
		AbilityTask_PlayMontageAndWait->ReadyForActivation();
	}
}

void UGA_Sprint::JumpingEvent(FGameplayTag JumpTag, int32 NewNumber)
{
	if (NewNumber>0)
	{
			if (AMyCharacter* Character = Cast<AMyCharacter>(GetAvatarActorFromActorInfo()))
			{
				if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
				{
					// 获取当前速度和最大步行速度
					FVector CurrentVelocity = MoveComp->Velocity;
					float MaxSpeed = MoveComp->MaxWalkSpeed;
 
					// 只有当当前速度显著大于最大速度时（即冲刺惯性还在），才进行裁剪
					if (CurrentVelocity.SizeSquared() > FMath::Square(MaxSpeed))
					{
						// 使用 GetSafeNormal 并提供一个默认向量（如 Forward），防止零向量崩溃
						// 同时只保留方向，将大小限制在 MaxWalkSpeed
						MoveComp->Velocity = CurrentVelocity.GetSafeNormal() * MaxSpeed;
					}
                
					// 关键：如果正在播放根运动蒙太奇，强制物理组件更新状态
					// 否则 CMC 可能还在等待根运动的下一帧数据，导致“卡住”感
					MoveComp->ClearAccumulatedForces();
				}
			}
		EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
	}
}

void UGA_Sprint::OnMovingTagRemoved(FGameplayTag MovingTag, int32 NewNewCount)
{
	if (NewNewCount == 0)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_Sprint::OnMontageTagRemoved(FGameplayTag MontageTag, int32 NewNewCount)
{
	if (NewNewCount >0)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Data.Jump.Jumping"))) || ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Data.Moving"))))
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			}
		}
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (UAbilitySystemComponent* ASC =GetAbilitySystemComponentFromActorInfo())
	{
		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Data.Jump.Jumping"))))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo,true,true);
			return;
		}
		
		JumpingHandle = ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Jump.Jumping"))).AddUObject(this,&ThisClass::JumpingEvent);
		
		MovingTagDelegateHandle = ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Moving"))).AddUObject(this, &ThisClass::OnMovingTagRemoved);
		
		MontageTagDelegateHandle = ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Montage"))).AddUObject(this, &ThisClass::OnMontageTagRemoved);
		
		if (GE_Speed && ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Data.Moving"))))
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(GE_Speed,1.f,EffectContext);
			if (EffectSpec.IsValid())
			{
				ActiveGameplayEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
			}
		}
		
		if (AMyCharacter* Character=Cast<AMyCharacter>(GetAvatarActorFromActorInfo()))
		{
			if (Character->SprintMontage)
			{
				if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Data.Moving"))))
				{
					if (UAnimMontage* Montage= Character->SprintMontage->GetMontageByTag(Tag))
					{
						PlayMontage(Montage);
					}
				}
				else
				{
					if (UAnimMontage* Montage= Character->SprintMontage->GetMontageByTag(RetreatTag))
					{
						PlayMontage(Montage);
					}
				}
			}
		}
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo,false,true);
	}
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UAbilitySystemComponent* ASC =GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Jump.Jumping"))).Remove(JumpingHandle);
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Moving"))).Remove(MovingTagDelegateHandle);
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Montage"))).Remove(MontageTagDelegateHandle);
		if (ActiveGameplayEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(ActiveGameplayEffectHandle, -1);
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
