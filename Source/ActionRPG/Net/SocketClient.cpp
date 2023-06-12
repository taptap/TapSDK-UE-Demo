// Copyright Epic Games, Inc. All Rights Reserved.


#include "SocketClient.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Json.h"

FSocketClient::FSocketClient()
{
}

FSocketClient::~FSocketClient()
{
	Close();
}

bool FSocketClient::Connect(const FString& AddressString, const FString Description)
{
	ISocketSubsystem* Sys = ISocketSubsystem::Get();
	if (!Sys)
	{
		return false;
	}
	if (Socket)
	{
		Close();
	}

	TSharedRef<FInternetAddr> Addr = Sys->CreateInternetAddr();
	bool bValid = false;
	Addr->SetIp(*AddressString, bValid);
	if (!bValid)
	{
		return false;
	}
	
	Socket = Sys->CreateSocket(NAME_Stream, Description, Addr->GetProtocolType());
	// Socket->SetNonBlocking();
	FString DebugAddr = Addr->ToString(true);
	if (!Socket->Connect(*Addr))
	{
		Sys->DestroySocket(Socket);
		Socket = nullptr;
		return false;
	}
	return true;
}

void FSocketClient::Close()
{
	ISocketSubsystem* Sys = ISocketSubsystem::Get();
	if (!Sys)
	{
		return;
	}
	
	if (Socket)
	{
		Sys->DestroySocket(Socket);
		Socket = nullptr;
	}
}

bool FSocketClient::Send(EMessageType Type, TSharedPtr<FJsonObject> JsonObject)
{
	if (!Socket) return false;
	
	TArray<uint8> Buffer;
	FMemoryWriter Writer(Buffer);
	int32 TypeNumber = Type;
	Writer << TypeNumber;
	
	if (JsonObject)
	{
		FString TCharStr;
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&TCharStr);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		FTCHARToUTF8 TCharToUtf8(*TCharStr);
		
		int32 Size = TCharToUtf8.Length() + 1;
		Writer << Size;
		Writer.Serialize((ANSICHAR*)TCharToUtf8.Get(), Size);
	}
	else
	{
		int32 Size = 0;
		Writer << Size;
	}

	if (!Socket->Wait(ESocketWaitConditions::WaitForWrite, FTimespan::FromMilliseconds(WAIT_TIME)))
	{
		return false;
	}

	int32 ByteSent = 0;
	Socket->Send(Buffer.GetData(), Buffer.Num(), ByteSent);
	return ByteSent == Buffer.Num();// 暂时不做考虑
}


void FSocketClient::Tick(float DeltaTime)
{
	if (!Socket) return;

	if (Socket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
	{
		Close();
		return;
	}
	
	uint32 PendingDataSize = 0;
	if (!Socket->HasPendingData(PendingDataSize)) return;

	if (!Socket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromMilliseconds(WAIT_TIME)))
	{
		return;
	}
		
	int32 Offset = RecvBuffer.Num(); 
	RecvBuffer.SetNum(Offset + static_cast<int32>(PendingDataSize));
		
	int32 Read = 0;
	if (!Socket->Recv(RecvBuffer.GetData() + Offset, static_cast<int32>(PendingDataSize), Read))
	{
		UE_LOG(LogActionRPG, Warning, TEXT("Socket Recv Failed. Return. RecvBuffer.Num: %d"), RecvBuffer.Num());
		return;
	}

	while (RecvBuffer.Num() > 8)
	{
		EMessageType Type = *reinterpret_cast<EMessageType*>(RecvBuffer.GetData());
		int32 DataSize = *reinterpret_cast<int32*>(RecvBuffer.GetData() + 4);
		int32 LeftDataSize = RecvBuffer.Num() - 8;

		if (DataSize > 0)
		{
			if (LeftDataSize >= DataSize)
			{
				FUTF8ToTCHAR TCHARData(reinterpret_cast<const ANSICHAR*>(RecvBuffer.GetData() + 8), DataSize);
				FString JsonStr = FString(TCHARData.Length(), TCHARData.Get());
				
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
				TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
				if (FJsonSerializer::Deserialize(Reader, JsonObject))
				{
					Callback.Broadcast(Type, JsonObject);
				}
				else
				{
					UE_LOG(LogActionRPG, Warning, TEXT("Socket Recv Exception. RecvBuffer.Num: %d"), RecvBuffer.Num());
				}
				
				RecvBuffer.RemoveAt(0, DataSize + 8);
			}
			else
			{
				break;
			}
		}
		else
		{
			Callback.Broadcast(Type, nullptr);
			RecvBuffer.RemoveAt(0, 8);
		}
	}
}


