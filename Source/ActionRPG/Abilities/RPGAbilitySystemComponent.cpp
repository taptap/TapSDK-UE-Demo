// Copyright Epic Games, Inc. All Rights Reserved.

#include "Abilities/RPGAbilitySystemComponent.h"
#include "GameFramework/RPGCharacter.h"
#include "Abilities/RPGGameplayAbility.h"
#include "AbilitySystemGlobals.h"

URPGAbilitySystemComponent::URPGAbilitySystemComponent() {}

void URPGAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<URPGGameplayAbility*>& ActiveAbilities)
{
	if (!GameplayTagContainer.IsValid())
	{
		return;
	}

	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{		
		if (Spec.Ability && Spec.Ability->AbilityTags.HasAll(GameplayTagContainer))
		{
			if (Spec.IsActive())
			{
				AbilitiesToActivate.Add(const_cast<FGameplayAbilitySpec*>(&Spec));
			}
		}
	}
	
	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			ActiveAbilities.Add(Cast<URPGGameplayAbility>(ActiveAbility));
		}
	}
}

int32 URPGAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	ARPGCharacter* OwningCharacter = Cast<ARPGCharacter>(GetOwnerActor());

	if (OwningCharacter)
	{
		return OwningCharacter->GetCharacterLevel();
	}
	return 1;
}

URPGAbilitySystemComponent* URPGAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<URPGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}
