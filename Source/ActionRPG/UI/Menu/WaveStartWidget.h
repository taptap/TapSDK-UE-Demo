// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaveStartWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UWaveStartWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWave(int32 Wave);
};
