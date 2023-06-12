// Copyright Epic Games, Inc. All Rights Reserved.

#include "TapAchievementModule.h"
#include "TapAchievementCommon.h"
#include "TULanguage.h"


FTapAchievementsPtr FTapAchievementModule::GetAchievementInterface()
{
	if (FTapAchievementModule* Module = FModuleManager::GetModulePtr<FTapAchievementModule>("TapAchievement"))
	{
		return Module->TapAchievement;
	}
	return nullptr;
}

void FTapAchievementModule::StartupModule()
{
	TapAchievement = MakeShared<FTapAchievementCommon, ESPMode::ThreadSafe>();
	TapAchievement->SyncLoadClass();
	GAchievementLanguage = new FAchievementLanguage();
	GAchievementLanguage->UpdateLanguage(TULanguage::GetCurrentType());
	TULanguage::OnLanguageChanged().AddRaw(this, &FTapAchievementModule::OnLanguageChanged);
}

void FTapAchievementModule::ShutdownModule()
{
	delete(GAchievementLanguage);
	GAchievementLanguage = nullptr;
	TapAchievement.Reset();
}

void FTapAchievementModule::OnLanguageChanged(ELanguageType OldType, ELanguageType NewType)
{
	if (GAchievementLanguage)
	{
		GAchievementLanguage->UpdateLanguage(NewType);
	}
}

IMPLEMENT_MODULE(FTapAchievementModule, TapAchievement)