// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfoBarWidget.generated.h"

class UImage;
class UTextBlock;
struct FTdsPlayer;
class APawn;

UCLASS(Abstract)
class ACTIONRPG_API UInfoBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateWidget(const FTdsPlayer& Player);

	UFUNCTION(BlueprintCallable)
	void UpdateInfoBarOwningPawn(APawn* NewPawn);

protected:
	virtual void NativeOnInitialized() override;

	void UpdateWithSavedPawn();
	
	UPROPERTY(Meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* NameTextBlock;

	TWeakObjectPtr<APawn> SavedPawn;
};
