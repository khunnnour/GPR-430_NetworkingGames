// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NetworkTestHUD.generated.h"

UCLASS()
class ANetworkTestHUD : public AHUD
{
	GENERATED_BODY()

public:
	ANetworkTestHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

