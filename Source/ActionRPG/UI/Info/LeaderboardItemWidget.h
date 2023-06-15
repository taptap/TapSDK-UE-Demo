// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/TdsInterface.h"
#include "LeaderboardItemWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;

/**
 * 
 */
UCLASS(Abstract)
class ACTIONRPG_API ULeaderboardItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateWidget(const FLeaderboardData& Data, bool bIsSelf);
	
protected:
	UFUNCTION()
	void HandleClicked();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDetailWidget(const FLeaderboardData& Data);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateIsSelf(bool bIsSelf);

	void OnDetailDataCallback(const TArray<FLeaderboardStatistic>& Statistics, FLeaderboardData TempData);
	
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* RankTextBlock;

	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UImage* AvatarImage;

	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* NickNameTextBlock;
	
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* ValueTextBlock;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* Button;

	UPROPERTY(BlueprintReadOnly)
	FLeaderboardData SavedData;

	UPROPERTY(EditAnywhere)
	UTexture2D* EmptyUserIcon;
};

 