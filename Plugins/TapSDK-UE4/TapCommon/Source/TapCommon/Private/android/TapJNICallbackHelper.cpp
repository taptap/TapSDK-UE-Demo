#include "TapJNICallbackHelper.h"

TMap<int, TSharedPtr<FTapJNICallbackHelper::FModel>> FTapJNICallbackHelper::CallBackModels;
int FTapJNICallbackHelper::CallBackID = 0;