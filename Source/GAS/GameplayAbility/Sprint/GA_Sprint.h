// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Sprint.generated.h"

class UGE_Speed;
/**
 * 
 */
UCLASS()
class GAS_API UGA_Sprint : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	UGA_Sprint();
	
	void PlayMontage(UAnimMontage* Montage);
	
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Sprint"));
	FGameplayTag RetreatTag = FGameplayTag::RequestGameplayTag(FName("Ability.Retreat"));
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameplayEffetc")
	TSubclassOf<UGE_Speed>GE_Speed;
	
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle;
	
	void JumpingEvent(FGameplayTag JumpTag,int32 NewNumber);
	FDelegateHandle JumpingHandle;
	
	void OnMovingTagRemoved(FGameplayTag MovingTag,int32 NewNewCount);
	FDelegateHandle MovingTagDelegateHandle;
	
	void OnMontageTagRemoved(FGameplayTag MontageTag,int32 NewNewCount);
	FDelegateHandle MontageTagDelegateHandle;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;

};
