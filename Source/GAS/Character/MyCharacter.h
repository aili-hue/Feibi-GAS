// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"

#include "MyCharacter.generated.h"


struct FGameplayTag;
struct FInputActionValue;
class UAttributeSet_General;
class UCharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UAbilitySystemComponent;
class UGameplayAbility;
class UMontageDataAsset;

UCLASS()
class GAS_API AMyCharacter : public ACharacter , public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties
	AMyCharacter();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AnimMontage")
	TObjectPtr<UMontageDataAsset> SprintMontage;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//CharacterMovementWalkSpeed
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent>CharacterMovementComponent;
	
	float WalkSpeed = 400.f;
	
	// SpringArm And Camera
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SpringArm")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Camera")
	TObjectPtr<UCameraComponent> Camera;
	
	// Input
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputMappingContext")
	TObjectPtr<UInputMappingContext>IMC_Basic;
	
	void InitIMCSubsystem();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputAction")
	TObjectPtr<UInputAction>IA_MovingPerspective;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputAction")
	TObjectPtr<UInputAction>IA_Move;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputAction")
	TObjectPtr<UInputAction>IA_Ctrl;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputAction")
	TObjectPtr<UInputAction>IA_RightMouseButton;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputAction")
	TObjectPtr<UInputAction>IA_Jump;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputAction")
	TObjectPtr<UInputAction>IA_Attack;
	
	float AttackStartTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float HeavyAttackThresholdFrames = 0.2f;
	
	// InputEvent
	void MovingPerspectiveEvent(const FInputActionValue& InputAction);
	void MoveEvent(const FInputActionValue& InputAction);
	void MoveEndEvent(const FInputActionValue& InputAction);
	void CtrlEvent(const FInputActionValue& InputAction);
	void RightMouseButtonEvent(const FInputActionValue& InputAction);
	void JumpEvent(const FInputActionValue& InputAction);
	void JumpEndEvent(const FInputActionValue& InputAction);
	void AttackEvent(const FInputActionValue& InputAction);
	void AttendEvent(const FInputActionValue& InputAction);
	
	//GAS
	//AbilitySystemComponent
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AbilitySystemComponent")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	//GamePlayAbility
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameplayAbility")
	TArray<TSubclassOf<UGameplayAbility>> MyGameplayAbility;
	
	FGameplayTag SprintTag = FGameplayTag::RequestGameplayTag(FName("Ability.Sprint"));
	FGameplayTag WalkTag = FGameplayTag::RequestGameplayTag(FName("Ability.Walk"));
	FGameplayTag StopWalkingTag = FGameplayTag::RequestGameplayTag(FName("Ability.StopWalking"));
	FGameplayTag JumpTag = FGameplayTag::RequestGameplayTag(FName("Ability.Jump"));
	FGameplayTag JumpingTag = FGameplayTag::RequestGameplayTag(FName("Data.Jump.Jumping"));
	FGameplayTag EndJumpTag = FGameplayTag::RequestGameplayTag(FName("Data.Jump"));
	FGameplayTag MoveTag = FGameplayTag::RequestGameplayTag(FName("Ability.Move"));
	FGameplayTag Attack = FGameplayTag::RequestGameplayTag(FName("Attack.NormalAttack"));
	FGameplayTag JumpAttack=FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Jump"));
	FGameplayTag LandedTag= FGameplayTag::RequestGameplayTag(FName("Data.Landed"));
	FGameplayTag TapTag= FGameplayTag::RequestGameplayTag(FName("Data.Attack.Tap"));
	FGameplayTag HeavyBlowTag= FGameplayTag::RequestGameplayTag(FName("Data.Attack.HeavyBlow"));
	FGameplayTag AttackingTag= FGameplayTag::RequestGameplayTag(FName("State.Attacking"));
	
	//AttributeSet
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AttributeSet")
	TObjectPtr<UAttributeSet_General>AttributeSet;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Init_GAS
	virtual void PossessedBy(AController* NewController)override;
	
	UFUNCTION(BlueprintCallable,Category="AbilitySystemComponent")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode = 0) override;
	virtual void Landed(const FHitResult& Hit)override;
};
