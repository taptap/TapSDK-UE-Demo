// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReachAchievementTipsUI.generated.h"

class UTextBlock;
class UImage;

USTRUCT()
struct FReachTipData
{
	GENERATED_BODY()

	UPROPERTY()
	FString Title;

	UPROPERTY()
	UTexture* Texture = nullptr;
};
/**
 * 
 */
UCLASS(Abstract)
class TAPACHIEVEMENT_API UReachAchievementTipsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void ShowReachAchievement(const FString& DisplayId);//Prepare data

	void PushAchievement(FReachTipData&& NewData);
	
	void RemoveSelf();//Animation remove

protected:
	void OnPlayStartAnimFinished_Loop(UUMGSequencePlayer& InPlayer);

	void OnPlayStartAnimFinished_Remove(UUMGSequencePlayer& InPlayer);
	
	void OnPlayLoopAnim(UUMGSequencePlayer& InPlayer);

	void LoopShowAchievement();

	void DelayUpdateIcon();
	
	void OnDownloadIconFinished(UTexture2D* Tex, FString Title);

	virtual void NativeOnInitialized() override;
	
	FTimerHandle DelayLoopTimer;

	FTimerHandle DelayUpdateIconTimer;
	
	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* StartAnim;

	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* LoopAnim;
	
	UPROPERTY(Meta = (BindWidget))
	UImage* Icon;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TB_Content;

	UPROPERTY(EditAnywhere)
	UTexture2D* DefaultText;

	UPROPERTY()
	FReachTipData CurrentData;

	/** For Texture GC */
	UPROPERTY()
	TArray<FReachTipData> ReachAchievementOpenIdQueue;
};

