// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LD35.h"
#include "LD35GameMode.h"
#include "LD35HUD.h"
#include "LD35Character.h"

ALD35GameMode::ALD35GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALD35HUD::StaticClass();
}
