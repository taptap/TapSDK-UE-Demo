// Fill out your copyright notice in the Description page of Project Settings.


#include "LeaderboardItemWidget.h"

#include "Common/GameStatic.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/TdsHud.h"
#include "Interface/TdsInterface.h"



void ULeaderboardItemWidget::UpdateWidget(const FLeaderboardData& Data, bool bIsSelf)
{
	SavedData = Data;
	RankTextBlock->SetText(FText::AsNumber(Data.RankIndex + 1, &FNumberFormattingOptions::DefaultNoGrouping()));
	FGameStatic::FindOrDownloadTexture(Data.AvatarUrl,
		FTextureDelegate::CreateWeakLambda(this, [this](UTexture2D* Tex)
		{
			AvatarImage->SetBrushFromTexture(Tex);
		}));
	NickNameTextBlock->SetText(FText::FromString(Data.NickName));
	ValueTextBlock->SetText(FText::AsNumber(Data.Value, &FNumberFormattingOptions::DefaultNoGrouping()));

	UpdateIsSelf(bIsSelf);
}

void ULeaderboardItemWidget::HandleClicked()
{
	if (GTdsInterface)
	{
		GTdsInterface->FetchLeaderboardDetail(
			{
				TEXT("wave_day"),
				TEXT("wave_week"),
				TEXT("wave_month"),
				TEXT("wave_total"),
				TEXT("kill_day"),
				TEXT("kill_week"),
				TEXT("kill_month"),
				TEXT("kill_total"),
			},
			SavedData.ObjectId,
			FLeaderboardDetailDelegate::CreateUObject(this, &ULeaderboardItemWidget::OnDetailDataCallback, SavedData));
	}
}

void ULeaderboardItemWidget::OnDetailDataCallback(const TArray<FLeaderboardStatistic>& Statistics, FLeaderboardData TempData)
{
	TempData.Statistics = Statistics;
	ShowDetailWidget(TempData);
}

void ULeaderboardItemWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button->OnClicked.AddDynamic(this, &ULeaderboardItemWidget::HandleClicked);
}
