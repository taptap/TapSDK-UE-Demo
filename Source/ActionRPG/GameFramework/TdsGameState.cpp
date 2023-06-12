// Copyright Epic Games, Inc. All Rights Reserved.

#include "TdsGameState.h"

#include "TdsHud.h"
#include "TdsPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Interface/TdsInterface.h"
#include "UI/Menu/GameHud.h"
#include "UI/Menu/WaveStartWidget.h"

void ATdsGameState::StartNewWave_Implementation(int32 NewWave, float WaveTime)
{
	Wave = NewWave;

	LiftTime = WaveTime;

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController())
		{
			if (UWaveStartWidget* UI = ATdsHud::ShowMenu<UWaveStartWidget>(this, ETapMenuType::WaveStart))
			{
				UI->UpdateWave(Wave);
			}
		}
	}

	if (HasAuthority())
	{
		FTimerManager& Manager = GetWorldTimerManager();
		Manager.ClearTimer(LiftTimeHandle);
	
		Manager.SetTimer(LiftTimeHandle,this, &ATdsGameState::CountDownServer, 1.f, true);
	}
}

void ATdsGameState::EndWave_Implementation()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController())
		{
			ATdsHud::ShowMenu(this, ETapMenuType::WaveEnd);
		}
	}
}

void ATdsGameState::EndGame_Implementation(bool bSuccess)
{
	if (HasAuthority())
	{
		for (APlayerState* PS : PlayerArray)
		{
			PS->ForceNetUpdate();
		}

		GetWorldTimerManager().ClearTimer(LiftTimeHandle);
		LiftTimeHandle.Invalidate();
	}
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController())
		{
			UUserWidget* UI = ATdsHud::ShowMenu(this, ETapMenuType::GameOver);
			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(UI ? UI->GetCachedWidget() : nullptr);
			PC->SetInputMode(Mode);

			if (GTdsInterface)
			{
				if (bSuccess)
				{
					GTdsInterface->AchievementReach(TEXT("win"));
				}

				int32 Kill = 0;
				if (ATdsPlayerState* PS = PC->GetPlayerState<ATdsPlayerState>())
				{
					Kill = PS->GetKillCount();
				}
				GTdsInterface->UpdateLeaderboard(bSuccess ? Wave + 1 : Wave, Kill);
			}
		}
	}
}

void ATdsGameState::AddBonusTime_Implementation(float Time)
{
	LiftTime += Time;
	if (UGameHud* UI = ATdsHud::FindMenu<UGameHud>(this, ETapMenuType::GameHud))
	{
		UI->AddBonusTime(Time);
		UI->UpdateLiftTime(LiftTime);
	}
}

void ATdsGameState::NotifyAchievementMultiPlayerGame_Implementation()
{
	if (GTdsInterface)
	{
		GTdsInterface->AchievementReach(TEXT("multiplayer"));
	}
}

void ATdsGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATdsGameState, LiftTime);
}

void ATdsGameState::BeginPlay()
{
	Super::BeginPlay();
	if (GTdsInterface)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (PC->IsLocalController())
			{
				GTdsInterface->StartupMarquee(true);
			}
		}
	}
}

void ATdsGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (GTdsInterface)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (PC->IsLocalController())
			{
				GTdsInterface->StartupMarquee(false);
			}
		}
	}
}

void ATdsGameState::CountDownServer()
{
	--LiftTime;

	OnRep_LiftTime();
	
	if (LiftTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(LiftTimeHandle);
	}
}

void ATdsGameState::OnRep_LiftTime()
{
	if (UGameHud* UI = ATdsHud::FindMenu<UGameHud>(this, ETapMenuType::GameHud))
	{
		UI->UpdateLiftTime(LiftTime);
	}

	if (LiftTime <= 0)
	{
		OnTimeOver.ExecuteIfBound();
	}
}


