// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AchievementUI.generated.h"

class UWidgetSwitcher;
class UAchievementItemUI;
class UUniformGridPanel;
class UPanelWidget;
class UScrollBox;
struct FAchievementTap;
class UDynamicOrientationBox;
struct FAchievementDescTap;
class USpacer;
class UButton;
class UTextBlock;
struct FTUError;

UENUM()
enum class EAchievementDeviceType : uint8
{
	PC,
	Phone,
	Tablet
};

UCLASS(Abstract)
class TAPACHIEVEMENT_API UAchievementUI : public UUserWidget
{
	GENERATED_BODY()
public:
	float GetOffset() const;

	void SetOffset(float NewOffset);
	
protected:
	virtual void NativeOnInitialized() override;

	virtual void NativePreConstruct() override;

	void UpdateStyle();

	void FullWithEmptyItem(TArray<UUserWidget*>& Array, int32 SnapNumber);

	void AddWidgetsToPanel(UUniformGridPanel* Panel, const TArray<UUserWidget*> Widgets, int32 SnapNumber);

	UFUNCTION()
	void OnScroll(float CurrentOffset);
	
	void OnAchievementStatusUpdate(const FAchievementDescTap* Desc, const FAchievementTap* Achievement, const TSharedPtr<FTUError>& Error);
	
	UPROPERTY()
	TMap<FString, UAchievementItemUI*> ItemUIMap;

	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_UnlockedAchievementNumber;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_AllAchievementNumber;

	UPROPERTY(Meta = (BindWidget))
	UButton* BTN_Close;

	UPROPERTY(Meta = (BindWidgetOptional))
	UPanelWidget* CloseButtonPanel;
	
	UPROPERTY(Meta = (BindWidgetOptional))
	UButton* BTN_Close_Hidden;

	UPROPERTY(Meta = (BindWidgetOptional))
	USpacer* HeaderSpacer;
	
	UPROPERTY(Meta = (BindWidget))
	UScrollBox* ScrollPanel;

	UPROPERTY(Meta = (BindWidgetOptional))
	UWidgetSwitcher* UnlockSwitcher;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* UnlockLabel;

	UPROPERTY(Meta = (BindWidgetOptional))
	UTextBlock* UnlockLabel_ZH;
	
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* UnlockBox;
	
	UPROPERTY(Meta = (BindWidget))
	UUniformGridPanel* UnlockPanel;

	UPROPERTY(Meta = (BindWidgetOptional))
	UWidgetSwitcher* LockSwitcher;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* LockLabel;

	UPROPERTY(Meta = (BindWidgetOptional))
	UTextBlock* LockLabel_ZH;
	
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* LockBox;
	
	UPROPERTY(Meta = (BindWidget))
	UUniformGridPanel* LockPanel;

	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAchievementItemUI> ItemUIClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> EmptyClass;

	UPROPERTY(EditAnywhere)
	bool bOrientationPortrait;

	UPROPERTY(EditAnywhere)
	EAchievementDeviceType DeviceType;

	UPROPERTY(EditAnywhere)
	TArray<FAchievementDescTap> Descriptions;
	
	UPROPERTY(EditAnywhere)
	TArray<FAchievementTap> Achievements;
};

 
 
