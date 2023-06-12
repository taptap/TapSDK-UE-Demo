// Copyright Epic Games, Inc. All Rights Reserved.


#include "LeaderboardWidget.h"

#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "GameFramework/RPGGameInstance.h"
#include "UI/Info/LeaderboardItemWidget.h"

void ULeaderboardWidget::ChangeCategory(ELeaderboardCategory InCategory)
{
	ChangeSelection(InCategory, Type);
}

void ULeaderboardWidget::ChangeType(ELeaderboardType InType)
{
	ChangeSelection(Category, InType);
}

void ULeaderboardWidget::ChangeSelection(ELeaderboardCategory InCategory, ELeaderboardType InType)
{
	if (GTdsInterface)
	{
		UpdateLoadingState(true);

		if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
		{
			if (TSharedPtr<FTdsPlayer> Player = GI->GetTdsPlayer())
			{
				GTdsInterface->FetchLeaderboardAround(
					{MakeLeaderboardFullKeyName(InCategory, InType)},
					Player->Id,
					1,
					FLeaderboardDelegate::CreateUObject(this, &ULeaderboardWidget::OnSelfPlayerCallback, InCategory, InType));
			}
		}
	}
}

void ULeaderboardWidget::GotoPage(ELeaderboardCategory InCategory, ELeaderboardType InType, int32 NewPageIndex)
{
	if (GTdsInterface && NewPageIndex >=0 && NewPageIndex <= GetMaxPageIndex())
	{
		UpdateLoadingState(true);

		CurrentPageIndex = NewPageIndex;
		
		GTdsInterface->FetchLeaderboardSkip(
			{MakeLeaderboardFullKeyName(InCategory, InType)},
			NewPageIndex * PageSize,
			PageSize,
			FLeaderboardDelegate::CreateUObject(this, &ULeaderboardWidget::OnRangeDataCallback, InCategory, InType));
	}
	else
	{
		OnRangeDataCallback({}, 0, InCategory, InType);
	}

	PageEditableTextBox->SetText(FText::AsNumber(CurrentPageIndex + 1, &FNumberFormattingOptions::DefaultNoGrouping()));
}

void ULeaderboardWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	for (int32 i = 0; i < PageSize; ++i)
	{
		ULeaderboardItemWidget* UI = CreateWidget<ULeaderboardItemWidget>(this, ItemClass);
		ItemBox->AddChild(UI);
	}

	ChangeSelection(Category, Type);
}

void ULeaderboardWidget::OnSelfPlayerCallback(const TArray<FLeaderboardData>& RankData, int64 Count, ELeaderboardCategory InCategory, ELeaderboardType InType)
{
	TotalCount = Count;
	if (RankData.IsValidIndex(0) && GTdsInterface)
	{
		SelfObjectId = RankData[0].ObjectId;
		
		int32 PageIndex = RankData[0].RankIndex / PageSize;
		
		GotoPage(InCategory, InType, PageIndex);
	}
	else
	{
		GotoPage(InCategory, InType, 0);
	}
}

void ULeaderboardWidget::OnRangeDataCallback(const TArray<FLeaderboardData>& RankData, int64 Count, ELeaderboardCategory InCategory, ELeaderboardType InType)
{
	UpdateLoadingState(false);

	UpdateCategorySelection(Category, false);
	UpdateTypeSelection(Type, false);
	
	Category = InCategory;
	Type = InType;
	
	UpdateCategorySelection(Category, true);
	UpdateTypeSelection(Type, true);

	ULeaderboardItemWidget* SelfUI = nullptr;
	for (int32 i = 0; i < ItemBox->GetChildrenCount(); ++i)
	{
		// @TODO 需要在这里记录加载的AvatarIcon，等待Widget关闭的时候统一释放

		if (ULeaderboardItemWidget* UI = Cast<ULeaderboardItemWidget>(ItemBox->GetChildAt(i)))
		{
			if (RankData.IsValidIndex(i))
			{
				bool bIsSelf = RankData[i].ObjectId == SelfObjectId;
				UI->UpdateWidget(RankData[i], bIsSelf);
				if (bIsSelf)
				{
					SelfUI = UI;
				}
				UI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				UI->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

	if (SelfUI)
	{
		ItemBox->ScrollWidgetIntoView(SelfUI);
	}
}