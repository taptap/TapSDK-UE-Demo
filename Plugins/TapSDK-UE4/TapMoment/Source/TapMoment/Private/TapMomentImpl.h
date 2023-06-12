#pragma once
#include "TapMoment.h"

class FTapMomentImpl {
public:

	FTapMomentConfig Config;

	virtual ~FTapMomentImpl();

	static TSharedPtr<FTapMomentImpl>& Get();

	virtual void Init(const FTapMomentConfig& InConfig);

	// 仅支持移动端
	virtual void SetCallback(FTapMoment::FDelegate CallBack);

	// 仅支持移动端
	virtual void Open(ETapMomentOrientation Orientation);

	// 仅支持移动端
	virtual void Publish(ETapMomentOrientation Orientation, const TArray<FString>& ImagePaths, const FString& Content);

	// 仅支持移动端
	virtual void PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths,
		const TArray<FString>& ImagePaths, const FString& Title, const FString& Desc);

	// 仅支持移动端
	virtual void PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths, const FString& Title, const FString& Desc);

	// 仅支持移动端
	virtual void FetchNotification();

	// 仅支持移动端
	virtual void Close();

	// 仅支持移动端
	virtual void Close(const FString& Title, const FString& Content);

	// 仅支持移动端
	virtual void DirectlyOpen(ETapMomentOrientation Orientation, const FString& Page, TSharedPtr<FJsonObject> Extras);

	// 仅支持iOS
	virtual void NeedDeferSystemGestures();

	// 仅支持 PC端
	virtual void OpenWebTopic();

private:
	static TSharedPtr<FTapMomentImpl> Instance;
};
