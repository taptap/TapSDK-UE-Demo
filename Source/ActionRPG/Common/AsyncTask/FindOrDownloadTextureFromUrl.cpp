// Copyright Epic Games, Inc. All Rights Reserved.


#include "FindOrDownloadTextureFromUrl.h"

#include "Common/GameStatic.h"

UFindOrDownloadTextureFromUrl* UFindOrDownloadTextureFromUrl::FindOrDownloadTexture(const FString& Url)
{
	UFindOrDownloadTextureFromUrl* Task = NewObject<UFindOrDownloadTextureFromUrl>();
	Task->SavedUrl = Url;
	return Task;
}

void UFindOrDownloadTextureFromUrl::Activate()
{
	FGameStatic::FindOrDownloadTexture(
		SavedUrl,
		FTextureDelegate::CreateWeakLambda(
			this,
			[this](UTexture2D* Tex)
			{
				if (Tex)
				{
					OnSuccess.Broadcast(Tex);
				}
				else
				{
					OnFailed.Broadcast(nullptr);
				}
			}));
}
