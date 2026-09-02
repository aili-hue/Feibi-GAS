// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AttributeSet_General.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UAttributeSet_General : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UAttributeSet_General();
	
	UPROPERTY(BlueprintReadWrite,Category="AttributeSet");
	FGameplayAttributeData HP;
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass,HP);
	
	UPROPERTY(BlueprintReadWrite,Category="AttributeSet");
	FGameplayAttributeData MaxHP;
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass,MaxHP);
	
	UPROPERTY(BlueprintReadWrite,Category="AttributeSet");
	FGameplayAttributeData MovingSpeed;
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass,MovingSpeed);
	
	UPROPERTY(BlueprintReadWrite,Category="AttributeSet");
	FGameplayAttributeData MaxMovingSpeed;
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass,MaxMovingSpeed);
	
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)override;
};
