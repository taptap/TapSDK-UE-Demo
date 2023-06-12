// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameMode.h"

#include "RPGCharacter.h"
#include "RPGGameInstance.h"
#include "TdsGameState.h"
#include "RPGPlayerController.h"
#include "GameFramework/GameSession.h"

ARPGGameMode::ARPGGameMode()
{
	GameStateClass = ATdsGameState::StaticClass();
	PlayerControllerClass = ARPGPlayerController::StaticClass();
}

void ARPGGameMode::RegisterNpcPoint(AActor* InPoint)
{
	NpcStartPoints.Add(InPoint);
}

void ARPGGameMode::ForceRestartGame()
{
	UWorld* World = GetWorld();
	UE_LOG(LogActionRPG, Log, TEXT("End Match, Force Restart Game. Kick Player number: %d"), World->GetNumPlayerControllers());

	for (FConstPlayerControllerIterator Iter =World->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		GameSession->KickPlayer((*Iter).Get(), NSLOCTEXT("TapSDKDemo", "GameOverTimeOut", "Game Over"));
	}

	GetWorldTimerManager().ClearTimer(GameTimer);
	GameTimer.Invalidate();

	RestartGame();
}

void ARPGGameMode::EndGame(bool bSuccess)
{
	if (TdsMatchState == ETdsMatchState::None)
	{
		TdsMatchState = bSuccess ? ETdsMatchState::Success : ETdsMatchState::Fail;
		if (ATdsGameState* GS = GetGameState<ATdsGameState>())
		{
			GS->EndGame(bSuccess);
		}
		EndMatch();
	}
}

void ARPGGameMode::StartWave(int32 InWave)
{
	if (ATdsGameState* GS = GetGameState<ATdsGameState>())
	{
		if (WaveSettings.IsValidIndex(InWave - 1))
		{
			Wave = WaveSettings[InWave - 1];
			GS->StartNewWave(InWave, Wave.WaveTime);
			GroupIndex = -1;
			
			GetWorldTimerManager().SetTimer(GameTimer, this, &ARPGGameMode::NextEnemiesGroup, 4.f);
		}
		else
		{
			AllWaveFinished();
		}
	}
}

void ARPGGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		GI->ServerUpdateReady(true);
	}
}

void ARPGGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		GI->ServerUpdateReady(false);
	}
	
	if (ATdsGameState* GS = GetGameState<ATdsGameState>())
	{
		GS->OnTimeOver.BindUObject(this, &ARPGGameMode::OnTimeOver);
	}
	
	TdsMatchState = ETdsMatchState::None;

	//Delay start
	GetWorldTimerManager().SetTimer(GameTimer, FTimerDelegate::CreateUObject(this, &ARPGGameMode::StartWave, 1), 1.f, false);
}

void ARPGGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	if (ATdsGameState* GS = GetGameState<ATdsGameState>())
	{
		GS->OnTimeOver.Unbind();
	}
	
	GetWorldTimerManager().ClearTimer(GameTimer);
	GetWorldTimerManager().SetTimer(GameTimer, this, &ARPGGameMode::ForceRestartGame, 30);
}

void ARPGGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++PlayerCounter;
	if (PlayerCounter > 1)
	{
		if (ATdsGameState* GS = GetGameState<ATdsGameState>())
		{
			GS->NotifyAchievementMultiPlayerGame();
		}
	}
}

void ARPGGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	UWorld* World = GetWorld();
	UE_LOG(LogActionRPG, Log, TEXT("Player logout, Lift players number: %d"), World->GetNumPlayerControllers());
	if (World->GetNumPlayerControllers() == 0)
	{
		UE_LOG(LogActionRPG, Log, TEXT("Player logout, Restart Game"));
		World->GetTimerManager().ClearTimer(GameTimer);
		GameTimer.Invalidate();
		RestartGame();
	}
}

APawn* ARPGGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	if (APawn* Pawn = Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform))
	{
		Pawn->OnDestroyed.AddDynamic(this, &ARPGGameMode::OnPlayerDestroy);
		return Pawn;
	}
	return nullptr;
}

void ARPGGameMode::NextEnemiesGroup()
{
	if (Wave.EnemySettings.IsValidIndex(++GroupIndex))
	{
		TArray<AActor*> Points = NpcStartPoints;
		for (TSubclassOf<ARPGCharacter>& Class : Wave.EnemySettings[GroupIndex].EnemyClasses)
		{
			check(Points.Num() > 0);
			int32 RandI = FMath::RandRange(0, Points.Num() - 1);
			if (ARPGCharacter* Cha = GetWorld()->SpawnActor<ARPGCharacter>(Class, Points[RandI]->GetActorLocation(), Points[RandI]->GetActorRotation()))
			{
				Cha->OnDestroyed.AddDynamic(this, &ARPGGameMode::OnEnemyDestroy);
				Points.RemoveAtSwap(RandI);
				EnemyCounter++;
			}
		}
	}
	else
	{
		WaveFinished();
	}
}

void ARPGGameMode::GroupFinished()
{
	GetWorldTimerManager().SetTimer(GameTimer, this, &ARPGGameMode::NextEnemiesGroup, 2.f);
}

void ARPGGameMode::WaveFinished()
{
	if (ATdsGameState* GS = GetGameState<ATdsGameState>())
	{
		GS->EndWave();

		GetWorldTimerManager().ClearTimer(GameTimer);
		GetWorldTimerManager().SetTimer(GameTimer, FTimerDelegate::CreateUObject(this, &ARPGGameMode::StartWave, GS->GetCurrentWave() + 1), WaveTimeGap, false);
	}
}

void ARPGGameMode::AllWaveFinished()
{
	EndGame(true);
}

void ARPGGameMode::OnTimeOver()
{
	EndGame(false);
}

void ARPGGameMode::OnEnemyDestroy(AActor* EnemyActor)
{
	if (--EnemyCounter == 0)
	{
		GroupFinished();
	}
}

void ARPGGameMode::OnPlayerDestroy(AActor* PawnActor)
{
	if (--PlayerCounter == 0)
	{
		EndGame(false);
	}
}

