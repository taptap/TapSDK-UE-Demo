// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "JsonUtilities.h"
#include "TUHelper.h"
#include "TUJsonHelper.h"
#include "TUType.h"
#include "TapNet.generated.h"


GENERATE_MEMBER_FUNCTION_CHECK(CustomParseData, void, ,const TSharedRef<FJsonObject>&);
template<typename T, bool Exists>	struct InvokeIfExists_CustomParseData;					
template<typename T>				struct InvokeIfExists_CustomParseData<T, false>		{ static auto Invoke(T& t, const TSharedRef<FJsonObject>& Obj) {							}};																					
template<typename T>				struct InvokeIfExists_CustomParseData<T, true>		{ static auto Invoke(T& t, const TSharedRef<FJsonObject>& Obj) { t.CustomParseData(Obj);	}};

GENERATE_MEMBER_FUNCTION_CHECK(PostParseData, void, ,);
template<typename T, bool Exists>	struct InvokeIfExists_PostParseData;					
template<typename T>				struct InvokeIfExists_PostParseData<T, false>		{ static auto Invoke(T& t) {						}};																					
template<typename T>				struct InvokeIfExists_PostParseData<T, true>		{ static auto Invoke(T& t) { t.PostParseData();	}};


USTRUCT()
struct FTapResponseWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	bool Success;

	UPROPERTY()
	FJsonObjectWrapper Data;
	
	UPROPERTY()
	int64 Now;
};


/**
 * @brief Parse http response with format(FTapResponseWrapper)
 * 
 * if (FTapResponseWrapper::Success)
 *		parse FTapResponseWrapper::Data with 'DataType'
 * else
 *		parse FTapResponseWrapper::Data with 'ErrorType'
 *
 * Call 'DataType'::CustomParseData instead of FJsonObjectConverter::JsonObjectToUStruct, if 'DataType' has member function 'CustomParseData'
 * Call 'DataType'::PostParseData after parse json data, if 'DataType' has member function 'PostParseData'
 * @see FAchievementDescListData, FAchievementListData, FAchievementPlatinumData
 */
template<typename DataType, typename ErrorType>
void ParseTapResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, TDelegate<void(const TSharedPtr<DataType>&, const TSharedPtr<ErrorType>&)> Callback)
{
	static_assert(TIsClass<DataType>::Value, "OnSuccess delegate arg(DataType) must be struct type.");
	static_assert(TIsClass<ErrorType>::Value, "OnFailed delegate arg(ErrorType) must be struct type.");
	
	FTapResponseWrapper WrapperData;
	if (FJsonObjectConverter::JsonObjectStringToUStruct(HttpResponse->GetContentAsString(), &WrapperData, 0, 0) && WrapperData.Data.JsonObject.IsValid())
	{
		if (WrapperData.Success)
		{
			TSharedPtr<DataType> Data = MakeShared<DataType>();

			if (THasMemberFunction_CustomParseData<DataType>::Value)
			{
				InvokeIfExists_CustomParseData<DataType, THasMemberFunction_CustomParseData<DataType>::Value>::Invoke(*Data, WrapperData.Data.JsonObject.ToSharedRef());
			}
			else
			{
				if (!FJsonObjectConverter::JsonObjectToUStruct(WrapperData.Data.JsonObject.ToSharedRef(), DataType::StaticStruct(), Data.Get()))
				{
					goto NetError;
				}
			}

			InvokeIfExists_PostParseData<DataType, THasMemberFunction_PostParseData<DataType>::Value>::Invoke(*Data);

			Callback.ExecuteIfBound(Data, nullptr);
			return;
		}
		else
		{
			TSharedPtr<ErrorType> Error = MakeShared<ErrorType>();
			if (FJsonObjectConverter::JsonObjectToUStruct(WrapperData.Data.JsonObject.ToSharedRef(), ErrorType::StaticStruct(), Error.Get()))
			{
				Callback.ExecuteIfBound(nullptr, Error);
				return;
			}
		}
	}

NetError:
	Callback.ExecuteIfBound(nullptr, MakeShared<ErrorType>(-1, TEXT("Net error")));
	
	if (ensureMsgf(HttpRequest && HttpResponse, TEXT("Net error, no http reqeust or http response.")))
	{
		UE_LOG(LogTap, Warning, TEXT("ParseTapResponse failed. Request url: %s, Response code: %d, Response content length: %d, Response content string: %s"),
			*HttpRequest->GetURL(),
			HttpResponse->GetResponseCode(),
			HttpResponse->GetContentLength(),
			*HttpResponse->GetContentAsString());
	}
}


/**
 * @brief Generate Tap http request with commonly header, query and send functions.
 * 
 * Inherit this, make your commonly http request format.
 * 
 * @see FTapHttpBuilder_Achievement(format)
 * @see FTapAchievementRest(usage)
 */
struct TAPCOMMON_API FTapHttpBuilder
{
	FTapHttpBuilder() = delete;

	FTapHttpBuilder(const FString& Verb, const FString& Host, const TCHAR* Path, const TMap<FString, FString>& Query = {});

	FORCEINLINE FHttpRequestRef Get()
	{
		return HttpRequest;
	}

	/** Common Send */
	template<typename DataType, typename ErrorType>
	FORCEINLINE_DEBUGGABLE void ProcessWithCallback(const TDelegate<void(const TSharedPtr<DataType>&, const TSharedPtr<ErrorType>&)>& Callback)
	{
		HttpRequest->OnProcessRequestComplete().BindStatic(ParseTapResponse, Callback);
		if (!HttpRequest->ProcessRequest())
		{
			TSharedPtr<ErrorType> Error = MakeShared<ErrorType>(-1, TEXT("Net error."));
			Callback.ExecuteIfBound(nullptr, Error);
		}
	}

	/** Request Build */
	FORCEINLINE FTapHttpBuilder& SetHeader(const FString& HeaderName, const FString& HeaderValue)
	{
		HttpRequest->SetHeader(HeaderName, HeaderValue);
		return *this;
	}
	FORCEINLINE_DEBUGGABLE FTapHttpBuilder& AppendQuery(const TCHAR* Key, const FString& Value)
	{
		int32 Index = INDEX_NONE;
		FString Url = HttpRequest->GetURL();
		if (Url.FindChar(TEXT('?'), Index))
		{
			Url.Appendf(TEXT("&%s=%s"), Key, *Value);
		}
		else
		{
			Url.Appendf(TEXT("?%s=%s"), Key, *Value);
		}
		HttpRequest->SetURL(Url);
		return *this;
	}
	FORCEINLINE FTapHttpBuilder& Content_String(const FString& Content)
	{
		HttpRequest->SetContentAsString(Content);
		return *this;
	}
	FORCEINLINE FTapHttpBuilder& Content_JsonObject(const TSharedPtr<FJsonObject>& JsonObj)
	{
		HttpRequest->SetContentAsString(TUJsonHelper::GetJsonString(JsonObj));
		return *this;
	}

	/** Common Headers */
	FORCEINLINE FTapHttpBuilder& ContentType_Json()
	{
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		return *this;
	}
	FORCEINLINE FTapHttpBuilder& ClientID()
	{
		HttpRequest->SetHeader(TEXT("X-LC-Id"), Config.ClientID);
		return *this;
	}
	FORCEINLINE FTapHttpBuilder& LCSign()
	{
		HttpRequest->SetHeader(TEXT("X-LC-Sign"), TUHelper::GetLCSign(Config.ClientToken));
		return *this;
	}
	FORCEINLINE FTapHttpBuilder& AuthType()
	{
		HttpRequest->SetHeader(TEXT("X-Authorization-Type"), TEXT("LC"));
		return *this;
	}
	FORCEINLINE FTapHttpBuilder& SessionToken(const FString& SessionToken)
	{
		HttpRequest->SetHeader(TEXT("X-LC-Session"), SessionToken);
		return *this;
	}

	const FTUConfig& Config;
	FHttpRequestRef HttpRequest;
};

