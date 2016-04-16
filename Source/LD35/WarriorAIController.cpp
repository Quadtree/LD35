// Fill out your copyright notice in the Description page of Project Settings.

#include "LD35.h"
#include "WarriorAIController.h"

void AWarriorAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	TimeToNextPath -= deltaTime;

	if (TimeToNextPath < 0)
	{
		TimeToNextPath = 3;
		MoveToLocation(CurrentDestination);
	}
}
