// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Json.h"
#include "TdsInterface.generated.h"

UENUM(BlueprintType)
enum class ETdsAuthType : uint8
{
	None,
	TapTap,
	Anonymous,
	Others
};

UENUM(BlueprintType)
enum class ELeaderboardCategory : uint8
{
	Kill,
	Wave
};

UENUM(BlueprintType)
enum class ELeaderboardType : uint8
{
	Total,
	Month,
	Week,
	Day
};

FORCEINLINE FString MakeLeaderboardFullKeyName(ELeaderboardCategory Category, ELeaderboardType Type)
{
	FString StrA = UEnum::GetValueAsString(Category).ToLower();
	StrA.RemoveFromStart(TEXT("ELeaderboardCategory::"));
	FString StrB = UEnum::GetValueAsString(Type).ToLower();
	StrB.RemoveFromStart(TEXT("ELeaderboardType::"));
	return StrA + TEXT("_") + StrB;
}

FORCEINLINE bool ParseLeaderboardFullKeyName(const FString& FullKeyName, ELeaderboardCategory& Category, ELeaderboardType& Type)
{
	FString L, R;
	if (!FullKeyName.Split(TEXT("_"), &L, &R)) return false;

	int64 VL = StaticEnum<ELeaderboardCategory>()->GetValueByNameString(L);
	int64 VR = StaticEnum<ELeaderboardType>()->GetValueByNameString(R);
	if(VL == INDEX_NONE || VR == INDEX_NONE) return false;

	Category = static_cast<ELeaderboardCategory>(VL);
	Type = static_cast<ELeaderboardType>(VR);
	return true;
}

USTRUCT(BlueprintType)
struct ACTIONRPG_API FTdsError
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 Code = 0;

	UPROPERTY(BlueprintReadOnly)
	FString Msg;

	UPROPERTY(BlueprintReadOnly)
	FString Description;
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FTdsPlayer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Id;

	UPROPERTY(BlueprintReadOnly)
	FString AvatarUrl;

	UPROPERTY(BlueprintReadOnly)
	FString NickName;

	UPROPERTY(BlueprintReadOnly, NotReplicated)
	FString ShortID;

	UPROPERTY(BlueprintReadOnly)
	FString Email;

	UPROPERTY(BlueprintReadOnly, NotReplicated)
	FString Username;

	UPROPERTY(BlueprintReadOnly, NotReplicated)
	FString Password;

	UPROPERTY(BlueprintReadOnly, NotReplicated)
	FString MobilePhoneNumber;

	UPROPERTY(BlueprintReadOnly, NotReplicated)
	FString SessionToken;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAnonymous;

	UPROPERTY(BlueprintReadOnly)
	bool bIsMobilePhoneVerified;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAuthenticated;

	UPROPERTY(BlueprintReadOnly, NotReplicated)
	int32 MinAge;

	UPROPERTY(BlueprintReadOnly, NotReplicated)
	int32 RemainingSeconds;

	bool IsValid() const { return !Id.IsEmpty(); }
};

UENUM(BlueprintType)
enum class EAntiAddictionMessageType : uint8
{
	Login,					// 登录成功
	Exited,					// 退出登录
	DurationLimit,			// 可玩时长耗尽
	PeriodRestrict,			// 达到宵禁时长
	RealNameStop,			// 实名过程中点击了关闭实名窗
};

USTRUCT(BlueprintType)
struct FLeaderboardStatistic
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ELeaderboardCategory Category;

	UPROPERTY(BlueprintReadOnly)
	ELeaderboardType Type;

	UPROPERTY(BlueprintReadOnly)
	float Value;
};

USTRUCT(BlueprintType)
struct FLeaderboardData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 RankIndex;

	UPROPERTY(BlueprintReadOnly)
	FString ObjectId;

	UPROPERTY(BlueprintReadOnly)
	FString NickName;

	UPROPERTY(BlueprintReadOnly)
	FString AvatarUrl;

	UPROPERTY(BlueprintReadOnly)
	float Value;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FLeaderboardStatistic> Statistics;
};


DECLARE_DELEGATE_TwoParams(FTdsUserDelegate, const TSharedPtr<FTdsPlayer>& Player, const TSharedPtr<FTdsError>& Error);
DECLARE_DELEGATE_OneParam(FSimpleTdsDelegate, const TSharedPtr<FTdsError>& Error/* Empty means successful.*/);
DECLARE_DELEGATE_OneParam(FBooleanDelegate, bool);
DECLARE_DELEGATE_OneParam(FAntiAddictionDelegate, EAntiAddictionMessageType MsgType);
DECLARE_DELEGATE_TwoParams(FLeaderboardDelegate, const TArray<FLeaderboardData>& RankData, int64 Count);
DECLARE_DELEGATE_OneParam(FLeaderboardDetailDelegate, const TArray<FLeaderboardStatistic>& Statistics);

#define TDS_WIDGET_Z_ORDER 9999

class ACTIONRPG_API ITdsInterface
{
public:
	virtual ~ITdsInterface() {}
	
	virtual void InitSdk() {}


	virtual void OpenSplashAnnouncement(const FSimpleTdsDelegate& InCallback, const FSimpleDelegate& OnClosed) {}

	virtual void CloseSplashAnnouncement() {}

	virtual void HasNavigateAnnouncement(const FBooleanDelegate& InCallback) {}
	
	virtual void OpenNavigateAnnouncement(const FSimpleTdsDelegate& InCallback, const FSimpleDelegate& OnClosed) {}

	virtual void SetAnnouncementRedDotCallback(const FBooleanDelegate& InCallback) {}

	virtual void StartupMarquee(bool bNewStart) {}
	

	virtual void TdsLoginWithStorage(const FTdsUserDelegate& InCallback) {}
	
	virtual void TdsLoginAnonymously(const FTdsUserDelegate& InCallback) {}
	
	virtual void TdsLoginWithTapTap(const FTdsUserDelegate& InCallback) {}

	virtual void TdsLogout(TSharedRef<FTdsPlayer> InPlayer) {}

	
	virtual void SetupAntiAddictionModule(TSharedRef<FTdsPlayer> InPlayer, const FSimpleTdsDelegate& InCallback) {}

	virtual void SetupAntiAddictionCallback(const FAntiAddictionDelegate& InCallback) {}

	virtual void GetAntiAddictionPlayerInfo(TSharedRef<FTdsPlayer> OutPlayer) {}
	
	
	virtual void SetupSupportModule(TSharedRef<FTdsPlayer> InPlayer, const TSharedPtr<FJsonObject>& FieldsData, const FSimpleTdsDelegate& InCallback) {}

	virtual void OpenSupportWebPage(const FString& Path = TEXT("/"), const TSharedPtr<FJsonObject>& FieldsData = nullptr) {}

	virtual void SetSupportRedDotCallback(const FBooleanDelegate& InCallback) {}

	
	virtual void OpenMomentPage() {}

	virtual void SetMomentRedDotCallback(const FBooleanDelegate& InCallback) {}


	virtual void SetupAchievement(UTexture2D* Icon, const FText& Title) {}

	virtual void InitAchievementData(TSharedRef<FTdsPlayer> InPlayer, const FSimpleTdsDelegate& InCallback) {}
	
	virtual void AchievementReach(const FString& OpenId){}
	
	virtual void AchievementMakeStep(const FString& OpenId, int32 Step){}

	virtual void AchievementGrowStep(const FString& OpenId, int32 Step){}
	
	virtual void OpenAchievementWidget() {}


	virtual void UpdateLeaderboard(int32 Wave, int32 Kill) {}

	virtual void FetchLeaderboardAround(const TArray<FString>& Names, const FString& ObjectId, int32 Limit, const FLeaderboardDelegate& InCallback) {}

	virtual void FetchLeaderboardSkip(const TArray<FString>& Names, int32 Skip, int32 Limit, const FLeaderboardDelegate& InCallback) {}

	virtual void FetchLeaderboardDetail(const TArray<FString>& Names, const FString& ObjectId, const FLeaderboardDetailDelegate& InCallback) {}
};

extern ACTIONRPG_API ITdsInterface* GTdsInterface;

