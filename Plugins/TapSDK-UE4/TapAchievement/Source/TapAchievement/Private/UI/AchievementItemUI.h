// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TapAchievementTypes.h"
#include "Blueprint/UserWidget.h"
#include "AchievementItemUI.generated.h"

class UTexture2D;
class UImage;
class UTextBlock;
class UPanelWidget;
class UButton;
class UAchievementDetailUI;

USTRUCT()
struct FTapAchievementLevelSetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* BackgroundTexture = nullptr;

	UPROPERTY(EditAnywhere)
	UTexture2D* LeftTexture = nullptr;

	UPROPERTY(EditAnywhere)
	UTexture2D* RightTexture = nullptr;
};

UCLASS(Abstract)
class TAPACHIEVEMENT_API UAchievementItemUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void UpdateDetailUI(const FAchievementDescTap& Description, const FAchievementTap& InAchievement);
	
	UPROPERTY()
	FAchievementDescTap Desc;
	
	UPROPERTY()
	FAchievementTap Achievement;
	
protected:
	UFUNCTION()
	void OnDetailButtonClicked();

	void OnDownloadIconComplete(UTexture2D* Texture);

	virtual void NativeOnInitialized() override;

	UPROPERTY(Meta = (BindWidget))
	UImage* Icon;

	UPROPERTY(Meta = (BindWidget))
	UImage* PlatinumIcon;
	
	UPROPERTY(Meta = (BindWidget))
	UImage* LevelBG2;
	
	UPROPERTY(Meta = (BindWidget))
	UImage* LevelBG;

	UPROPERTY(Meta = (BindWidget))
	UImage* LevelL;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_Level;

	UPROPERTY(Meta = (BindWidget))
	UImage* LevelR;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_Title;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_Subtitle;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_Percentage;
	
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* PercentagePanel;

	UPROPERTY(Meta = (BindWidgetOptional))
	UButton* BTN_Detail;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* HiddenTexture;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAchievementDetailUI> DetailUIClass;
	
	UPROPERTY(EditAnywhere)
	TMap<int32, FTapAchievementLevelSetting> LevelTextureSettings;

	UPROPERTY(EditAnywhere)
	float BaseProgress;
};
