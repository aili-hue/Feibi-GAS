// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_StopWalking.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UGA_StopWalking : public UGameplayAbility
{
	GENERATED_BODY()
public:
	
	UGA_StopWalking();
	
	void PlayMontage(UAnimMontage* Montage);
	
	FGameplayTag StopWalkingTag=FGameplayTag::RequestGameplayTag(FName("Stop.Walking"));
	FGameplayTag StopRunningTag=FGameplayTag::RequestGameplayTag(FName("Stop.Running"));
	
	void OnMovingTagChanged(FGameplayTag Tag, int32 NewCount);
	FDelegateHandle MovingTagDelegateHandle;
	
	void JumpTagChanged(FGameplayTag Tag, int32 NewCount);
	FDelegateHandle JumpTagDelegateHandle;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameplayEffect")
	TSubclassOf<UGameplayEffect> Effect;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;
};
