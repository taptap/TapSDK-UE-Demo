// Copyright Epic Games, Inc. All Rights Reserved.


#include "TdsPlayerState.h"

#include "RPGGameInstance.h"


ATdsPlayerState::ATdsPlayerState()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATdsPlayerState::UpdateTdsPlayer_Implementation(const FTdsPlayer& NewPlayer)
{
	TdsPlayer = NewPlayer;
	OnRep_Player();
}

void ATdsPlayerState::UploadTdsPlayer()
{
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		if (GI->GetTdsPlayer())
		{
			if (APawn* Pawn = GetPawn())
			{
				if (Pawn->IsLocallyControlled())
				{
					UpdateTdsPlayer(*GI->GetTdsPlayer());
				}
			}
			else
			{
				FTimerHandle Handle;
				GetWorldTimerManager().SetTimer(Handle, this, &ATdsPlayerState::UploadTdsPlayer, 0.5f);
			}
		}
	}
}

void ATdsPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATdsPlayerState, TdsPlayer);
	DOREPLIFETIME(ATdsPlayerState, KillCount);
}

void ATdsPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	if (ATdsPlayerState* PS = Cast<ATdsPlayerState>(PlayerState))
	{
		TdsPlayer = PS->TdsPlayer;
	}
}

void ATdsPlayerState::BeginPlay()
{
	Super::BeginPlay();
	UploadTdsPlayer();
}

void ATdsPlayerState::OnRep_Player()
{
	OnTdsPlayerChanged.Broadcast(TdsPlayer);
}

