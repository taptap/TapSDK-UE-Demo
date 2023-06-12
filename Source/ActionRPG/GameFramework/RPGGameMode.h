// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActionRPG.h"
#include "GameFramework/GameMode.h"
#include "RPGGameMode.generated.h"

class ARPGCharacter;

USTRUCT()
struct FEnemySetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ARPGCharacter>> EnemyClasses;
};

USTRUCT()
struct FWaveSetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FEnemySetting> EnemySettings;

	UPROPERTY(EditAnywhere)
	float WaveTime;
};

UENUM(BlueprintType)
enum class ETdsMatchState : uint8
{
	None,
	Fail,
	Success
};

/** Base class for GameMode, should be blueprinted */
UCLASS()
class ACTIONRPG_API ARPGGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ARPGGameMode();

	UFUNCTION(BlueprintCallable)
	void RegisterNpcPoint(AActor* InPoint);

	void ForceRestartGame();
	
	void EndGame(bool bSuccess);

	void StartWave(int32 InWave);

	FORCEINLINE ETdsMatchState GetTdsMatchState() const { return TdsMatchState; }
	
protected:
	virtual void HandleMatchIsWaitingToStart() override;

	virtual void HandleMatchHasStarted() override;

	virtual void HandleMatchHasEnded() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

	void NextEnemiesGroup();

	void GroupFinished();
	
	void WaveFinished();

	void AllWaveFinished();

	void OnTimeOver();
	
	UFUNCTION()
	void OnEnemyDestroy(AActor* EnemyActor);

	UFUNCTION()
	void OnPlayerDestroy(AActor* PawnActor);
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FWaveSetting> WaveSettings;

	UPROPERTY(EditDefaultsOnly)
	float WaveTimeGap = 3.f;
	
	UPROPERTY()
	FWaveSetting Wave;

	UPROPERTY()
	int32 GroupIndex;

	UPROPERTY()
	TArray<AActor*> NpcStartPoints;

	UPROPERTY()
	ETdsMatchState TdsMatchState = ETdsMatchState::None;

	UPROPERTY()
	int32 EnemyCounter;

	UPROPERTY()
	int32 PlayerCounter;

	FTimerHandle GameTimer;
};

