// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/TdsInterface.h"
#include "LeaderboardWidget.generated.h"

class UTextBlock;
class ULeaderboardItemWidget;
class UScrollBox;
class UEditableTextBox;

/**
 * 
 */
UCLASS(Abstract)
class ACTIONRPG_API ULeaderboardWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ChangeCategory(ELeaderboardCategory InCategory);

	UFUNCTION(BlueprintCallable)
	void ChangeType(ELeaderboardType InType);

	UFUNCTION(BlueprintCallable)
	void ChangeSelection(ELeaderboardCategory InCategory, ELeaderboardType InType);

	UFUNCTION(BlueprintCallable)
	void GotoPage(ELeaderboardCategory InCategory, ELeaderboardType InType, int32 NewPageIndex);

	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetMaxPageIndex() const { return TotalCount / PageSize; }
	
protected:
	virtual void NativeOnInitialized() override;

	void OnSelfPlayerCallback(const TArray<FLeaderboardData>& RankData, int64 Count, ELeaderboardCategory InCategory, ELeaderboardType InType);
	
	void OnRangeDataCallback(const TArray<FLeaderboardData>& RankData, int64 Count, ELeaderboardCategory InCategory, ELeaderboardType InType);
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCategorySelection(ELeaderboardCategory NewCategory, bool bNewSelection);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTypeSelection(ELeaderboardType NewType, bool bNewSelection);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateLoadingState(bool bLoading);
	
	UPROPERTY(Meta = (BindWidget))
	UScrollBox* ItemBox;

	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* PageEditableTextBox;
	
	UPROPERTY(EditDefaultsOnly)
	int32 PageSize = 20;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPageIndex;

	int32 TotalCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ELeaderboardCategory Category;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ELeaderboardType Type;

	UPROPERTY()
	FString SelfObjectId;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULeaderboardItemWidget> ItemClass;
	
};

 