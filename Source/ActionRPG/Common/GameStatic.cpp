// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameStatic.h"
#include "GameFramework/RPGAssetManager.h"
#include "Http.h"
#include "ImageUtils.h"

void FGameStatic::FindOrDownloadTexture(const FString& Url, const FTextureDelegate& Callback)
{
	if (Url.IsEmpty())
	{
		Callback.ExecuteIfBound(nullptr);
		UE_LOG(LogActionRPG, Warning, TEXT("Net error, Http download image failed. empty url"));
		return;
	}
	
	if (UTexture2D* Tex = URPGAssetManager::Get().TextureMap.FindRef(Url))
	{
		Callback.ExecuteIfBound(Tex);
	}
	else
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

		HttpRequest->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			if ( bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0 )
			{
				UTexture2D* Tex = FImageUtils::ImportBufferAsTexture2D(HttpResponse->GetContent());
				Callback.ExecuteIfBound(Tex);		
			}
			else
			{
				Callback.ExecuteIfBound(nullptr);
				UE_LOG(LogActionRPG, Warning, TEXT("Net error, Http download image failed:%s."), HttpRequest ? *HttpRequest->GetURL() : TEXT(""));
			}
		});
		HttpRequest->SetURL(Url);
		HttpRequest->SetVerb(TEXT("GET"));
		if (!HttpRequest->ProcessRequest())
		{
			Callback.ExecuteIfBound(nullptr);
			UE_LOG(LogActionRPG, Warning, TEXT("Net error, Http download image failed:%s."), *Url);
		}
	}
}
