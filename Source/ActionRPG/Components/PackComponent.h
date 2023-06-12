// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Components/ActorComponent.h"
#include "PackComponent.generated.h"

class URPGAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FItemChangedDelegate, const FRPGItemSlot&, Slot, const FRPGItemData&, OldItem, const FRPGItemData&, NewItem, bool, bInitialize);

UCLASS()
class ACTIONRPG_API UPackComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPackComponent();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool ChangeItemsServer(const FRPGItemData& DeltaItem);

	UFUNCTION(BlueprintCallable)
	const FRPGItemData& GetItemDataAtSlot(FRPGItemSlot Slot) const;

	UFUNCTION(BlueprintCallable)
	const FRPGItemData& GetItemDataByItem(URPGItem* Item) const;

	UFUNCTION(BlueprintCallable)
	TArray<FRPGItemData> GetItemsByType(FPrimaryAssetType ItemType) const;

	UFUNCTION(BlueprintPure)
	int32 FindNextWeaponIndex(int32 CurrentIndex) const;

	FGameplayAbilitySpecHandle GetAbilityHandleAtSlot(const FRPGItemSlot& Slot);
	
	UPROPERTY(ReplicatedUsing = "OnRep_Items", EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TArray<FRPGItemData> Items;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FItemChangedDelegate OnItemChanged;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void InitializeComponent() override;

	void HandleItemChanged(const FRPGItemSlot& Slot, const FRPGItemData& OldItem, const FRPGItemData& NewItem, bool bInitialize);
	
	int32 SlotToIndex(const FRPGItemSlot& Slot) const;

	FRPGItemSlot IndexToSlot(int32 Index) const;
	
	UFUNCTION()
	void OnRep_Items(const TArray<FRPGItemData>& OldItems);

	void OnNewPawn(APawn* NewPawn);
	
	UPROPERTY(EditAnywhere)
	TArray<FRPGItemSlot> SlotDefines;

	UPROPERTY(Replicated)
	TArray<FGameplayAbilitySpecHandle> ItemAbilityHandles;

	URPGAbilitySystemComponent* GetAbilityComponent() const;
};
