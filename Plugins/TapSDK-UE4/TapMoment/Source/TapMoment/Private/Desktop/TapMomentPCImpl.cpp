#include "TapMomentPCImpl.h"
#include "TUHelper.h"

void FTapMomentPCImpl::Init(const FTapMomentConfig& InConfig) {
	Config = InConfig;
}

void FTapMomentPCImpl::OpenWebTopic() {
	FString Url;
	if (FTapMomentImpl::Get()->Config.RegionType == ERegionType::CN) {
		Url = "https://www.taptap.cn/app" / FTapMomentImpl::Get()->Config.AppID / "topic?utm_medium=link&utm_source=pc_sdk";
	} else {
		Url = "https://www.taptap.io/app" / FTapMomentImpl::Get()->Config.AppID + "?utm_medium=link&utm_source=pc_sdk";
	}
	TUHelper::LaunchURL(*Url, nullptr, nullptr);
}
