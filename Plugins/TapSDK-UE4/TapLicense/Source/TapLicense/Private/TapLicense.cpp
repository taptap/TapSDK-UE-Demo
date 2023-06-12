
#include "TapLicense.h"

#include "TapLicenseImpl.h"

void FTapLicense::SetLicenseCallback(FSimpleDelegate OnSuccess) {
	FTapLicenseImpl::Get()->SetLicenseCallback(OnSuccess);
}

void FTapLicense::SetDLCCallback(FDLCQueryDelegate OnQuery, FDLCOrderDelegate OnOrder) {
	SetDLCCallback(false, "", OnQuery, OnOrder);
}

void FTapLicense::SetDLCCallback(bool bCheckOnce, const FString& PublicKey, FDLCQueryDelegate OnQuery,
	FDLCOrderDelegate OnOrder) {
	FTapLicenseImpl::Get()->SetDLCCallback(bCheckOnce, PublicKey, OnQuery, OnOrder);
}

void FTapLicense::Check(bool bIsForce) {
	FTapLicenseImpl::Get()->Check(bIsForce);
}

void FTapLicense::QueryDLC(const TArray<FString>& DLCList) {
	FTapLicenseImpl::Get()->QueryDLC(DLCList);
}

void FTapLicense::PurchaseDLC(const FString& DLC) {
	FTapLicenseImpl::Get()->PurchaseDLC(DLC);
}

