// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActionRPG.h"
#include "Json.h"
#include "Interface/TdsInterface.h"
#include "RPGGameInstance.generated.h"

struct FTdsPlayer;
enum EMessageType : int32;
class URPGItem;
class URPGSaveGame;

class FSocketClient;


USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Id;
	
	UPROPERTY(BlueprintReadOnly)
	FString AvatarUrl;
	
	UPROPERTY(BlueprintReadOnly)
	FString NickName;
};

USTRUCT(BlueprintType)
struct FDSInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 SessionId = 0;
	UPROPERTY(BlueprintReadOnly)
	FString TapSessionName;
	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayer = 0;

	UPROPERTY(BlueprintReadOnly)
	TArray<FPlayerInfo> Players;
};


USTRUCT()
struct FRefreshGameSessionWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FDSInfo> Sessions;
};

/**
 * Base class for GameInstance, should be blueprinted
 * Most games will need to make a game-specific subclass of GameInstance
 * Once you make a blueprint subclass of your native subclass you will want to set it to be the default in project settings
 */
UCLASS()
class ACTIONRPG_API URPGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	URPGGameInstance();
	
	TSharedPtr<FSocketClient> SocketClient;


	uint8 bTdsSplashPanelReady : 1;
	uint8 bTdsNavigatePanelReady : 1;
	uint8 bTdsAccountReady : 1;
	uint8 bTdsAntiAddictionReady : 1;
	uint8 bTdsSupportReady : 1;
	uint8 bTdsAchievement : 1;
	
	UFUNCTION(BlueprintCallable)
	void StartTds();

	void TdsTryOpenSplashPanel();

	void TdsTryOpenNavigatePanel();
	
	void TdsLogin();
	
	void TdsSetupAntiAddiction();

	void TdsSetupSupport();

	void TdsSetupAchievement();

	void TdsLoginSuccess(TSharedRef<FTdsPlayer> Player);

	void TdsLoginFail(TSharedRef<FTdsError> Error);

	void TdsLogout();

	void TdsHandleAntiAddictionMessage(EAntiAddictionMessageType MsgType);
	
	const TSharedPtr<FTdsPlayer>& GetTdsPlayer() const;


	void SocketDsSayHello(const FString& TapSessionName, int32 MaxPlayers, const FString& IpOverride);

	void SocketClientLogin(const FTdsPlayer& Player);

	void SocketClientLogout();
	
	UFUNCTION(BlueprintCallable)
	void StartRefreshGameSessions();

	UFUNCTION(BlueprintCallable)
	void StopRefreshGameSession();

	
	UFUNCTION(BlueprintCallable)
	bool ServerUpdateReady(bool bNewReady);

	UFUNCTION(BlueprintCallable)
	void ClientRefreshGameSessions();
	
	UFUNCTION(BlueprintCallable)
	bool ClientJoinSession(int32 SessionId);

	UFUNCTION(BlueprintCallable)
	bool ClientStartGame();

	UFUNCTION(BlueprintCallable)
	bool ClientLeaveSession();

protected:
	void HandleSocketCallback(EMessageType Type, TSharedPtr<FJsonObject> JsonObject);

	virtual void Init() override;

	virtual void OnStart() override;
	
	virtual void Shutdown() override;

	FTimerHandle RefreshTimer;

	TSharedPtr<FTdsPlayer> TdsPlayer;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* AchievementIcon;

	UPROPERTY(EditDefaultsOnly)
	FText AchievementTitle;

private:
	uint32 GetLocalVersion();
};