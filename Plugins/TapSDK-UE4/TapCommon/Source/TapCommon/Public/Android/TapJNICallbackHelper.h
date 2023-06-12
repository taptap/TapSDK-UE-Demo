
#pragma once
#include "TUDebuger.h"

class TAPCOMMON_API FTapJNICallbackHelper {
public:
	template<typename... CallBackType>
	static int AddCallBack(const CallBackType&... CallBacks) {
		CallBackID++;
		TSharedPtr<FModel> ModelPtr = MakeShared<FModel>();
		ModelAddCallBacks(ModelPtr, CallBacks...);
		CallBackModels.Add(CallBackID, ModelPtr);
		return CallBackID;
	}

	static void RemoveCallBack(int InCallBackID) {
		CallBackModels.Remove(InCallBackID);
	}

	template<typename CallBackType>
	static CallBackType* FindCallBack(int InCallBackID, int Index = 0) {
		TSharedPtr<FModel> *Ptr = CallBackModels.Find(InCallBackID);
		if (Ptr == nullptr) {
			return nullptr;
		}
		return (CallBackType*)((*Ptr)->CallBacks[Index]);
	}

private:
	struct FModel {
		TArray<void *> CallBacks;
		TArray<TFunction<void()>> DeleteActions;
		
		~FModel() {
			for (auto DeleteAction : DeleteActions) {
				DeleteAction();
			}
		}
	};
	
	static TMap<int, TSharedPtr<FModel>> CallBackModels;
	static int CallBackID;

	template <typename CallBackType, typename... OtherCallBackType>
	static void ModelAddCallBacks(TSharedPtr<FModel> ModelPtr, const CallBackType& CallBack, const OtherCallBackType&... OtherCallBacks) {
		CallBackType* CallBackPtr = new CallBackType(CallBack);
		ModelPtr->CallBacks.Add((void*)CallBackPtr);
		ModelPtr->DeleteActions.Add([=]() {
			delete CallBackPtr;
		});
		ModelAddCallBacks(ModelPtr, OtherCallBacks...);
	}
	
	template <typename CallBackType>
	static void ModelAddCallBacks(TSharedPtr<FModel> ModelPtr, const CallBackType& CallBack) {
		CallBackType* CallBackPtr = new CallBackType(CallBack);
		ModelPtr->CallBacks.Add(CallBackPtr);
		ModelPtr->DeleteActions.Add([=]() {
			delete CallBackPtr;
		});
	}
};
