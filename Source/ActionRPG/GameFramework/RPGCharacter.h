// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActionRPG.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Abilities/RPGAbilitySystemComponent.h"
#include "Abilities/RPGAttributeSet.h"
#include "GenericTeamAgentInterface.h"
#include "RPGCharacter.generated.h"

class URPGGameplayAbility;
class UGameplayEffect;
class AWeaponActor;

/** Base class for Character, Designed to be blueprinted */
UCLASS()
class ACTIONRPG_API ARPGCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface, public IGameplayCueInterface
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	ARPGCharacter();

	UFUNCTION(BlueprintCallable)
	float GetNumericAttribute(FGameplayAttribute Attribute);

	/** Returns current health, will be 0 if dead */
	UFUNCTION(BlueprintCallable)
	virtual float GetHealth() const;

	/** Returns maximum health, health will never be greater than this */
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const;

	/** Returns current mana */
	UFUNCTION(BlueprintCallable)
	virtual float GetMana() const;

	/** Returns maximum mana, mana will never be greater than this */
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxMana() const;

	/** Returns current movement speed */
	UFUNCTION(BlueprintCallable)
	virtual float GetMoveSpeed() const;

	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintCallable)
	virtual int32 GetCharacterLevel() const;

	/** Modifies the character level, this may change abilities. Returns true on success */
	UFUNCTION(BlueprintCallable)
	virtual bool SetCharacterLevel(int32 NewLevel);

	/**
	 * Attempts to activate any ability in the specified item slot. Will return false if no activatable ability found or activation fails
	 * Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	 * If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilitiesWithItemSlot(FRPGItemSlot ItemSlot, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities bound to the item slot. This only returns if the ability is currently running */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GetActiveAbilitiesWithItemSlot(FRPGItemSlot ItemSlot, TArray<URPGGameplayAbility*>& ActiveAbilities);

	/**
	 * Attempts to activate all abilities that match the specified tags
	 * Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	 * If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities matching the specified tags. This only returns if the ability is currently running */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities);

	/** Returns total time and remaining time for cooldown tags. Returns false if no active cooldowns found */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool CanUseAnyAbility() const;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool IsUsingSkill() const;

	UFUNCTION(BlueprintCallable, Category = "Abilities", DisplayName="Execute GameplayCue")
	virtual void K2_ExecuteGameplayCue(FGameplayTag GameplayCueTag, FGameplayEffectContextHandle Context);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SwitchWeapon();
	
	UFUNCTION(BlueprintPure)
	FRPGItemSlot GetCurrentWeaponSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetDefaultWeapon(AWeaponActor* Weapon);

	UFUNCTION(BlueprintPure)
	bool IsAlive() const;
	
	void UpdateDefaultPassive();

protected:
	/** Character Interface Start */
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	/** Cue Interface Start */
	virtual void GetGameplayCueSets(TArray<UGameplayCueSet*>& OutSets) const override { OutSets = CueSets;}
	
	/** Called from RPGAttributeSet, these call BP events above */
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacter* InstigatorCharacter, AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags, AActor* Causer);
	virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	
	/** Required to support AIPerceptionSystem */
	virtual FGenericTeamId GetGenericTeamId() const override;

	/** Implement IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** The component used to handle ability system interactions */
	UPROPERTY(BlueprintReadOnly)
	URPGAbilitySystemComponent* AbilitySystemComponent;
	
	/** List of attributes modified by the ability system */
	UPROPERTY()
	URPGAttributeSet* AttributeSet;
	
	UPROPERTY(BlueprintReadOnly)
	uint8 bAlive : 1;

	/** 护盾 */
	UPROPERTY(BlueprintReadOnly)
	uint8 bShield : 1;
	
	/** 无敌 */
	UPROPERTY(BlueprintReadOnly)
	uint8 bInvincible : 1;
	
	/** 眩晕 */
	UPROPERTY(BlueprintReadOnly)
	uint8 bStunned : 1;

	/** The level of this character, should not be modified directly once it has already spawned */
	UPROPERTY(EditAnywhere, Replicated, Category = Abilities)
	int32 CharacterLevel;

	UPROPERTY(ReplicatedUsing = "OnRep_CurrentWeapon", BlueprintReadOnly)
	AWeaponActor* CurrentWeapon;

	UPROPERTY(ReplicatedUsing = "OnRep_CurrentWeaponIndex", BlueprintReadOnly)
	int32 CurrentWeaponIndex = INDEX_NONE;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TArray<UGameplayCueSet*> CueSets;

	/** Map of item slot to gameplay ability class, these are bound before any abilities added by the inventory */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TMap<FRPGItemSlot, TSubclassOf<URPGGameplayAbility>> DefaultSlottedAbilities;

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacter* InstigatorCharacter, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags, AActor* Causer);

	UFUNCTION()
	void OnRep_CurrentWeapon();

	UFUNCTION()
	void OnRep_CurrentWeaponIndex();
	
	friend URPGAttributeSet;
};
