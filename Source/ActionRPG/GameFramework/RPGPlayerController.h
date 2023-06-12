// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActionRPG.h"
#include "GameFramework/PlayerController.h"
#include "RPGPlayerController.generated.h"


class UPackComponent;

UCLASS()
class ACTIONRPG_API ARPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARPGPlayerController();



protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPackComponent* PackComponent;
};
