// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/RPGCharacter.h"
#include "Items/RPGItem.h"
#include "AbilitySystemGlobals.h"
#include "TdsHud.h"
#include "TdsPlayerState.h"
#include "Abilities/RPGGameplayAbility.h"
#include "Components/PackComponent.h"
#include "Components/WidgetComponent.h"
#include "Items/RPGWeaponItem.h"
#include "Items/WeaponActor.h"
#include "UI/Menu/GameHud.h"

ARPGCharacter::ARPGCharacter()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<URPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Create the attribute set, this replicates by default
	AttributeSet = CreateDefaultSubobject<URPGAttributeSet>(TEXT("AttributeSet"));
	
	CharacterLevel = 1;

	bAlive = true;
	bShield = false;
	bInvincible = false;
	bStunned = false;
}

float ARPGCharacter::GetNumericAttribute(FGameplayAttribute Attribute)
{
	return AbilitySystemComponent->GetNumericAttribute(Attribute);
}

float ARPGCharacter::GetHealth() const
{
	if (!AttributeSet)
	return 1.f;

	return AttributeSet->GetHealth();
}

float ARPGCharacter::GetMaxHealth() const
{	
	return AttributeSet->GetMaxHealth();
}

float ARPGCharacter::GetMana() const
{
	return AttributeSet->GetMana();
}

float ARPGCharacter::GetMaxMana() const
{
	return AttributeSet->GetMaxMana();
}

float ARPGCharacter::GetMoveSpeed() const
{
	return AttributeSet->GetMoveSpeed();
}

int32 ARPGCharacter::GetCharacterLevel() const
{
	return CharacterLevel;
}

bool ARPGCharacter::SetCharacterLevel(int32 NewLevel)
{
	if (CharacterLevel != NewLevel && NewLevel > 0)
	{
		CharacterLevel = NewLevel;

		return true;
	}
	return false;
}

bool ARPGCharacter::ActivateAbilitiesWithItemSlot(FRPGItemSlot ItemSlot, bool bAllowRemoteActivation)
{
	if (AController* Con = GetController())
	{
		if (UPackComponent* Comp = Con->FindComponentByClass<UPackComponent>())
		{
			FGameplayAbilitySpecHandle Handle = Comp->GetAbilityHandleAtSlot(ItemSlot);
			if (Handle.IsValid())
			{
				return AbilitySystemComponent->TryActivateAbility(Handle, bAllowRemoteActivation);
			}
		}
	}
	if (TSubclassOf<URPGGameplayAbility>* AbilityClassP = DefaultSlottedAbilities.Find(ItemSlot))
	{
		return AbilitySystemComponent->TryActivateAbilityByClass(*AbilityClassP, bAllowRemoteActivation);
	}
	return false;
}

void ARPGCharacter::GetActiveAbilitiesWithItemSlot(FRPGItemSlot ItemSlot, TArray<URPGGameplayAbility*>& ActiveAbilities)
{
	
}

bool ARPGCharacter::ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

void ARPGCharacter::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

bool ARPGCharacter::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (AbilitySystemComponent && CooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}
	return false;
}

bool ARPGCharacter::CanUseAnyAbility() const
{
	return bAlive && !UGameplayStatics::IsGamePaused(this) && !IsUsingSkill();
}

bool ARPGCharacter::IsUsingSkill() const
{
	TArray<URPGGameplayAbility*> ActiveAbilities;
	AbilitySystemComponent->GetActiveAbilitiesWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("Ability.Skill"))), ActiveAbilities);
	return ActiveAbilities.Num() > 0;
}

void ARPGCharacter::K2_ExecuteGameplayCue(FGameplayTag GameplayCueTag, FGameplayEffectContextHandle Context)
{
	AbilitySystemComponent->ExecuteGameplayCue(GameplayCueTag, Context);
}

void ARPGCharacter::SwitchWeapon_Implementation()
{
	if (GetCurrentMontage())
	{
		return;
	}
	if (Controller)
	{
		if (UPackComponent* Comp = Controller->FindComponentByClass<UPackComponent>())
		{
			int32 NewIndex = Comp->FindNextWeaponIndex(CurrentWeaponIndex);
			if (NewIndex != CurrentWeaponIndex)
			{
				if (CurrentWeapon)
				{
					CurrentWeapon->Destroy();
				}

				CurrentWeaponIndex = NewIndex;

				FRPGItemSlot PendingSlot = FRPGItemSlot(URPGAssetManager::WeaponItemType, CurrentWeaponIndex);
				const FRPGItemData& Item = Comp->GetItemDataAtSlot(PendingSlot);
				if (const URPGWeaponItem* WeaponItem = Cast<URPGWeaponItem>(Item.Item))
				{
					if (ensure(WeaponItem->WeaponActor))
					{
						FActorSpawnParameters Param;
						Param.bDeferConstruction = true;
						Param.Instigator = this;
						CurrentWeapon = GetWorld()->SpawnActor<AWeaponActor>(WeaponItem->WeaponActor, Param);

						CurrentWeapon->WeaponItem = WeaponItem;
						CurrentWeapon->Slot = PendingSlot;

						CurrentWeapon->FinishSpawning(FTransform(FVector(0.f, 0.f, -1000.f)));
					}
				}
				OnRep_CurrentWeapon();
			}
		}
	}
}

FRPGItemSlot ARPGCharacter::GetCurrentWeaponSlot() const
{
	return FRPGItemSlot(URPGAssetManager::WeaponItemType, CurrentWeaponIndex);
}

void ARPGCharacter::SetDefaultWeapon(AWeaponActor* Weapon)
{
	CurrentWeapon = Weapon;
	OnRep_CurrentWeapon();
}

bool ARPGCharacter::IsAlive() const
{
	return AttributeSet->GetHealth() > 0.f;
}

void ARPGCharacter::UpdateDefaultPassive()
{
	for (TSubclassOf<UGameplayEffect>& Effect : PassiveGameplayEffects)
	{
		if (Effect)
		{
			FGameplayEffectQuery Query;
			Query.EffectDefinition = Effect;
			TArray<FActiveGameplayEffectHandle> Handles = AbilitySystemComponent->GetActiveEffects(Query);
			if (Handles.Num() > 0)
			{
				AbilitySystemComponent->SetActiveGameplayEffectLevel(Handles[0], CharacterLevel);
			}
			else
			{
				FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
				AbilitySystemComponent->ApplyGameplayEffectToSelf(Effect->GetDefaultObject<UGameplayEffect>(), CharacterLevel, Context);
			}
		}
	}
}

void ARPGCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ARPGCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ARPGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGCharacter, CharacterLevel);
	DOREPLIFETIME(ARPGCharacter, CurrentWeapon);
	DOREPLIFETIME(ARPGCharacter, CurrentWeaponIndex);
}

void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		UpdateDefaultPassive();

		for (TTuple<FRPGItemSlot, TSubclassOf<URPGGameplayAbility>>& T : DefaultSlottedAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(T.Value));
		}
	}
}

void ARPGCharacter::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ARPGCharacter* InstigatorPawn, AActor* DamageCauser)
{
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorPawn, DamageCauser);
	AbilitySystemComponent->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hurt")));
}

void ARPGCharacter::HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags, AActor* Causer)
{
	OnHealthChanged(DeltaValue, EventTags, Causer);
	if (!IsAlive())
	{
		GetCharacterMovement()->DisableMovement();

		if (HasAuthority())
		{
			SetLifeSpan(1.f);
			if (CurrentWeapon)
			{
				CurrentWeapon->Destroy();
			}
		}
	}
}

void ARPGCharacter::HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	
}

void ARPGCharacter::HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	// Update the character movement's walk speed
	GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();

}

FGenericTeamId ARPGCharacter::GetGenericTeamId() const
{
	static const FGenericTeamId PlayerTeam(0);
	static const FGenericTeamId AITeam(1);
	return Cast<APlayerController>(GetController()) ? PlayerTeam : AITeam;
}

UAbilitySystemComponent* ARPGCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARPGCharacter::OnRep_CurrentWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->WeaponAttachSocket);
	}
	if (IsPlayerControlled())
	{
		if (UGameHud* UI = ATdsHud::FindMenu<UGameHud>(this, ETapMenuType::GameHud))
		{
			UI->UpdateWeapon();
		}
	}
}

void ARPGCharacter::OnRep_CurrentWeaponIndex()
{
}