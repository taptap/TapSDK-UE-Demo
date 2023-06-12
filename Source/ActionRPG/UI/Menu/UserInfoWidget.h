// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserInfoWidget.generated.h"

class UCheckBox;
class UTextBlock;
class UButton;

struct FTdsPlayer;

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UUserInfoWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateWidget(const FTdsPlayer& Player);

protected:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* CloseButton;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ObjectId;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Avatar;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* NickName;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ShortID;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Email;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Username;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Password;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MobilePhoneNumber;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* SessionToken;

	UPROPERTY(Meta = (BindWidget))
	UCheckBox* IsAnonymous;

	UPROPERTY(Meta = (BindWidget))
	UCheckBox* IsMobilePhoneVerified;

	UPROPERTY(Meta = (BindWidget))
	UCheckBox* IsAuthenticated;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* AgeRange;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* RemainingTime;
};
