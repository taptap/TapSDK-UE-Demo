#pragma once
#include "TUType.h"

namespace TUMomentType {

	class TAPMOMENT_API Config {
	public:
		FString ClientID;
		ERegionType RegionType;
		FString AppID;  // PC 如果国内国外论坛ID不一样，那么在RegionType不同的时候传不同的值
	};

	enum Orientation {
		DEFAULT = -1,
		LANDSCAPE = 0,
		PORTRAIT = 1
	};
	


	class TAPMOMENT_API Page {
		static FString ShortCut;
		static FString ShortCutKey;
		static FString User;
		static FString UserKey;
	};
}
