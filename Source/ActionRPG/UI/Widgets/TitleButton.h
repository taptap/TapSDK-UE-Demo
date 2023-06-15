// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleButton.generated.h"

class UButton;
class UTextBlock;
class UWidgetAnimation;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleDyanmicMultiDelegate);
/**
 * 
 */
UCLASS(Abstract)
class ACTIONRPG_API UTitleButton : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FSimpleDyanmicMultiDelegate OnClicked;

	void SetButtonText(const FText& NewText);

	void UpdateRedDot(bool bHasRedDot);
	
protected:
	virtual void NativeOnInitialized() override;
	
	virtual void NativePreConstruct() override;

	UFUNCTION()
	void OnButtonClickedEvent();
	
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* ButtonText;

	UPROPERTY(Meta = (BindWidget))
	UButton* Button;
	
	UPROPERTY(Transient, Meta = (BindWidgetAnimOptional))
	UWidgetAnimation* RedDotAnim;

	UPROPERTY(EditAnywhere)
	FText ContentText;
};
