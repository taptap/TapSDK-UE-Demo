// Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/Menu/UserInfoWidget.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Interface/TdsInterface.h"

#define LOCTEXT_NAMESPACE "TapSDKDemo"

void UUserInfoWidget::UpdateWidget(const FTdsPlayer& Player)
{
	ObjectId->SetText(FText::FromString(Player.Id));
	Avatar->SetText(FText::FromString(Player.AvatarUrl));
	NickName->SetText(FText::FromString(Player.NickName));
	ShortID->SetText(FText::FromString(Player.ShortID));
	Email->SetText(FText::FromString(Player.Email));
	Username->SetText(FText::FromString(Player.Username));
	Password->SetText(FText::FromString(Player.Password));
	MobilePhoneNumber->SetText(FText::FromString(Player.MobilePhoneNumber));
	SessionToken->SetText(FText::FromString(Player.SessionToken));
	IsAnonymous->SetCheckedState(Player.bIsAnonymous ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	IsMobilePhoneVerified->SetCheckedState(Player.bIsMobilePhoneVerified ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	IsAuthenticated->SetCheckedState(Player.bIsAuthenticated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	AgeRange->SetText(FText::Format(LOCTEXT("AgeRangeFormat", "{0}+"), FText::AsNumber(Player.MinAge, &FNumberFormattingOptions::DefaultNoGrouping())));
	RemainingTime->SetText(FText::AsNumber(Player.RemainingSeconds, &FNumberFormattingOptions::DefaultNoGrouping()));
}

void UUserInfoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CloseButton->OnClicked.AddDynamic(this, &UUserInfoWidget::RemoveFromParent);
}

#undef LOCTEXT_NAMESPACE