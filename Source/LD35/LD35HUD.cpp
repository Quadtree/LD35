// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LD35.h"
#include "LD35HUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "LD35Character.h"
#include "EngineUtils.h"

ALD35HUD::ALD35HUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshiarTexObj.Object;
}


void ALD35HUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	

	// draw the crosshair
	

	if (auto chr = Cast<ALD35Character>(GetOwningPawn()))
	{
		if (chr->IsInAlternateForm)
		{
			for (TActorIterator<ALD35Character> i(GetWorld()); i; ++i)
			{
				if (*i == chr) continue;

				if (Cast<ALD35Character>(*i)->Health > 0)
				{
					FVector pos = Project(i->GetActorLocation() + FVector(0, 0, 150));

					if (pos.Z > 0 && pos.X >= -200 && pos.Y >= -200 && pos.X <= Canvas->ClipX + 200 && pos.Y <= Canvas->ClipY + 200)
					{
						const FVector2D CrosshairDrawPosition((pos.X - 8), (pos.Y - 8));

						FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
						TileItem.BlendMode = SE_BLEND_Translucent;
						TileItem.Size.X = 16;
						TileItem.Size.Y = 16;
						Canvas->DrawItem(TileItem);
					}
				}
			}
		}
	}
}

