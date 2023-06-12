// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageBox.generated.h"

class UButton;
class UTextBlock;
class UWidgetAnimation;
/**
 * 
 */
UCLASS()
class ACTIONRPG_API UMessageBox : public UUserWidget
{
	GENERATED_BODY()
public:
	FSimpleDelegate OnConfirm;

	void UpdateBox( const FText& NewContent);

protected:
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	
	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* FadeAnim;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ContentTextBlock;
};

