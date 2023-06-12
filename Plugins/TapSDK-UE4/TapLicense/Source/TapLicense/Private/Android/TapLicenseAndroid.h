#pragma once
#include "TapLicenseImpl.h"

class FTapLicenseAndroid: public FTapLicenseImpl {
public:
	virtual ~FTapLicenseAndroid() override;
	virtual void SetLicenseCallback(FSimpleDelegate OnSuccess) override;
	virtual void SetDLCCallback(bool bCheckOnce, const FString& PublicKey, FTapLicense::FDLCQueryDelegate OnQuery,
		FTapLicense::FDLCOrderDelegate OnOrder) override;
	virtual void Check(bool bIsForce) override;
	virtual void QueryDLC(const TArray<FString>& DLCList) override;
	virtual void PurchaseDLC(const FString& DLC) override;
};
