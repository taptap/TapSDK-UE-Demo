// Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/Common/MessageBox.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"


void UMessageBox::UpdateBox( const FText& NewContent)
{
	ContentTextBlock->SetText(NewContent);
	PlayAnimation(FadeAnim);
}

void UMessageBox::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (FadeAnim == Animation)
	{
		RemoveFromParent();
	}
}


