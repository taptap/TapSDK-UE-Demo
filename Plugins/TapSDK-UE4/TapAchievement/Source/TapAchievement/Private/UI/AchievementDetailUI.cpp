// Fill out your copyright notice in the Description page of Project Settings.


#include "AchievementDetailUI.h"

#include "TapAchievementCommon.h"
#include "TapAchievementModule.h"
#include "TapAchievementTypes.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "TapAchievement"

void UAchievementDetailUI::UpdateDetailUI(const FAchievementDescTap& Description, const FAchievementTap& InAchievement)
{
	Super::UpdateDetailUI(Description, InAchievement);

	if (UTexture2D* Tex = FTapAchievementModule::GetAchievementInterface()->GetApplicationIcon())
	{
		if (UMaterialInstanceDynamic* DM = ProjectIcon->GetDynamicMaterial())
		{
			DM->SetTextureParameterValue(TEXT("Texture"), Tex);
		}
	}
	else if (const FSlateBrush* Brush = FSlateApplicationBase::Get().GetAppIcon())
	{
		ProjectIcon->SetBrush(*Brush);
	}
	
	FText ProjectName = FTapAchievementModule::GetAchievementInterface()->GetApplicationName();
	if (ProjectName.IsEmpty())
	{
		ProjectName = FText::FromString(FApp::GetProjectName());
	}
	TB_ProjectName->SetText(ProjectName);
	
	if (const FTapAchievementsPtr AchievementsPtr = FTapAchievementModule::GetAchievementInterface())
	{
		const int32 AllAchievementsNumber = AchievementsPtr->GetLocalAllAchievementList().Num();
		int32 UnlockAchievementsNumber = 0;
		for (const FAchievementTap& EachAchievement : AchievementsPtr->GetLocalUserAchievementList())
		{
			if (EachAchievement.bFullCompleted)
			{
				++UnlockAchievementsNumber;
			}
		}
		const FText ProgressText = FText::Format(GAchievementLanguage->DialogStatus,
			FText::AsNumber(UnlockAchievementsNumber, &FNumberFormattingOptions::DefaultNoGrouping()),
			FText::AsNumber(AllAchievementsNumber, &FNumberFormattingOptions::DefaultNoGrouping()));
		TB_AchievementProgress->SetText(ProgressText);
	}

	if (Description.Level == 0 || Description.RarityD == 0.0 || (!Achievement.bFullCompleted && Description.bIsHide))
	{
		TB_Rarity->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		TB_Rarity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (Description.Rarity < 0.001f)
		{
			TB_Rarity->SetText(GAchievementLanguage->DialogPercentLow);
		}
		else
		{
			FNumberFormattingOptions Options = FNumberFormattingOptions().SetUseGrouping(false).SetMaximumFractionalDigits(1);
			const FText RarityText = FText::Format(GAchievementLanguage->DialogPercent, FText::AsNumber(Description.Rarity * 100.f, &Options));
			TB_Rarity->SetText(RarityText);
		}
	}

	if (Achievement.bFullCompleted)
	{
		FNumberFormattingOptions Options = FNumberFormattingOptions().SetUseGrouping(false).SetMinimumIntegralDigits(2);
		const FText TimeText = FText::Format(GAchievementLanguage->DialogEarnedDate,
		FText::AsNumber(Achievement.CompleteTime.GetYear(), &Options),
		FText::AsNumber(Achievement.CompleteTime.GetMonth(), &Options),
		FText::AsNumber(Achievement.CompleteTime.GetDay(), &Options));
		TB_CompleteTime->SetText(TimeText);
		TB_CompleteTime->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		TB_CompleteTime->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UAchievementDetailUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BTN_Close->OnClicked.AddDynamic(this, &UAchievementDetailUI::RemoveFromParent);
}


#undef LOCTEXT_NAMESPACE
