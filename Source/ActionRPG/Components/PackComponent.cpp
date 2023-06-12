// Copyright Epic Games, Inc. All Rights Reserved.


#include "PackComponent.h"

#include "Abilities/RPGAbilitySystemComponent.h"
#include "Abilities/RPGGameplayAbility.h"
#include "GameFramework/GameStateBase.h"
#include "Interface/TdsInterface.h"


UPackComponent::UPackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

bool UPackComponent::ChangeItemsServer(const FRPGItemData& DeltaItem)
{
	ensure(GetOwner()->HasAuthority());
	check(Items.Num() == SlotDefines.Num());
	
	TArray<FRPGItemData> OldItems = Items;
	FRPGItemData TempDelta = DeltaItem;

	for (int32 i = 0; i < SlotDefines.Num(); ++i)
	{
		if (IndexToSlot(i).ItemType == DeltaItem.Item->ItemType)
		{
			if (Items[i].Merge(TempDelta) && TempDelta.ItemCount == 0)
			{
				OnRep_Items(OldItems);
				return true;// @TODO
			}
		}
	}
	return false;
}

const FRPGItemData& UPackComponent::GetItemDataAtSlot(FRPGItemSlot Slot) const
{
	int32 i = SlotToIndex(Slot);
	return Items.IsValidIndex(i) ? Items[i] : FRPGItemData::EmptyItem;
}

const FRPGItemData& UPackComponent::GetItemDataByItem(URPGItem* Item) const
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].Item == Item)
		{
			return Items[i];
		}
	}
	return FRPGItemData::EmptyItem;
}

TArray<FRPGItemData> UPackComponent::GetItemsByType(FPrimaryAssetType ItemType) const
{
	TArray<FRPGItemData> Results;
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].Item && Items[i].Item->ItemType == ItemType)
		{
			Results.Add(Items[i]);
		}
	}
	return Results;
}

int32 UPackComponent::FindNextWeaponIndex(int32 CurrentIndex) const
{
	int32 Result = INDEX_NONE;
	int32 WeaponSlotNumber = 0;
	for (const FRPGItemSlot& Slot : SlotDefines)
	{
		if (Slot.ItemType == URPGAssetManager::WeaponItemType)
		{
			WeaponSlotNumber++;
		}
	}

	int32 FindCount = WeaponSlotNumber;
	while (FindCount-- > 0)
	{
		if (++CurrentIndex >= WeaponSlotNumber)
		{
			CurrentIndex = 0;
		}

		if (GetItemDataAtSlot(FRPGItemSlot(URPGAssetManager::WeaponItemType, CurrentIndex)).IsValid())
		{
			Result = CurrentIndex;
			break;
		}
	}

	return Result;
}

FGameplayAbilitySpecHandle UPackComponent::GetAbilityHandleAtSlot(const FRPGItemSlot& Slot)
{
	int32 i = SlotToIndex(Slot);
	if (ItemAbilityHandles.IsValidIndex(i))
	{
		return ItemAbilityHandles[i];
	}
	return FGameplayAbilitySpecHandle();
}

void UPackComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPackComponent, Items);
	DOREPLIFETIME(UPackComponent, ItemAbilityHandles);
}

void UPackComponent::InitializeComponent()
{
	Super::InitializeComponent();
	Items.SetNum(SlotDefines.Num());
	ItemAbilityHandles.SetNum(SlotDefines.Num());

	if (AController* Con = GetOwner<AController>())
	{
		Con->GetOnNewPawnNotifier().AddUObject(this, &UPackComponent::OnNewPawn);
	}
}

void UPackComponent::HandleItemChanged(const FRPGItemSlot& Slot, const FRPGItemData& OldItem, const FRPGItemData& NewItem, bool bInitialize)
{
	if (GetOwner()->HasAuthority())
	{
		if (OldItem.IsValid() != NewItem.IsValid())
		{
			int32 i = SlotToIndex(Slot);
			if (OldItem.IsValid())
			{
				if (OldItem.Item->GrantedAbility)
				{
					if(URPGAbilitySystemComponent* Comp = GetAbilityComponent())
					{
						if (ItemAbilityHandles.IsValidIndex(i))
						{
							Comp->ClearAbility(ItemAbilityHandles[i]);
						}
						else
						{
							UE_LOG(LogActionRPG, Warning, TEXT("Handle item changed, Remove Ability, invalid slot index: %d"), i);
						}
					}
				}
			}
			if (NewItem.IsValid())
			{
				if (NewItem.Item->GrantedAbility)
				{
					if(URPGAbilitySystemComponent* Comp = GetAbilityComponent())
					{
						if (!ItemAbilityHandles.IsValidIndex(i))
						{
							UE_LOG(LogActionRPG, Warning, TEXT("Handle item changed, Give Ability, invalid slot index: %d"), i);
							ItemAbilityHandles.SetNum(i + 1);
						}
						ItemAbilityHandles[i] = Comp->GiveAbility(FGameplayAbilitySpec(NewItem.Item->GrantedAbility, NewItem.Item->AbilityLevel));
					}
				}
			}
		}
	}

	if (GTdsInterface)
	{
		if (AController* Con = GetOwner<AController>())
		{
			if (Con->IsLocalController())
			{
				if (!bInitialize)
				{
					int32 Delta = NewItem.ItemCount - OldItem.ItemCount;

					if (Slot.ItemType == URPGAssetManager::TokenItemType)
					{
						GTdsInterface->AchievementMakeStep(TEXT("soul20"), NewItem.ItemCount);
						GTdsInterface->AchievementMakeStep(TEXT("soul50"), NewItem.ItemCount);
						GTdsInterface->AchievementMakeStep(TEXT("soul100"), NewItem.ItemCount);

						if (UWorld* World = GetWorld())
						{
							if (AGameStateBase* GS = World->GetGameState())
							{
								if (GS->PlayerArray.Num() > 1)
								{
									GTdsInterface->AchievementGrowStep(TEXT("total_soul_500"), Delta);
									GTdsInterface->AchievementGrowStep(TEXT("total_soul_2000"), Delta);
								}
							}
						}
					}
				}
			}
		}
	}
}

int32 UPackComponent::SlotToIndex(const FRPGItemSlot& Slot) const
{
	for (int32 i = 0; i < SlotDefines.Num(); ++i)
	{
		if (SlotDefines[i] == Slot)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

FRPGItemSlot UPackComponent::IndexToSlot(int32 Index) const
{
	return SlotDefines.IsValidIndex(Index) ? SlotDefines[Index] : FRPGItemSlot();
}

void UPackComponent::OnRep_Items(const TArray<FRPGItemData>& OldItems)
{
	for (int32 i = 0; i < OldItems.Num(); i++)
	{
		check(Items.IsValidIndex(i) && SlotDefines.IsValidIndex(i));
		if (OldItems[i] != Items[i])
		{
			HandleItemChanged(SlotDefines[i], OldItems[i], Items[i], false);
			OnItemChanged.Broadcast(SlotDefines[i], OldItems[i], Items[i], false);
		}
	}
}

void UPackComponent::OnNewPawn(APawn* NewPawn)
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].IsValid())
		{
			HandleItemChanged(IndexToSlot(i), FRPGItemData::EmptyItem, Items[i], true);
		}
	}
}

URPGAbilitySystemComponent* UPackComponent::GetAbilityComponent() const
{
	if (AController* Con = GetOwner<AController>())
	{
		if (APawn* Pawn = Con->GetPawn())
		{
			return Pawn->FindComponentByClass<URPGAbilitySystemComponent>();
		}
	}
	return nullptr;
}
