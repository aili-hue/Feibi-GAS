
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_JumpAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class GAS_API UGA_JumpAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	UGA_JumpAttack();
	
	UPROPERTY() 
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveTask;
	
	void JumpLight_Attack(UAnimMontage* Montage);
	FGameplayTag JumpTag=FGameplayTag::RequestGameplayTag(FName("Data.Jump"));
	FGameplayTag JumpingTag = FGameplayTag::RequestGameplayTag(FName("Data.Jump.Jumping"));
	FGameplayTag LandedTag=FGameplayTag::RequestGameplayTag(FName("Data.Landed"));
	FGameplayTag MontageTag=FGameplayTag::RequestGameplayTag(FName("Data.Montage"));
	FGameplayTag MovingTag=FGameplayTag::RequestGameplayTag(FName("Data.Moving"));
	FGameplayTag TapTag= FGameplayTag::RequestGameplayTag(FName("Data.Attack.Tap"));
	FGameplayTag HeavyBlowTag= FGameplayTag::RequestGameplayTag(FName("Data.Attack.HeavyBlow"));
	FGameplayTag AttackTapFallingTag=FGameplayTag::RequestGameplayTag(FName("Data.Attack.Tap.Falling"));
	FGameplayTag AttackHeavyBlowFallingTag= FGameplayTag::RequestGameplayTag(FName("Data.Attack.HeavyBlow.Falling"));

	FDelegateHandle LandedHandle;
	FDelegateHandle MontageHandle;
	
	UFUNCTION()
	void JumpLight_AttackEnds(FGameplayTag StartTag, int32 NewNumber);
	
	UFUNCTION()
	void JumpingLight_Attack();
	
	UFUNCTION()
	void HeavyHitAnimation(UAnimMontage* Montage);
	
	UFUNCTION()
	void EndGameplayAbility();
	
	UFUNCTION()
	void RemoveTag (FGameplayTag RemoveTag,int32 TagIndex);
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;

};
