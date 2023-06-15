// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/RPGGameInstance.h"
#include "TitleWidget.generated.h"

class UScrollBox;
class UTextBlock;
class UCheckBox;
class UWidgetSwitcher;
class UImage;
class UTitleButton;
class UWidgetAnimation;
struct FTdsPlayer;

UCLASS(Abstract)
class ACTIONRPG_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateWidgetWithTdsUser(const FTdsPlayer& Player);

	void UpdateAntiAddictionWithTdsUser(const FTdsPlayer& Player);
	
	void EnableSupport(bool bNewEnable);
	
	void StartLogin();

	UFUNCTION(BlueprintImplementableEvent)
	void RefreshSessions(const TArray<FDSInfo>& Sessions);

	UFUNCTION(BlueprintImplementableEvent)
	void HandleJoinSession(const FDSInfo& Info);

	void HandleLeaveSession();
	
protected:
	virtual void NativeOnInitialized() override;

	void OnAnnouncementRedDotCallback(bool bNewVisible);
	
	UFUNCTION()
	void OnMultiPlayerGameClicked();
	
	UFUNCTION()
	void OnAnonymouslyLoginClicked();
	
	UFUNCTION()
	void OnTapLoginClicked();
	
	UFUNCTION()
	void OnLogoutClicked();
	
	
	UFUNCTION()
	void OnBillboardButtonClicked();
	
	UFUNCTION()
	void OnSupportButtonClicked();

	UFUNCTION()
	void OnMomentButtonClicked();

	UFUNCTION()
	void OnAchievementButtonClicked();

	UFUNCTION()
	void OnLeaderboardButtonClicked();

	UFUNCTION()
	void HandleBackButtonClicked();
	
	UFUNCTION(BlueprintImplementableEvent)
	void HandleBillboardAudioStatusChanged(bool bAudioPlay);

	FTimerHandle MomentHandle;

	
	UPROPERTY(Meta = (BindWidget))
	UTitleButton* TdsLoginWithTapButton;
	
	UPROPERTY(Meta = (BindWidget))
	UTitleButton* TdsLoginAnonymouslyButton;

	UPROPERTY(Meta = (BindWidget))
	UTitleButton* TdsBillboardButton;

	
	UPROPERTY(Meta = (BindWidget))
	UImage* UserIcon;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* AgeRange;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* RemainingTime;

	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ObjectId;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* NickName;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ShortID;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Email;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Username;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MobilePhoneNumber;

	UPROPERTY(Meta = (BindWidget))
	UCheckBox* IsAnonymous;

	UPROPERTY(Meta = (BindWidget))
	UCheckBox* IsMobilePhoneVerified;

	UPROPERTY(Meta = (BindWidget))
	UCheckBox* IsAuthenticated;


	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UTitleButton* MultiPlayerButton;
	
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UTitleButton* StrandAloneButton;


	UPROPERTY(Meta = (BindWidget))
	UTitleButton* AchievementButton;

	UPROPERTY(Meta = (BindWidget))
	UTitleButton* LeaderBoardButton;
		
	UPROPERTY(Meta = (BindWidget))
	UTitleButton* MomentButton;

	UPROPERTY(Meta = (BindWidget))
	UTitleButton* SupportButton;
	
	UPROPERTY(Meta = (BindWidget))
	UTitleButton* BillboardButton;

	
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UWidgetSwitcher* ServerSwitcher;

	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UScrollBox* ServerBox;
	
	UPROPERTY(Meta = (BindWidget))
	UTitleButton* BackButton;


	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* SessionTitleTextBlock;
	
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UScrollBox* PlayerBox;
	
	UPROPERTY(Meta = (BindWidget))
	UTitleButton* LeaveButton;

	UPROPERTY(Meta = (BindWidget))
	UTitleButton* StartButton;

	
	UPROPERTY(Meta = (BindWidget))
	UTitleButton* TdsLogoutButton;
	


	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* ShowUserAnim;
	UPROPERTY()
	bool bShowUserForward = true;
	
	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* ShowOnlineAnim;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* EmptyUserIcon;

	UPROPERTY()
	float StartTime;
};
