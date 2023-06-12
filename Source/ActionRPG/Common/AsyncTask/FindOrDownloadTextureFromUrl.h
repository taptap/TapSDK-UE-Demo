// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FindOrDownloadTextureFromUrl.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTextureDelegateDynamic, UTexture2D*, Texture);
/**
 * 
 */
UCLASS()
class ACTIONRPG_API UFindOrDownloadTextureFromUrl : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FTextureDelegateDynamic OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FTextureDelegateDynamic OnFailed;

	UFUNCTION(BlueprintCallable, Category = "Ability|Async", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UFindOrDownloadTextureFromUrl* FindOrDownloadTexture(const FString& Url);

	virtual void Activate() override;

	FString SavedUrl;
};
