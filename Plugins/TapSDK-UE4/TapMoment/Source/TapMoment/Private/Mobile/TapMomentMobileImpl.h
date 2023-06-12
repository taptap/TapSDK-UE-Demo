#pragma once
#include "TapMomentImpl.h"

class FTapMomentMobileImpl : public FTapMomentImpl {
public:
	FTapMomentMobileImpl();
	
	virtual void Init(const FTapMomentConfig& InConfig) override;
	virtual void SetCallback(FTapMoment::FDelegate CallBack) override;
	virtual void Open(ETapMomentOrientation Orientation) override;
	virtual void Publish(ETapMomentOrientation Orientation, const TArray<FString>& ImagePaths,
		const FString& Content) override;
	virtual void PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths,
		const TArray<FString>& ImagePaths, const FString& Title, const FString& Desc) override;
	virtual void PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths,
		const FString& Title, const FString& Desc) override;
	virtual void FetchNotification() override;
	virtual void Close() override;
	virtual void Close(const FString& Title, const FString& Content) override;
	virtual void DirectlyOpen(ETapMomentOrientation Orientation, const FString& Page,
		TSharedPtr<FJsonObject> Extras) override;
	virtual void NeedDeferSystemGestures() override;
};
