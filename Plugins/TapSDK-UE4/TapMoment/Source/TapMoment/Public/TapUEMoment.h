#pragma once
#include "TUMomentType.h"

// 请使用FTapMoment，将废弃
class TAPMOMENT_API TapUEMoment {
public:
	
	static void Init(TUMomentType::Config Config);

	// 仅支持移动端
	static void SetCallback(TFunction<void(int Code, const FString& Msg)> CallBack);

	// 仅支持移动端
	static void Open(TUMomentType::Orientation Orientation);

	// 仅支持移动端
	static void Publish(TUMomentType::Orientation Orientation, const TArray<FString>& ImagePaths, const FString& Content);

	// 仅支持移动端
	static void PublishVideo(TUMomentType::Orientation Orientation, const TArray<FString>& VideoPaths,
		const TArray<FString>& ImagePaths, const FString& Title, const FString& Desc);

	// 仅支持移动端
	static void PublishVideo(TUMomentType::Orientation Orientation, const TArray<FString>& VideoPaths, const FString& Title, const FString& Desc);

	// 仅支持移动端
	static void FetchNotification();

	// 仅支持移动端
	static void Close();

	// 仅支持移动端
	static void Close(const FString& Title, const FString& Content);

	// // 仅支持Android
	// static void SetUseAutoRotate(bool UseAuto);

	// 仅支持移动端
	static void DirectlyOpen(TUMomentType::Orientation Orientation, const FString& Page, TSharedPtr<FJsonObject> Extras);

	// 仅支持iOS
	static void NeedDeferSystemGestures();

	// 支持 PC端
	static void OpenWebTopic();
};
