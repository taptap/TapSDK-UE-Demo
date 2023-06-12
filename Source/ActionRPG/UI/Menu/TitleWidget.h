// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UserInfoWidget.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

class UWidgetSwitcher;
class UEditableTextBox;
class UImage;
class UTitleButton;
class UPanelWidget;
class UButton;

class UUserInfoWidget;

struct FTdsPlayer;

UCLASS(Abstract)
class ACTIONRPG_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateWidgetWithTdsUser(const FTdsPlayer& Player);

	void EnableSupport(bool bNewEnable);
	
	void StartLogin();

protected:
	virtual void NativeOnInitialized() override;

	void OnMomentRedDotChanged(bool bNewVisible);

	void OnSupportRedDotChanged(bool bNewVisible);

	void OnBillboardRedDotChanged(bool bNewVisible);


	UFUNCTION()
	void OnStartGameClicked();
	
	UFUNCTION()
	void OnJoinGameClicked();
	
	UFUNCTION()
	void OnAnonymouslyLoginClicked();
	
	UFUNCTION()
	void OnTapLoginClicked();
	
	UFUNCTION()
	void OnLogoutClicked();
	
	
	UFUNCTION()
	void OnUserButtonClicked();
	
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

	
	UFUNCTION(BlueprintImplementableEvent)
	void HandleBillboardAudioStatusChanged(bool bAudioPlay);

	UFUNCTION(BlueprintImplementableEvent)
	void HandleStartGame();
	
	UFUNCTION(BlueprintImplementableEvent)
	void HandleJoinButtonClicked();
	
	FTimerHandle MomentHandle;

	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UWidgetSwitcher* ServerSwitcher;
	
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UTitleButton* StrandAloneButton;

	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly)
	UTitleButton* JoinButton;
	
	UPROPERTY(Meta = (BindWidget))
	UTitleButton* TdsLoginAnonymouslyButton;

	UPROPERTY(Meta = (BindWidget))
	UTitleButton* TdsLoginWithTapButton;

	UPROPERTY(Meta = (BindWidget))
	UTitleButton* TdsLogoutButton;

	
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* UserPanel;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* UserButton;	

	UPROPERTY(Meta = (BindWidget))
	UImage* UserIcon;
	
	
	UPROPERTY(Meta = (BindWidget))
	UButton* BillboardButton;

	UPROPERTY(Meta = (BindWidget))
	UImage* BillboardRedDot;


	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* SupportPanel;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* SupportButton;
	
	UPROPERTY(Meta = (BindWidget))
	UImage* SupportRedDot;
	

	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* MomentPanel;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* MomentButton;
	
	UPROPERTY(Meta = (BindWidget))
	UImage* MomentRedDot;

	
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* LeaderBoardPanel;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* LeaderBoardButton;

	
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* AchievementPanel;
	
	UPROPERTY(Meta = (BindWidget))
	UButton* AchievementButton;


	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserInfoWidget> UserInfoClass;
};
