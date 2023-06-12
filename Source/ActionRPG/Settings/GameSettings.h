// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "GameSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class ACTIONRPG_API UGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UGameSettings();
	
};
