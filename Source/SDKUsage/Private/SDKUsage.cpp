// Copyright Epic Games, Inc. All Rights Reserved.

#include "SDKUsage.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#include "TdsConfig.h"
#include "AAUType.h"
#include "AntiAddictionUE.h"
#include "LCLeaderboard.h"
#include "LeanCloud.h"
#include "TapAchievementCommon.h"
#include "TapBootstrap.h"
#include "TapUESupport.h"
#include "TUSupportType.h"
#include "TapBillboardCommon.h"
#include "TapCommon.h"
#include "TapMoment.h"
#include "TDSUser.h"
#include "TUJsonHelper.h"
#include "TULanguage.h"
#include "TUSettings.h"

TSharedPtr<FTdsError> MakeError(const FTUError& InError)
{
	TSharedPtr<FTdsError> Error = MakeShared<FTdsError>();
	Error->Code = InError.code;
	Error->Msg = InError.msg;
	Error->Description = InError.error_description;
	return Error;
}

TSharedPtr<FTdsError> MakeError(const FLCError& InError)
{
	TSharedPtr<FTdsError> Error = MakeShared<FTdsError>();
	Error->Code = InError.Code;
	Error->Msg = InError.Message;
	Error->Description = InError.Message;
	return Error;
}

TSharedPtr<FTdsError> MakeError(const TCHAR* Message)
{
	TSharedPtr<FTdsError> Error = MakeShared<FTdsError>();
	Error->Code = -1;
	Error->Msg = Message;
	Error->Description = Message;
	return Error;
}

TSharedPtr<FTdsPlayer> MakePlayer(const FTDSUser& InUser)
{
	TSharedPtr<FTdsPlayer> Player = MakeShared<FTdsPlayer>();
	Player->Id = InUser.GetObjectId();
	Player->AvatarUrl = InUser.GetAvatar();
	Player->NickName = InUser.GetNickName();
	Player->ShortID = InUser.GetShortID();
	Player->Email = InUser.GetEmail();
	Player->Username = InUser.GetUsername();
	Player->Password = InUser.GetPassword();
	Player->MobilePhoneNumber = InUser.GetMobilePhoneNumber();
	Player->SessionToken = InUser.GetSessionToken();
	Player->bIsAnonymous = InUser.IsAnonymous();
	Player->bIsMobilePhoneVerified = InUser.IsMobilePhoneVerified();
	Player->bIsAuthenticated = InUser.IsAuthenticated();
	return Player;
}

bool IdenticalTo(TSharedRef<FTdsPlayer> Player, const FTDSUser& User)
{
	return Player->Id == User.GetObjectId();
}

void MakeStatistic(FLeaderboardStatistic& OutData, const FLCLeaderboardStatistic& InData)
{
	ParseLeaderboardFullKeyName(InData.Name, OutData.Category, OutData.Type);
	OutData.Value = InData.Value;
}

void MakeRankData(FLeaderboardData& OutData, const FLCLeaderboardRanking& InData)
{
	OutData.RankIndex = InData.Rank;
	if (InData.User.IsValid())
	{
		OutData.ObjectId = InData.User->GetObjectId();
		InData.User->GetServerData()->TryGetStringField(TEXT("nickname"), OutData.NickName);
		InData.User->GetServerData()->TryGetStringField(TEXT("avatar"), OutData.AvatarUrl);
	}
	OutData.Value = InData.Value;
	for (const FLCLeaderboardStatistic& Statistic : InData.Statistics)
	{
		MakeStatistic(OutData.Statistics.AddDefaulted_GetRef(), Statistic);
	}
}

FTdsImpl::FTdsImpl()
{
	Billboard = FTapBillboardModule::GetTapBillboardInterface();
	AchievementsPtr = FTapAchievementModule::GetAchievementInterface();
	LanguageMap = {
		{ELanguageType::EN, TEXT("en")},
		{ELanguageType::ZH, TEXT("zh-CN")}
	};
}

FTdsImpl::~FTdsImpl()
{
}

void FTdsImpl::InitSdk()
{
	const UTdsConfig* Config = GetDefault<UTdsConfig>();

	FTUConfig TapConfig;
	TapConfig.ClientID = Config->ClientId;
	TapConfig.ClientToken = Config->ClientToken;
	TapConfig.RegionType = Config->RegionType;
	TapConfig.ServerURL = Config->ServerURL;
	
	if (Config->bEnableTapDB)
	{
		TapConfig.DBConfig.Enable = Config->bEnableTapDB;
		TapConfig.DBConfig.Channel = Config->Channel;
		TapConfig.DBConfig.GameVersion = Config->GameVersion;
		TapConfig.DBConfig.AdvertiserIDCollectionEnabled = Config->AdvertiserIDCollectionEnabled;
	}

	if (Config->bEnableBillboard)
	{
		TapConfig.BillboardConfig = MakeShared<FBillboardConfig>();
		TapConfig.BillboardConfig->Dimensions = Config->Dimensions;
		TapConfig.BillboardConfig->BillboardUrl = Config->BillboardUrl;
	}

	FTapBootstrap::Init(TapConfig);
	
	Billboard->Init(TapConfig);//@TODO 暂时有bug

	FAAUConfig AntiAddictionConfig;
	AntiAddictionConfig.ClientID = Config->ClientId;
	AntiAddictionUE::Init(AntiAddictionConfig);

	if (Config->bEnableSupport)
	{
		FTapSupportConfig SupportConfig;
		SupportConfig.ServerUrl = Config->SupportUrl;
		SupportConfig.ProductID = Config->ProductId;
		TapUESupport::Init(SupportConfig);
	}
	if (Config->bEnableMoment)
	{
		FTapMomentConfig MomentConfig;
		MomentConfig.RegionType = Config->RegionType;
		MomentConfig.ClientID = Config->ClientId;
		MomentConfig.AppID = Config->AppId;
		FTapMoment::Init(MomentConfig);
	}

	if (!bAchievementInitialized)
	{
		AchievementsPtr->InitData(
			FSimpleDelegate::CreateLambda([this]()
			{
				bAchievementInitialized = true;
			}),
			FTUError::FDelegate());
	}
	
	TUSettings::SetUILevel(TDS_WIDGET_Z_ORDER);
	
	AntiAddictionUE::OnCallBack.BindSP(this, &FTdsImpl::HandleAntiAddictionCallback);

	UpdateSdkLanguage();
	
	if (!CultureHandle.IsValid())
	{
		CultureHandle = FInternationalization::Get().OnCultureChanged().AddSP(this, &FTdsImpl::UpdateSdkLanguage);
	}
}

void FTdsImpl::OpenSplashAnnouncement(const FSimpleTdsDelegate& InCallback, const FSimpleDelegate& OnClosed)
{
	Billboard->OpenSplashPanel(
		FSimpleDelegate::CreateLambda([=]()
		{
			InCallback.ExecuteIfBound(nullptr);
		}),
		FTapFailed::CreateLambda([=](const FTUError& Error)
		{
			TSharedPtr<FTdsError> TdsError = MakeError(Error);
			InCallback.ExecuteIfBound(TdsError);
		}),
		FSimpleDelegate::CreateLambda([=]()
		{
			OnClosed.ExecuteIfBound();
		}));
}

void FTdsImpl::CloseSplashAnnouncement()
{
	Billboard->CloseSplashPanel();
}

void FTdsImpl::HasNavigateAnnouncement(const FBooleanDelegate& InCallback)
{
	Billboard->GetBadgeDetails(
		FTapBadgeDetailsResult::CreateLambda([InCallback](const FBadgeDetails& BadgeDetails)
		{
			InCallback.ExecuteIfBound(BadgeDetails.show_red_dot);
		}),
		FTapFailed::CreateLambda([InCallback](const FTUError& Error)
		{
			InCallback.ExecuteIfBound(false);
		}));
}

void FTdsImpl::OpenNavigateAnnouncement(const FSimpleTdsDelegate& InCallback, const FSimpleDelegate& OnClosed)
{
	Billboard->OpenPanel(
		FSimpleDelegate::CreateLambda([=](){ InCallback.ExecuteIfBound(nullptr); }),
		FTapFailed::CreateLambda([=](const FTUError& Error){ InCallback.ExecuteIfBound(MakeError(Error)); }),
		FSimpleDelegate::CreateLambda([=]()
		{
			OnClosed.ExecuteIfBound();
			Billboard->GetBadgeDetails(
				FTapBadgeDetailsResult::CreateSP(this, &FTdsImpl::BadgeDetailsCallback, false),
				FTapFailed());
		}));
}

void FTdsImpl::SetAnnouncementRedDotCallback(const FBooleanDelegate& InCallback)
{
	AnnouncementRedDotChangedCallback = InCallback;
	if (InCallback.IsBound())
	{
		Billboard->GetBadgeDetails(
			FTapBadgeDetailsResult::CreateSP(this, &FTdsImpl::BadgeDetailsCallback, true),
			FTapFailed());
	}
	else
	{
		AnnouncementRedDotChangedCallback.Unbind();
	}
}

void FTdsImpl::StartupMarquee(bool bNewStart)
{
	if (bNewStart)
	{
		Billboard->StartFetchMarqueeData();
	}
	else
	{
		Billboard->StopFetchMarqueeData(false);
	}
}

void FTdsImpl::TdsLoginWithStorage(const FTdsUserDelegate& InCallback)
{
	if (TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
	{
		FTDSUser::BecomeWithSessionToken(
			User->GetSessionToken(),
			FTDSUser::FDelegate::CreateLambda([InCallback](const FTDSUser& User)
			{
				InCallback.ExecuteIfBound(MakePlayer(User), nullptr);
			}),
			FTUError::FDelegate::CreateLambda([InCallback](const FTUError& Error)
			{
				FTDSUser::Logout();
				InCallback.ExecuteIfBound(nullptr, MakeError(Error));
			}));
	}
	else
	{
		InCallback.ExecuteIfBound(nullptr, MakeError(TEXT("No local storage")));
	}
}

void FTdsImpl::TdsLoginAnonymously(const FTdsUserDelegate& InCallback)
{
	FTDSUser::LoginAnonymously(
		FTDSUser::FDelegate::CreateLambda([=](const FTDSUser& InUser)
		{
			InCallback.ExecuteIfBound(MakePlayer(InUser), nullptr);
		}),
		FTUError::FDelegate::CreateLambda([=](const FTUError& Error)
		{
			InCallback.ExecuteIfBound(nullptr, MakeError(Error));
		}));
}

void FTdsImpl::TdsLoginWithTapTap(const FTdsUserDelegate& InCallback)
{
	FTDSUser::LoginWithTapTap(
		{TUType::PermissionScope::Profile},
		FTDSUser::FDelegate::CreateLambda([=](const FTDSUser& InUser)
		{
			InCallback.ExecuteIfBound(MakePlayer(InUser), nullptr);
		}),
		FTUError::FDelegate::CreateLambda([=](const FTUError& Error)
		{
			InCallback.ExecuteIfBound(nullptr, MakeError(Error));
		}));
}

void FTdsImpl::TdsLogout(TSharedRef<FTdsPlayer> InPlayer)
{
	if (TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
	{
		if (IdenticalTo(InPlayer, *User))
		{
			FTDSUser::Logout();
		}
	}
}

void FTdsImpl::SetupAntiAddictionModule(TSharedRef<FTdsPlayer> InPlayer, const FSimpleTdsDelegate& InCallback)
{
	if (!InPlayer->IsValid())
	{
		InCallback.ExecuteIfBound(MakeError(TEXT("Must Login")));
		return;
	}
	
	AntiAddictionUE::Startup(InPlayer->Id, false);

	InCallback.ExecuteIfBound(nullptr);
}

void FTdsImpl::SetupAntiAddictionCallback(const FAntiAddictionDelegate& InCallback)
{
	AntiAddictionCallback = InCallback;
}

void FTdsImpl::GetAntiAddictionPlayerInfo(TSharedRef<FTdsPlayer> OutPlayer)
{
	OutPlayer->MinAge = static_cast<int32>(AntiAddictionUE::GetAgeRange());
	OutPlayer->RemainingSeconds = AntiAddictionUE::GetRemainingTime();
}

void FTdsImpl::SetupSupportModule(TSharedRef<FTdsPlayer> InPlayer, const TSharedPtr<FJsonObject>& FieldsData, const FSimpleTdsDelegate& InCallback)
{
	if (!InPlayer->IsValid())
	{
		InCallback.ExecuteIfBound(MakeError(TEXT("Must Login")));
		return;
	}
	
	if (TSharedPtr<FTDSUser> User = FTDSUser::GetCurrentUser())
	{
		if (IdenticalTo(InPlayer, *User))
		{
			User->RetrieveShortToken(
				FStringSignature::CreateLambda([=](const FString& Credential)
				{
					TapUESupport::LoginWithTDSCredential(
						Credential,
						FSimpleDelegate::CreateLambda([=]()
						{
							TapUESupport::SetDefaultFieldsData(FieldsData);
							InCallback.ExecuteIfBound(nullptr);
						}),
						FTapFailed::CreateLambda([=](const FTUError& Error)
						{
							InCallback.ExecuteIfBound(MakeError(Error));
						}));
				}),
				FLCError::FDelegate::CreateLambda([=](const FLCError& Error)
				{
					InCallback.ExecuteIfBound(MakeError(Error));
				}));
			return;
		}
	}
	
	InCallback.ExecuteIfBound(MakeError(TEXT("Unknow Error.")));
}

void FTdsImpl::OpenSupportWebPage(const FString& Path, const TSharedPtr<FJsonObject>& FieldsData)
{
	TapUESupport::OpenSupportViewV2(Path, FieldsData);
}

void FTdsImpl::SetSupportRedDotCallback(const FBooleanDelegate& InCallback)
{
	if (InCallback.IsBound())
	{
		TapUESupport::OnUnreadStatusChanged = InCallback;
	}
	else
	{
		TapUESupport::OnUnreadStatusChanged.Unbind();
	}
}

void FTdsImpl::OpenMomentPage()
{
	FTapMoment::Open(ETapMomentOrientation::DEFAULT);
}

void FTdsImpl::SetMomentRedDotCallback(const FBooleanDelegate& InCallback)
{
	if (InCallback.IsBound())
	{
		MomentHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateSP(this, &FTdsImpl::UpdateMomentRedDot), 3.f);
		FTapMoment::SetCallback(FTapMoment::FDelegate::CreateSP(this, &FTdsImpl::UpdateMomentRedDotCallback));
	}
	else
	{
		FTapMoment::SetCallback(FTapMoment::FDelegate());
		FTicker::GetCoreTicker().RemoveTicker(MomentHandle);
	}
}

void FTdsImpl::SetupAchievement(UTexture2D* Icon, const FText& Title)
{
	if (bAchievementInitialized)
	{
		AchievementsPtr->SetApplicationIcon(Icon);
		AchievementsPtr->SetApplicationName(Title);
	}
}

void FTdsImpl::AchievementReach(const FString& OpenId)
{
	if (ensure(bAchievementInitialized))
	{
		AchievementsPtr->Reach(OpenId);
	}
}

void FTdsImpl::AchievementMakeStep(const FString& OpenId, int32 Step)
{
	if (ensure(bAchievementInitialized))
	{
		AchievementsPtr->MakeSteps(OpenId, Step);
	}
}

void FTdsImpl::AchievementGrowStep(const FString& OpenId, int32 Step)
{
	if (ensure(bAchievementInitialized))
	{
		AchievementsPtr->GrowSteps(OpenId, Step);
	}
}

void FTdsImpl::OpenAchievementWidget()
{
	if (ensure(bAchievementInitialized))
	{
		AchievementsPtr->ShowAchievementsUI();
	}
}

void FTdsImpl::UpdateLeaderboard(int32 Wave, int32 Kill)
{
	TMap<FString, double> Values = {
		{TEXT("wave_day"), static_cast<double>(Wave)},
		{TEXT("wave_week"), static_cast<double>(Wave)},
		{TEXT("wave_month"), static_cast<double>(Wave)},
		{TEXT("wave_total"), static_cast<double>(Wave)},
		{TEXT("kill_day"), static_cast<double>(Kill)},
		{TEXT("kill_week"), static_cast<double>(Kill)},
		{TEXT("kill_month"), static_cast<double>(Kill)},
		{TEXT("kill_total"), static_cast<double>(Kill)}
	};
	FLCLeaderboard::UpdateCurrentUserStatistics(Values,
		FLCLeaderboard::FStatisticsDelegate::CreateLambda([](TArray<FLCLeaderboardStatistic> Statistics)
		{

			
		}),
		FLCError::FDelegate::CreateLambda([](const FLCError& Error){

			
		}));
}

void FTdsImpl::FetchLeaderboardAround(const TArray<FString>& Names, const FString& ObjectId, int32 Limit, const FLeaderboardDelegate& InCallback)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC
	InCallback.ExecuteIfBound({}, 0);
#endif
	if (!ensure(Names.Num() > 0))
	{
		InCallback.ExecuteIfBound({}, 0);
		return;
	}
	
	FLCLeaderboard Query(Names[0]);
	Query.Limit = Limit;
	Query.WithCount = true;
	Query.Version = -1;
	
	for (int32 i = 1; i < Names.Num(); ++i)
	{
		Query.IncludeStatistics.Add(Names[0]);
	}
	Query.SelectMemberKeys.Add(TEXT("nickname"));
	Query.SelectMemberKeys.Add(TEXT("avatar"));
	Query.GetAroundResults(
		ObjectId,
		FLCLeaderboard::FRankingsDelegate::CreateSP(this, &FTdsImpl::OnFetchLeaderboardCallback, InCallback),
		FLCError::FDelegate::CreateSP(this, &FTdsImpl::OnFetchLeaderboardFailed, InCallback));
}

void FTdsImpl::FetchLeaderboardSkip(const TArray<FString>& Names, int32 Skip, int32 Limit, const FLeaderboardDelegate& InCallback)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC
	InCallback.ExecuteIfBound({}, 0);
#endif
	if (!ensure(Names.Num() > 0))
	{
		InCallback.ExecuteIfBound({}, 0);
		return;
	}
	
	FLCLeaderboard Query(Names[0]);
	Query.Skip = Skip;
	Query.Limit = Limit;
	Query.WithCount = true;
	Query.Version = -1;

	for (int32 i = 1; i < Names.Num(); ++i)
	{
		Query.IncludeStatistics.Add(Names[i]);
	}
	Query.SelectMemberKeys.Add(TEXT("nickname"));
	Query.SelectMemberKeys.Add(TEXT("avatar"));
	Query.GetResults(
		FLCLeaderboard::FRankingsDelegate::CreateSP(this, &FTdsImpl::OnFetchLeaderboardCallback, InCallback),
		FLCError::FDelegate::CreateSP(this, &FTdsImpl::OnFetchLeaderboardFailed, InCallback));
}

void FTdsImpl::FetchLeaderboardDetail(const TArray<FString>& Names, const FString& ObjectId, const FLeaderboardDetailDelegate& InCallback)
{
	FLCLeaderboard::GetStatistics(
		ObjectId,
		FLCLeaderboard::FStatisticsDelegate::CreateSP(this, &FTdsImpl::OnFetchLeaderboardDetailCallback, InCallback),
		FLCError::FDelegate::CreateSP(this, &FTdsImpl::OnFetchLeaderboardDetailFailed, InCallback),
		Names);
}

void FTdsImpl::BadgeDetailsCallback(const FBadgeDetails& Callback, bool bForce)
{
	if (bForce || bLastAnnouncementRedDot != Callback.show_red_dot)
	{
		bLastAnnouncementRedDot = Callback.show_red_dot;
		AnnouncementRedDotChangedCallback.ExecuteIfBound(bLastAnnouncementRedDot);
	}
}

bool FTdsImpl::UpdateMomentRedDot(float DeltaSecond)
{
	FTapMoment::FetchNotification();
	return MomentRedDotChangedCallback.IsBound();
}

void FTdsImpl::UpdateMomentRedDotCallback(int Code, const FString& Msg)
{
	switch (Code)
	{
	case 10000://动态发布成功
		break;
	case 10100://动态发布失败
		break;
	case 10200://关闭动态发布页面
		break;
	case 20000://获取新消息成功
		{
			int32 MessageNumber = FCString::Atoi(*Msg);
			MomentRedDotChangedCallback.ExecuteIfBound(MessageNumber != 0);
		}
		break;
	case 20100://获取新消息失败
		break;
	case 30000://动态页面打开
		break;
	case 30100://动态页面关闭
		break;
	case 50000://取消关闭所有动态界面（弹框点击取消按钮）
		break;
	case 50100://确认关闭所有动态界面（弹框点击确认按钮）
		break;
	case 60000://动态页面内登录成功
		break;
	case 70000://场景化入口回调
		break;
	default:
		break;
	}
}

void FTdsImpl::HandleAntiAddictionCallback(AntiAddictionUE::ResultHandlerCode Code, const FString& Message)
{
	switch (Code)
	{
	case AntiAddictionUE::LoginSuccess:
		AntiAddictionUE::EnterGame();
		AntiAddictionCallback.ExecuteIfBound(EAntiAddictionMessageType::Login);
		break;
	case AntiAddictionUE::Exited:
	case AntiAddictionUE::SwitchAccount:
		AntiAddictionUE::LeaveGame();
		AntiAddictionCallback.ExecuteIfBound(EAntiAddictionMessageType::Exited);
		break;
	case AntiAddictionUE::DurationLimit:
		AntiAddictionCallback.ExecuteIfBound(EAntiAddictionMessageType::DurationLimit);
		break;
	case AntiAddictionUE::PeriodRestrict:
		AntiAddictionCallback.ExecuteIfBound(EAntiAddictionMessageType::PeriodRestrict);
		break;
	case AntiAddictionUE::RealNameStop:
		AntiAddictionCallback.ExecuteIfBound(EAntiAddictionMessageType::RealNameStop);
		break;
	default:
		break;
	}
}

void FTdsImpl::OnFetchLeaderboardCallback(TArray<FLCLeaderboardRanking> LCRankings, int64 Count, FLeaderboardDelegate Callback)
{
	TArray<FLeaderboardData> RankData;
	for (const FLCLeaderboardRanking& Ranking : LCRankings)
	{
		MakeRankData(RankData.AddDefaulted_GetRef(), Ranking);
	}
	Callback.ExecuteIfBound(RankData, Count);
}

void FTdsImpl::OnFetchLeaderboardFailed(const FLCError& Error, FLeaderboardDelegate Callback)
{
	Callback.ExecuteIfBound({}, 0);
}

void FTdsImpl::OnFetchLeaderboardDetailCallback(TArray<FLCLeaderboardStatistic> InStatistics, FLeaderboardDetailDelegate Callback)
{
	TArray<FLeaderboardStatistic> Statistics;
	for (const FLCLeaderboardStatistic& S : InStatistics)
	{
		MakeStatistic(Statistics.AddDefaulted_GetRef(), S);
	}
	Callback.ExecuteIfBound(Statistics);
}

void FTdsImpl::OnFetchLeaderboardDetailFailed(const FLCError& Error, FLeaderboardDetailDelegate Callback)
{
	Callback.ExecuteIfBound({});
}

void FTdsImpl::UpdateSdkLanguage()
{
	FString CultureString = FInternationalization::Get().GetCurrentCulture()->GetName();
	ELanguageType Type = FindLanguageString(CultureString);
	TULanguage::SetCurrentType(Type);
}

ELanguageType FTdsImpl::FindLanguageString(const FString& String) const
{
	for (const TTuple<ELanguageType, FString>& T : LanguageMap)
	{
		if (T.Value == String)
		{
			return T.Key;
		}
	}
	return ELanguageType::EN;
}

class FSDKUsageModule: public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		Interface = MakeShared<FTdsImpl>();
		GTdsInterface = Interface.Get();
	}
	virtual void ShutdownModule() override
	{
		GTdsInterface = nullptr;
		Interface.Reset();
	}
	TSharedPtr<FTdsImpl> Interface;
};

IMPLEMENT_MODULE(FSDKUsageModule, SDKUsage);
