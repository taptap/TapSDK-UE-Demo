// Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/Widgets/TitleButton.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"


void UTitleButton::SetButtonText(const FText& NewText)
{
	ButtonText->SetText(NewText);
}

void UTitleButton::UpdateRedDot(bool bHasRedDot)
{
	if(!RedDotAnim) return;
	
	if (bHasRedDot)
	{
		PlayAnimation(RedDotAnim, 0.f, 0);
	}
	else
	{
		StopAnimation(RedDotAnim);
	}
}

void UTitleButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button->OnClicked.AddDynamic(this, &UTitleButton::OnButtonClickedEvent);
}

void UTitleButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetButtonText(ContentText);
}


void UTitleButton::OnButtonClickedEvent()
{
	OnClicked.Broadcast();
}
