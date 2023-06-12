// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/HUD.h"
#include "TdsHud.generated.h"

class UCloseButton;
class UMessageBox;

UENUM(BlueprintType)
enum class ETapMenuType : uint8
{
	None,
	Title,
	PlayerInfo,
	GameHud,
	Leaderboard,
	
	WaveStart,
	WaveEnd,
	GameOver,
};

USTRUCT()
struct FTapMenuData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> Class;

	UPROPERTY()
	UUserWidget* Menu = nullptr;
};

UCLASS()
class ACTIONRPG_API ATdsHud : public AHUD
{
	GENERATED_BODY()
public:
	ATdsHud();
	
	template<typename T = ATdsHud>
	static T* GetHud(const UObject* WorldContextObject);
	
	template<typename T = UUserWidget>
	static T* ShowMenu(const UObject* WorldContextObject, ETapMenuType Type, int32 ZOrder = 0);

	template<typename T = UUserWidget>
	static T* FindMenu(const UObject* WorldContextObject, ETapMenuType Type);
	
	UFUNCTION(BlueprintCallable, Meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static bool RemoveMenu(const UObject* WorldContextObject, ETapMenuType Type, bool bDestroy = true);

	UFUNCTION(BlueprintCallable, DisplayName = "ShowMenu", Meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UUserWidget* K2_ShowMenu(const UObject* WorldContextObject, ETapMenuType Type, int32 ZOrder);

	UFUNCTION(BlueprintPure, DisplayName = "FindMenu", Meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UUserWidget* K2_FindMenu(const UObject* WorldContextObject, ETapMenuType Type);
	
	UClass* FindMenuClass(ETapMenuType Type);

	UFUNCTION(BlueprintCallable, Meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UMessageBox* ShowMessage(const UObject* WorldContextObject, const FText& Content);

	UPROPERTY(EditAnywhere, Category = "Settings")
	TMap<ETapMenuType, FTapMenuData> MenuMap;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TSubclassOf<UMessageBox> MessageBoxClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MobileHudClass;
};

template <typename T>
T* ATdsHud::GetHud(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			return PC->GetHUD<ATdsHud>();
		}
	}
	return nullptr;
}

template <typename T>
T* ATdsHud::ShowMenu(const UObject* WorldContextObject, ETapMenuType Type, int32 ZOrder)
{
	if (ATdsHud* Hud = GetHud<ATdsHud>(WorldContextObject))
	{
		if (FTapMenuData* DataP = Hud->MenuMap.Find(Type))
		{
			if (!DataP->Menu)
			{
				UClass* MenuClass = Hud->FindMenuClass(Type);
				if (ensure(MenuClass))
				{
					DataP->Menu = CreateWidget<T>(Hud->GetWorld(), MenuClass);
				}
			}

			check(DataP->Menu);

			if (!DataP->Menu->IsInViewport())
			{
				DataP->Menu->AddToViewport(ZOrder);
			}
			return Cast<T>(DataP->Menu);
		}
	}
	return nullptr;
}

template <typename T>
T* ATdsHud::FindMenu(const UObject* WorldContextObject, ETapMenuType Type)
{
	if (ATdsHud* Hud = GetHud<ATdsHud>(WorldContextObject))
	{
		if (FTapMenuData* DataP = Hud->MenuMap.Find(Type))
		{
			return Cast<T>(DataP->Menu);
		}
	}
	return nullptr;
}
