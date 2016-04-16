// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LD35.h"
#include "LD35Character.h"
#include "LD35Projectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ALD35Character

ALD35Character::ALD35Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALD35Character::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	InputComponent->BindAxis("Fire", this, &ALD35Character::SetIsFiring);
	
	InputComponent->BindAxis("MoveForward", this, &ALD35Character::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALD35Character::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ALD35Character::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ALD35Character::LookUpAtRate);

	InputComponent->BindAction("Transform", IE_Pressed, this, &ALD35Character::Transform);
}

void ALD35Character::OnFire()
{ 
	if (IsInAlternateForm)
	{
		FVector trgPos = FindComponentByClass<UCameraComponent>()->GetComponentLocation() + GetActorRotation().RotateVector(FVector(140, 0, 0));

		DrawDebugSphere(GetWorld(), trgPos, 150, 8, FColor::Red, false, 0.5f);

		TArray<FOverlapResult> res;

		if (GetWorld()->OverlapMultiByChannel(res, trgPos, FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(150)))
		{
			for (auto& a : res)
			{
				if (a.Actor.Get() && a.Actor != this)
				{
					a.Actor->TakeDamage(1, FDamageEvent(), this->GetController(), this);
				}
			}
		}

		return;
	}

	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		USceneComponent* cam = FindComponentByClass<UCameraComponent>();

		if (cam)
		{
			FRotator SpawnRotation = cam->GetComponentRotation();
			FVector SpawnLocation = cam->GetComponentLocation();

			TArray<UActorComponent*> childComponents = GetComponentsByTag(USceneComponent::StaticClass(), "Gun");

			for (auto& b : childComponents)
			{
				if (auto a = Cast<USceneComponent>(b))
				{
					if (a->ComponentHasTag(TEXT("Gun")))
					{
						SpawnRotation = a->GetComponentRotation();
						SpawnLocation = a->GetComponentLocation();

						//UE_LOG(LogTemp, Display, TEXT("NM %s %s %s %s %s"), *a->GetName(), *cam->GetName(), *SpawnLocation.ToString(), *cam->GetComponentLocation().ToString(), *a->GetRelativeTransform().ToString());
					}
				}
			}

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile at the muzzle
				World->SpawnActor<ALD35Projectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

}

void ALD35Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if( TouchItem.bIsPressed == true )
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ALD35Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if( ( FingerIndex == TouchItem.FingerIndex ) && (TouchItem.bMoved == false) )
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void ALD35Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && ( TouchItem.FingerIndex==FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D( MoveDelta.X, MoveDelta.Y) / ScreenSize;									
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y* BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void ALD35Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALD35Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ALD35Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALD35Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ALD35Character::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if(FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch )
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ALD35Character::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &ALD35Character::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ALD35Character::TouchUpdate);
	}
	return bResult;
}

void ALD35Character::BeginPlay()
{
	Super::BeginPlay();

	InitialSpeed = GetCharacterMovement()->MaxWalkSpeed;
	InitialJumpPower = GetCharacterMovement()->JumpZVelocity;
}

void ALD35Character::Transform()
{
	if (CanTransform)
	{
		IsInAlternateForm = !IsInAlternateForm;

		if (IsInAlternateForm)
		{
			GetCharacterMovement()->MaxWalkSpeed = 1000;
			GetCharacterMovement()->JumpZVelocity = 900;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = InitialSpeed;
			GetCharacterMovement()->JumpZVelocity = InitialJumpPower;
		}
	}
}

void ALD35Character::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	ShotCooldown -= deltaTime;

	if (IsFiring && ShotCooldown <= 0)
	{
		ShotCooldown = IsInAlternateForm ? 0.5f : 1.5f;

		OnFire();
	}
}

void ALD35Character::SetIsFiring(float isFiring)
{
	IsFiring = isFiring > 0.5f;
}

float ALD35Character::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	float dmg = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= dmg;

	if (Health <= 0) Destroy();

	return dmg;
}
