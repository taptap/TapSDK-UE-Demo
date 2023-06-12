#pragma once

#define TapUELicense_VERSION_NUMBER "31804001"
#define TapUELicense_VERSION "3.18.4"

class TAPLICENSE_API FTapLicense {
public:
	enum class EQueryResult : int {
		OK = 0,					// 查询成功
		NotInstallTapTap = 1,	// 未安装客户端，sdk有相应处理，正常不需要开发者做处理
		Error = 2,				// 查询失败
	};

	enum class EOrderStatus : int {
		DLCNotPurchased = 0,		// 支付失败
		DLCPurchased = 1,			// 支付成功
		DLCReturnError = -1,		// 支付出现问题
	};
	
	typedef TMap<FString, int> Map;
	DECLARE_DELEGATE_TwoParams(FDLCQueryDelegate, EQueryResult Code, const Map& QueryList);
	DECLARE_DELEGATE_TwoParams(FDLCOrderDelegate, const FString& Sku, EOrderStatus Status);

	static void SetLicenseCallback(FSimpleDelegate OnSuccess);
	static void SetDLCCallback(FDLCQueryDelegate OnQuery, FDLCOrderDelegate OnOrder);
	static void SetDLCCallback(bool bCheckOnce, const FString& PublicKey, FDLCQueryDelegate OnQuery, FDLCOrderDelegate OnOrder);
	static void Check(bool bIsForce = false);
	static void QueryDLC(const TArray<FString>& DLCList);
	static void PurchaseDLC(const FString& DLC);
};
