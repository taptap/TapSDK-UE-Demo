// Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/Menu/TitleWidget.h"


#include "Common/GameStatic.h"
#include "Components/CheckBox.h"
#include "GameFramework/RPGGameInstance.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/Widgets/TitleButton.h"
#include "Components/WidgetSwitcher.h"
#include "GameFramework/TdsHud.h"
#include "Interface/TdsInterface.h"

#define LOCTEXT_NAMESPACE "TapSDKDemo"



void UTitleWidget::UpdateWidgetWithTdsUser(const FTdsPlayer& Player)
{
	if (bShowUserForward)
	{
		PlayAnimationForward(ShowUserAnim);
		bShowUserForward = !bShowUserForward;
	}
	
	ObjectId->SetText(FText::FromString(Player.Id));
	NickName->SetText(FText::FromString(Player.NickName));
	ShortID->SetText(FText::FromString(Player.ShortID));
	Email->SetText(FText::FromString(Player.Email));
	Username->SetText(FText::FromString(Player.Username));
	MobilePhoneNumber->SetText(FText::FromString(Player.MobilePhoneNumber));
	IsAnonymous->SetCheckedState(Player.bIsAnonymous ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	IsMobilePhoneVerified->SetCheckedState(Player.bIsMobilePhoneVerified ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	IsAuthenticated->SetCheckedState(Player.bIsAuthenticated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

	UpdateAntiAddictionWithTdsUser(Player);
	
	FGameStatic::FindOrDownloadTexture(Player.AvatarUrl, FTextureDelegate::CreateWeakLambda(this, [this](UTexture2D* Tex)
	{
		if (UMaterialInstanceDynamic* DMI = UserIcon->GetDynamicMaterial())
		{
			if (Tex)
			{
				DMI->SetTextureParameterValue(TEXT("Texture"), Tex);
			}
			else
			{
				DMI->SetTextureParameterValue(TEXT("Texture"), EmptyUserIcon);
			}
		}
	}));
}

void UTitleWidget::UpdateAntiAddictionWithTdsUser(const FTdsPlayer& Player)
{
	AgeRange->SetText(FText::Format(LOCTEXT("AgeRangeFormat", "{0}+"), FText::AsNumber(Player.MinAge, &FNumberFormattingOptions::DefaultNoGrouping())));
	if (Player.RemainingSeconds >= 9999)
	{
		RemainingTime->SetText(LOCTEXT("Unlimited", "不限时"));
	}
	else
	{
		RemainingTime->SetText(FText::Format(LOCTEXT("RemainingTimeFormat", "限时：{0}秒"),
			FText::AsNumber(Player.RemainingSeconds, &FNumberFormattingOptions::DefaultNoGrouping())));
	}
}

void UTitleWidget::EnableSupport(bool bNewEnable)
{
	if (GTdsInterface)
	{
		SupportButton->SetIsEnabled(bNewEnable);
		if (bNewEnable)
		{
			GTdsInterface->SetSupportRedDotCallback(FBooleanDelegate::CreateUObject(SupportButton, &UTitleButton::UpdateRedDot));
		}
		else
		{
			GTdsInterface->SetSupportRedDotCallback(FBooleanDelegate());
		}
	}
}

void UTitleWidget::StartLogin()
{
	if (!bShowUserForward)
	{
		PlayAnimationReverse(ShowUserAnim);
		bShowUserForward = !bShowUserForward;
	}
	
	EnableSupport(false);
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MomentHandle);
	}
}


void UTitleWidget::HandleLeaveSession()
{
	ServerSwitcher->SetActiveWidgetIndex(0);
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		GI->StartRefreshGameSessions();
	}
}

void UTitleWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MultiPlayerButton->OnClicked.AddDynamic(this, &UTitleWidget::OnMultiPlayerGameClicked);
	TdsLoginAnonymouslyButton->OnClicked.AddDynamic(this, &UTitleWidget::OnAnonymouslyLoginClicked);
	TdsLoginWithTapButton->OnClicked.AddDynamic(this, &UTitleWidget::OnTapLoginClicked);
	TdsLogoutButton->OnClicked.AddDynamic(this, &UTitleWidget::OnLogoutClicked);
	TdsBillboardButton->OnClicked.AddDynamic(this, &UTitleWidget::OnBillboardButtonClicked);
	
	BillboardButton->OnClicked.AddDynamic(this, &UTitleWidget::OnBillboardButtonClicked);
	SupportButton->OnClicked.AddDynamic(this, &UTitleWidget::OnSupportButtonClicked);
	MomentButton->OnClicked.AddDynamic(this, &UTitleWidget::OnMomentButtonClicked);
	LeaderBoardButton->OnClicked.AddDynamic(this, &UTitleWidget::OnLeaderboardButtonClicked);
	AchievementButton->OnClicked.AddDynamic(this, &UTitleWidget::OnAchievementButtonClicked);

	BackButton->OnClicked.AddDynamic(this, &UTitleWidget::HandleBackButtonClicked);

	if (UWorld* World = GetWorld())
	{
		StartTime = World->GetTimeSeconds();
	}

	if (GTdsInterface)
	{
		GTdsInterface->SetMomentRedDotCallback(FBooleanDelegate::CreateUObject(MomentButton, &UTitleButton::UpdateRedDot));
		GTdsInterface->SetAnnouncementRedDotCallback(FBooleanDelegate::CreateUObject(this, &UTitleWidget::OnAnnouncementRedDotCallback));
	}
}

void UTitleWidget::OnAnnouncementRedDotCallback(bool bNewVisible)
{
	BillboardButton->UpdateRedDot(bNewVisible);
	TdsBillboardButton->UpdateRedDot(bNewVisible);
}

void UTitleWidget::OnMultiPlayerGameClicked()
{
	PlayAnimationForward(ShowOnlineAnim);
	ServerSwitcher->SetActiveWidgetIndex(0);
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		GI->StartRefreshGameSessions();
	}
}

void UTitleWidget::OnAnonymouslyLoginClicked()
{
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		if (GI->GetTdsPlayer())
		{
			GI->TdsLoginSuccess(GI->GetTdsPlayer().ToSharedRef());
			return;
		}

		if (GTdsInterface)
		{
			GTdsInterface->TdsLoginAnonymously(
				FTdsUserDelegate::CreateWeakLambda(this,
					[this](const TSharedPtr<FTdsPlayer>& Player, const TSharedPtr<FTdsError>& Error)
					{
						if (Player)
						{
							if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
							{
								GI->TdsLoginSuccess(Player.ToSharedRef());
							}
						}
						else if(Error)
						{
							ATdsHud::ShowMessage(this,
								NSLOCTEXT("TapSDKDemo", "LoginFailed", "登录失败"));
						}
					}));
		}
	}
}

void UTitleWidget::OnTapLoginClicked()
{
	if (GTdsInterface)
	{
		GTdsInterface->TdsLoginWithTapTap(
			FTdsUserDelegate::CreateWeakLambda(this,
				[this](const TSharedPtr<FTdsPlayer>& Player, const TSharedPtr<FTdsError>& Error)
				{
					if (Player)
					{
						if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
						{
							GI->TdsLoginSuccess(Player.ToSharedRef());
						}
					}
					else if(Error)
					{
						ATdsHud::ShowMessage(this,
							NSLOCTEXT("TapSDKDemo", "LoginFailed", "登录失败"));
					}
				}));
	}
}

void UTitleWidget::OnLogoutClicked()
{
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		GI->TdsLogout();
	}
}


void UTitleWidget::OnBillboardButtonClicked()
{
	if (GTdsInterface)
	{
		GTdsInterface->OpenNavigateAnnouncement(
			FSimpleTdsDelegate(),
			FSimpleDelegate());
	}
}

void UTitleWidget::OnSupportButtonClicked()
{
	if (GTdsInterface)
	{
		GTdsInterface->OpenSupportWebPage();
	}
}

void UTitleWidget::OnMomentButtonClicked()
{
	if (GTdsInterface)
	{
		GTdsInterface->OpenMomentPage();
	}
}

void UTitleWidget::OnAchievementButtonClicked()
{
	if (GTdsInterface)
	{
		GTdsInterface->OpenAchievementWidget();
	}
}

void UTitleWidget::OnLeaderboardButtonClicked()
{
	ATdsHud::ShowMenu(this, ETapMenuType::Leaderboard);
}

void UTitleWidget::HandleBackButtonClicked()
{
	PlayAnimationReverse(ShowOnlineAnim);
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		GI->StopRefreshGameSession();
	}
}


#undef LOCTEXT_NAMESPACE
