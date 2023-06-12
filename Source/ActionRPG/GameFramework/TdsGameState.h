// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActionRPG.h"
#include "GameFramework/GameState.h"
#include "TdsGameState.generated.h"



UCLASS()
class ACTIONRPG_API ATdsGameState : public AGameState
{
	GENERATED_BODY()
public:
	FORCEINLINE int32 GetCurrentWave() const { return Wave; }

	UFUNCTION(NetMulticast, Reliable)
	void StartNewWave(int32 NewWave, float WaveTime);

	UFUNCTION(NetMulticast, Reliable)
	void EndWave();

	UFUNCTION(NetMulticast, Reliable)
	void EndGame(bool bSuccess);
	
	UFUNCTION(NetMulticast, Reliable)
	void AddBonusTime(float Time);

	/** Achievement */
	UFUNCTION(NetMulticast, Reliable)
	void NotifyAchievementMultiPlayerGame();
	
	FSimpleDelegate OnTimeOver;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void CountDownServer();
	
	UFUNCTION()
	void OnRep_LiftTime();

	UPROPERTY(ReplicatedUsing = "OnRep_LiftTime")
	int32 LiftTime;

	UPROPERTY(BlueprintReadOnly)
	int32 Wave;

	FTimerHandle LiftTimeHandle;
};

