#include "TapMoment.h"

#include "TapMomentImpl.h"


void FTapMoment::Init(const FTapMomentConfig& Config) {
	FTapMomentImpl::Get()->Init(Config);
}

void FTapMoment::SetCallback(FDelegate CallBack) {
	FTapMomentImpl::Get()->SetCallback(CallBack);
}

void FTapMoment::Open(ETapMomentOrientation Orientation) {
	FTapMomentImpl::Get()->Open(Orientation);
}

void FTapMoment::Publish(ETapMomentOrientation Orientation, const TArray<FString>& ImagePaths, const FString& Content) {
	FTapMomentImpl::Get()->Publish(Orientation, ImagePaths, Content);
}

void FTapMoment::PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths,
	const TArray<FString>& ImagePaths, const FString& Title, const FString& Desc) {
	FTapMomentImpl::Get()->PublishVideo(Orientation, VideoPaths,ImagePaths, Title, Desc);
}

void FTapMoment::PublishVideo(ETapMomentOrientation Orientation, const TArray<FString>& VideoPaths,
	const FString& Title, const FString& Desc) {
	FTapMomentImpl::Get()->PublishVideo(Orientation, VideoPaths,Title, Desc);
}

void FTapMoment::FetchNotification() {
	FTapMomentImpl::Get()->FetchNotification();
}

void FTapMoment::Close() {
	FTapMomentImpl::Get()->Close();
}

void FTapMoment::Close(const FString& Title, const FString& Content) {
	FTapMomentImpl::Get()->Close(Title, Content);
}

void FTapMoment::DirectlyOpen(ETapMomentOrientation Orientation, const FString& Page, TSharedPtr<FJsonObject> Extras) {
	FTapMomentImpl::Get()->DirectlyOpen(Orientation, Page, Extras);
}

void FTapMoment::NeedDeferSystemGestures() {
	FTapMomentImpl::Get()->NeedDeferSystemGestures();
}

void FTapMoment::OpenWebTopic() {
	FTapMomentImpl::Get()->OpenWebTopic();
}
