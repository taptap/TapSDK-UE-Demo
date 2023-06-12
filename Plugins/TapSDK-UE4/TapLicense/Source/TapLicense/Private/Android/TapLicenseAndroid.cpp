#include "TapLicenseAndroid.h"

#include "TapJavaHelper.h"
#include "TapJNI.h"
#include "TapJNICallbackHelper.h"
#include "TUHelper.h"

#define TapLicenseUE "com/tds/TapLicenseUE"

FTapLicenseAndroid::~FTapLicenseAndroid() {
}

void FTapLicenseAndroid::SetLicenseCallback(FSimpleDelegate OnSuccess) {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapLicenseUE);
	JNI.CallStaticVoidMethod(ClassObject, "setLicenseCallback", "(I)V", FTapJNICallbackHelper::AddCallBack(OnSuccess));
}

void FTapLicenseAndroid::SetDLCCallback(bool bCheckOnce, const FString& PublicKey,
                                        FTapLicense::FDLCQueryDelegate OnQuery,
                                        FTapLicense::FDLCOrderDelegate OnOrder) {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapLicenseUE);
	JNI.CallStaticVoidMethod(ClassObject, "setDLCCallback", "(ZLjava/lang/String;I)V", bCheckOnce,
	                         *JNI.ToJavaString(PublicKey), FTapJNICallbackHelper::AddCallBack(OnQuery, OnOrder));
}

void FTapLicenseAndroid::Check(bool bIsForce) {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapLicenseUE);
	JNI.CallStaticVoidMethod(ClassObject, "check", "(Landroid/app/Activity;Z)V", *JNI.GetActivity(), bIsForce);
}

void FTapLicenseAndroid::QueryDLC(const TArray<FString>& DLCList) {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapLicenseUE);
	JNI.CallStaticVoidMethod(ClassObject, "queryDLC", "(Landroid/app/Activity;Ljava/util/List;)V", *JNI.GetActivity(),
	                         *TapJavaHelper::ConvertUEArray(JNI, DLCList));
}

void FTapLicenseAndroid::PurchaseDLC(const FString& DLC) {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapLicenseUE);

	JNI.CallStaticVoidMethod(ClassObject, "purchaseDLC", "(Landroid/app/Activity;Ljava/lang/String;)V",
	                         *JNI.GetActivity(), *JNI.ToJavaString(DLC));
}

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((visibility("default"))) void Java_com_tds_TapLicenseUE_onLicenseSuccess(
	JNIEnv* jenv, jclass thiz, int callBackID) {
	auto CallBackPtr = FTapJNICallbackHelper::FindCallBack<FSimpleDelegate>(callBackID);
	if (CallBackPtr == nullptr) {
		return;
	}
	TUHelper::PerformOnGameThread([=]() {
		CallBackPtr->ExecuteIfBound();
	});

}

__attribute__((visibility("default"))) void Java_com_tds_TapLicenseUE_onQueryCallBack(
	JNIEnv* jenv, jclass thiz, int code, jobject queryList, int callBackID) {

	auto CallBackPtr = FTapJNICallbackHelper::FindCallBack<FTapLicense::FDLCQueryDelegate>(callBackID);
	if (CallBackPtr == nullptr) {
		return;
	}
	TapJNI::JNI JNI(jenv);
	FTapLicense::Map Map;
	TSharedPtr<FJsonObject> MapPtr = TapJavaHelper::GetJsonObjectFromMap(JNI, JNI.MakeJavaObject(queryList));
	if (MapPtr.IsValid()) {
		for (auto para : MapPtr->Values) {
			int value;
			if (para.Value->TryGetNumber(value)) {
				Map.Add(para.Key, value);
			}
		}
	}
	TUHelper::PerformOnGameThread([=]() {
		CallBackPtr->ExecuteIfBound((FTapLicense::EQueryResult)code, Map);
	});
}

__attribute__((visibility("default"))) void Java_com_tds_TapLicenseUE_onOrderCallBack(
	JNIEnv* jenv, jclass thiz, jstring sku, int status, int callBackID) {
	auto CallBackPtr = FTapJNICallbackHelper::FindCallBack<FTapLicense::FDLCOrderDelegate>(callBackID, 1);
	if (CallBackPtr == nullptr) {
		return;
	}
	TapJNI::JNI JNI(jenv);
	FString SKU = JNI.GetFStringFromParam(sku);
	TUHelper::PerformOnGameThread([=]() {
		CallBackPtr->ExecuteIfBound(SKU, (FTapLicense::EOrderStatus)status);
	});
}


#ifdef __cplusplus
}
#endif
