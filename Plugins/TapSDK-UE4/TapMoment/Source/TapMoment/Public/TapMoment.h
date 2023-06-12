#pragma once
#include "TapMomentType.h"

class TAPMOMENT_API FTapMoment {
public:

	DECLARE_DELEGATE_TwoParams(FDelegate, int Code, const FString& Msg);
	
	static void Init(const FTapMomentConfig& Config);

	// 仅支持移动端
	static void SetCallback(FDelegate CallBack);

	// 仅支持移动端
	static void Open(ETapMomentOrientation Orientation);

	// 仅支持移动端
	static void Publish(ETapMomentOrientation Orientation, const TArray<FString>& ImagePaths, const FString& Content);

	// 仅支持移动端
	static void PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths,
		const TArray<FString>& ImagePaths, const FString& Title, const FString& Desc);

	// 仅支持移动端
	static void PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths, const FString& Title, const FString& Desc);

	// 仅支持移动端
	static void FetchNotification();

	// 仅支持移动端
	static void Close();

	// 仅支持移动端
	static void Close(const FString& Title, const FString& Content);

	// 仅支持移动端
	static void DirectlyOpen(ETapMomentOrientation Orientation, const FString& Page, TSharedPtr<FJsonObject> Extras);

	// 仅支持iOS
	static void NeedDeferSystemGestures();

	// 仅支持 PC端
	static void OpenWebTopic();
};
