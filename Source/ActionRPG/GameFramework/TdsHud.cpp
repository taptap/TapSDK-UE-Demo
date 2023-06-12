// Copyright Epic Games, Inc. All Rights Reserved.


#include "TdsHud.h"

#include "UI/Common/MessageBox.h"


ATdsHud::ATdsHud()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ATdsHud::RemoveMenu(const UObject* WorldContextObject, ETapMenuType Type, bool bDestroy)
{
	if (ATdsHud* Hud = GetHud<ATdsHud>(WorldContextObject))
	{
		if (FTapMenuData* DataP = Hud->MenuMap.Find(Type))
		{
			if (DataP->Menu)
			{
				DataP->Menu->RemoveFromParent();
				if (bDestroy)
				{
					DataP->Menu = nullptr;
				}
				return true;
			}
		}
	}
	return false;
}

UUserWidget* ATdsHud::K2_ShowMenu(const UObject* WorldContextObject, ETapMenuType Type, int32 ZOrder)
{
	return ShowMenu(WorldContextObject, Type, ZOrder);
}

UUserWidget* ATdsHud::K2_FindMenu(const UObject* WorldContextObject, ETapMenuType Type)
{
	return FindMenu(WorldContextObject, Type);
}

UClass* ATdsHud::FindMenuClass(ETapMenuType Type)
{
#if PLATFORM_IOS || PLATFORM_ANDROID
	if (Type == ETapMenuType::GameHud)
	{
		return MobileHudClass;
	}
#endif
	if (FTapMenuData* DataP = MenuMap.Find(Type))
	{
		return DataP->Class;
	}
	return nullptr;
}

UMessageBox* ATdsHud::ShowMessage(const UObject* WorldContextObject, const FText& Content)
{
	if (ATdsHud* Hud = GetHud(WorldContextObject))
	{
		if (ensure(Hud->MessageBoxClass))
		{
			if (UMessageBox* UI = CreateWidget<UMessageBox>(Hud->GetWorld(), Hud->MessageBoxClass))
			{
				UI->UpdateBox(Content);
				UI->AddToViewport(100);
				return UI;
			}
		}
	}
	return nullptr;
}
