// Fill out your copyright notice in the Description page of Project Settings.


#include "TapNet.h"



FTapHttpBuilder::FTapHttpBuilder(const FString& Verb, const FString& Host, const TCHAR* Path, const TMap<FString, FString>& Query)
	: Config(*FTUConfig::Get())
    , HttpRequest(FHttpModule::Get().CreateRequest())
{
	FString Url = Host;
	Url.Append(Path);

	if (Query.Num() > 0)
	{
		int32 Counter = 0;
		for (const TTuple<FString, FString>& T : Query)
		{
			if (Counter++ == 0)
			{
				Url.Appendf(TEXT("?%s=%s"), *T.Key, *T.Value);
			}
			else
			{
				Url.Appendf(TEXT("&%s=%s"), *T.Key, *T.Value);
			}
		}
	}
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(Verb);
}
