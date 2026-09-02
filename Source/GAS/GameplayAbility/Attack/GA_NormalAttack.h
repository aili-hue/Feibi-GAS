// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_NormalAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_WaitInputPress;
class UMontageDataAsset;
class AMyCharacter;
/**
 * 
 */
UCLASS()
class GAS_API UGA_NormalAttack : public UGameplayAbility
{
	GENERATED_BODY()
public:
	
	UGA_NormalAttack();
	
	FGameplayTag TapTag= FGameplayTag::RequestGameplayTag(FName("Data.Attack.Tap"));
	FGameplayTag HeavyBlowTag= FGameplayTag::RequestGameplayTag(FName("Data.Attack.HeavyBlow"));
	FGameplayTag MontageTag=FGameplayTag::RequestGameplayTag(FName("Data.Montage"));
	FGameplayTag AttackingTag=FGameplayTag::RequestGameplayTag(FName("State.Attacking"));
	
	FDelegateHandle MontageHandle;
	
	void PlayMontage(UAnimMontage* Montage);
	
	UFUNCTION()
	void Reset();
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitEventTask;

	
	UFUNCTION()
	void OnComboInputReceived(FGameplayEventData Payload);
	
	UPROPERTY(EditAnywhere,Category="MontageGameplayTag")
	TArray<FGameplayTag>GameplayMontageTag;
	
	UPROPERTY(EditAnywhere,Category="MontageGameplayTag")
	FGameplayTag HeavyBlowMontageTag;
	
	UFUNCTION()
	UAnimMontage* CurrentMontage(UMontageDataAsset* DataAsset,FGameplayTag GameTag);
	
	UFUNCTION()
	void MontageEvent(FGameplayTag MontageEventTag,int32 NewNub);
	
	FGameplayTag CurrentTag;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> PlayMontageTask;
	
	UFUNCTION()
	void OnMontageInterrupted();
	
	uint8 bIsComboTransition :1 = false;
	
	int32 CurrentInt= 0;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;
};
