// Fill out your copyright notice in the Description page of Project Settings.


#include "ReachAchievementTipsUI.h"

#include "TapAchievementCommon.h"
#include "TapAchievementModule.h"
#include "TapCommon.h"
#include "Animation/UMGSequencePlayer.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2DDynamic.h"

void UReachAchievementTipsUI::ShowReachAchievement(const FString& DisplayId)
{
	if (FTapAchievementsPtr AchievementsPtr = FTapAchievementModule::GetAchievementInterface())
	{
		if (FAchievementDescTap* DescP = AchievementsPtr->FindAchievementDescriptionById(DisplayId))
		{
			if (DescP->IconTexture)
			{
				FReachTipData Data;
				Data.Texture = DescP->IconTexture;
				Data.Title = DescP->AchievementTitle;
				PushAchievement(MoveTemp(Data));
			}
			else
			{
				FTapCommonModule::AsyncDownloadImage(DescP->AchievementIconUrl, FAsyncDownloadImage::CreateUObject(this, &UReachAchievementTipsUI::OnDownloadIconFinished, DescP->AchievementTitle));
			}
		}
	}
}

void UReachAchievementTipsUI::PushAchievement(FReachTipData&& NewData)
{
	if (ReachAchievementOpenIdQueue.Num() == 0)
	{
		TB_Content->SetText(GAchievementLanguage->PopTitle);
		if (UMaterialInstanceDynamic* DM = Icon->GetDynamicMaterial())
		{
			DM->SetTextureParameterValue(TEXT("Texture"), DefaultText);
		}
		if (UUMGSequencePlayer* Player = PlayAnimationForward(StartAnim))
		{
			Player->OnSequenceFinishedPlaying().RemoveAll(this);
			Player->OnSequenceFinishedPlaying().AddUObject(this, &UReachAchievementTipsUI::OnPlayStartAnimFinished_Loop);
		}
	}

	ReachAchievementOpenIdQueue.Insert(NewData, 0);
}

void UReachAchievementTipsUI::RemoveSelf()
{
	if (UUMGSequencePlayer* Player = PlayAnimationReverse(StartAnim, 3.f))
	{
		Player->OnSequenceFinishedPlaying().RemoveAll(this);
		Player->OnSequenceFinishedPlaying().AddUObject(this, &UReachAchievementTipsUI::OnPlayStartAnimFinished_Remove);
	}
}

void UReachAchievementTipsUI::OnPlayStartAnimFinished_Loop(UUMGSequencePlayer& InPlayer)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(DelayLoopTimer, this, &UReachAchievementTipsUI::LoopShowAchievement, 1.2f);
	}
}

void UReachAchievementTipsUI::OnPlayStartAnimFinished_Remove(UUMGSequencePlayer& InPlayer)
{
	RemoveFromParent();
}

void UReachAchievementTipsUI::OnPlayLoopAnim(UUMGSequencePlayer& InPlayer)
{
	LoopShowAchievement();
}

void UReachAchievementTipsUI::LoopShowAchievement()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	if (ReachAchievementOpenIdQueue.Num() > 0)
	{
		CurrentData = ReachAchievementOpenIdQueue.Pop();
		World->GetTimerManager().SetTimer(DelayLoopTimer, this, &UReachAchievementTipsUI::DelayUpdateIcon, 0.15f);
		
		if (UUMGSequencePlayer* Player = PlayAnimation(LoopAnim))//Loop anim include fade-in(0.15s), fade-out and wait time
		{
			Player->OnSequenceFinishedPlaying().RemoveAll(this);
			Player->OnSequenceFinishedPlaying().AddUObject(this, &UReachAchievementTipsUI::OnPlayLoopAnim);
		}
	}
	else
	{
		RemoveSelf();
	}
}

void UReachAchievementTipsUI::DelayUpdateIcon()
{
	TB_Content->SetText(FText::FromString(CurrentData.Title));
	if (CurrentData.Texture)
	{
		if (UMaterialInstanceDynamic* DM = Icon->GetDynamicMaterial())
		{
			DM->SetTextureParameterValue(TEXT("Texture"), CurrentData.Texture);
		}
	}
}

void UReachAchievementTipsUI::OnDownloadIconFinished(UTexture2D* Tex, FString Title)
{
	FReachTipData Data;
	if (Tex)
	{
		Data.Texture = Tex;
	}
	else
	{
		Data.Texture = DefaultText; 
	}
	Data.Title = Title;
	PushAchievement(MoveTemp(Data));
}

void UReachAchievementTipsUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
#if ENGINE_MAJOR_VERSION > 4
	TB_Content->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
#endif
}
