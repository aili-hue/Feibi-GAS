// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GAS/AttributeSet/AttributeSet/General/AttributeSet_General.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	AbilitySystemComponent=CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet=CreateDefaultSubobject<UAttributeSet_General>(TEXT("AttributeSet"));
	
	SpringArm=CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength=150.f;
	
	Camera=CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(SpringArm);
	
	CharacterMovementComponent = Cast<UCharacterMovementComponent>(GetCharacterMovement());
	CharacterMovementComponent->MaxWalkSpeed=AttributeSet->GetMovingSpeed();
	
	
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	InitIMCSubsystem();
}

void AMyCharacter::InitIMCSubsystem()
{
	if (AController* LController = GetController())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(LController))
		{
			if (ULocalPlayer* Player = PlayerController->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem=Player->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					if (IMC_Basic)
					{
						Subsystem->AddMappingContext(IMC_Basic,0);
					}
				}
			}
		}
	}
}

void AMyCharacter::MovingPerspectiveEvent(const FInputActionValue& InputAction)
{
	const FVector2D& MovingPerspect=InputAction.Get<FVector2D>();
	AddControllerYawInput(MovingPerspect.X);
	AddControllerPitchInput(MovingPerspect.Y);
}

void AMyCharacter::MoveEvent(const FInputActionValue& InputAction)
{
	const FVector2D& Move=InputAction.Get<FVector2D>();
	const FRotator& ControllerRotator= GetController()->GetControlRotation();
	FRotator Yaw(0.f,ControllerRotator.Yaw,0.f);
	FVector Ahead= FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
	FVector RightSide= FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y);
	AddMovementInput(Ahead,Move.X);
	AddMovementInput(RightSide,Move.Y);
	
	if (AbilitySystemComponent->HasMatchingGameplayTag(MoveTag) || !AbilitySystemComponent)return;
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(MoveTag);
	AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
}

void AMyCharacter::MoveEndEvent(const FInputActionValue& InputAction)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(StopWalkingTag);
		AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
	}
}

void AMyCharacter::CtrlEvent(const FInputActionValue& InputAction)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(WalkTag);
		AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
	}
}

void AMyCharacter::RightMouseButtonEvent(const FInputActionValue& InputAction)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer RightMouseButtons;
		RightMouseButtons.AddTag(SprintTag);
		AbilitySystemComponent->TryActivateAbilitiesByTag(RightMouseButtons);
	}
}

void AMyCharacter::JumpEvent(const FInputActionValue& InputAction)
{
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(LandedTag);
		
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(JumpTag);
		AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
	}
}

void AMyCharacter::JumpEndEvent(const FInputActionValue& InputAction)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(EndJumpTag);
	}
}

void AMyCharacter::AttackEvent(const FInputActionValue& InputAction)
{
	if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(AttackingTag))
	{
		FGameplayEventData Payload;
		Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Attack.ComboInput"));
		AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		return;
	}
	if (AttackStartTime<=0.f)
	{
		AttackStartTime=GetWorld()->GetTimeSeconds();
	}
}

void AMyCharacter::AttendEvent(const FInputActionValue& InputAction)
{
	
	if (!AbilitySystemComponent)return;
	AbilitySystemComponent->RemoveLooseGameplayTag(TapTag);
	AbilitySystemComponent->RemoveLooseGameplayTag(HeavyBlowTag);
	
	float CurrentTime = GetWorld()->GetTimeSeconds()-AttackStartTime;
	AttackStartTime =0.f;
	
	if (CurrentTime > HeavyAttackThresholdFrames)
	{
		AbilitySystemComponent->AddLooseGameplayTag(HeavyBlowTag);
	}
	else
	{
		AbilitySystemComponent->AddLooseGameplayTag(TapTag);
	}
	
	if (AbilitySystemComponent->HasMatchingGameplayTag(JumpingTag)||AbilitySystemComponent->HasMatchingGameplayTag(EndJumpTag))
	{
		
		FGameplayTagContainer Container;
		Container.AddTag(JumpAttack); 
		AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
		
	}
	else
	{
		FGameplayTagContainer Container;
		Container.AddTag(Attack); 
		AbilitySystemComponent->TryActivateAbilitiesByTag(Container);
	}
	
}


void AMyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this,this);
		
		if (HasAuthority())
		{
			for (const TSubclassOf<UGameplayAbility>& Ability:MyGameplayAbility)
			{
				AbilitySystemComponent->GiveAbility(Ability);
			}
		}
	}
}

UAbilitySystemComponent* AMyCharacter::GetAbilitySystemComponent() const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent;
	}
	return nullptr;
}

void AMyCharacter::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	
	if (GetMovementComponent()->IsFalling() && AbilitySystemComponent)
	{
		if ( !AbilitySystemComponent->HasMatchingGameplayTag(JumpingTag))
		{
			AbilitySystemComponent->AddLooseGameplayTag(JumpingTag);
		}
	}
	else
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(EndJumpTag);
		AbilitySystemComponent->RemoveLooseGameplayTag(JumpingTag);
	}
}

void AMyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	if (!AbilitySystemComponent) return;
	AbilitySystemComponent->RemoveLooseGameplayTag(JumpingTag);
	AbilitySystemComponent->AddLooseGameplayTag(LandedTag);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent =Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_MovingPerspective)
		{
			EnhancedInputComponent->BindAction(IA_MovingPerspective,ETriggerEvent::Triggered,this,&ThisClass::MovingPerspectiveEvent);
		}
		if (IA_Move)
		{
			EnhancedInputComponent->BindAction(IA_Move,ETriggerEvent::Triggered,this,&ThisClass::MoveEvent);
			EnhancedInputComponent->BindAction(IA_Move,ETriggerEvent::Completed,this,&ThisClass::MoveEndEvent);
		}
		if (IA_Ctrl)
		{
			EnhancedInputComponent->BindAction(IA_Ctrl,ETriggerEvent::Started,this,&ThisClass::CtrlEvent);
		}
		if (IA_RightMouseButton)
		{
			EnhancedInputComponent->BindAction(IA_RightMouseButton,ETriggerEvent::Started,this,&ThisClass::RightMouseButtonEvent);
		}
		if (IA_Jump)
		{
			EnhancedInputComponent->BindAction(IA_Jump,ETriggerEvent::Started,this,&ThisClass::JumpEvent);
			EnhancedInputComponent->BindAction(IA_Jump,ETriggerEvent::Completed,this,&ThisClass::JumpEndEvent);
		}
		if (IA_Attack)
		{
			EnhancedInputComponent->BindAction(IA_Attack,ETriggerEvent::Started,this,&ThisClass::AttackEvent);
			EnhancedInputComponent->BindAction(IA_Attack,ETriggerEvent::Completed,this,&ThisClass::AttendEvent);
		}
	}
}

