// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGBlueprintLibrary.h"
#include "ActionRPGLoadingScreen.h"
#include "Interface/TdsInterface.h"


URPGBlueprintLibrary::URPGBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URPGBlueprintLibrary::PlayLoadingScreen(bool bPlayUntilStopped, float PlayTime)
{
	IActionRPGLoadingScreenModule& LoadingScreenModule = IActionRPGLoadingScreenModule::Get();
	LoadingScreenModule.StartInGameLoadingScreen(bPlayUntilStopped, PlayTime);
}

void URPGBlueprintLibrary::StopLoadingScreen()
{
	IActionRPGLoadingScreenModule& LoadingScreenModule = IActionRPGLoadingScreenModule::Get();
	LoadingScreenModule.StopInGameLoadingScreen();
}

bool URPGBlueprintLibrary::IsInEditor()
{
	return GIsEditor;
}

bool URPGBlueprintLibrary::EqualEqual_RPGItemSlot(const FRPGItemSlot& A, const FRPGItemSlot& B)
{
	return A == B;
}

bool URPGBlueprintLibrary::NotEqual_RPGItemSlot(const FRPGItemSlot& A, const FRPGItemSlot& B)
{
	return A != B;
}

bool URPGBlueprintLibrary::IsValidItemSlot(const FRPGItemSlot& ItemSlot)
{
	return ItemSlot.IsValid();
}

bool URPGBlueprintLibrary::DoesEffectContainerSpecHaveEffects(const FRPGGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidEffects();
}

bool URPGBlueprintLibrary::DoesEffectContainerSpecHaveTargets(const FRPGGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidTargets();
}

FRPGGameplayEffectContainerSpec URPGBlueprintLibrary::AddTargetsToEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	FRPGGameplayEffectContainerSpec NewSpec = ContainerSpec;
	NewSpec.AddTargets(HitResults, TargetActors);
	return NewSpec;
}

TArray<FActiveGameplayEffectHandle> URPGBlueprintLibrary::ApplyExternalEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of gameplay effects
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			// If effect is valid, iterate list of targets and apply to all
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}
	return AllEffects;
}

FString URPGBlueprintLibrary::GetProjectVersion()
{
	FString ProjectVersion;

	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		ProjectVersion,
		GGameIni
	);

	return ProjectVersion;
}

bool URPGBlueprintLibrary::IsValidItemData(const FRPGItemData& InData)
{
	return InData.IsValid();
}

void URPGBlueprintLibrary::NotifyAchievementBuyItem(URPGItem* Item, int32 Number)
{
	if (GTdsInterface)
	{
		if (Item->ItemType == URPGAssetManager::PotionItemType && Number > 0)
		{
			GTdsInterface->AchievementReach(TEXT("potion"));
		}
		else if (Item->ItemType == URPGAssetManager::SkillItemType && Number > 0)
		{
			GTdsInterface->AchievementReach(TEXT("skill"));
		}
		else if (Item->ItemType == URPGAssetManager::WeaponItemType && Number > 0)
		{
			GTdsInterface->AchievementReach(TEXT("weapon"));
		}
	}
}
