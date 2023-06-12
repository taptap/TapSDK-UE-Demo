// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AntiAddictionUE.h"
#include "TapAchievementModule.h"
#include "TapBillboardModule.h"
#include "ActionRPG/Interface/TdsInterface.h"


class FLCError;
struct FLCLeaderboardStatistic;
struct FLCLeaderboardRanking;
struct FBadgeDetails;

class SDKUSAGE_API FTdsImpl : public ITdsInterface, public TSharedFromThis<FTdsImpl>
{
public:
	FTdsImpl();

	virtual ~FTdsImpl() override;

	
	virtual void InitSdk() override;


	virtual void OpenSplashAnnouncement(const FSimpleTdsDelegate& InCallback, const FSimpleDelegate& OnClosed) override;

	virtual void CloseSplashAnnouncement() override;

	virtual void HasNavigateAnnouncement(const FBooleanDelegate& InCallback) override;
	
	virtual void OpenNavigateAnnouncement(const FSimpleTdsDelegate& InCallback, const FSimpleDelegate& OnClosed) override;

	virtual void SetAnnouncementRedDotCallback(const FBooleanDelegate& InCallback) override;

	virtual void StartupMarquee(bool bNewStart) override;
	

	virtual void TdsLoginWithStorage(const FTdsUserDelegate& InCallback) override;
	
	virtual void TdsLoginAnonymously(const FTdsUserDelegate& InCallback) override;
	
	virtual void TdsLoginWithTapTap(const FTdsUserDelegate& InCallback) override;

	virtual void TdsLogout(TSharedRef<FTdsPlayer> InPlayer) override;

	
	virtual void SetupAntiAddictionModule(TSharedRef<FTdsPlayer> InPlayer, const FSimpleTdsDelegate& InCallback) override;

	virtual void SetupAntiAddictionCallback(const FAntiAddictionDelegate& InCallback) override;

	virtual void GetAntiAddictionPlayerInfo(TSharedRef<FTdsPlayer> OutPlayer) override;
	

	virtual void SetupSupportModule(TSharedRef<FTdsPlayer> InPlayer, const TSharedPtr<FJsonObject>& FieldsData, const FSimpleTdsDelegate& InCallback) override;
	
	virtual void OpenSupportWebPage(const FString& Path, const TSharedPtr<FJsonObject>& FieldsData) override;

	virtual void SetSupportRedDotCallback(const FBooleanDelegate& InCallback) override;


	virtual void OpenMomentPage() override;

	virtual void SetMomentRedDotCallback(const FBooleanDelegate& InCallback) override;
	

	virtual void SetupAchievement(UTexture2D* Icon, const FText& Title) override;

	virtual void InitAchievementData(TSharedRef<FTdsPlayer> InPlayer, const FSimpleTdsDelegate& InCallback) override;
	
	virtual void AchievementReach(const FString& OpenId) override;
	
	virtual void AchievementMakeStep(const FString& OpenId, int32 Step) override;
	
	virtual void AchievementGrowStep(const FString& OpenId, int32 Step) override;
	
	virtual void OpenAchievementWidget() override;


	virtual void UpdateLeaderboard(int32 Wave, int32 Kill) override;

	virtual void FetchLeaderboardAround(const TArray<FString>& Names, const FString& ObjectId, int32 Limit, const FLeaderboardDelegate& InCallback) override;

	virtual void FetchLeaderboardSkip(const TArray<FString>& Names, int32 Skip, int32 Limit, const FLeaderboardDelegate& InCallback) override;

	virtual void FetchLeaderboardDetail(const TArray<FString>& Names, const FString& ObjectId, const FLeaderboardDetailDelegate& InCallback) override;
	
protected:
	void BadgeDetailsCallback(const FBadgeDetails& Callback, bool bForce);

	bool UpdateMomentRedDot(float DeltaSecond);

	void UpdateMomentRedDotCallback(int Code, const FString& Msg);

	void HandleAntiAddictionCallback(AntiAddictionUE::ResultHandlerCode Code, const FString& Message);

	void OnFetchLeaderboardCallback(TArray<FLCLeaderboardRanking> Rankings, int64 Count, FLeaderboardDelegate Callback);

	void OnFetchLeaderboardFailed(const FLCError& Error, FLeaderboardDelegate Callback);

	void OnFetchLeaderboardDetailCallback(TArray<FLCLeaderboardStatistic> InStatistics, FLeaderboardDetailDelegate Callback);

	void OnFetchLeaderboardDetailFailed(const FLCError& Error, FLeaderboardDetailDelegate Callback);
	
	void UpdateSdkLanguage();

	ELanguageType FindLanguageString(const FString& String) const;

	TMap<ELanguageType, FString> LanguageMap;
	
	FTapBillboardPtr Billboard;
	FTapAchievementsPtr AchievementsPtr;
	
	bool bLastAnnouncementRedDot = false;
	bool bAchievementInitialized = false;
	FBooleanDelegate AnnouncementRedDotChangedCallback;

	FBooleanDelegate MomentRedDotChangedCallback;
	FDelegateHandle MomentHandle;
	FDelegateHandle CultureHandle;

	FAntiAddictionDelegate AntiAddictionCallback;
};
