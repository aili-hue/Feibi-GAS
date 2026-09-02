// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagAnimNotifyState.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class GAS_API UGameplayTagAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere,Category="GameplayTag")
	FGameplayTag GameplayTag;
	
	UPROPERTY(EditAnywhere,Category="GameplayEffect")
	TSubclassOf<UGameplayEffect > GameplayEffectClass;
	
	
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)override;
};
