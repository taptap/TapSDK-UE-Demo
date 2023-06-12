// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AchievementItemUI.h"
#include "AchievementDetailUI.generated.h"

class UButton;
class UTextBlock;
class UImage;
struct FAchievementTap;
struct FAchievementDescTap;

/**
 * 
 */
UCLASS(Abstract)
class TAPACHIEVEMENT_API UAchievementDetailUI : public UAchievementItemUI
{
	GENERATED_BODY()
public:
	virtual void UpdateDetailUI(const FAchievementDescTap& Description, const FAchievementTap& InAchievement) override;

protected:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* BTN_Close;

	UPROPERTY(Meta = (BindWidget))
	UImage* ProjectIcon;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_ProjectName;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_AchievementProgress;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_Rarity;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_CompleteTime;
};
