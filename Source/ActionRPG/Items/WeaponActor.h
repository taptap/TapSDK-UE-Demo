// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

class URPGWeaponItem;

UCLASS()
class ACTIONRPG_API AWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	AWeaponActor();

	UPROPERTY(BlueprintReadOnly, Meta = (ExposeOnSpawn))
	const URPGWeaponItem* WeaponItem;

	UPROPERTY(BlueprintReadOnly, Meta = (ExposeOnSpawn))
	FRPGItemSlot Slot;

	UPROPERTY(EditDefaultsOnly)
	FName WeaponAttachSocket = TEXT("hand_rSocket");
};
