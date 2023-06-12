#pragma once

#include "TUType.generated.h"

// 这个影响服务的host以及TapTap授权客户端的种类（是国内还是海外）
UENUM(BlueprintType)
enum class ERegionType : uint8
{
	CN,			// 国内
	Global		// 海外
};

UENUM(BlueprintType)
enum class ELanguageType : uint8
{
	AUTO = 0,   // 国内默认简体中文
	ZH,			// 简体中文
	EN,			// 英文，海外默认语言
	ZHTW,		// 繁体中文
	JA,			// 日语
	KO,			// 韩语
	TH,			// 泰文
	ID,			// 印尼文
};

USTRUCT(BlueprintType)
struct TAPCOMMON_API FBillboardConfig
{
	GENERATED_BODY()

	/** Dimensions configured in DC */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FString> Dimensions;

	/** Billboard url configured in DC. Unnecessary when FTUConfig::RegionType == ERegionType::IO */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BillboardUrl;
};

USTRUCT(BlueprintType)
struct TAPCOMMON_API FTUConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ClientID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ClientToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERegionType RegionType = ERegionType::CN;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ServerURL;

	static void Init(const FTUConfig& InitConfig);
	static TSharedPtr<FTUConfig>& Get();

	FTUConfig() = default;
	FTUConfig(const FTUConfig& Other);

	struct FTapDB
	{
		bool Enable = false; // 如果该值为true，将会自动把TapDB模块一起初始化，不用手动去初始化TapDB
	
		FString Channel;

		FString GameVersion;
			
		bool AdvertiserIDCollectionEnabled = false;
	};
		
	FTapDB DBConfig;
		
	TSharedPtr<FBillboardConfig> BillboardConfig;
	
private:
	static TSharedPtr<FTUConfig> Instance;
		
};

namespace TUType {
	class TAPCOMMON_API PermissionScope {
	public:
		static FString BasicInfo;	// 无感登录（PC暂不支持）
		static FString Profile;		// 获得用户信息的权限
		static FString Friend;		// 获得好友的权限
		static FString Email;
	};
}

TAPCOMMON_API DECLARE_LOG_CATEGORY_EXTERN(LogTap, Log, All);