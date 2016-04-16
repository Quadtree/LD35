// Fill out your copyright notice in the Description page of Project Settings.

#include "LD35.h"
#include "WarriorAIController.h"
#include "DrawDebugHelpers.h"
#include "LD35Character.h"

void AWarriorAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (auto chr = Cast<ALD35Character>(GetPawn()))
	{
		chr->IsFiring = false;
	}

	TimeToNextPath -= deltaTime;
	TimeToNextScan -= deltaTime;

	if (!CurrentAttackTarget)
	{
		if (TimeToNextPath < 0)
		{
			TimeToNextPath = 3;
			MoveToLocation(CurrentDestination);
		}
	}
	else
	{
		// too much threat. run away!
		if (LastContactThreat > 20)
		{
			FVector delta = (GetPawn()->GetActorLocation() - CurrentAttackTarget->GetActorLocation()).GetSafeNormal();

			FVector fleePos = GetPawn()->GetActorLocation() + delta * 2000;

			FHitResult res;

			if (GetWorld()->LineTraceSingleByObjectType(res, fleePos + FVector(0, 0, 5000), fleePos - FVector(0, 0, 5000), FCollisionObjectQueryParams::AllStaticObjects))
			{
				UE_LOG(LogTemp, Display, TEXT("Fleeing to %s"), *res.Location.ToCompactString());
				MoveToLocation(res.Location);
			}
		}
		else
		{
			FVector delta = (CurrentAttackTarget->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal();

			DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), CurrentAttackTarget->GetActorLocation(), FColor::Yellow, true, 0.5f);

			StopMovement();

			SetControlRotation(delta.Rotation());

			if (auto chr = Cast<ALD35Character>(GetPawn()))
			{
				chr->IsFiring = true;
			}
		}
	}

	if (TimeToNextScan < 0)
	{
		TimeToNextScan = 1;

		TArray<FOverlapResult> res;

		if (GetWorld()->OverlapMultiByObjectType(res, GetPawn()->GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(2000)))
		{
			LastContactThreat = 0.001f;
			CurrentAttackTarget = nullptr;

			for (auto& a : res)
			{
				if (a.Actor.Get())
				{
					if (auto chr = Cast<ALD35Character>(a.Actor.Get()))
					{
						float threat = this->GetThreatLevel(chr);

						if (threat > LastContactThreat)
						{
							LastContactThreat = threat;
							CurrentAttackTarget = chr;
						}
					}
				}
			}

			if (CurrentAttackTarget)
			{
				UE_LOG(LogTemp, Display, TEXT("Trg=%s"), *CurrentAttackTarget->GetName());
			}
		}
	}
}

void AWarriorAIController::BeginPlay()
{
	Super::BeginPlay();

	LastContactTimer = 99999;
}

float AWarriorAIController::GetThreatLevel(ALD35Character* chr)
{
	ALD35Character* self = Cast<ALD35Character>(GetPawn());

	if (!self)
	{
		UE_LOG(LogTemp, Warning, TEXT("Incorrect pawn type"));
		return 0;
	}

	if (!CanPawnSee(chr)) return 0;

	float range = FVector::Dist(chr->GetActorLocation(), GetPawn()->GetActorLocation());

	float threat = 0;

	if ((chr->IsInAlternateForm || (chr->CanTransform && KnowsWeretigerIdentity)) && range < 500) threat = FMath::Max(threat, 30.f);
	if (chr->IsInAlternateForm) threat = FMath::Max(threat, 0.5f);

	if (chr->Faction != self->Faction) threat = FMath::Max(threat, 1.f);

	threat -= range / 2000;

	return threat;
}

bool AWarriorAIController::CanPawnSee(ALD35Character * chr)
{
	FVector eyePos;
	FRotator eyeRot;

	GetActorEyesViewPoint(eyePos, eyeRot);

	uint32 obstructed = 0;
	uint32 tot = 0;

	for (int32 h = 0; h < 100; h += 20)
	{
		++tot;
		if (GetWorld()->LineTraceTestByObjectType(eyePos, chr->GetActorLocation(), FCollisionObjectQueryParams::AllStaticObjects)) ++obstructed;
	}

	if (obstructed >= tot) return false;

	float range = FVector::Dist(chr->GetActorLocation(), GetPawn()->GetActorLocation()) + (obstructed * 800 / tot);

	FVector forwardVector = GetControlRotation().RotateVector(FVector(1, 0, 0));

	FVector bearingVector = (chr->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal();

	if (FVector::DistSquared(forwardVector, bearingVector) < FMath::Square(1))
	{
		// target is in front of us
		return range < 2000;
	}
	else
	{
		// target is behind us
		return range < 350;
	}
}
