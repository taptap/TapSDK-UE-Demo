// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPlayerController.h"
#include "GameFramework/RPGCharacter.h"
#include "RPGGameInstance.h"
#include "RPGSaveGame.h"
#include "Components/PackComponent.h"
#include "Items/RPGItem.h"
#include "Kismet/BlueprintMapLibrary.h"

ARPGPlayerController::ARPGPlayerController()
{
	PackComponent = CreateDefaultSubobject<UPackComponent>(TEXT("PackComponent"));
}


