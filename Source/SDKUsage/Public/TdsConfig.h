// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TUType.h"
#include "TdsConfig.generated.h"


UCLASS(Config = "Game", DefaultConfig)
class SDKUSAGE_API UTdsConfig : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Config)
	FString ClientId;
	
	UPROPERTY(EditAnywhere, Config)
	FString ClientToken;

	UPROPERTY(EditAnywhere, Config)
	ERegionType RegionType = ERegionType::CN;

	UPROPERTY(EditAnywhere, Config)
	FString ServerURL;

	UPROPERTY(EditAnywhere, Config)
	bool bEnableTapDB;

	UPROPERTY(EditAnywhere, Config, Meta = (EditCondition = bEnableTapDB))
	FString Channel;

	UPROPERTY(EditAnywhere, Config, Meta = (EditCondition = bEnableTapDB))
	FString GameVersion;

	UPROPERTY(EditAnywhere, Config, Meta = (EditCondition = bEnableTapDB))
	bool AdvertiserIDCollectionEnabled = false;

	UPROPERTY(EditAnywhere, Config)
	bool bEnableBillboard;

	UPROPERTY(EditAnywhere, Config, Meta = (EditCondition = bEnableBillboard))
	TMap<FString, FString> Dimensions;

	UPROPERTY(EditAnywhere, Config, Meta = (EditCondition = bEnableBillboard))
	FString BillboardUrl;

	UPROPERTY(EditAnywhere, Config)
	bool bEnableSupport;

	UPROPERTY(EditAnywhere, Config, Meta = (EditCondition = bEnableSupport))
	FString SupportUrl;

	UPROPERTY(EditAnywhere, Config, Meta = (EditCondition = bEnableSupport))
	FString ProductId;

	UPROPERTY(EditAnywhere, Config)
	bool bEnableMoment;

	UPROPERTY(EditAnywhere, Config, Meta = (EditCondition = bEnableMoment))
	FString AppId;
};
