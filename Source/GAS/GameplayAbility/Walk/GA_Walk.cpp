// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Walk.h"
#include "AbilitySystemComponent.h"
#include "GAS/AttributeSet/AttributeSet/General/AttributeSet_General.h"


UGA_Walk::UGA_Walk()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Walk::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilitySystemComponent* ASC=GetAbilitySystemComponentFromActorInfo();
	if (ASC && WalkEffect)
	{
		float Speed =ASC->GetNumericAttribute(UAttributeSet_General::GetMovingSpeedAttribute());
		float FinalSpeed=FMath::IsNearlyEqual(Speed,100.f,1.f)?  400.f : 100.f;
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(WalkEffect,1.f,ContextHandle);
		if (SpecHandle.IsValid())
		{
			FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Data.MovingSpeed"));
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(Tag,FinalSpeed);
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
}

void UGA_Walk::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
