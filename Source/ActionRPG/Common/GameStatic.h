// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_OneParam(FTextureDelegate, UTexture2D* Texture);

class ACTIONRPG_API FGameStatic
{
public:
	static void FindOrDownloadTexture(const FString& Url, const FTextureDelegate& Callback);
};

