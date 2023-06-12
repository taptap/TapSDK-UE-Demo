// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TapAchievementTypes.h"
#include "TUError.h"

struct FAchievementPlatinumData;
struct FAchievementListData;
struct FAchievementDescListData;
class UAchievementUI;
class UReachAchievementTipsUI;
struct FTUError;

DECLARE_DELEGATE_OneParam(FTapAchievementProgressResult, const TArray<FAchievementTap>&);
DECLARE_DELEGATE_OneParam(FTapAchievementDescriptionResult, const TArray<FAchievementDescTap>&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FTapAchievementStatusUpdate, const FAchievementDescTap*, const FAchievementTap*, const TSharedPtr<FTUError>&);
DECLARE_DELEGATE_TwoParams(FDownloadIconStatus, const FString&/** DisplayId */, int32/** LiftCount */);

/**
 * TapAchievement Interface class
 */
class TAPACHIEVEMENT_API FTapAchievementCommon : public FGCObject, public TSharedFromThis<FTapAchievementCommon, ESPMode::ThreadSafe>
{
public:
	FTapAchievementCommon();

	/**
	 * Check the achievement module is available
	 */
	TSharedPtr<FTUError> CheckConfig() const;
	
	/**
	 *	Fetch all achievement information
	 */
	void InitData(const FSimpleDelegate& OnSuccess, const FTUError::FDelegate& OnFailed);

	/**
	 *	Set show achievement reach tip
	 *	@param bNewShow show or not
	 */
	void SetShowToast(bool bNewShow);

	/**
	 *	Fetch application all achievement descriptions information, include not reached achievement
	 */	
	void FetchAllAchievementList(const FTapAchievementDescriptionResult& OnSuccess, const FTUError::FDelegate& OnFailed);
	
	/**
	 *	Fetch user's all achievement progress, exclude 0 step achievement
	 */	
	void FetchUserAchievementList(const FTapAchievementProgressResult& OnSuccess, const FTUError::FDelegate& OnFailed);

	/**
	 *	Get cached achievement descriptions in memory
	 *	@return All achievement descriptions;
	 */	
	const TArray<FAchievementDescTap>& GetLocalAllAchievementList();

	/**
	 *	Get cached achievement progress in memory
	 *	@return All achievement progress;
	 */	
	const TArray<FAchievementTap>& GetLocalUserAchievementList();

	/**
	 *	Set specify achievement full complete, then save to local and upload to server
	 *	@param AchievementOpenId The achievement open id
	 */	
	void Reach(const FString& AchievementOpenId);

	/**
	 *	Set specify achievement grow steps, then save to local and upload to server
	 *	@param AchievementOpenId The achievement open id
	 *	@param Steps Means CurrentSteps = FMath::Clamp(CurrentSteps + Steps, 0, MaxSteps)
	 */	
	void GrowSteps(const FString& AchievementOpenId, int32 Steps);

	/**
	 *	Set specify achievement steps, then save to local and upload to server
	 *	@param AchievementOpenId The achievement open id
	 *	@param Steps Means CurrentSteps = FMath::Clamp(Steps, 0, MaxSteps)
	 */	
	void MakeSteps(const FString& AchievementOpenId, int32 Steps);

	/**
	 *	Show TapTap achievement widget with cached achievement data 
	 */	
	bool ShowAchievementsUI();

	/**
	 * Platinum achievement changed delegate
	 */
	FTapAchievementStatusUpdate OnAchievementStatusUpdate;

	/**
	 * Application name display in achievement detail widget
	 */
	FORCEINLINE void SetApplicationName(const FText& Name) { ApplicationName = Name; }

	/**
	 * Application name display in achievement detail widget
	 */
	FORCEINLINE const FText& GetApplicationName() const { return ApplicationName; }

	/**
	 * Application icon texture display in achievement detail widget
	 */
	FORCEINLINE void SetApplicationIcon(UTexture2D* IconTexture) { ApplicationIconTexture = IconTexture; }

	/**
	 * Application icon texture display in achievement detail widget
	 */
	FORCEINLINE UTexture2D* GetApplicationIcon() const { return ApplicationIconTexture; }

	int32 AsyncDownloadAllAchievementIcon(const FDownloadIconStatus& StatusChanged);

	FAchievementTap* FindAchievementById(const FString& DisplayId);

	FAchievementDescTap* FindAchievementDescriptionById(const FString& DisplayId);

	void SyncLoadClass();

	bool LoadAllAchievement(const FString& InKey);

	bool LoadUserAchievement(const FString& ObjectId, const FString& InKey);

	bool SaveAllAchievement(const FString& InKey);

	bool SaveUserAchievement(const FString& ObjectId, const FString& InKey);

protected:
	void OnInitData_FetchAllAchievementsCallback(const TSharedPtr<FAchievementDescListData>& Result, const TSharedPtr<FTUError>& Error, FSimpleDelegate OnSuccess, FTUError::FDelegate OnFailed);

	void OnInitData_FetchUserAchievementCallback(const TSharedPtr<FAchievementListData>& Result, const TSharedPtr<FTUError>& Error, FSimpleDelegate OnSuccess, FTUError::FDelegate OnFailed);
	
	void OnFetchAllAchievementListFinished(const TSharedPtr<FAchievementDescListData>& Result, const TSharedPtr<FTUError>& Error, FTapAchievementDescriptionResult OnSuccess, FTUError::FDelegate OnFailed);

	void OnFetchUserAchievementListFinished(const TSharedPtr<FAchievementListData>& Result, const TSharedPtr<FTUError>& Error, FTapAchievementProgressResult OnSuccess, FTUError::FDelegate OnFailed);
	
	void OnWriteAchievementFinished(const TSharedPtr<FAchievementPlatinumData>& Result, const TSharedPtr<FTUError>& Error, FAchievementTap PendingAchievement);
	
	void ShowReachAchievementTips(const FString& AchievementOpenId);

	void HandleDownloadIcon(UTexture2D* IconTexture, FString DisplayId);

	TSubclassOf<UAchievementUI> GetAchievementWidgetClass() const;

	void OnDeviceOrientationChanged(int32 Orientation);
	
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual FString GetReferencerName() const override { return TEXT("TapAchievementInterface"); }

	void HandleUserLogout();

	bool bShowToast = true;

	TArray<FAchievementTap> Achievements;
	TArray<FAchievementDescTap> AchievementDescriptions;

	UAchievementUI* AchievementUI;
	UReachAchievementTipsUI* ReachTipsUI;

	TSoftClassPtr<UAchievementUI> AchievementWidgetClass_PC;
	TSoftClassPtr<UAchievementUI> AchievementWidgetClass_H_P;
	TSoftClassPtr<UAchievementUI> AchievementWidgetClass_V_P;
	TSoftClassPtr<UAchievementUI> AchievementWidgetClass_H_T;
	TSoftClassPtr<UAchievementUI> AchievementWidgetClass_V_T;
	TSoftClassPtr<UReachAchievementTipsUI> ReachTipsClass;

	int32 DownloadCounter;
	FDownloadIconStatus DownloadCallback;

	FText ApplicationName;
	UTexture2D* ApplicationIconTexture;

	FDelegateHandle OrientationChangeHandle;
};

 

