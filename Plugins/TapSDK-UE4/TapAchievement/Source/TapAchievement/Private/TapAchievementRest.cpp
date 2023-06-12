// Fill out your copyright notice in the Description page of Project Settings.


#include "TapAchievementRest.h"

#include "TapNet.h"
#include "TUHelper.h"
#include "TDSUser.h"
#include "TUDeviceInfo.h"
#include "TapAchievementModule.h"


struct FTapHttpBuilder_Achievement : public FTapHttpBuilder
{
	FTapHttpBuilder_Achievement(const FString& InSessionToken, const FString& Verb, const TCHAR* Path, const TMap<FString, FString>& Query = {})
		: FTapHttpBuilder(Verb, FString(FTUConfig::Get()->RegionType == ERegionType::CN ? TEXT(ACHIEVEMENT_HOST_CN) : TEXT(ACHIEVEMENT_HOST_IO)), Path, Query)
	{
		ContentType_Json();
		ClientID();
		LCSign();
		AuthType();
		SetHeader(TEXT("XUA"), MakeXUA());
		SessionToken(InSessionToken);
	}
	
	static FString MakeXUA()
	{
		return FString::Printf(TEXT("Device-ID=%s&SDK-Version-Name=%s&OS-Version=%s&&Platform=%s&Lang=%s&SDK-Version-Code=%s&SDK-Name=TapAchievement"),
			*TUDeviceInfo::GetLoginId(),
			TEXT(TapAchievement_VERSION),
			*TUDeviceInfo::GetOSVersion(),
			*TUDeviceInfo::GetPlatform(),
			*TULanguage::GetLanguageString(),
			TEXT(TapAchievement_VERSION_NUMBER));
	}
};

void FAchievementDescListData::CustomParseData(const TSharedRef<FJsonObject>& JsonObject)
{
	const TArray<TSharedPtr<FJsonValue>>* ValuesP = nullptr;
	if (JsonObject->TryGetArrayField(TEXT("list"), ValuesP))
	{
		List.Empty(ValuesP->Num());

		for (const TSharedPtr<FJsonValue>& Value : *ValuesP)
		{
			const TSharedPtr<FJsonObject>* AchievementObjectP = nullptr;
			if (Value->TryGetObject(AchievementObjectP) && ensure(AchievementObjectP))
			{
				List.Add(*AchievementObjectP);
			}
		}
	}
}

void FAchievementListData::CustomParseData(const TSharedRef<FJsonObject>& JsonObject)
{
	const TArray<TSharedPtr<FJsonValue>>* ValuesP = nullptr;
	if (JsonObject->TryGetArrayField(TEXT("list"), ValuesP))
	{
		List.Empty(ValuesP->Num());

		for (const TSharedPtr<FJsonValue>& Value : *ValuesP)
		{
			const TSharedPtr<FJsonObject>* AchievementObjectP = nullptr;
			if (Value->TryGetObject(AchievementObjectP) && ensure(AchievementObjectP))
			{
				List.Add(FAchievementTap(*AchievementObjectP));
			}
		}
	}
}

void FAchievementPlatinumData::CustomParseData(const TSharedRef<FJsonObject>& JsonObject)
{
	const TSharedPtr<FJsonObject>* ObjP = nullptr;
	if (JsonObject->TryGetObjectField(TEXT("platinum"),ObjP) && ObjP)//如果没有{"platinum",null} 拿不到JsonObj
	{
		Platinum = MakeShared<FAchievementTap>(*ObjP);
	}
}

void FTapAchievementRest::FetchAllAchievementList(const FString& SessionToken, const FAllAchievementsResult& Callback, const FString& LanguageString)
{
	check(FTUConfig::Get());
	
	const FTUConfig& Config = *FTUConfig::Get();
	FString Path = FString::Printf(TEXT("/achievement/api/v1/clients/%s/achievements/languages/%s"), *Config.ClientID, *LanguageString);
	
	FTapHttpBuilder_Achievement(SessionToken, TEXT("GET"), *Path).ProcessWithCallback(Callback);
}

void FTapAchievementRest::FetchUserAchievementList(const FString& SessionToken, const FString& ObjectId, const FUserAchievementResult& Callback, const FString& LanguageString)
{
	check(FTUConfig::Get());
	
	const FTUConfig& Config = *FTUConfig::Get();
	FString Path = FString::Printf(TEXT("/achievement/api/v1/clients/%s/users/%s/achievements/languages/%s"), *Config.ClientID, *ObjectId, *LanguageString);
	
	FTapHttpBuilder_Achievement(SessionToken, TEXT("GET"), *Path).ProcessWithCallback(Callback);
}

void FTapAchievementRest::WriteAchievement(const FString& SessionToken, const FString& ObjectId, const TArray<FAchievementTap>& Achievements, const FWriteAchievementResult& Callback)
{
	check(FTUConfig::Get());
	
	const FTUConfig& Config = *FTUConfig::Get();
	FString Path = FString::Printf(TEXT("/achievement/api/v1/clients/%s/users/%s/achievements"), *Config.ClientID, *ObjectId);
	
	FString ContentString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteArrayStart(TEXT("list"));
	for (const FAchievementTap& Achievement : Achievements)
	{
		Achievement.Write(Writer);
	}
	Writer->WriteArrayEnd();
	Writer->WriteObjectEnd();
	Writer->Close();
	
	FTapHttpBuilder_Achievement(SessionToken, TEXT("POST"), *Path)
		.Content_String(ContentString)
		.ProcessWithCallback(Callback);
}
