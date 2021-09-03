// Copyright Epic Games, Inc. All Rights Reserved.

#include "DPLMGameMode.h"
#include "DPLMHUD.h"
#include "DPLMCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADPLMGameMode::ADPLMGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADPLMHUD::StaticClass();
}
