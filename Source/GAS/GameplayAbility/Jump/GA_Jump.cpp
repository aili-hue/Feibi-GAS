// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Jump.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "GAS/Character/MyCharacter.h"

UGA_Jump::UGA_Jump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Jump::JumpEvent(AMyCharacter* Character, UAbilitySystemComponent* ASComponent)
{
	Character->Jump();
	
	if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Stop(0.2f);
	}
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (AMyCharacter* Character = Cast<AMyCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent=GetAbilitySystemComponentFromActorInfo())
		{
			
			if (!AbilitySystemComponent->HasMatchingGameplayTag(AttackTapFallingTag) && !AbilitySystemComponent->HasMatchingGameplayTag(AttackHeavyBlowFallingTag))
			{
				JumpEvent(Character,AbilitySystemComponent);
			}
			else
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, true,false);
			}
		}
	}
}

void UGA_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UAbilitySystemComponent* AbilitySystemComponent=GetAbilitySystemComponentFromActorInfo())
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(JumpTag);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
