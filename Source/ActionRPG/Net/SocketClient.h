// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "Tickable.h"

class FSocket;

enum EMessageType : int32
{
	SayHello = 1,
	ServerUpdateReadyNtf,
	ServerStartGameResponse,

	ClientStartGameResponse,
	ClientStartGameRequest,

	Login,
	Logout,
	RefreshGameSessionRequest,
	RefreshGameSessionResponse,
	JoinSessionRequest,
	JoinSessionResponse,
	LeaveSessionRequest,
	LeaveSessionResponse
};

enum EErrorType : int32
{
	// None = 0,
	InvalidSessionId = 1,
	JoinFailed,
};

#define WAIT_TIME 200

DECLARE_MULTICAST_DELEGATE_TwoParams(FSocketCallback, EMessageType MessageType, TSharedPtr<FJsonObject> JsonObject);

/**
 * 4Bytes 			MessageType
 * 4Bytes 			ContentBytes
 * ContentBytes		Body
 */
class ACTIONRPG_API FSocketClient : public FTickableGameObject
{
public:
	FSocketClient();

	virtual ~FSocketClient() override;
	
	bool Connect(const FString& AddressString, const FString Description);

	void Close();

	bool Send(EMessageType Type, TSharedPtr<FJsonObject> JsonObject);
	
	FSocketCallback Callback;

protected:
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override { return TStatId(); }
	
	FSocket* Socket = nullptr;
	TArray<uint8> RecvBuffer;
};

