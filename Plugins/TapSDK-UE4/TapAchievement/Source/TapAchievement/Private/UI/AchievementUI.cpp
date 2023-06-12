// Fill out your copyright notice in the Description page of Project Settings.


#include "AchievementUI.h"

#include "TapAchievementCommon.h"
#include "TapAchievementModule.h"
#include "AchievementItemUI.h"
#include "TULanguage.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/PanelWidget.h"
#include "Components/WidgetSwitcher.h"

float UAchievementUI::GetOffset() const
{
	return ScrollPanel->GetScrollOffset();
}

void UAchievementUI::SetOffset(float NewOffset)
{
	ScrollPanel->SetScrollOffset(NewOffset);
	OnScroll(ScrollPanel->GetScrollOffset());
}

void UAchievementUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	BTN_Close->OnClicked.AddDynamic(this, &UAchievementUI::RemoveFromParent);
	if (BTN_Close_Hidden)
	{
		BTN_Close_Hidden->OnClicked.AddDynamic(this, &UAchievementUI::RemoveFromParent);
	}
	if (const FTapAchievementsPtr Interface = FTapAchievementModule::GetAchievementInterface())
	{
		Interface->OnAchievementStatusUpdate.AddUObject(this, &UAchievementUI::OnAchievementStatusUpdate);
	}

	if (DeviceType != EAchievementDeviceType::PC)
	{
		ScrollPanel->OnUserScrolled.AddDynamic(this, &UAchievementUI::OnScroll);
	}
}

void UAchievementUI::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!IsDesignTime())
	{
		if (const FTapAchievementsPtr Interface = FTapAchievementModule::GetAchievementInterface())
		{
			Descriptions = Interface->GetLocalAllAchievementList();
			Achievements = Interface->GetLocalUserAchievementList();
		}
	}
	
	ItemUIMap.Reset();

	int32 UnlockCounter = 0;
	for (const FAchievementTap& Achievement : Achievements)
	{
		if (Achievement.bFullCompleted)
		{
			++UnlockCounter;
		}
	}
	TB_UnlockedAchievementNumber->SetText(FText::AsNumber(UnlockCounter, &FNumberFormattingOptions::DefaultNoGrouping()));
	TB_AllAchievementNumber->SetText(FText::AsNumber(Descriptions.Num(), &FNumberFormattingOptions::DefaultNoGrouping()));

	if (TULanguage::GetCurrentType() == ELanguageType::ZH && !bOrientationPortrait)
	{
		if (LockSwitcher)
		{
			LockSwitcher->SetActiveWidgetIndex(1);
		}
		if (UnlockSwitcher)
		{
			UnlockSwitcher->SetActiveWidgetIndex(1);
		}
		if (UnlockLabel_ZH)
		{
			UnlockLabel_ZH->SetText(GAchievementLanguage->Unlocked);
		}
		if (LockLabel_ZH)
		{
			LockLabel_ZH->SetText(GAchievementLanguage->Locked);
		}
	}
	else
	{
		if (LockSwitcher)
		{
			LockSwitcher->SetActiveWidgetIndex(0);
		}
		if (UnlockSwitcher)
		{
			UnlockSwitcher->SetActiveWidgetIndex(0);
		}
		UnlockLabel->SetText(GAchievementLanguage->Unlocked);
		LockLabel->SetText(GAchievementLanguage->Locked);
	}
	
	for (const FAchievementDescTap& Desc : Descriptions)
	{
		auto Pred = [&Desc](const FAchievementTap& Achievement)
		{
			return Achievement.DisplayId == Desc.DisplayId;
		};
		const FAchievementTap* AchievementP = Achievements.FindByPredicate(Pred);

		if (UAchievementItemUI* UI = CreateWidget<UAchievementItemUI>(this, ItemUIClass))
		{
			UI->UpdateDetailUI(Desc, AchievementP ? *AchievementP : FAchievementTap(Desc));
			ItemUIMap.Add(Desc.DisplayId, UI);
		}
	}

	UpdateStyle();
}

void UAchievementUI::UpdateStyle()
{
	UnlockPanel->ClearChildren();
	LockPanel->ClearChildren();
	
	TArray<UUserWidget*> LockArr, UnlockArr;
	for (TTuple<FString, UAchievementItemUI*>& T : ItemUIMap)
	{
		if (T.Value)
		{
			TArray<UUserWidget*>& TargetArr = T.Value->Achievement.bFullCompleted ? UnlockArr : LockArr;
			TargetArr.Add(T.Value);
		}
	}

	int32 SnapNumber = 2;
	switch (DeviceType)
	{
	case EAchievementDeviceType::PC:
		SnapNumber = 4;
		break;
	case EAchievementDeviceType::Tablet:
		SnapNumber = 3;
		break;
	case EAchievementDeviceType::Phone:
		SnapNumber = 2;
		break;
	default:
		ensure(false);
	}

	auto Pred = [](const UUserWidget& A, const UUserWidget& B)
	{
		return CastChecked<UAchievementItemUI>(&A)->Achievement.CompleteTime > CastChecked<UAchievementItemUI>(&B)->Achievement.CompleteTime;
	};
	UnlockArr.StableSort(Pred);
	
	FullWithEmptyItem(LockArr, SnapNumber);
	FullWithEmptyItem(UnlockArr, SnapNumber);
	if (LockArr.Num() == 0)
	{
		LockBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		LockBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		AddWidgetsToPanel(LockPanel, LockArr, SnapNumber);
	}
	if (UnlockArr.Num() == 0)
	{
		UnlockBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UnlockBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		AddWidgetsToPanel(UnlockPanel, UnlockArr, SnapNumber);
	}

	OnScroll(ScrollPanel->GetScrollOffset());
}

void UAchievementUI::FullWithEmptyItem(TArray<UUserWidget*>& Array, int32 SnapNumber)
{
	int32 EmptyNumber = SnapNumber - Array.Num() % SnapNumber;
	if (EmptyNumber == SnapNumber)
	{
		return;
	}
	while (EmptyNumber-- > 0)
	{
		if (UUserWidget* UI = CreateWidget(this, EmptyClass))
		{
			Array.Add(UI);
		}
		else
		{
			ensure(false);
		}
	}
}

void UAchievementUI::AddWidgetsToPanel(UUniformGridPanel* Panel, TArray<UUserWidget*> Widgets, int32 SnapNumber)
{
	if (Widgets.Num() == 0)
	{
		Panel->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Panel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		for (int32 i = 0; i < Widgets.Num(); ++i)
		{
			int32 Row;
			int32 Column;
			if (DeviceType == EAchievementDeviceType::PC || bOrientationPortrait)
			{
				Row = i / SnapNumber;
				Column = i % SnapNumber;
			}
			else
			{
				Row = i % SnapNumber;
				Column = i / SnapNumber;
			}
			Panel->AddChildToUniformGrid(Widgets[i], Row, Column);
		}
	}
}

void UAchievementUI::OnScroll(float CurrentOffset)
{
	if (HeaderSpacer)
	{
#if ENGINE_MAJOR_VERSION > 4
		float Offset = bOrientationPortrait ? HeaderSpacer->GetSize().X : HeaderSpacer->GetSize().Y;
#else
		float Offset = bOrientationPortrait ? HeaderSpacer->Size.X : HeaderSpacer->Size.Y;
#endif
		if (CurrentOffset >= Offset)
		{
			CloseButtonPanel->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CloseButtonPanel->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UAchievementUI::OnAchievementStatusUpdate(const FAchievementDescTap* Desc, const FAchievementTap* Achievement, const TSharedPtr<FTUError>& Error)
{
	if (Desc && Achievement)
	{
		if (UAchievementItemUI* UI = ItemUIMap.FindRef(Desc->DisplayId))
		{
			UI->UpdateDetailUI(*Desc, *Achievement);
		}
	}
}
