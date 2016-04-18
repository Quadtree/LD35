// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LD35.h"
#include "LD35GameMode.h"
#include "LD35HUD.h"
#include "LD35Character.h"
#include "Engine.h"

ALD35GameMode::ALD35GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALD35HUD::StaticClass();
}

void ALD35GameMode::BeginPlay()
{
	Super::BeginPlay();

#if PLATFORM_HTML5
	//GEngine->GameUserSettings->ScalabilityQuality.SetFromSingleQualityLevel(2);
	GEngine->GameUserSettings->ScalabilityQuality.AntiAliasingQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.EffectsQuality = 3; // 2
	GEngine->GameUserSettings->ScalabilityQuality.PostProcessQuality = 0; // 2
	GEngine->GameUserSettings->ScalabilityQuality.ResolutionQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.TextureQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.ViewDistanceQuality = 0; // 2
	GEngine->GameUserSettings->ScalabilityQuality.ShadowQuality = 3; // 1
	GEngine->GameUserSettings->ApplySettings();
#endif
}
