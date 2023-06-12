// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FSlateStyleSet;

/**
 * 
 */
class TAPCOMMON_API FTapStyleCommon
{
public:
	static void Initialize();

	static void Shutdown();

	static const ISlateStyle& Get();

	static FName GetStyleSetName();

private:
	static TSharedRef<FSlateStyleSet > Create();

	static TSharedPtr<FSlateStyleSet > TapStyleInstance;
};
