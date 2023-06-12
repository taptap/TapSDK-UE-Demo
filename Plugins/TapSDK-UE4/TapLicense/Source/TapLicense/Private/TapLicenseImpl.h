#pragma once
#include "TapLicense.h"

class FTapLicenseImpl {
public:
	
	virtual ~FTapLicenseImpl();

	static TSharedPtr<FTapLicenseImpl>& Get();

	virtual void SetLicenseCallback(FSimpleDelegate OnSuccess);
	virtual void SetDLCCallback(bool bCheckOnce, const FString& PublicKey, FTapLicense::FDLCQueryDelegate OnQuery, FTapLicense::FDLCOrderDelegate OnOrder);
	virtual void Check(bool bIsForce = false);
	virtual void QueryDLC(const TArray<FString>& DLCList);
	virtual void PurchaseDLC(const FString& DLC);
	

private:
	static TSharedPtr<FTapLicenseImpl> Instance;
	
};
