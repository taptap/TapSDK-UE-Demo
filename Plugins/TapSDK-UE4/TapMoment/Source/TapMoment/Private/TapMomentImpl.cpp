#include "TapMomentImpl.h"
#include "TUDebuger.h"
#if PLATFORM_MAC || PLATFORM_WINDOWS
#include "Desktop/TapMomentPCImpl.h"
#elif PLATFORM_IOS || PLATFORM_ANDROID
#include "Mobile/TapMomentMobileImpl.h"
#endif

#define UnsupportedPlatformsLog TUDebuger::ErrorLog(FString::Printf(TEXT("Unsupported Platforms Call %s"), ANSI_TO_TCHAR(__FUNCTION__)));

TSharedPtr<FTapMomentImpl> FTapMomentImpl::Instance = nullptr;

FTapMomentImpl::~FTapMomentImpl() {
}

void FTapMomentImpl::Init(const FTapMomentConfig& InConfig) {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::SetCallback(FTapMoment::FDelegate CallBack) {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::Open(ETapMomentOrientation Orientation) {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::Publish(ETapMomentOrientation Orientation, const TArray<FString>& ImagePaths,
	const FString& Content) {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths,
	const TArray<FString>& ImagePaths, const FString& Title, const FString& Desc) {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths,
	const FString& Title, const FString& Desc) {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::FetchNotification() {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::Close() {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::Close(const FString& Title, const FString& Content) {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::DirectlyOpen(ETapMomentOrientation Orientation, const FString& Page,
	TSharedPtr<FJsonObject> Extras) {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::NeedDeferSystemGestures() {
	UnsupportedPlatformsLog
}

void FTapMomentImpl::OpenWebTopic() {
	UnsupportedPlatformsLog
}

TSharedPtr<FTapMomentImpl>& FTapMomentImpl::Get() {
	if (!Instance.IsValid()) {
#if PLATFORM_MAC || PLATFORM_WINDOWS
		Instance = MakeShareable(new FTapMomentPCImpl);
#elif PLATFORM_IOS || PLATFORM_ANDROID
		Instance = MakeShareable(new FTapMomentMobileImpl);
#else
		Instance = MakeShareable(new FTapMomentImpl);
#endif
	}
	return Instance;
}


