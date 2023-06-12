// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TULanguage.h"
#include "TapAchievementTypes.h"
#include "TapAchievementRest.generated.h"

struct FTDSUser;
struct FTUError;

#define ACHIEVEMENT_HOST_CN "https://tds-tapsdk.cn.tapapis.com"
#define ACHIEVEMENT_HOST_IO "https://tds-tapsdk0.intl.tapapis.com"

USTRUCT()
struct FAchievementDescListData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FAchievementDescTap> List;

	void CustomParseData(const TSharedRef<FJsonObject>& JsonObject);
};

USTRUCT()
struct FAchievementListData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAchievementTap> List;

	void CustomParseData(const TSharedRef<FJsonObject>& JsonObject);
};

USTRUCT()
struct FAchievementPlatinumData
{
	GENERATED_BODY()

	TSharedPtr<FAchievementTap> Platinum;
	
	void CustomParseData(const TSharedRef<FJsonObject>& JsonObject);
};

DECLARE_DELEGATE_TwoParams(FAllAchievementsResult, const TSharedPtr<FAchievementDescListData>&, const TSharedPtr<FTUError>&);
DECLARE_DELEGATE_TwoParams(FUserAchievementResult, const TSharedPtr<FAchievementListData>&, const TSharedPtr<FTUError>&);
DECLARE_DELEGATE_TwoParams(FWriteAchievementResult, const TSharedPtr<FAchievementPlatinumData>&, const TSharedPtr<FTUError>&);

namespace FTapAchievementRest
{
	void FetchAllAchievementList(const FString& SessionToken, const FAllAchievementsResult& Callback, const FString& LanguageString = TULanguage::GetLanguageString());

	void FetchUserAchievementList(const FString& SessionToken, const FString& ObjectId, const FUserAchievementResult& Callback, const FString& LanguageString = TULanguage::GetLanguageString());

	void WriteAchievement(const FString& SessionToken, const FString& ObjectId, const TArray<FAchievementTap>& Achievements, const FWriteAchievementResult& Callback);
}
