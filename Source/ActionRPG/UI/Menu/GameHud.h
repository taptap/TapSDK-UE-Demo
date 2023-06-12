// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Info/InfoBarWidget.h"
#include "GameHud.generated.h"

class UProgressBar;
class UWidgetAnimation;
class UInfoBarWidget;
/**
 * 
 */
UCLASS()
class ACTIONRPG_API UGameHud : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateLiftTime(int32 Time);

	UFUNCTION(BlueprintImplementableEvent)
	void AddBonusTime(int32 Time);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateWeapon();

	UInfoBarWidget* GetInfoBarWidget() const;
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UProgressBar* HP_ProgressBar;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UProgressBar* MP_ProgressBar;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* TimeAnimation;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* TimeBonusAnim;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UInfoBarWidget* InfoBarWidget;
};

