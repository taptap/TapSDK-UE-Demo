#include "TUDeviceInfo.h"
#include "SocketSubsystem.h"
#include "TUCommonStorage.h"
#include "TUDebuger.h"
#include "TUType.h"
#include "HAL/PlatformSurvey.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJava.h"
#include <jni.h>
#endif

static TAutoConsoleVariable<FString> CVarLoginId(
	TEXT("Tap.LoginId"),
	TEXT(""),
	TEXT("Change Login-id, empty string means not change, and use the no-debug Login-id."),
	ECVF_Default);

FString TUDeviceInfo::GetCPU()
{
	return FPlatformMisc::GetCPUBrand();
}
	
FString TUDeviceInfo::GetGPU()
{
	return FPlatformMisc::GetPrimaryGPUBrand();
}

// ++UE4+Release-4.27-CL-18319896
FString TUDeviceInfo::GetBuildVersion()
{
	return FApp::GetBuildVersion();
}

// 4.27.2-18319896+++UE4+Release-4.27
FString TUDeviceInfo::GetEngineVersion()
{
	return FEngineVersion::Current().ToString();
}

// 4.27.0-17155196+++UE4+Release-4.27
FString TUDeviceInfo::GetCompatibleEngineVersion()
{
	return FEngineVersion::CompatibleWith().ToString();
}
	
//OS: 10.16 (21E258)
FString TUDeviceInfo::GetOSVersion()
{
	return GetPlatform() + " " + FPlatformMisc::GetOSVersion();
}

int TUDeviceInfo::GetScreenWidth() {
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport) {
		return GEngine->GameViewport->Viewport->GetSizeXY().X;
	}
	return 0;
}

int TUDeviceInfo::GetScreenHeight() {
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport) {
		return GEngine->GameViewport->Viewport->GetSizeXY().Y;
	}
	return 0;
}

FString TUDeviceInfo::GetLoginId()
{
#if !UE_BUILD_SHIPPING
	FString DebugId = CVarLoginId.GetValueOnGameThread();
	if (!DebugId.IsEmpty())
	{
		UE_LOG(LogTap, VeryVerbose, TEXT("Get login id, use debug id: %s"), *DebugId);
		return DebugId;
	}
#endif
	return FPlatformMisc::GetLoginId();
}

FString TUDeviceInfo::GetInstallId()
{
	FString InstallID = TUDataStorage<FTUCommonStorage>::LoadString(FTUCommonStorage::InstallID);
	if (InstallID.Len() <= 0)
	{
		InstallID = FGuid::NewGuid().ToString();
		TUDataStorage<FTUCommonStorage>::SaveString(FTUCommonStorage::InstallID, InstallID);
	}
	return InstallID;
	
}

// FString DeviceInfo::GetMacAddress()
// {
// 	auto MacAddr = FPlatformMisc::GetMacAddress();
// 	TArray<FString> MacAddrStr;
// 	for (TArray<uint8>::TConstIterator it(MacAddr); it; ++it)
// 	{
// 		MacAddrStr.Add(FString::Printf(TEXT("%02x"), *it));
// 	}
// 	return FString::Join(MacAddrStr, TEXT(":"));
// }

FString TUDeviceInfo::GetPlatform()
{
	FString OS;
#if PLATFORM_IOS
	OS = "iOS";
#elif PLATFORM_ANDROID
	OS = "Android";
#elif PLATFORM_MAC
	OS = "macOS";
#elif PLATFORM_WINDOWS
	OS = "Windows";
#elif PLATFORM_LINUX
	OS = "Linux";
#endif
	return OS;
}

FString TUDeviceInfo::GetProjectName()
{
	return FApp::GetProjectName();
}

FString TUDeviceInfo::GetDeviceId()
{
	return FPlatformMisc::GetDeviceId();
}

FString TUDeviceInfo::GetIpv4()
{
	ISocketSubsystem *socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	FString HostName;
	FString Ipv4;
	if (socket->GetHostName(HostName))
	{
		FAddressInfoResult GAIRequest = socket->GetAddressInfo(*HostName, nullptr,
			EAddressInfoFlags::AllResultsWithMapping | EAddressInfoFlags::OnlyUsableAddresses,
			FNetworkProtocolTypes::IPv6);

		// Start packing the addresses we got to the results.
		if (GAIRequest.ReturnCode == SE_NO_ERROR)
		{
			for (auto Result : GAIRequest.Results)
			{
				FString IPStr = Result.Address->ToString(false);
				if (IPStr != "127.0.0.1" && IPStr != "0.0.0.0")
				{
					Ipv4 = IPStr;
				}
			}
		}
	}
	return Ipv4;
}

FString TUDeviceInfo::GetIpv6()
{
	ISocketSubsystem *socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	FString HostName;
	FString Ipv6;
	if (socket->GetHostName(HostName))
	{
		FAddressInfoResult GAIRequest = socket->GetAddressInfo(*HostName, nullptr,
			EAddressInfoFlags::AllResultsWithMapping | EAddressInfoFlags::OnlyUsableAddresses,
			FNetworkProtocolTypes::IPv6);

		// Start packing the addresses we got to the results.
		if (GAIRequest.ReturnCode == SE_NO_ERROR)
		{
			if (GAIRequest.Results.Num() > 0)
			{
				Ipv6 = GAIRequest.Results.Last().Address->ToString(false);
			}
		}
	}
	return Ipv6;
}

bool TUDeviceInfo::IsSmartPhone()
{
#if PLATFORM_IOS
	FString ProfileName = FPlatformMisc::GetDefaultDeviceProfileName();
	return ProfileName.StartsWith(TEXT("iPhone"));
#elif PLATFORM_ANDROID
	JNIEnv *env = FAndroidApplication::GetJavaEnv();
	auto TapCommonClass = FAndroidApplication::FindJavaClass("com/tds/TapCommonUE");
	if (TapCommonClass)
	{
		const char *strMethod = "isPad";
		auto jMethod = env->GetStaticMethodID(TapCommonClass, strMethod, "()Z");
		if (jMethod)
		{
			return !env->CallStaticBooleanMethod(TapCommonClass, jMethod);
		}
	}
	env->DeleteLocalRef(TapCommonClass);   
#endif
	return false;
}

void TUDeviceInfo::GetCountryAndLanguage(FString& Country, FString& Language) {
	static FString G_Country;
	static FString G_Language;
	if (G_Language.IsEmpty() && G_Country.IsEmpty()) {
		// #if PLATFORM_IOS || PLATFORM_MAC
		// 		G_Country = FString([[NSLocale currentLocale] objectForKey:NSLocaleCountryCode]);
		// 		G_Language = FString([[NSLocale currentLocale] objectForKey:NSLocaleLanguageCode]);
		// #else
		FHardwareSurveyResults OutResults;
		FPlatformSurvey::GetSurveyResults(OutResults);
		FString OSLanguage = FString(OutResults.OSLanguage);
		TArray<FString> ResultArray;
		OSLanguage.ParseIntoArray(ResultArray, TEXT("-"));
		if (ResultArray.Num() == 1) {
			G_Language = ResultArray[0];
		} else if (ResultArray.Num() > 1) {
			G_Country = ResultArray.Last();
			// ResultArray.RemoveAt(ResultArray.Num() -1);
			G_Language = OSLanguage;
		}
		// #endif
	}
	Country = G_Country;
	Language = G_Language;
}


