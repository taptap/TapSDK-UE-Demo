// Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/Widgets/TitleButton.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTitleButton::SetIsEnabled(bool bInIsEnabled)
{
	Super::SetIsEnabled(bInIsEnabled);

	if (bInIsEnabled)
	{
		PlayAnimationForward(EnableAnim);
	}
	else
	{
		PlayAnimationReverse(EnableAnim);
	}
}

void UTitleButton::SetButtonText(const FText& NewText)
{
	ButtonText->SetText(NewText);
}

void UTitleButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button->OnClicked.AddDynamic(this, &UTitleButton::OnButtonClicked);
}

void UTitleButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetButtonText(ContentText);
}

void UTitleButton::NativeConstruct()
{
	Super::NativeConstruct();
	if (GetIsEnabled())
	{
		SetAnimationCurrentTime(EnableAnim, 0.5f);
	}
	else
	{
		SetAnimationCurrentTime(EnableAnim, 0.f);
	}
}

void UTitleButton::OnButtonClicked()
{
	OnClicked.ExecuteIfBound();
}
