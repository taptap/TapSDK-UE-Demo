// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleButton.generated.h"

class UButton;
class UTextBlock;
class UWidgetAnimation;
/**
 * 
 */
UCLASS(Abstract)
class ACTIONRPG_API UTitleButton : public UUserWidget
{
	GENERATED_BODY()
public:
	FSimpleDelegate OnClicked;

	virtual void SetIsEnabled(bool bInIsEnabled) override;
	
	void SetButtonText(const FText& NewText);

	UPROPERTY(Meta = (BindWidget))
	UButton* Button;
	
protected:
	virtual void NativeOnInitialized() override;
	
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonClicked();
	
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* ButtonText;

	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* EnableAnim;

	UPROPERTY(EditAnywhere)
	FText ContentText;
};
