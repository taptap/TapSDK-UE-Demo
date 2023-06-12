// Fill out your copyright notice in the Description page of Project Settings.


#include "PCButtonHandle.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "TapCommand.h"
#include "TapCommon.h"
#include "Components/Button.h"

FPCButtonHandle::FPCButtonHandle(UTapPlatformButton* Button)
	: SavedButton(Button)
{
}

void FPCButtonHandle::UpdateTexture(const FString IconUrl)
{
	FTapCommonModule::AsyncDownloadImage(IconUrl, FAsyncDownloadImage::CreateSP(this, &FPCButtonHandle::OnDownloadFinished));
}

void FPCButtonHandle::OnDownloadFinished(UTexture2D* Texture)
{
	if (Texture)
	{
		if (IsValid(SavedButton.Get()))
		{
			SavedButton->Button->WidgetStyle.Normal.SetResourceObject(Texture);
			SavedButton->Button->WidgetStyle.Hovered.SetResourceObject(Texture);
			SavedButton->Button->WidgetStyle.Pressed.SetResourceObject(Texture);
			SavedButton->Button->SetStyle(SavedButton->Button->WidgetStyle);
		}
	}
}
