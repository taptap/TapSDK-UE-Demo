// Fill out your copyright notice in the Description page of Project Settings.


#include "TapAchievementCommon.h"

#include "TUSettings.h"
#include "UI/AchievementUI.h"
#include "IImageWrapper.h"
#include "UI/ReachAchievementTipsUI.h"
#include "TapAchievementRest.h"
#include "TapBootstrap.h"
#include "TapCommon.h"
#include "TapNet.h"
#include "TDSUser.h"
#include "TUCrypto.h"
#include "TUError.h"
#if PLATFORM_IOS || PLATFORM_ANDROID
#include "TUDeviceInfo.h"
#endif

FTapAchievementCommon::FTapAchievementCommon()
	: AchievementUI()
	, ReachTipsUI()
#if PLATFORM_IOS || PLATFORM_ANDROID
	, AchievementWidgetClass_H_P(FString(TEXT("WidgetBlueprint'/TapAchievement/Phone/AchievementWidget_H_Phone.AchievementWidget_H_Phone_C'")))
	, AchievementWidgetClass_V_P(FString(TEXT("WidgetBlueprint'/TapAchievement/Phone/AchievementWidget_V_Phone.AchievementWidget_V_Phone_C'")))
	, AchievementWidgetClass_H_T(FString(TEXT("WidgetBlueprint'/TapAchievement/Tablet/AchievementWidget_H_Tablet.AchievementWidget_H_Tablet_C'")))
	, AchievementWidgetClass_V_T(FString(TEXT("WidgetBlueprint'/TapAchievement/Tablet/AchievementWidget_V_Tablet.AchievementWidget_V_Tablet_C'")))
	, ReachTipsClass(FString(TEXT("WidgetBlueprint'/TapAchievement/Phone/ReachTips_Phone.ReachTips_Phone_C'")))
#else
	, AchievementWidgetClass_PC(FString(TEXT("WidgetBlueprint'/TapAchievement/PC/AchievementWidget_PC.AchievementWidget_PC_C'")))
	, ReachTipsClass(FString(TEXT("WidgetBlueprint'/TapAchievement/PC/ReachTips_PC.ReachTips_PC_C'")))
#endif
	, DownloadCounter()
	, ApplicationIconTexture()
{
	
}

TSharedPtr<FTUError> FTapAchievementCommon::CheckConfig() const
{
	check(FTUConfig::Get());
	if (FTDSUser::GetCurrentUser() == nullptr)
	{
		return MakeShared<FTUError>(-1, TEXT("Bootstrap must be logged in."));
	}
	if (FTUConfig::Get()->ClientToken.IsEmpty())
	{
		return MakeShared<FTUError>(-1, TEXT("Bootstrap init, Need client token."));
	}
	return nullptr;
}

void FTapAchievementCommon::InitData(const FSimpleDelegate& OnSuccess, const FTUError::FDelegate& OnFailed)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnFailed.ExecuteIfBound(*Error);
		return;
	}

	if (!FTapBootstrap::OnUserLogout.IsBoundToObject(this))
	{
		FTapBootstrap::OnUserLogout.AddThreadSafeSP(this, &FTapAchievementCommon::HandleUserLogout);
	}
	
	FTapAchievementRest::FetchAllAchievementList(
		FTDSUser::GetCurrentUser()->GetSessionToken(),
		FAllAchievementsResult::CreateThreadSafeSP(this, &FTapAchievementCommon::OnInitData_FetchAllAchievementsCallback, OnSuccess, OnFailed));
}

void FTapAchievementCommon::SetShowToast(bool bNewShow)
{
	bShowToast = bNewShow;
}

void FTapAchievementCommon::FetchAllAchievementList(const FTapAchievementDescriptionResult& OnSuccess, const FTUError::FDelegate& OnFailed)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnFailed.ExecuteIfBound(*Error);
	}
	else
	{
		if (const TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
		{
			FTapAchievementRest::FetchAllAchievementList(
				User->GetSessionToken(),
				FAllAchievementsResult::CreateThreadSafeSP(this, &FTapAchievementCommon::OnFetchAllAchievementListFinished, OnSuccess, OnFailed));
		}
	}
}

void FTapAchievementCommon::FetchUserAchievementList(const FTapAchievementProgressResult& OnSuccess, const FTUError::FDelegate& OnFailed)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnFailed.ExecuteIfBound(*Error);
	}
	else
	{
		if (const TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
		{
			FTapAchievementRest::FetchUserAchievementList(
				User->GetSessionToken(),
				User->GetObjectId(),
				FUserAchievementResult::CreateThreadSafeSP(this, &FTapAchievementCommon::OnFetchUserAchievementListFinished, OnSuccess, OnFailed));
		}
	}
}

const TArray<FAchievementDescTap>& FTapAchievementCommon::GetLocalAllAchievementList()
{
	return AchievementDescriptions;
}

const TArray<FAchievementTap>& FTapAchievementCommon::GetLocalUserAchievementList()
{
	return Achievements;
}

void FTapAchievementCommon::Reach(const FString& AchievementOpenId)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, Error);
		return;
	}

	if (const FAchievementDescTap* DescP = FindAchievementDescriptionById(AchievementOpenId))
	{
		FAchievementTap* AchievementP = FindAchievementById(AchievementOpenId);
		
		FAchievementTap PendingAchievement = AchievementP ? *AchievementP : FAchievementTap(*DescP);

		if (PendingAchievement.UpdateStep(*DescP, DescP->CountStep))
		{
			if (const TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
			{
				FTapAchievementRest::WriteAchievement(
					User->GetSessionToken(),
					User->GetObjectId(),
					{PendingAchievement},
					FWriteAchievementResult::CreateThreadSafeSP(this, &FTapAchievementCommon::OnWriteAchievementFinished, PendingAchievement));
			}
		}
		else
		{
			OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, MakeShared<FTUError>(-1, TEXT("Achievement nothing changed.")));
		}
	}
	else
	{
		OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, MakeShared<FTUError>(-1, TEXT("Can not find achievement description.")));
	}
}

void FTapAchievementCommon::GrowSteps(const FString& AchievementOpenId, int32 Steps)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, Error);
		return;
	}
	
	if (const FAchievementDescTap* DescP = FindAchievementDescriptionById(AchievementOpenId))
	{
		FAchievementTap* AchievementP = FindAchievementById(AchievementOpenId);

		FAchievementTap PendingAchievement = AchievementP ? *AchievementP : FAchievementTap(*DescP);

		if (PendingAchievement.UpdateStep(*DescP, PendingAchievement.CompleteStep + Steps))
		{
			if (const TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
			{
				FTapAchievementRest::WriteAchievement(
					User->GetSessionToken(),
					User->GetObjectId(),
					{PendingAchievement},
					FWriteAchievementResult::CreateThreadSafeSP(this, &FTapAchievementCommon::OnWriteAchievementFinished, PendingAchievement));
			}
		}
		else
		{
			OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, MakeShared<FTUError>(-1, TEXT("Achievement nothing changed.")));
		}
	}
	else
	{
		OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, MakeShared<FTUError>(-1, TEXT("Can not find achievement description.")));
	}
}

void FTapAchievementCommon::MakeSteps(const FString& AchievementOpenId, int32 Steps)
{
	if (TSharedPtr<FTUError> Error = CheckConfig())
	{
		OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, Error);
		return;
	}
	
	if (const FAchievementDescTap* DescP = FindAchievementDescriptionById(AchievementOpenId))
	{
		FAchievementTap* AchievementP = FindAchievementById(AchievementOpenId);

		FAchievementTap PendingAchievement = AchievementP ? *AchievementP : FAchievementTap(*DescP);

		if (PendingAchievement.UpdateStep(*DescP, Steps))
		{
			if (const TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
			{
				FTapAchievementRest::WriteAchievement(
					User->GetSessionToken(),
					User->GetObjectId(),
					{PendingAchievement},
					FWriteAchievementResult::CreateThreadSafeSP(this, &FTapAchievementCommon::OnWriteAchievementFinished, PendingAchievement));
			}
		}
		else
		{
			OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, MakeShared<FTUError>(-1, TEXT("Achievement nothing changed.")));
		}
	}
	else
	{
		OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, MakeShared<FTUError>(-1, TEXT("Can not find achievement description.")));
	}
}

bool FTapAchievementCommon::ShowAchievementsUI()
{
	TSubclassOf<UAchievementUI> Class = GetAchievementWidgetClass();
	if (!Class)
	{
		UE_LOG(LogTap, Warning, TEXT("Show achievement ui, achievement ui class is null."));
		return false;
	}
	if (TUSettings::GetGameInstance().IsValid())
	{
		AchievementUI = CreateWidget<UAchievementUI>(TUSettings::GetGameInstance().Get(), Class);
#if PLATFORM_IOS || PLATFORM_ANDROID
		if (!OrientationChangeHandle.IsValid())
		{
			OrientationChangeHandle = FCoreDelegates::ApplicationReceivedScreenOrientationChangedNotificationDelegate.AddThreadSafeSP(this, &FTapAchievementCommon::OnDeviceOrientationChanged);
		}
#endif
		if (IsValid(AchievementUI))
		{
			AchievementUI->AddToViewport(TUSettings::GetUILevel());
			return true;
		}
	}
	return false;
}

int32 FTapAchievementCommon::AsyncDownloadAllAchievementIcon(const FDownloadIconStatus& StatusChanged)
{
	if (DownloadCallback.IsBound())
	{
		return 0;
	}
	DownloadCounter = 0;
	for (const FAchievementDescTap& Desc : AchievementDescriptions)
	{
		if (Desc.AchievementIconUrl.IsEmpty())
		{
			UE_LOG(LogTap, Warning, TEXT("Down load icon with url null."));
		}
		else
		{
			FTapCommonModule::AsyncDownloadImage(
				Desc.AchievementIconUrl,
				FAsyncDownloadImage::CreateThreadSafeSP(this, &FTapAchievementCommon::HandleDownloadIcon, Desc.DisplayId));
			++DownloadCounter;
		}
	}
	
	if (DownloadCounter != 0)
	{
		DownloadCallback = StatusChanged;
	}
	return DownloadCounter;
}

FAchievementTap* FTapAchievementCommon::FindAchievementById(const FString& DisplayId)
{
	return Achievements.FindByPredicate([DisplayId](const FAchievementTap& Data) { return Data.DisplayId == DisplayId; });
}

FAchievementDescTap* FTapAchievementCommon::FindAchievementDescriptionById(const FString& DisplayId)
{
	return AchievementDescriptions.FindByPredicate([DisplayId](const FAchievementTap& Data) { return Data.DisplayId == DisplayId; });
}

void FTapAchievementCommon::SyncLoadClass()
{
	ReachTipsClass.LoadSynchronous();
#if PLATFORM_IOS || PLATFORM_ANDROID
	AchievementWidgetClass_H_P.LoadSynchronous();
	AchievementWidgetClass_V_P.LoadSynchronous();
	AchievementWidgetClass_H_T.LoadSynchronous();
	AchievementWidgetClass_V_T.LoadSynchronous();
#else
	AchievementWidgetClass_PC.LoadSynchronous();
#endif
}

bool FTapAchievementCommon::LoadAllAchievement(const FString& InKey)
{
	if (InKey.IsEmpty())
	{
		return false;
	}
	const FString FilePath = FPaths::SandboxesDir() + TEXT("/DataStorage/cache");
	if (FPaths::FileExists(FilePath))
	{
		TArray<uint8> Data;
		if (FFileHelper::LoadFileToArray(Data, *FilePath))
		{
			const FString JsonStr = TUCrypto::UTF8Encode(TUCrypto::AesDecode(Data, TUCrypto::UTF8Encode(InKey)));
			AchievementDescriptions = TUJsonHelper::GetUStructArray<FAchievementDescTap>(JsonStr);
			return true;
		}
	}
	return false;
}

bool FTapAchievementCommon::LoadUserAchievement(const FString& ObjectId, const FString& InKey)
{
	if (InKey.IsEmpty())
	{
		return false;
	}
	const FString FilePath = FPaths::SandboxesDir() + TEXT("/DataStorage/cache_") + ObjectId;
	if (FPaths::FileExists(FilePath))
	{
		TArray<uint8> Data;
		if (FFileHelper::LoadFileToArray(Data, *FilePath))
		{
			const FString JsonStr = TUCrypto::UTF8Encode(TUCrypto::AesDecode(Data, TUCrypto::UTF8Encode(InKey)));
			Achievements = TUJsonHelper::GetUStructArray<FAchievementTap>(JsonStr);
			return true;
		}
	}
	return false;
}

bool FTapAchievementCommon::SaveAllAchievement(const FString& InKey)
{
	if (InKey.IsEmpty())
	{
		return false;
	}
	FString Str = TUJsonHelper::GetJsonString(AchievementDescriptions);
	TArray<uint8> Data = TUCrypto::AesEncode(TUCrypto::UTF8Encode(Str), TUCrypto::UTF8Encode(InKey));
	const FString FilePath = FPaths::SandboxesDir() + TEXT("/DataStorage/cache");
	return FFileHelper::SaveArrayToFile(Data, *FilePath);
}

bool FTapAchievementCommon::SaveUserAchievement(const FString& ObjectId, const FString& InKey)
{
	if (InKey.IsEmpty())
	{
		return false;
	}
	FString Str = TUJsonHelper::GetJsonString(Achievements);
	TArray<uint8> Data = TUCrypto::AesEncode(TUCrypto::UTF8Encode(Str), TUCrypto::UTF8Encode(InKey));
	const FString FilePath = FPaths::SandboxesDir() + TEXT("/DataStorage/cache_") + ObjectId;
	return FFileHelper::SaveArrayToFile(Data, *FilePath);
}

void FTapAchievementCommon::OnInitData_FetchAllAchievementsCallback(const TSharedPtr<FAchievementDescListData>& Result, const TSharedPtr<FTUError>& Error, FSimpleDelegate OnSuccess, FTUError::FDelegate OnFailed)
{
	if (Error)
	{
		OnFailed.ExecuteIfBound(*Error);
	}
	else
	{
		if (TSharedPtr<FTUError> ErrorConfig = CheckConfig())
		{
			OnFailed.ExecuteIfBound(*ErrorConfig);
		}
		else
		{
			AchievementDescriptions = MoveTemp(Result->List);
			if (const TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
			{
				FTapAchievementRest::FetchUserAchievementList(User->GetSessionToken(), User->GetObjectId(), FUserAchievementResult::CreateThreadSafeSP(this, &FTapAchievementCommon::OnInitData_FetchUserAchievementCallback, OnSuccess, OnFailed));
			}
		}
	}
}

void FTapAchievementCommon::OnInitData_FetchUserAchievementCallback(const TSharedPtr<FAchievementListData>& Result, const TSharedPtr<FTUError>& Error, FSimpleDelegate OnSuccess, FTUError::FDelegate OnFailed)
{
	if (Error)
	{
		OnFailed.ExecuteIfBound(*Error);
	}
	else
	{
		Achievements = MoveTemp(Result->List);
		OnSuccess.ExecuteIfBound();
	}
}

void FTapAchievementCommon::OnFetchAllAchievementListFinished(const TSharedPtr<FAchievementDescListData>& Result, const TSharedPtr<FTUError>& Error, FTapAchievementDescriptionResult OnSuccess, FTUError::FDelegate OnFailed)
{
	if (Result)
	{
		AchievementDescriptions = MoveTemp(Result->List);
		if (TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
		{
			SaveAllAchievement(User->GetObjectId());
		}
		OnSuccess.ExecuteIfBound(AchievementDescriptions);
	}
	else if(Error)
	{
		OnFailed.ExecuteIfBound(*Error);
	}
	else
	{
		ensure(false);
		OnFailed.ExecuteIfBound(FTUError(-1, FString::Printf(TEXT("Unkown error. %s"), ANSI_TO_TCHAR(__FUNCTION__))));
	}
}

void FTapAchievementCommon::OnFetchUserAchievementListFinished(const TSharedPtr<FAchievementListData>& Result, const TSharedPtr<FTUError>& Error, FTapAchievementProgressResult OnSuccess, FTUError::FDelegate OnFailed)
{
	if (Result)
	{
		if (TSharedPtr<FTUError> ConfigError = CheckConfig())
		{
			OnFailed.ExecuteIfBound(*ConfigError);
			return;
		}

		TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser();
		check(User);//上面判断过了
		
		FString ObjId = User->GetObjectId();
		LoadUserAchievement(ObjId, ObjId);

		TArray<FAchievementTap> PendingUpload;
		TArray<FAchievementTap> RemoteList = Result->List;
		for (FAchievementTap& Remote : RemoteList)
		{
			if (FAchievementTap* LocalP = FindAchievementById(Remote.DisplayId))
			{
				if (Remote.CompleteStep < LocalP->CompleteStep)
				{
					PendingUpload.Add(*LocalP);
					Remote = *LocalP;
				}
			}
		}
		
		if (PendingUpload.Num() > 0)
		{
			FTapAchievementRest::WriteAchievement(User->GetSessionToken(), ObjId, PendingUpload, FWriteAchievementResult());
		}
		Achievements = MoveTemp(RemoteList);

		SaveUserAchievement(ObjId, ObjId);
		
		OnSuccess.ExecuteIfBound(Achievements);
	}
	else if(Error)
	{
		OnFailed.ExecuteIfBound(*Error);
	}
	else
	{
		ensure(false);
		OnFailed.ExecuteIfBound(FTUError(-1, FString::Printf(TEXT("Unkown error. %s"), ANSI_TO_TCHAR(__FUNCTION__))));
	}
}

void FTapAchievementCommon::OnWriteAchievementFinished(const TSharedPtr<FAchievementPlatinumData>& Result, const TSharedPtr<FTUError>& Error, FAchievementTap PendingAchievement)
{
	if (Result)
	{
		if (FAchievementDescTap* DescP = FindAchievementDescriptionById(PendingAchievement.DisplayId))
		{
			FAchievementTap* CurrentAchievementP = FindAchievementById(PendingAchievement.DisplayId);
			if (!CurrentAchievementP)
			{
				CurrentAchievementP = &Achievements.Add_GetRef(FAchievementTap(*DescP));
			}
			
			if (PendingAchievement.bFullCompleted && bShowToast && !CurrentAchievementP->bFullCompleted)
			{
				ShowReachAchievementTips(PendingAchievement.DisplayId);
			}

			*CurrentAchievementP = PendingAchievement;
			
			OnAchievementStatusUpdate.Broadcast(DescP, CurrentAchievementP, nullptr);
		}
		
		if (Result->Platinum)
		{
			if (FAchievementDescTap* DescP = FindAchievementDescriptionById(Result->Platinum->DisplayId))
			{
				FAchievementTap* CurrentAchievementP = FindAchievementById(Result->Platinum->DisplayId);
				if (!CurrentAchievementP)
				{
					CurrentAchievementP = &Achievements.Add_GetRef(FAchievementTap(*DescP));
				}
				
				if (Result->Platinum->bFullCompleted && bShowToast && !CurrentAchievementP->bFullCompleted)
				{
					ShowReachAchievementTips(Result->Platinum->DisplayId);
				}

				*CurrentAchievementP = *Result->Platinum;
				
				OnAchievementStatusUpdate.Broadcast(DescP, CurrentAchievementP, nullptr);
			}
		}
		if (TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
		{
			SaveUserAchievement(User->GetObjectId(), User->GetObjectId());
		}
	}
	else if(Error)
	{
		OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, Error);
	}
	else
	{
		ensure(false);
		OnAchievementStatusUpdate.Broadcast(nullptr, nullptr, MakeShared<FTUError>(-1, FString::Printf(TEXT("Unkown error. %s"), ANSI_TO_TCHAR(__FUNCTION__))));
	}
}

void FTapAchievementCommon::ShowReachAchievementTips(const FString& AchievementOpenId)
{
	if (!IsValid(ReachTipsUI))
	{
		if (!ReachTipsClass)
		{
			UE_LOG(LogTap, Warning, TEXT("Show reach achievement tips, reach tips widget class is null."));
			return;
		}
		if (TUSettings::GetGameInstance().IsValid())
		{
			ReachTipsUI = CreateWidget<UReachAchievementTipsUI>(TUSettings::GetGameInstance().Get(), ReachTipsClass.Get());
		}
	}

	if (IsValid(ReachTipsUI))
	{
		if (!ReachTipsUI->IsInViewport())
		{
			ReachTipsUI->AddToViewport(TUSettings::GetUILevel() + 100);
		}
		ReachTipsUI->ShowReachAchievement(AchievementOpenId);
	}
}

void FTapAchievementCommon::HandleDownloadIcon(UTexture2D* IconTexture, FString DisplayId)
{
	--DownloadCounter;
	if (FAchievementDescTap* DescP = FindAchievementDescriptionById(DisplayId))
	{
		DescP->IconTexture = IconTexture;
	}
	DownloadCallback.ExecuteIfBound(DisplayId, DownloadCounter);
	if (DownloadCounter == 0)
	{
		DownloadCallback.Unbind();
	}
}

TSubclassOf<UAchievementUI> FTapAchievementCommon::GetAchievementWidgetClass() const
{
#if PLATFORM_WINDOWS || PLATFORM_MAC
	return AchievementWidgetClass_PC.Get();
#else
	EDeviceScreenOrientation Orientation = FPlatformMisc::GetDeviceOrientation();
	if (TUDeviceInfo::IsSmartPhone())
	{
		if (Orientation == EDeviceScreenOrientation::Portrait || Orientation == EDeviceScreenOrientation::PortraitUpsideDown)
		{
			return AchievementWidgetClass_V_P.Get();
		}
		else
		{
			return AchievementWidgetClass_H_P.Get();
		}
	}
	else
	{
		if (Orientation == EDeviceScreenOrientation::Portrait || Orientation == EDeviceScreenOrientation::PortraitUpsideDown)
		{
			return AchievementWidgetClass_V_T.Get();
		}
		else
		{
			return AchievementWidgetClass_H_T.Get();
		}
	}
#endif
}

void FTapAchievementCommon::OnDeviceOrientationChanged(int32 Orientation)
{
	if (!AchievementUI || !AchievementUI->IsInViewport())
	{
		return;
	}
	
	float Offset = AchievementUI->GetOffset();
	AchievementUI->RemoveFromParent();
	AchievementUI = nullptr;

	TSubclassOf<UAchievementUI> Class = GetAchievementWidgetClass();
	if (!Class)
	{
		UE_LOG(LogTap, Warning, TEXT("Show achievement ui, achievement ui class is null."));
	}
	if (TUSettings::GetGameInstance().IsValid())
	{
		AchievementUI = CreateWidget<UAchievementUI>(TUSettings::GetGameInstance().Get(), Class);
		if (IsValid(AchievementUI))
		{
			AchievementUI->SetOffset(Offset);
			AchievementUI->AddToViewport(TUSettings::GetUILevel());
		}
	}
}

void FTapAchievementCommon::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(AchievementUI);
	Collector.AddReferencedObject(ReachTipsUI);
	Collector.AddReferencedObject(ApplicationIconTexture);
	for (FAchievementDescTap& Desc : AchievementDescriptions)
	{
		Collector.AddReferencedObject(Desc.IconTexture);
	}
}

void FTapAchievementCommon::HandleUserLogout()
{
	Achievements.Reset();
	AchievementDescriptions.Reset();
	DownloadCallback.Unbind();
}
