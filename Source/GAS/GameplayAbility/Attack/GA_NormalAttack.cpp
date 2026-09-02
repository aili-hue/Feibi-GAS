// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_NormalAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/Character/MyCharacter.h"
#include "GAS/DataAsset/Montage/MontageDataAsset.h"


UGA_NormalAttack::UGA_NormalAttack()
{
	InstancingPolicy=EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_NormalAttack::PlayMontage(UAnimMontage* Montage)
{
	if (!Montage) return;

	// 停止当前动画
	if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UAnimInstance* AnimInstance = MyCharacter->GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(0.1f);
		}
	}

	// 清理旧任务
	if (PlayMontageTask)
	{
		PlayMontageTask->OnCompleted.Clear();
		PlayMontageTask->OnInterrupted.Clear();
		PlayMontageTask->EndTask();
		PlayMontageTask = nullptr;
	}

	// 创建新任务
	PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, Montage, 1.f, NAME_None);

	if (PlayMontageTask)
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_NormalAttack::Reset);
		// 所有中断都走同一个回调，在回调里判断
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UGA_NormalAttack::OnMontageInterrupted);
		PlayMontageTask->ReadyForActivation();
	}
}

void UGA_NormalAttack::Reset()
{
	CurrentTag= GameplayMontageTag[0];
	CurrentInt= 0;
	
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
}

void UGA_NormalAttack::OnComboInputReceived(FGameplayEventData Payload)
{
	if (GameplayMontageTag.Num() == 0) return;
	
	  
	CurrentInt = (CurrentInt + 1) % GameplayMontageTag.Num();
	CurrentTag = GameplayMontageTag[CurrentInt]; 
	
	if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UMontageDataAsset* DataAsset=MyCharacter->SprintMontage)
		{
			bIsComboTransition = true;
			PlayMontage(CurrentMontage(DataAsset,CurrentTag));
		}
	}
}

UAnimMontage* UGA_NormalAttack::CurrentMontage(UMontageDataAsset* DataAsset,FGameplayTag GameTag)
{
	if (DataAsset)
	{
		return DataAsset->GetMontageByTag(GameTag);
	}
	return nullptr;
}

void UGA_NormalAttack::MontageEvent(FGameplayTag MontageEventTag, int32 NewNub)
{
	
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.Clear();
		WaitEventTask->EndTask();
		WaitEventTask=nullptr;
	}
	if (NewNub>0)
	{
		WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,FGameplayTag::RequestGameplayTag(FName("Event.Attack.ComboInput")));
	}
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.AddDynamic(this,&ThisClass::OnComboInputReceived);
		WaitEventTask->ReadyForActivation();
	}
}

void UGA_NormalAttack::OnMontageInterrupted()
{
	// 如果是连招切换导致的中断，忽略（新动画已经接管）
	if (bIsComboTransition)
	{
		bIsComboTransition = false;
		return;
	}

	// 否则是真正的打断（跳跃等），结束技能
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_NormalAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (UAbilitySystemComponent* ASC=GetAbilitySystemComponentFromActorInfo())
	{
		MontageHandle= ASC->RegisterGameplayTagEvent(MontageTag).AddUObject(this,&ThisClass::MontageEvent);
		ASC->AddLooseGameplayTag(AttackingTag);
		
		if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetAvatarActorFromActorInfo()))
		{
			if (UMontageDataAsset* DataAsset=MyCharacter->SprintMontage)
			{
					if (ASC->HasMatchingGameplayTag(TapTag))
					{
						CurrentTag= GameplayMontageTag[CurrentInt];
						UE_LOG(LogTemp,Error,TEXT("%icesh"),CurrentInt);
						PlayMontage(CurrentMontage(DataAsset,CurrentTag));
					}
					else
					{
						CurrentTag= HeavyBlowMontageTag;
						PlayMontage(CurrentMontage(DataAsset,CurrentTag));
					}
			}
		}
	}
}

void UGA_NormalAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.Clear();
		WaitEventTask->EndTask();
		WaitEventTask=nullptr;
	}
	
	if (UAbilitySystemComponent* ASCc=GetAbilitySystemComponentFromActorInfo())
	{
		ASCc->RemoveLooseGameplayTag(TapTag);
		ASCc->RemoveLooseGameplayTag(HeavyBlowTag);
		ASCc->RemoveLooseGameplayTag(AttackingTag);
		ASCc->RegisterGameplayTagEvent(MontageTag).Remove(MontageHandle);
	}
	bIsComboTransition = false;
}
