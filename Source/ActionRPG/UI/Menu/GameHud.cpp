// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameHud.h"

#include "Abilities/RPGAbilitySystemComponent.h"
#include "Abilities/RPGAttributeSet.h"
#include "Components/ProgressBar.h"

UInfoBarWidget* UGameHud::GetInfoBarWidget() const
{
	return InfoBarWidget;
}

void UGameHud::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateWeapon();
}

void UGameHud::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (URPGAbilitySystemComponent* Comp = Pawn->FindComponentByClass<URPGAbilitySystemComponent>())
		{
			float Health = Comp->GetNumericAttributeChecked(URPGAttributeSet::GetHealthAttribute());
			float HealthMax = Comp->GetNumericAttributeChecked(URPGAttributeSet::GetMaxHealthAttribute());
			float Mana = Comp->GetNumericAttributeChecked(URPGAttributeSet::GetManaAttribute());
			float ManaMax = Comp->GetNumericAttributeChecked(URPGAttributeSet::GetMaxManaAttribute());
			HP_ProgressBar->SetPercent(HealthMax > 0.f ? Health / HealthMax : 0.f);
			MP_ProgressBar->SetPercent(ManaMax > 0.f ? Mana / ManaMax : 0.f);
		}
	}
}
