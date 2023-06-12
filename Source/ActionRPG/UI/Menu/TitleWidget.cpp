// Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/Menu/TitleWidget.h"


#include "Common/GameStatic.h"
#include "GameFramework/RPGGameInstance.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "UI/Widgets/TitleButton.h"
#include "UI/Menu/UserInfoWidget.h"
#include "Components/WidgetSwitcher.h"
#include "GameFramework/TdsHud.h"
#include "Interface/TdsInterface.h"

void UTitleWidget::UpdateWidgetWithTdsUser(const FTdsPlayer& Player)
{
	StrandAloneButton->SetIsEnabled(true);
	JoinButton->SetIsEnabled(true);
	ServerSwitcher->SetVisibility(ESlateVisibility::Visible);
	TdsLoginAnonymouslyButton->SetIsEnabled(false);
	TdsLoginWithTapButton->SetIsEnabled(false);
	TdsLogoutButton->SetIsEnabled(true);
	
	UserPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AchievementPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
#if PLATFORM_IOS || PLATFORM_ANDROID
	MomentPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	LeaderBoardPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
#endif

	FGameStatic::FindOrDownloadTexture(Player.AvatarUrl, FTextureDelegate::CreateWeakLambda(this, [this](UTexture2D* Tex)
	{
		if (Tex)
		{
			UserIcon->SetBrushFromTexture(Tex);
			UserIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			//显示下面层级默认头像
			UserIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}));
}

void UTitleWidget::EnableSupport(bool bNewEnable)
{
	if (GTdsInterface)
	{
		if (bNewEnable)
		{
			GTdsInterface->SetSupportRedDotCallback(FBooleanDelegate::CreateUObject(this, &UTitleWidget::OnSupportRedDotChanged));
			SupportPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			GTdsInterface->SetSupportRedDotCallback(FBooleanDelegate());
			SupportPanel->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UTitleWidget::StartLogin()
{
	StrandAloneButton->SetIsEnabled(false);
	JoinButton->SetIsEnabled(false);
	ServerSwitcher->SetVisibility(ESlateVisibility::Hidden);
	TdsLoginAnonymouslyButton->SetIsEnabled(true);
	TdsLoginWithTapButton->SetIsEnabled(true);
	TdsLogoutButton->SetIsEnabled(false);
	
	UserPanel->SetVisibility(ESlateVisibility::Collapsed);
	AchievementPanel->SetVisibility(ESlateVisibility::Collapsed);
#if PLATFORM_IOS || PLATFORM_ANDROID
	MomentPanel->SetVisibility(ESlateVisibility::Collapsed);
	LeaderBoardPanel->SetVisibility(ESlateVisibility::Collapsed);
#endif
	EnableSupport(false);
#if PLATFORM_IOS || PLATFORM_ANDROID
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MomentHandle);
	}
#endif
}

void UTitleWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	UserPanel->SetVisibility(ESlateVisibility::Collapsed);
	SupportPanel->SetVisibility(ESlateVisibility::Collapsed);
	MomentPanel->SetVisibility(ESlateVisibility::Collapsed);
	LeaderBoardPanel->SetVisibility(ESlateVisibility::Collapsed);
	AchievementPanel->SetVisibility(ESlateVisibility::Collapsed);
	
	BillboardRedDot->SetVisibility(ESlateVisibility::Hidden);
	SupportRedDot->SetVisibility(ESlateVisibility::Hidden);
	MomentRedDot->SetVisibility(ESlateVisibility::Hidden);
	UserIcon->SetVisibility(ESlateVisibility::Hidden);
	
	StrandAloneButton->Button->OnClicked.AddDynamic(this, &UTitleWidget::OnStartGameClicked);
	JoinButton->Button->OnClicked.AddDynamic(this, &UTitleWidget::OnJoinGameClicked);
	TdsLoginAnonymouslyButton->Button->OnClicked.AddDynamic(this, &UTitleWidget::OnAnonymouslyLoginClicked);
	TdsLoginWithTapButton->Button->OnClicked.AddDynamic(this, &UTitleWidget::OnTapLoginClicked);
	TdsLogoutButton->Button->OnClicked.AddDynamic(this, &UTitleWidget::OnLogoutClicked);
		
	UserButton->OnClicked.AddDynamic(this, &UTitleWidget::OnUserButtonClicked);
	BillboardButton->OnClicked.AddDynamic(this, &UTitleWidget::OnBillboardButtonClicked);
	SupportButton->OnClicked.AddDynamic(this, &UTitleWidget::OnSupportButtonClicked);
	MomentButton->OnClicked.AddDynamic(this, &UTitleWidget::OnMomentButtonClicked);
	LeaderBoardButton->OnClicked.AddDynamic(this, &UTitleWidget::OnLeaderboardButtonClicked);
	AchievementButton->OnClicked.AddDynamic(this, &UTitleWidget::OnAchievementButtonClicked);

#if PLATFORM_IOS || PLATFORM_ANDROID
	if (GTdsInterface)
	{
		GTdsInterface->SetMomentRedDotCallback(FBooleanDelegate::CreateUObject(this, &UTitleWidget::OnMomentRedDotChanged));
	}
#endif

	if (GTdsInterface)
	{
		GTdsInterface->SetAnnouncementRedDotCallback(FBooleanDelegate::CreateUObject(this, &UTitleWidget::OnBillboardRedDotChanged));
	}
}

void UTitleWidget::OnMomentRedDotChanged(bool bNewVisible)
{
	MomentRedDot->SetVisibility(bNewVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void UTitleWidget::OnSupportRedDotChanged(bool bNewVisible)
{
	SupportRedDot->SetVisibility(bNewVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void UTitleWidget::OnBillboardRedDotChanged(bool bNewVisible)
{
	BillboardRedDot->SetVisibility(bNewVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void UTitleWidget::OnStartGameClicked()
{
	HandleStartGame();
}

void UTitleWidget::OnJoinGameClicked()
{
	HandleJoinButtonClicked();
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
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		if (GI->GetTdsPlayer())
		{
			UpdateWidgetWithTdsUser(*GI->GetTdsPlayer().ToSharedRef());
			return;
		}
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
}

void UTitleWidget::OnLogoutClicked()
{
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		GI->TdsLogout();
	}
}

void UTitleWidget::OnUserButtonClicked()
{
	if (URPGGameInstance* GI = GetGameInstance<URPGGameInstance>())
	{
		if (GI->GetTdsPlayer())
		{
			if (UUserInfoWidget* UI = ATdsHud::ShowMenu<UUserInfoWidget>(this, ETapMenuType::PlayerInfo, 1))
			{
				if (GTdsInterface)
				{
					GTdsInterface->GetAntiAddictionPlayerInfo(GI->GetTdsPlayer().ToSharedRef());
				}
				UI->UpdateWidget(*GI->GetTdsPlayer());
				return;
			}
		}
	}
	ensure(false);
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


