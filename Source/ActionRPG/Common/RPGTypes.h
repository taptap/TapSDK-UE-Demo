// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is for enums and structs used by classes and blueprints accross the game
// Collecting these in a single header helps avoid problems with recursive header includes
// It's also a good place to put things like data table row structs
// ----------------------------------------------------------------------------------------------------------------

#include "Items/RPGItem.h"
#include "UObject/PrimaryAssetId.h"
#include "RPGTypes.generated.h"

class URPGItem;
class URPGSaveGame;

/** Struct representing a slot for an item, shown in the UI */
USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGItemSlot
{
	GENERATED_BODY()

	/** Constructor, -1 means an invalid slot */
	FRPGItemSlot()
		: SlotNumber(-1)
	{}

	FRPGItemSlot(const FPrimaryAssetType& InItemType, int32 InSlotNumber)
		: ItemType(InItemType)
		, SlotNumber(InSlotNumber)
	{}

	/** The type of items that can go in this slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

	/** The number of this slot, 0 indexed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 SlotNumber;

	/** Equality operators */
	bool operator==(const FRPGItemSlot& Other) const
	{
		return ItemType == Other.ItemType && SlotNumber == Other.SlotNumber;
	}
	bool operator!=(const FRPGItemSlot& Other) const
	{
		return !(*this == Other);
	}

	/** Implemented so it can be used in Maps/Sets */
	friend inline uint32 GetTypeHash(const FRPGItemSlot& Key)
	{
		uint32 Hash = 0;

		Hash = HashCombine(Hash, GetTypeHash(Key.ItemType));
		Hash = HashCombine(Hash, (uint32)Key.SlotNumber);
		return Hash;
	}

	/** Returns true if slot is valid */
	bool IsValid() const
	{
		return ItemType.IsValid() && SlotNumber >= 0;
	}
};


/** Extra information about a URPGItem that is in a player's inventory */
USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGItemData
{
	GENERATED_BODY()

	/** Constructor, default to count/level 1 so declaring them in blueprints gives you the expected behavior */
	FRPGItemData()
		: ItemCount(0)
		, ItemLevel(0)
	{}

	FRPGItemData(int32 InItemCount, int32 InItemLevel)
		: ItemCount(InItemCount)
		, ItemLevel(InItemLevel)
	{}

	/** The number of instances of this item in the inventory, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemCount;

	/** The level of this item. This level is shared for all instances, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	const URPGItem* Item = nullptr;

	/** Equality operators */
	bool operator==(const FRPGItemData& Other) const
	{
		return ItemCount == Other.ItemCount && ItemLevel == Other.ItemLevel && Item == Other.Item;
	}
	bool operator!=(const FRPGItemData& Other) const
	{
		return !(*this == Other);
	}

	/** Returns true if count is greater than 0 */
	bool IsValid() const
	{
		return ItemCount > 0 && Item != nullptr;
	}

	/** Append an item data, this adds the count and overrides everything else */
	bool Merge(FRPGItemData& Other)
	{
		if (!Other.Item) return false;
		if (Item != nullptr && Item != Other.Item) return false;

		if (!Item)
		{
			Item = Other.Item;
		}
		
		if (Item->MaxCount <= 0 || ItemCount + Other.ItemCount <= Item->MaxCount)
		{
			ItemCount += Other.ItemCount;
			Other.ItemCount = 0;
		}
		else
		{
			Other.ItemCount -= (Item->MaxCount - ItemCount);
			ItemCount = Item->MaxCount;
		}
		if (ItemCount == 0)
		{
			Item = nullptr;
		}
		return true;
	}

	bool IsSame(const FRPGItemData& Other) const
	{
		return Item != nullptr && Item == Other.Item;
	}

	const static FRPGItemData EmptyItem;
};


