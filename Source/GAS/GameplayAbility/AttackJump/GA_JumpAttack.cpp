// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_JumpAttack.h"
#include "AbilitySystemComponent.h"
#include "GAS/Character/MyCharacter.h"
#include "GAS/DataAsset/Montage/MontageDataAsset.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_JumpAttack::UGA_JumpAttack()
{
	InstancingPolicy=EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_JumpAttack::JumpLight_Attack(UAnimMontage* Montage)
{
	
	ActiveTask= UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,Montage,1.f,FName("Default"));
	ActiveTask->OnCompleted.AddDynamic(this, &ThisClass::JumpingLight_Attack);
	ActiveTask->OnInterrupted.AddDynamic(this, &ThisClass::JumpingLight_Attack);
	ActiveTask->ReadyForActivation();
}


void UGA_JumpAttack::JumpLight_AttackEnds(FGameplayTag StartTag, int32 NewNumber)
{
	if (NewNumber <= 0) return;
	
	if (UAbilitySystemComponent* ASComp = GetAbilitySystemComponentFromActorInfo())
	{
		if (LandedTag.IsValid())
		{
			ASComp->RegisterGameplayTagEvent(LandedTag).Remove(LandedHandle);
			LandedHandle.Reset();
		}
	}
	
	if (ActiveTask)
	{
		ActiveTask->OnCompleted.Clear();
		ActiveTask->OnInterrupted.Clear();
		ActiveTask->EndTask();
		ActiveTask=nullptr;
	}
	if (AMyCharacter* Character = Cast<AMyCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UMontageDataAsset* MontageDataAsset= Character->SprintMontage)
		{
			if (UAnimMontage* AnimMontage=MontageDataAsset->GetMontageByTag(AttackTapFallingTag))
			{
				ActiveTask= UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,AnimMontage,1.f,FName("AirAttackEnd"));
				ActiveTask->OnCompleted.AddDynamic(this, &ThisClass::EndGameplayAbility);
				ActiveTask->OnInterrupted.AddDynamic(this, &ThisClass::EndGameplayAbility);
				ActiveTask->ReadyForActivation();
			}
		}
	}
}

void UGA_JumpAttack::JumpingLight_Attack()
{
	
	if (AMyCharacter* Character = Cast<AMyCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_JumpToSection(FName("Jumping"));
		}
	}
}

void UGA_JumpAttack::HeavyHitAnimation(UAnimMontage* Montage)
{
	
	if (ActiveTask)
	{
		ActiveTask->EndTask();
		ActiveTask=nullptr;
	}
	
	if (UAbilitySystemComponent* ASComp = GetAbilitySystemComponentFromActorInfo())
	{
		ASComp->RemoveLooseGameplayTag(JumpTag);
	}
	
	ActiveTask= UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,Montage,1.f,NAME_None);
	ActiveTask->OnCompleted.AddDynamic(this, &ThisClass::EndGameplayAbility);
	ActiveTask->OnInterrupted.AddDynamic(this, &ThisClass::EndGameplayAbility);
	ActiveTask->ReadyForActivation();
	
}

void UGA_JumpAttack::EndGameplayAbility()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
}

void UGA_JumpAttack::RemoveTag(FGameplayTag RemoveTag, int32 TagIndex)
{
	if (UAbilitySystemComponent* ASComp = GetAbilitySystemComponentFromActorInfo())
	{
		ASComp->RemoveLooseGameplayTag(AttackTapFallingTag);
		
		if (ASComp->HasMatchingGameplayTag(JumpTag)||ASComp->HasMatchingGameplayTag(MovingTag))
		{
			if (AMyCharacter* Character = Cast<AMyCharacter>(GetAvatarActorFromActorInfo()))
			{
				if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
				{
					AnimInstance->Montage_Stop(0.2f);
				}
			}
		}
	}
}

void UGA_JumpAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
	if (UAbilitySystemComponent* ASComp = GetAbilitySystemComponentFromActorInfo())
	{
		if (AMyCharacter* Character = Cast<AMyCharacter>(GetAvatarActorFromActorInfo()))
		{
			if (UMontageDataAsset* MontageDataAsset= Character->SprintMontage)
			{
				MontageHandle= ASComp->RegisterGameplayTagEvent(MontageTag).AddUObject(this, &ThisClass::RemoveTag);
				
				if (!ASComp->HasMatchingGameplayTag(JumpTag)||!ASComp->HasMatchingGameplayTag(JumpingTag))
				{
					EndGameplayAbility();
					return;
				}
					if (ASComp->HasMatchingGameplayTag(TapTag))
					{
						if (UAnimMontage* AnimMontage= MontageDataAsset->GetMontageByTag(AttackTapFallingTag))
						{
							JumpLight_Attack(AnimMontage);
							ASComp->AddLooseGameplayTag(AttackTapFallingTag);
							LandedHandle= ASComp->RegisterGameplayTagEvent(LandedTag).AddUObject(this, &ThisClass::JumpLight_AttackEnds);
						}
					}
					else
					{
						if (UAnimMontage* AnimMontage = MontageDataAsset->GetMontageByTag(AttackHeavyBlowFallingTag))
						{
							HeavyHitAnimation(AnimMontage);
							ASComp->AddLooseGameplayTag(AttackHeavyBlowFallingTag);
						}
					}
			}
		}
	}
}

void UGA_JumpAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	if (UAbilitySystemComponent* ASComp = GetAbilitySystemComponentFromActorInfo())
	{
		if (LandedHandle.IsValid())
		{
			ASComp->RegisterGameplayTagEvent(LandedTag).Remove(LandedHandle);
		}
		if (MontageHandle.IsValid())
		{
			ASComp->RegisterGameplayTagEvent(MontageTag).Remove(MontageHandle);
		}
		ASComp->RemoveLooseGameplayTag(JumpTag);
		ASComp->RemoveLooseGameplayTag(TapTag);
		ASComp->RemoveLooseGameplayTag(HeavyBlowTag);
		ASComp->RemoveLooseGameplayTag(AttackTapFallingTag);
		ASComp->RemoveLooseGameplayTag(AttackHeavyBlowFallingTag);
	}
	if (ActiveTask)
	{
		ActiveTask->OnCompleted.Clear();
		ActiveTask->OnInterrupted.Clear();
		ActiveTask->EndTask();
		ActiveTask = nullptr;
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}