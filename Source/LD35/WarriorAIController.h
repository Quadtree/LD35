// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "WarriorAIController.generated.h"

/**
 * 
 */
UCLASS()
class LD35_API AWarriorAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = Destination)
	FVector CurrentDestination;

	void Tick(float deltaTime);

	void BeginPlay();

	UPROPERTY(BlueprintReadOnly, Category = Transformation)
	bool KnowsWeretigerIdentity;

	bool CanPawnSee(class ALD35Character *chr);
	
private:
	float TimeToNextPath;
	float TimeToNextScan;
	float LastContactTimer;

	UPROPERTY()
	AActor* CurrentAttackTarget;

	float LastContactThreat;

	float GetThreatLevel(class ALD35Character* chr);

	void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;
};
