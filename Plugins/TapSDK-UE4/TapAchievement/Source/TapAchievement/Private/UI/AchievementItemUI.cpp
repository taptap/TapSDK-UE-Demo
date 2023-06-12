// Fill out your copyright notice in the Description page of Project Settings.


#include "AchievementItemUI.h"

#include "TapAchievementCommon.h"
#include "TapAchievementModule.h"
#include "TapAchievementTypes.h"
#include "TapCommon.h"
#include "TUSettings.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2DDynamic.h"
#include "UI/AchievementDetailUI.h"


void UAchievementItemUI::UpdateDetailUI(const FAchievementDescTap& Description, const FAchievementTap& InAchievement)
{
	Desc = Description;
	Achievement = InAchievement;
	
	if (Description.bIsHide)
	{
		if (UMaterialInstanceDynamic* DMat = Icon->GetDynamicMaterial())
		{
			DMat->SetTextureParameterValue(TEXT("Texture"), HiddenTexture);
		}
	}
	else if(UTexture2D* IconTex = Description.IconTexture)
	{
		if (UMaterialInstanceDynamic* DMat = Icon->GetDynamicMaterial())
		{
			DMat->SetTextureParameterValue(TEXT("Texture"), IconTex);
		}
	}
	else
	{
		FTapCommonModule::AsyncDownloadImage(Description.AchievementIconUrl, FAsyncDownloadImage::CreateUObject(this, &UAchievementItemUI::OnDownloadIconComplete));
	}

	LevelBG2->SetVisibility(Description.Level == 4 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	
	switch (Desc.Level)
	{
	case 1:
		TB_Level->SetText(GAchievementLanguage->RarityCommon);
		break;
	case 2:
		TB_Level->SetText(GAchievementLanguage->RarityUncommon);
		break;
	case 3:
		TB_Level->SetText(GAchievementLanguage->RarityRare);
		break;
	case 4:
		TB_Level->SetText(GAchievementLanguage->RarityUltraRare);
		break;
	default:
		break;
	}
	if (const FTapAchievementLevelSetting* SettingP = LevelTextureSettings.Find(Description.Level))
	{
		LevelBG->SetBrushFromTexture(SettingP->BackgroundTexture);
		LevelL->SetBrushFromTexture(SettingP->LeftTexture);
		LevelR->SetBrushFromTexture(SettingP->RightTexture);
	}
	if (Description.bIsHide)
	{
		TB_Title->SetText(GAchievementLanguage->InvisibleTitle);
		TB_Subtitle->SetText(GAchievementLanguage->InvisibleSubtitle);
	}
	else
	{
		TB_Title->SetText(FText::FromString(Description.AchievementTitle));
		TB_Subtitle->SetText(FText::FromString(Description.AchievementSubtitle));
	}

	if (Desc.IsPlatinum())
	{
		if (UMaterialInstanceDynamic* DMat = Icon->GetDynamicMaterial())
		{
			DMat->SetScalarParameterValue(TEXT("Platinum"), 1);
		}
		PlatinumIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		if (UMaterialInstanceDynamic* DMat = Icon->GetDynamicMaterial())
		{
			DMat->SetScalarParameterValue(TEXT("Platinum"), 0);
		}
		PlatinumIcon->SetVisibility(ESlateVisibility::Hidden);
	}
	float Percentage = Description.CountStep > 0 ? static_cast<float>(Achievement.CompleteStep) / static_cast<float>(Description.CountStep) : 0.f;

	if (Achievement.CompleteStep != 0 && !Achievement.bFullCompleted && !Desc.bIsHide)
	{

		FNumberFormattingOptions Options = FNumberFormattingOptions()
			.SetMinimumFractionalDigits(0)
			.SetMaximumFractionalDigits(0)
			.SetUseGrouping(false);
		const FText PercentageText = FText::AsNumber(Percentage * 100.f, &Options);
		
		TB_Percentage->SetText(PercentageText);
		PercentagePanel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PercentagePanel->SetVisibility(ESlateVisibility::Hidden);
	}

	if (UMaterialInstanceDynamic* DMat = Icon->GetDynamicMaterial())
	{
		if (Achievement.CompleteStep == 0)
		{
			Percentage = 0.f;
		}
		else
		{
			Percentage = Desc.bIsHide ? 1.f : FMath::GetMappedRangeValueClamped(TRange<float>(0.f, 1.f), TRange<float>(BaseProgress, 1.f), Percentage);
		}
		DMat->SetScalarParameterValue(TEXT("Alpha"), Desc.bIsHide ? 1.f : Percentage);
	}
}

void UAchievementItemUI::OnDetailButtonClicked()
{
	if (const FTapAchievementsPtr AchievementsPtr = FTapAchievementModule::GetAchievementInterface())
	{
		if (const FAchievementDescTap* DescP = AchievementsPtr->FindAchievementDescriptionById(Desc.DisplayId))
		{
			if (UAchievementItemUI* UI = CreateWidget<UAchievementItemUI>(this, DetailUIClass))
			{
				const FAchievementTap* AchievementP = AchievementsPtr->FindAchievementById(Desc.DisplayId);
				
				UI->UpdateDetailUI(*DescP, AchievementP ? *AchievementP : FAchievementTap());
				UI->AddToViewport(TUSettings::GetUILevel() + 1);
			}
		}
	}
}

void UAchievementItemUI::OnDownloadIconComplete(UTexture2D* Texture)
{
	if (UMaterialInstanceDynamic* DMat = Icon->GetDynamicMaterial())
	{
		DMat->SetTextureParameterValue(TEXT("Texture"), Texture);
	}
}

void UAchievementItemUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (BTN_Detail)
	{
		BTN_Detail->OnClicked.AddDynamic(this, &UAchievementItemUI::OnDetailButtonClicked);
	}

	PlatinumIcon->SetVisibility(ESlateVisibility::Hidden);

#if ENGINE_MAJOR_VERSION > 4
	TB_Title->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
	TB_Subtitle->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
#endif
}


