// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet_General.h"

#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAttributeSet_General::UAttributeSet_General()
{
	InitHP(100.f);
	InitMaxHP(100.f);
	
	InitMovingSpeed(400.f);
	InitMaxMovingSpeed(600.f);
}

void UAttributeSet_General::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute==GetHPAttribute())
	{
		SetHP(FMath::Clamp(GetHP(),0, GetMaxHP()));
	}
	if (Data.EvaluatedData.Attribute==GetMovingSpeedAttribute())
	{
		SetMovingSpeed(FMath::Clamp(GetMovingSpeed(),0,GetMaxMovingSpeed()));
	}
}

void UAttributeSet_General::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute==GetHPAttribute())
	{
		NewValue=FMath::Clamp(NewValue,0,GetMaxHP());
	}
	if (Attribute==GetMovingSpeedAttribute())
	{
		NewValue=FMath::Clamp(NewValue,0,GetMaxMovingSpeed());
	}
}

void UAttributeSet_General::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute==GetHPAttribute())
	{
		SetHP(FMath::Clamp(GetHP(),0, GetMaxHP()));
		if (GetHP()<=0.f)
		{
			UE_LOG(LogTemp,Error,TEXT("死亡"));
		}
	}
	if (Attribute==GetMovingSpeedAttribute())
	{
		if (AActor* OwnerActor=GetOwningActor())
		{
			if (ACharacter* OwnerCharacter=Cast<ACharacter>(OwnerActor))
			{
				if (auto* CharacterMovementComponent=OwnerCharacter->GetCharacterMovement())
				{
					CharacterMovementComponent->MaxWalkSpeed=GetMovingSpeed();
				}
			}
		}
	}
}
