// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interface/TdsInterface.h"
#include "TdsPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FTdsPlayerDelegate, const FTdsPlayer& NewPlayer);

UCLASS()
class ACTIONRPG_API ATdsPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ATdsPlayerState();

	FTdsPlayerDelegate OnTdsPlayerChanged;

	FORCEINLINE const FTdsPlayer& GetTdsPlayer() const { return TdsPlayer; }

	UFUNCTION(Server, Reliable)
	void UpdateTdsPlayer(const FTdsPlayer& NewPlayer);

	void UploadTdsPlayer();

	FORCEINLINE int32 GetKillCount() const { return KillCount; }
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void CopyProperties(APlayerState* PlayerState) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_Player();
	
	UPROPERTY(ReplicatedUsing = "OnRep_Player")
	FTdsPlayer TdsPlayer;

	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 KillCount;
};
