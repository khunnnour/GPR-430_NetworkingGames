// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkTestGameMode.h"
#include "NetworkTestHUD.h"
#include "NetworkTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANetworkTestGameMode::ANetworkTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ANetworkTestHUD::StaticClass();
}
