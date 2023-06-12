#pragma once
#include "TUType.h"

struct TAPMOMENT_API FTapMomentConfig {
	FString ClientID;
	ERegionType RegionType;
	FString AppID;  // PC 如果国内国外论坛ID不一样，那么在RegionType不同的时候传不同的值
};

// 这个影响服务的host以及TapTap授权客户端的种类（是国内还是海外）
enum class ETapMomentOrientation : int {
	DEFAULT = -1,
	LANDSCAPE = 0,
	PORTRAIT = 1
};

struct TAPMOMENT_API FTapMomentPage {
	static FString ShortCut;
	static FString ShortCutKey;
	static FString User;
	static FString UserKey;
};

