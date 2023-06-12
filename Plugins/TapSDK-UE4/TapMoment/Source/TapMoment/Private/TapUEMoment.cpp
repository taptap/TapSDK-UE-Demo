#include "TapUEMoment.h"
#include "TapMomentImpl.h"

void TapUEMoment::Init(TUMomentType::Config Config) {
	FTapMomentConfig _Config;
	_Config.RegionType = Config.RegionType;
	_Config.ClientID = Config.ClientID;
	_Config.AppID = Config.AppID;
	FTapMomentImpl::Get()->Init(_Config);
}

void TapUEMoment::SetCallback(TFunction<void(int Code, const FString& Msg)> CallBack) {
	FTapMomentImpl::Get()->SetCallback(FTapMoment::FDelegate::CreateLambda([=](int Code, const FString& Msg) {
		if (CallBack) {
			CallBack(Code, Msg);
		}
	}));
}

void TapUEMoment::Open(TUMomentType::Orientation Orientation) {
	ETapMomentOrientation _Orientation = (ETapMomentOrientation)Orientation;
	FTapMomentImpl::Get()->Open(_Orientation);
}

void TapUEMoment::Publish(TUMomentType::Orientation Orientation, const TArray<FString>& ImagePaths,
	const FString& Content) {
	ETapMomentOrientation _Orientation = (ETapMomentOrientation)Orientation;
	FTapMomentImpl::Get()->Publish(_Orientation, ImagePaths, Content);
}

void TapUEMoment::PublishVideo(TUMomentType::Orientation Orientation, const TArray<FString>& VideoPaths,
	const TArray<FString>& ImagePaths, const FString& Title, const FString& Desc) {
	ETapMomentOrientation _Orientation = (ETapMomentOrientation)Orientation;
	FTapMomentImpl::Get()->PublishVideo(_Orientation, VideoPaths, ImagePaths, Title, Desc);
}

void TapUEMoment::PublishVideo(TUMomentType::Orientation Orientation, const TArray<FString>& VideoPaths,
	const FString& Title, const FString& Desc) {
	ETapMomentOrientation _Orientation = (ETapMomentOrientation)Orientation;
	FTapMomentImpl::Get()->PublishVideo(_Orientation, VideoPaths, Title, Desc);
}

void TapUEMoment::FetchNotification() {
	FTapMomentImpl::Get()->FetchNotification();
}

void TapUEMoment::Close() {
	FTapMomentImpl::Get()->Close();
}

void TapUEMoment::Close(const FString& Title, const FString& Content) {
	FTapMomentImpl::Get()->Close(Title, Content);
}

// void TapUEMoment::SetUseAutoRotate(bool UseAuto) {
// 	FTapMomentImpl::Get()->SetUseAutoRotate(UseAuto);
// }

void TapUEMoment::DirectlyOpen(TUMomentType::Orientation Orientation, const FString& Page,
	TSharedPtr<FJsonObject> Extras) {
	ETapMomentOrientation _Orientation = (ETapMomentOrientation)Orientation;
	FTapMomentImpl::Get()->DirectlyOpen(_Orientation, Page, Extras);
}

void TapUEMoment::NeedDeferSystemGestures() {
	FTapMomentImpl::Get()->NeedDeferSystemGestures();
}

void TapUEMoment::OpenWebTopic() {
	FTapMomentImpl::Get()->OpenWebTopic();
}
