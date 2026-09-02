// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MontageDataAsset.generated.h"

UCLASS()
class GAS_API UMontageDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Montage")
	TMap<FGameplayTag,TObjectPtr<UAnimMontage>> TagMap;
	
	UAnimMontage* GetMontageByTag(const FGameplayTag& InTag) const
	{
		const TObjectPtr<UAnimMontage>* pMontage = TagMap.Find(InTag);
		return (pMontage && *pMontage) ? pMontage->Get() :nullptr;
	}
	
};
