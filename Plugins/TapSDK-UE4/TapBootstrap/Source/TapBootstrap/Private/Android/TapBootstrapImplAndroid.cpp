#include "TapBootstrapImplAndroid.h"

#include "TapBootstrap.h"
#include "TapBootstrapJavaHelper.h"
#include "TapJavaHelper.h"
#include "TapJNI.h"
#include "TUHelper.h"
#include "Android/AndroidJavaEnv.h"
#include "Android/AndroidJNI.h"

typedef TTuple<FTDSUser::FDelegate, FTUError::FDelegate> UserResultDelegate;
typedef TTuple<FTDSLeaderBoardRanking::FRankingsDelegate, FTUError::FDelegate> RankingsResultDelegate;
typedef TTuple<FStringSignature, FLCError::FDelegate> RetrieveShortTokenDelegate;
TMap<int, UserResultDelegate> UserResultCallBacks;
TMap<int, RankingsResultDelegate> RankingsResultCallBacks;
TMap<int, RetrieveShortTokenDelegate> RetrieveShortTokenCallBacks;


#define TapBootstrapUE "com/tds/TapBootstrapUE"

FTapBootstrapImplAndroid::~FTapBootstrapImplAndroid() {
}

void FTapBootstrapImplAndroid::Init(const FTUConfig& InConfig) {
	FTUConfig::Init(InConfig);
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	auto Activity = JNI.GetActivity();
	auto ClientID = JNI.ToJavaString(InConfig.ClientID);
	auto ClientToken = JNI.ToJavaString(InConfig.ClientToken);
	auto ServerURL = JNI.ToJavaString(InConfig.ServerURL);
	auto Channel = JNI.ToJavaString(InConfig.DBConfig.Channel);
	auto GameVersion = JNI.ToJavaString(InConfig.DBConfig.GameVersion);
	bool IsCN = InConfig.RegionType == ERegionType::CN;
	bool DBConfigEnable = InConfig.DBConfig.Enable;
	bool bBillboardEnable = InConfig.BillboardConfig.IsValid();
	if (bBillboardEnable) {
		auto jBillboardUrl = JNI.ToJavaString(InConfig.BillboardConfig->BillboardUrl);

		JNIEnv*	JEnv = *JNI;
	
		auto jDimensionString = NewScopedJavaObject(JEnv, (jobjectArray)JEnv->NewObjectArray(InConfig.BillboardConfig->Dimensions.Num() * 2, FJavaWrapper::JavaStringClass, nullptr));
		int32 Index = 0;
		for (TTuple<FString, FString>& T : InConfig.BillboardConfig->Dimensions)
		{
			JEnv->SetObjectArrayElement(*jDimensionString, Index++, *FJavaHelper::ToJavaString(JEnv, T.Key));
			JEnv->SetObjectArrayElement(*jDimensionString, Index++, *FJavaHelper::ToJavaString(JEnv, T.Value));
		}
		JNI.CallStaticVoidMethod(ClassObject, "init", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZLjava/lang/String;Ljava/lang/String;ZLjava/lang/String;[Ljava/lang/String;)V", *Activity, *ClientID, *ClientToken, *ServerURL, IsCN, DBConfigEnable, *Channel, *GameVersion, bBillboardEnable, *jBillboardUrl, *jDimensionString);
	} else {
		JNI.CallStaticVoidMethod(ClassObject, "init", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZLjava/lang/String;Ljava/lang/String;ZLjava/lang/String;[Ljava/lang/String;)V", *Activity, *ClientID, *ClientToken, *ServerURL, IsCN, DBConfigEnable, *Channel, *GameVersion, bBillboardEnable, NULL, NULL);
	}
	
	
}

TSharedPtr<FTDSUser> FTapBootstrapImplAndroid::GetCurrentUser() {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	auto UserObject = JNI.CallStaticObjectMethod(ClassObject, "getCurrentUser",
	                                             "()Lcom/tapsdk/bootstrap/account/TDSUser;");
	return TapBootstrapJavaHelper::ConvertUser(JNI, UserObject);
}

void FTapBootstrapImplAndroid::Logout() {
	if (GetCurrentUser().IsValid())
	{
		FTapBootstrap::OnUserLogout.Broadcast();
	}
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	JNI.CallStaticVoidMethod(ClassObject, "logout", "()V");
}

void FTapBootstrapImplAndroid::LoginAnonymously(FTDSUser::FDelegate OnSuccess, FTUError::FDelegate OnError) {
	int InCallBackID = GetCallBackID();
	UserResultCallBacks.Add(InCallBackID, MakeTuple(OnSuccess, OnError));

	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	JNI.CallStaticVoidMethod(ClassObject, "loginAnonymously", "(I)V", InCallBackID);
}

void FTapBootstrapImplAndroid::LoginWithTapTap(const TArray<FString>& Permissions, FTDSUser::FDelegate OnSuccess,
                                               FTUError::FDelegate OnError) {
	int InCallBackID = GetCallBackID();
	UserResultCallBacks.Add(InCallBackID, MakeTuple(OnSuccess, OnError));

	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	auto Activity = JNI.GetActivity();
	auto PermissionsObject = JNI.GetStringArray(Permissions);
	JNI.CallStaticVoidMethod(ClassObject, "loginWithTapTap", "(Landroid/app/Activity;[Ljava/lang/String;I)V", *Activity, *PermissionsObject, InCallBackID);
}

void FTapBootstrapImplAndroid::BecomeWithSessionToken(const FString& SessionToken, FTDSUser::FDelegate OnSuccess,
                                                      FTUError::FDelegate OnError) {
	int InCallBackID = GetCallBackID();
	UserResultCallBacks.Add(InCallBackID, MakeTuple(OnSuccess, OnError));

	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	JNI.CallStaticVoidMethod(ClassObject, "becomeWithSessionToken", "(Ljava/lang/String;I)V",
	                         *JNI.ToJavaString(SessionToken), InCallBackID);
}

void FTapBootstrapImplAndroid::LoginWithAuthData(const FString& Platform, TSharedPtr<FJsonObject> AuthData,
                                                 FTDSUser::FDelegate OnSuccess, FTUError::FDelegate OnError) {
	int InCallBackID = GetCallBackID();
	UserResultCallBacks.Add(InCallBackID, MakeTuple(OnSuccess, OnError));

	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	auto map = TapJavaHelper::GetMapFromJsonObject(JNI, AuthData);
	JNI.CallStaticVoidMethod(ClassObject, "loginWithAuthData", "(Ljava/util/Map;Ljava/lang/String;I)V", *map,
	                         *JNI.ToJavaString(Platform), InCallBackID);
}

void FTapBootstrapImplAndroid::AssociateWithAuthData(const FTDSUser& InUser, const FString& Platform,
                                                     TSharedPtr<FJsonObject> AuthData, FTDSUser::FDelegate OnSuccess,
                                                     FTUError::FDelegate OnError) {
	int InCallBackID = GetCallBackID();
	UserResultCallBacks.Add(InCallBackID, MakeTuple(OnSuccess, OnError));

	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	auto map = TapJavaHelper::GetMapFromJsonObject(JNI, AuthData);
	auto java_user = TapBootstrapJavaHelper::ConvertUser(JNI, InUser);
	JNI.CallStaticVoidMethod(ClassObject, "associateWithAuthData",
	                         "(Lcom/tapsdk/bootstrap/account/TDSUser;Ljava/util/Map;Ljava/lang/String;I)V", *java_user,
	                         *map, *JNI.ToJavaString(Platform), InCallBackID);
}

void FTapBootstrapImplAndroid::DisassociateAuthData(const FTDSUser& InUser, const FString& Platform,
                                                    FTDSUser::FDelegate OnSuccess, FTUError::FDelegate OnError) {
	int InCallBackID = GetCallBackID();
	UserResultCallBacks.Add(InCallBackID, MakeTuple(OnSuccess, OnError));

	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	auto java_user = TapBootstrapJavaHelper::ConvertUser(JNI, InUser);
	JNI.CallStaticVoidMethod(ClassObject, "dissociateWithAuthData",
	                         "(Lcom/tapsdk/bootstrap/account/TDSUser;Ljava/lang/String;I)V", *java_user,
	                         *JNI.ToJavaString(Platform), InCallBackID);
}

void FTapBootstrapImplAndroid::SaveUser(const FTDSUser& InUser) {
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	auto java_user = TapBootstrapJavaHelper::ConvertUser(JNI, InUser);
	JNI.CallStaticVoidMethod(ClassObject, "save", "(Lcom/tapsdk/bootstrap/account/TDSUser;)V", *java_user);
}

void FTapBootstrapImplAndroid::QueryTapFriendsLeaderBoard(const FString& Name, int From, int Limit,
	FTDSLeaderBoardRanking::FRankingsDelegate OnSuccess, FTUError::FDelegate OnError) {
	int InCallBackID = GetCallBackID();
	RankingsResultCallBacks.Add(InCallBackID, MakeTuple(OnSuccess, OnError));
	
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	JNI.CallStaticVoidMethod(ClassObject, "queryTapFriendsLeaderBoard",
							 "(Ljava/lang/String;III)V",
							 *JNI.ToJavaString(Name), From, Limit, InCallBackID);
}

void FTapBootstrapImplAndroid::RetrieveShortToken(const FLCUser& InUser, const FStringSignature& OnSuccess,
	const FLCError::FDelegate& OnFailed) {
	int InCallBackID = GetCallBackID();
	RetrieveShortTokenCallBacks.Add(InCallBackID, MakeTuple(OnSuccess, OnFailed));
	
	TapJNI::JNI JNI;
	auto ClassObject = JNI.FindClass(TapBootstrapUE);
	JNI.CallStaticVoidMethod(ClassObject, "retrieveShortToken",
							 "(Ljava/lang/String;I)V",
							 *JNI.ToJavaString(InUser.GetSessionToken()), InCallBackID);
}


int FTapBootstrapImplAndroid::GetCallBackID() {
	return CallBackID++;
}

#ifdef __cplusplus
extern "C" {
#endif

	JNIEXPORT void Java_com_tds_TapBootstrapUE_onUserError(JNIEnv* jenv, jclass thiz, jint code, jstring Message,
	                                                       jint callBackID) {
	auto CallBackPtr = UserResultCallBacks.Find(callBackID);
	if (CallBackPtr == nullptr) {
		return;
	}
	TapJNI::JNI JNI(jenv);
	FString Msg_UE = JNI.GetFStringFromParam(Message);
	FTUError Error(code, Msg_UE);
	FTUError::FDelegate CallBack = (*CallBackPtr).Get<1>();
	TUHelper::PerformOnGameThread([=]() {
		CallBack.ExecuteIfBound(Error);
		UserResultCallBacks.Remove(callBackID);
	});
}

	JNIEXPORT void Java_com_tds_TapBootstrapUE_onUserSuccess(JNIEnv* jenv, jclass thiz, jobject user, jint callBackID) {
	auto CallBackPtr = UserResultCallBacks.Find(callBackID);
	if (CallBackPtr == nullptr) {
		return;
	}
	TapJNI::JNI JNI(jenv);
	auto Java_User = TapJNI::MakeScopedJavaObject(jenv, user, false);
	TSharedPtr<FTDSUser> UserPtr = TapBootstrapJavaHelper::ConvertUser(JNI, Java_User);

	TUHelper::PerformOnGameThread([=]() {
		if (UserPtr.IsValid()) {
			FTDSUser::FDelegate CallBack = (*CallBackPtr).Get<0>();
			CallBack.ExecuteIfBound(*UserPtr);
		}
		else {
			FTUError::FDelegate CallBack = (*CallBackPtr).Get<1>();
			CallBack.ExecuteIfBound(FTUError(-1, "java user empty"));
		}
		UserResultCallBacks.Remove(callBackID);
	});
}

	JNIEXPORT void Java_com_tds_TapBootstrapUE_onRankingsError(JNIEnv* jenv, jclass thiz, jint code, jstring Message,
														   jint callBackID) {
		auto CallBackPtr = RankingsResultCallBacks.Find(callBackID);
		if (CallBackPtr == nullptr) {
			return;
		}
		TapJNI::JNI JNI(jenv);
		FString Msg_UE = JNI.GetFStringFromParam(Message);
		FTUError Error(code, Msg_UE);
		FTUError::FDelegate CallBack = (*CallBackPtr).Get<1>();
		TUHelper::PerformOnGameThread([=]() {
			CallBack.ExecuteIfBound(Error);
			RankingsResultCallBacks.Remove(callBackID);
		});
	}

	JNIEXPORT void Java_com_tds_TapBootstrapUE_onRankingsSuccess(JNIEnv* jenv, jclass thiz, jobject java_rankings, jint callBackID) {
		auto CallBackPtr = RankingsResultCallBacks.Find(callBackID);
		if (CallBackPtr == nullptr) {
			return;
		}
		TapJNI::JNI JNI(jenv);
		TArray<FTDSLeaderBoardRanking> UE_Rankings;
		TapJavaHelper::ListForEach(JNI, TapJNI::MakeScopedJavaObject(*JNI, java_rankings, false), [=, &UE_Rankings](int Index, const TapJNI::Object& Object) {
			FTDSLeaderBoardRanking UE_Ranking;
			UE_Ranking.Rank = JNI.CallIntMethod(Object, "getRank", "()I");
			UE_Ranking.StatisticValue = JNI.CallIntMethod(Object, "getStatisticValue", "()I");
			auto StatisticName = JNI.CallObjectMethod(Object, "getStatisticName", "()Ljava/lang/String;");
			UE_Ranking.StatisticName = JNI.GetFString(StatisticName);
			auto User = JNI.CallObjectMethod(Object, "getUser", "()Lcom/tapsdk/bootstrap/account/TDSUser;");
			UE_Ranking.User = TapBootstrapJavaHelper::ConvertUser(JNI, User);
			UE_Rankings.Add(UE_Ranking);
		});

		TUHelper::PerformOnGameThread([=]() {
			FTDSLeaderBoardRanking::FRankingsDelegate CallBack = (*CallBackPtr).Get<0>();
			CallBack.ExecuteIfBound(UE_Rankings);
			RankingsResultCallBacks.Remove(callBackID);
		});
	}

	__attribute__((visibility("default"))) void Java_com_tds_TapBootstrapUE_onRetrieveShortTokenSuccess(JNIEnv *jenv, jclass thiz, jstring credential, int callBackID)
	{
		auto CallBackPtr = RetrieveShortTokenCallBacks.Find(callBackID);
		if (CallBackPtr == nullptr) {
			return;
		}
		TapJNI::JNI JNI(jenv);
		FString Credential = JNI.GetFStringFromParam(credential);
		FStringSignature CallBack = (*CallBackPtr).Get<0>();
		TUHelper::PerformOnGameThread([=]() {
			CallBack.ExecuteIfBound(Credential);
			RetrieveShortTokenCallBacks.Remove(callBackID);
		});
	}

	__attribute__((visibility("default"))) void Java_com_tds_TapBootstrapUE_onRetrieveShortTokenError(JNIEnv* jenv, jclass thiz, jint code, jstring Message, jint callBackID) {
		auto CallBackPtr = RetrieveShortTokenCallBacks.Find(callBackID);
		if (CallBackPtr == nullptr) {
			return;
		}
		TapJNI::JNI JNI(jenv);
		FString Msg_UE = JNI.GetFStringFromParam(Message);
		FLCError Error(code, Msg_UE);
		FLCError::FDelegate CallBack = (*CallBackPtr).Get<1>();
		TUHelper::PerformOnGameThread([=]() {
			CallBack.ExecuteIfBound(Error);
			RetrieveShortTokenCallBacks.Remove(callBackID);
		});
	}


#ifdef __cplusplus
}
#endif
