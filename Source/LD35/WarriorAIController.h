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
	
private:
	float TimeToNextPath;
};
