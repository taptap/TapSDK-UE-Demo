// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameInstance.h"

#include "JsonObjectConverter.h"
#include "RPGAssetManager.h"
#include "RPGSaveGame.h"
#include "TdsHud.h"
#include "Dom/JsonObject.h"
#include "Interface/TdsInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/NetworkVersion.h"
#include "Net/SocketClient.h"
#include "UI/Menu/TitleWidget.h"


URPGGameInstance::URPGGameInstance()
{
	bTdsSplashPanelReady = false;
	bTdsNavigatePanelReady = false;
	bTdsAccountReady = false;
	bTdsAntiAddictionReady = false;
	bTdsSupportReady = false;
}

void URPGGameInstance::StartTds()
{
	if (GTdsInterface)
	{
		TdsTryOpenSplashPanel();
	}
}

void URPGGameInstance::TdsTryOpenSplashPanel()
{
	check(GTdsInterface);

	if (bTdsSplashPanelReady)
	{
		TdsTryOpenNavigatePanel();
		return;
	}
	
	GTdsInterface->OpenSplashAnnouncement(
		FSimpleTdsDelegate::CreateWeakLambda(this, [this](const TSharedPtr<FTdsError>& Error)
		{
			if (!GTdsInterface) return;
			
			if (Error)
			{
				TdsTryOpenNavigatePanel();
			}
			else
			{
				//停服
			}
		}),
		FSimpleDelegate());
}

void URPGGameInstance::TdsTryOpenNavigatePanel()
{
	if (bTdsNavigatePanelReady)
	{
		TdsLogin();
		return;
	}

	GTdsInterface->HasNavigateAnnouncement(
		FBooleanDelegate::CreateWeakLambda(this, [this](bool bHas)
		{
			bTdsNavigatePanelReady = true;
			if (bHas)
			{
				GTdsInterface->OpenNavigateAnnouncement(
					FSimpleTdsDelegate::CreateWeakLambda(this, [this](const TSharedPtr<FTdsError>& Error)
					{
						if (Error)
						{
							TdsLogin();
						}
						else
						{
							//等待玩家关闭触发下面的TdsLogin
						}
					}),
					FSimpleDelegate::CreateWeakLambda(this, [this]()
					{
						TdsLogin();
					}));
			}
			else
			{
				TdsLogin();
			}
		}));
}

void URPGGameInstance::TdsLogin()
{
	check(GTdsInterface);

	if (bTdsAccountReady)
	{
		ensure(TdsPlayer);
		ATdsHud::ShowMenu<UTitleWidget>(this, ETapMenuType::Title);
		TdsLoginSuccess(TdsPlayer.ToSharedRef());
		return;
	}
	
	GTdsInterface->TdsLoginWithStorage(
		FTdsUserDelegate::CreateWeakLambda(
			this,
			[this](const TSharedPtr<FTdsPlayer>& Player, const TSharedPtr<FTdsError>& Error)
			{
				if (UTitleWidget* UI = ATdsHud::ShowMenu<UTitleWidget>(this, ETapMenuType::Title))
				{
					if (Player)
					{
						TdsLoginSuccess(Player.ToSharedRef());
					}
					else
					{
						UI->StartLogin();		
					}
				}
			}));
}

void URPGGameInstance::TdsSetupAntiAddiction()
{
	check(GTdsInterface);
	
	if (!bTdsAntiAddictionReady)
	{
		GTdsInterface->SetupAntiAddictionModule(
			TdsPlayer.ToSharedRef(),
			FSimpleTdsDelegate::CreateWeakLambda(this, [this](const TSharedPtr<FTdsError>& Error)
			{
				if (Error)
				{
					UE_LOG(LogActionRPG, Warning, TEXT("AntiAddiction failed: %s"), *Error->Description);
					GTdsInterface->SetupAntiAddictionCallback(FAntiAddictionDelegate());
				}
				else
				{
					GTdsInterface->SetupAntiAddictionCallback(FAntiAddictionDelegate::CreateUObject(this, &URPGGameInstance::TdsHandleAntiAddictionMessage));
				}
			}));
	}
}

void URPGGameInstance::TdsSetupSupport()
{
	if (bTdsSupportReady)
	{
		if (UTitleWidget* UI = ATdsHud::FindMenu<UTitleWidget>(this, ETapMenuType::Title))
		{
			UI->EnableSupport(true);
		}
		return;
	}

	check(GTdsInterface);

	GTdsInterface->SetupSupportModule(
		TdsPlayer.ToSharedRef(),
		nullptr,
		FSimpleTdsDelegate::CreateWeakLambda(this,
			[this](const TSharedPtr<FTdsError>& Error)
			{
				if (Error)
				{
					bTdsSupportReady = false;
					if (UTitleWidget* UI = ATdsHud::FindMenu<UTitleWidget>(this, ETapMenuType::Title))
					{
						UI->EnableSupport(false);
					}
				}
				else
				{
					bTdsSupportReady = true;
					if (UTitleWidget* UI = ATdsHud::FindMenu<UTitleWidget>(this, ETapMenuType::Title))
					{
						UI->EnableSupport(true);
					}
				}
			}));
}

void URPGGameInstance::TdsLoginSuccess(TSharedRef<FTdsPlayer> Player)
{
	bTdsAccountReady = true;
	TdsPlayer = Player;
	if (UTitleWidget* UI = ATdsHud::FindMenu<UTitleWidget>(this, ETapMenuType::Title))
	{
		UI->UpdateWidgetWithTdsUser(*Player);
	}

	SocketClientLogin(*Player);
	
	TdsSetupAntiAddiction();
	TdsSetupSupport();
}

void URPGGameInstance::TdsLoginFail(TSharedRef<FTdsError> Error)
{
	ATdsHud::ShowMessage(this, NSLOCTEXT("TapSDKDemo", "LoginFailed", "登录失败"));
}

void URPGGameInstance::TdsLogout()
{
	if (GTdsInterface && TdsPlayer)
	{
		GTdsInterface->TdsLogout(TdsPlayer.ToSharedRef());
		TdsPlayer.Reset();

		bTdsAccountReady = false;
		bTdsAntiAddictionReady = false;
		bTdsSupportReady = false;

		if (UTitleWidget* UI = ATdsHud::FindMenu<UTitleWidget>(this, ETapMenuType::Title))
		{
			UI->StartLogin();
			UI->EnableSupport(false);
		}

		SocketClientLogout();
	}
}

void URPGGameInstance::TdsHandleAntiAddictionMessage(EAntiAddictionMessageType MsgType)
{
	switch (MsgType)
	{
	case EAntiAddictionMessageType::Login:
		if (TdsPlayer)
		{
			GTdsInterface->GetAntiAddictionPlayerInfo(TdsPlayer.ToSharedRef());
		}
		break;
	case EAntiAddictionMessageType::Exited:
		break;
	case EAntiAddictionMessageType::DurationLimit:
		ATdsHud::ShowMessage(this, NSLOCTEXT("TapSDKDemo", "DurationLimit", "可玩时长耗尽"));
		break;
	case EAntiAddictionMessageType::PeriodRestrict:
		ATdsHud::ShowMessage(this, NSLOCTEXT("TapSDKDemo", "PeriodRestrict", "达到宵禁时长"));
		break;
	case EAntiAddictionMessageType::RealNameStop:
		ATdsHud::ShowMessage(this, NSLOCTEXT("TapSDKDemo", "RealNameStop", "请进行实名认证"));
		TdsLogout();
		break;
	default: ;
	}

}

const TSharedPtr<FTdsPlayer>& URPGGameInstance::GetTdsPlayer() const
{
	return TdsPlayer;
}

void URPGGameInstance::SocketDsSayHello(const FString& TapSessionName, int32 MaxPlayers, const FString& IpOverride)
{
	if(!SocketClient) return;

	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
	
	Obj->SetStringField(TEXT("TapSessionName"), TapSessionName);
	Obj->SetNumberField(TEXT("MaxPlayers"), MaxPlayers);
	Obj->SetStringField(TEXT("IpOverride"), IpOverride);
	
	if (!SocketClient->Send(EMessageType::SayHello, Obj))
	{
		UE_LOG(LogActionRPG, Warning, TEXT("%s SayHello Send failed."), ANSI_TO_TCHAR(__FUNCTION__));
	}
}

void URPGGameInstance::SocketClientLogin(const FTdsPlayer& Player)
{
	if(!SocketClient) return;
	
	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();

	Obj->SetStringField(TEXT("Id"), Player.Id);
	Obj->SetStringField(TEXT("AvatarUrl"), Player.AvatarUrl);
	Obj->SetStringField(TEXT("NickName"), Player.NickName);
			
	if (!SocketClient->Send(EMessageType::Login, Obj))
	{
		UE_LOG(LogActionRPG, Warning, TEXT("%s SayHello Send failed."), ANSI_TO_TCHAR(__FUNCTION__));
	}
}

void URPGGameInstance::SocketClientLogout()
{
	if(!SocketClient) return;

	if (!SocketClient->Send(EMessageType::Logout, nullptr))
	{
		UE_LOG(LogActionRPG, Warning, TEXT("%s SayHello Send failed."), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}
	HandleLeaveSession();
}

void URPGGameInstance::StartRefreshGameSessions()
{
	if (RefreshTimer.IsValid())
	{
		return;
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RefreshTimer, this, &URPGGameInstance::ClientRefreshGameSessions, 3.f, true);
	}
}

void URPGGameInstance::StopRefreshGameSession()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RefreshTimer);
		RefreshTimer.Invalidate();
	}
}

bool URPGGameInstance::ServerUpdateReady(bool bNewReady)
{
#if UE_SERVER
	if(!SocketClient) return false;

	if (SocketClient)
	{
		TArray<int32> arr;
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetBoolField(TEXT("Ready"), bNewReady);
		return SocketClient->Send(EMessageType::ServerUpdateReadyNtf, Obj);
	}
	return false;
#else
	return false;
#endif
}

void URPGGameInstance::ClientRefreshGameSessions()
{
	if (SocketClient)
	{
		if (SocketClient->Send(EMessageType::RefreshGameSessionRequest, nullptr))
		{
			UE_LOG(LogActionRPG, Log, TEXT("Refresh game sessions."));
		}
		else
		{
			UE_LOG(LogActionRPG, Warning, TEXT("%s Send failed."), ANSI_TO_TCHAR(__FUNCTION__));
		}
	}
}

bool URPGGameInstance::ClientJoinSession(int32 SessionId)
{
	if (SocketClient)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetNumberField(TEXT("SessionId"), SessionId);
		return SocketClient->Send(EMessageType::JoinSessionRequest, Obj);
	}
	return false;
}

bool URPGGameInstance::ClientStartGame()
{
	if (SocketClient)
	{
		return SocketClient->Send(EMessageType::ClientStartGameRequest, nullptr);
	}
	return false;
}

bool URPGGameInstance::ClientLeaveSession()
{
	if (SocketClient)
	{
		return SocketClient->Send(EMessageType::LeaveSessionRequest, nullptr);
	}
	return false;
}

void URPGGameInstance::HandleSocketCallback(EMessageType Type, TSharedPtr<FJsonObject> JsonObject)
{
	switch (Type)
	{
	case ServerStartGameResponse: //暂时没有
		break;
	case ClientStartGameResponse:
		if (JsonObject)
		{
			FString Ip;
			if (JsonObject->TryGetStringField(TEXT("Ip"), Ip))
			{
				if (APlayerController* PC = GetFirstLocalPlayerController())
				{
					UE_LOG(LogActionRPG, Log, TEXT("%s ClientStartGameResponse Ip:%s"), ANSI_TO_TCHAR(__FUNCTION__), *Ip);
					PC->ClientTravel(Ip, ETravelType::TRAVEL_Absolute);
				}
			}
		}
		break;
	case RefreshGameSessionResponse:
		if (JsonObject)
		{
			FRefreshGameSessionWrapper Wrapper;
			if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &Wrapper))
			{
				HandleRefreshSessions(Wrapper.Sessions);
			}
		}
		break;
	case JoinSessionResponse:
		if (JsonObject)
		{
			int32 ErrorCode;
			if (JsonObject->TryGetNumberField(TEXT("ErrorCode"), ErrorCode))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Join failed: %d"), ErrorCode));
			}
			else
			{
				FDSInfo DSInfo;
				if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &DSInfo))
				{
					HandleJoinSession(DSInfo);
				}
			}
		}
		break;
	case LeaveSessionResponse:
		HandleLeaveSession();
		break;
	default: ;
	}
}


void URPGGameInstance::Init()
{
	Super::Init();
	SocketClient = MakeShared<FSocketClient>();
	if (SocketClient->Connect(CONNECT_ADDRESS, TEXT("RPG Socket Connection")))
	{
		SocketClient->Callback.AddUObject(this, &URPGGameInstance::HandleSocketCallback);
	}
	else
	{
		UE_LOG(LogActionRPG, Warning, TEXT("Socket connect failed. %s"), CONNECT_ADDRESS);
		SocketClient.Reset();
	}
#if UE_SERVER
	if (SocketClient)
	{
		FString TapSessionName;
		int32 MaxPlayers = 8;
		FString IpOverride;
		FParse::Value( FCommandLine::Get(), TEXT("-TapSessionName="), TapSessionName);
		FParse::Value( FCommandLine::Get(), TEXT("-MaxPlayers="), MaxPlayers);
		FParse::Value( FCommandLine::Get(), TEXT("-IpOverride="), IpOverride);
		SocketDsSayHello(TapSessionName, MaxPlayers, IpOverride);
	}
#endif
	if (GTdsInterface)
	{
		GTdsInterface->InitSdk();
		GTdsInterface->SetupAchievement(AchievementIcon, AchievementTitle);
	}

	FNetworkVersion::GetLocalNetworkVersionOverride.BindUObject(this, &URPGGameInstance::GetLocalVersion);
}

void URPGGameInstance::Shutdown()
{
	Super::Shutdown();
	if (SocketClient)
	{
		SocketClient->Close();
	}
}

uint32 URPGGameInstance::GetLocalVersion()
{
	FString VersionString = FString::Printf(TEXT("%s %s, EngineNetVer: %d, GameNetVer: %d"),
		FApp::GetProjectName(),
		*FNetworkVersion::GetProjectVersion(),
		FNetworkVersion::GetEngineNetworkProtocolVersion(),
		FNetworkVersion::GetGameNetworkProtocolVersion());

	return FCrc::StrCrc32(*VersionString.ToLower());
}



