// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Jump.generated.h"

class AMyCharacter;
class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class GAS_API UGA_Jump : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	UGA_Jump();
	
	void JumpEvent(AMyCharacter* Character, UAbilitySystemComponent* ASComponent);
	
	FGameplayTag JumpTag=FGameplayTag::RequestGameplayTag(FName("Data.Jump"));
	FGameplayTag AttackTapFallingTag=FGameplayTag::RequestGameplayTag(FName("Data.Attack.Tap.Falling"));
	FGameplayTag AttackHeavyBlowFallingTag= FGameplayTag::RequestGameplayTag(FName("Data.Attack.HeavyBlow.Falling"));
	
	void LandedEvent(FGameplayTag LandedEventTag,int32 NewLandedNumber);
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;
	
};
