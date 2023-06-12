// Copyright Epic Games, Inc. All Rights Reserved.


#include "InfoBarWidget.h"

#include "Common/GameStatic.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/TdsPlayerState.h"
#include "Interface/TdsInterface.h"

void UInfoBarWidget::UpdateWidget(const FTdsPlayer& Player)
{
	NameTextBlock->SetText(FText::FromString(Player.NickName));
	FGameStatic::FindOrDownloadTexture(Player.AvatarUrl, FTextureDelegate::CreateWeakLambda(this, [this](UTexture2D* Tex)
	{
		if (Tex)
		{
			IconImage->SetBrushFromTexture(Tex);
			IconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}));
}

void UInfoBarWidget::UpdateInfoBarOwningPawn(APawn* NewPawn)
{
	SavedPawn = NewPawn;
	if (NewPawn)
	{
		UpdateWithSavedPawn();
	}
	else
	{
		UpdateWidget(FTdsPlayer());
	}
}

void UInfoBarWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	IconImage->SetVisibility(ESlateVisibility::Hidden);
}

void UInfoBarWidget::UpdateWithSavedPawn()
{
	if (SavedPawn.IsValid())
	{
		ATdsPlayerState* PS = SavedPawn->GetPlayerState<ATdsPlayerState>();
		if (PS && PS->GetTdsPlayer().IsValid())
		{
			UpdateWidget(PS->GetTdsPlayer());
			PS->OnTdsPlayerChanged.AddUObject(this, &UInfoBarWidget::UpdateWidget);
		}
		else
		{
			if (UWorld* World = GetWorld())
			{
				FTimerHandle Handle;
				World->GetTimerManager().SetTimer(Handle, this, &UInfoBarWidget::UpdateWithSavedPawn, 0.5f);	
			}
		}
	}
}

