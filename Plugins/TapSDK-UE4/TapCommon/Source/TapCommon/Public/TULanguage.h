#pragma once
#include "TUType.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentLanguageChanged, ELanguageType/** OldType */, ELanguageType/** NewType */);

class TAPCOMMON_API TULanguage {
public:

	static ELanguageType GetNativeType();

	static FString GetLanguageString();
	// 获得当前语言
	static ELanguageType GetCurrentType();
	// 设置当前语言
	static void SetCurrentType(ELanguageType Type);

	static FOnCurrentLanguageChanged& OnLanguageChanged(); 

private:
	static TULanguage& Get();
	static TULanguage *SingleInstance;

	TMap<FString, ELanguageType> LanguageMap;
	void ParseLanguages();
	ELanguageType CurrentType = ELanguageType::AUTO;
	
	FOnCurrentLanguageChanged OnCurrentLanguageChanged;
};

