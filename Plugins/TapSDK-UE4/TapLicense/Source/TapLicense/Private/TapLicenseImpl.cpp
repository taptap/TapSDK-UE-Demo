#include "TapLicenseImpl.h"
#include "TUDebuger.h"
#if PLATFORM_ANDROID
#include "Android/TapLicenseAndroid.h"
#endif


#define UnsupportedPlatformsLog TUDebuger::ErrorLog(FString::Printf(TEXT("Unsupported Platforms Call %s"), ANSI_TO_TCHAR(__FUNCTION__)));

TSharedPtr<FTapLicenseImpl> FTapLicenseImpl::Instance = nullptr;

FTapLicenseImpl::~FTapLicenseImpl() {
}


TSharedPtr<FTapLicenseImpl>& FTapLicenseImpl::Get() {
	if (!Instance.IsValid()) {
#if PLATFORM_ANDROID
		Instance = MakeShareable(new FTapLicenseAndroid);
#else
		Instance = MakeShareable(new FTapLicenseImpl);
#endif
	}
	return Instance;
}

void FTapLicenseImpl::SetLicenseCallback(FSimpleDelegate OnSuccess) {
	UnsupportedPlatformsLog
}

void FTapLicenseImpl::SetDLCCallback(bool bCheckOnce, const FString& PublicKey, FTapLicense::FDLCQueryDelegate OnQuery,
	FTapLicense::FDLCOrderDelegate OnOrder) {
	UnsupportedPlatformsLog
}

void FTapLicenseImpl::Check(bool bIsForce) {
	UnsupportedPlatformsLog
}

void FTapLicenseImpl::QueryDLC(const TArray<FString>& DLCList) {
	UnsupportedPlatformsLog
}

void FTapLicenseImpl::PurchaseDLC(const FString& DLC) {
	UnsupportedPlatformsLog
}
