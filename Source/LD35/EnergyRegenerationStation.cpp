// Fill out your copyright notice in the Description page of Project Settings.

#include "LD35.h"
#include "EnergyRegenerationStation.h"
#include "LD35Character.h"


// Sets default values
AEnergyRegenerationStation::AEnergyRegenerationStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnergyRegenerationStation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnergyRegenerationStation::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	TArray<FOverlapResult> res;

	if (GetWorld()->OverlapMultiByObjectType(res, GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(Radius)))
	{
		for (auto& a : res)
		{
			if (auto chr = Cast<ALD35Character>(a.Actor.Get()))
			{
				if (chr->EnergyRechargeTime > 0)
				{
					if (!chr->IsInAlternateForm)
					{
						chr->Energy = FMath::Clamp(chr->Energy + DeltaTime / chr->EnergyRechargeTime, 0.f, 1.f);
					}
				}
			}
		}
	}
}

