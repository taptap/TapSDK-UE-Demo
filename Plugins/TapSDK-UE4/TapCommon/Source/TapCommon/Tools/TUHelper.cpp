#include "TUHelper.h"
#include "ImageUtils.h"
#include "TUSettings.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#if PLATFORM_WINDOWS
#include "Windows/WindowsWindow.h"
#endif

#if PLATFORM_MAC || PLATFORM_WINDOWS
#include "Desktop/qrcodegen.hpp"
#endif

void TUHelper::LaunchURL(const TCHAR* URL, const TCHAR* Param, FString* Error)
{
	auto Block = TUSettings::GetBlockBeforeLaunchUrl();
	if (Block) {
		Block();
	}
	FPlatformProcess::LaunchURL(URL, Param, Error);
}

void TUHelper::PerformOnGameThread(TFunction<void()> Function) {
	if (!Function) {
		return;
	}
	if (IsInGameThread())
	{
		Function();
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, Function);
	}
}


FString TUHelper::GetRandomStr(int len)
{
	FString LetterStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	FString SB;
	int LetterLen = LetterStr.Len();
	for (int i = 0; i < len; i++){
		SB += LetterStr[FMath::RandRange(0, LetterLen-1)];
	}

	return SB;
}

void TUHelper::GenerateBrush(FSlateBrush& Brush, const FString& TexturePath)
{
	UTexture2D *Texture = LoadObject<UTexture2D>(nullptr, *TexturePath);
	if(IsValid(Texture) && Brush.GetResourceObject() != Texture)
	{
		Brush.SetResourceObject(Texture);

		if (Texture) // Since this texture is used as UI, don't allow it affected by budget.
			{
			Texture->bForceMiplevelsToBeResident = true;
			Texture->bIgnoreStreamingMipBias = true;
			}

		if (Texture)
		{
			Brush.ImageSize.X = Texture->GetSizeX();
			Brush.ImageSize.Y = Texture->GetSizeY();
		}
	}
}

void TUHelper::JsonObjectAppend(TSharedPtr<FJsonObject>& Object, const TSharedPtr<FJsonObject>& Adder)
{
	if (Object.IsValid() && Adder.IsValid())
	{
		Object->Values.Append(Adder->Values);
	}
}

void TUHelper::JsonObjectAddNotEmptyString(TSharedPtr<FJsonObject>& Object, const FString& Key, const FString& Value)
{
	if (Value.Len() > 0)
	{
		Object->SetStringField(Key, Value);
	}
}

FString TUHelper::CombinParameters(const TSharedPtr<FJsonObject>& parameters, bool isEncode) {
	TArray<FString> keyValues;
	for (auto para : parameters->Values)
	{
		FString value;
		if (para.Value->TryGetString(value))
		{
			keyValues.Add(para.Key + "=" +  (isEncode ? FGenericPlatformHttp::UrlEncode(value) : value));
		}
	}
	return FString::Join(keyValues, TEXT("&"));
}

FString TUHelper::GetLCSign(const FString& ClientToken)
{
	const int64 Ticks = FDateTime::UtcNow().ToUnixTimestamp() * 1000;
	const FString TimeStr = FString::Printf(TEXT("%010lld"), Ticks);
	return FMD5::HashAnsiString(*(TimeStr + ClientToken)) + TEXT(",") + TimeStr;
}

#if PLATFORM_MAC
void TUHelper::ActivateItself() {
	FString Path = FPlatformProcess::BaseDir();
	int32 index = Path.Find(TEXT(".app"));
	if (index == INDEX_NONE) {
		return;
	}
	Path.LeftInline(index + 4);
	
	TUDebuger::DisplayLog(Path);
	FMacPlatformProcess::LaunchFileInDefaultExternalApplication(*Path);
}
#endif

#if PLATFORM_WINDOWS
void TUHelper::ActivateItself() {
	TSharedPtr<SWindow> Win;
	if (GEngine && GEngine->GameViewport)
	{
		Win = GEngine->GameViewport->GetWindow();
	}
	if (!Win)
	{
		Win = FSlateApplication::Get().GetActiveTopLevelWindow();
	}
	if (Win)
	{
		if (TSharedPtr<FGenericWindow> NWin = Win->GetNativeWindow())
		{
			if (Windows::HWND wnd_ = static_cast<Windows::HWND>(NWin->GetOSWindowHandle()))
			{
				HWND hForeWnd = ::GetForegroundWindow();
				DWORD dwForeID = ::GetWindowThreadProcessId(hForeWnd, NULL);
				DWORD dwCurID = ::GetCurrentThreadId();
				::AttachThreadInput(dwCurID, dwForeID, true);
				::ShowWindow(wnd_, SW_SHOW);
				::SetWindowPos(wnd_, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				::SetWindowPos(wnd_, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				::SetForegroundWindow(wnd_);
				::AttachThreadInput(dwCurID, dwForeID, false);
			}
		}
	}
}
#endif


#if PLATFORM_MAC || PLATFORM_WINDOWS
	
UTexture2D* TUHelper::GenerateQrCode(const FString& string)
{
	qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(TCHAR_TO_UTF8(*string), qrcodegen::QrCode::Ecc::LOW);
	int border = 0;
	int size = qr.getSize() + border * 2;
	TArray<FColor> pixels;
	pixels.SetNumZeroed(size * size);
	FColor black = FColor::Black;
	FColor white = FColor::White;
	for (int y = -border; y < qr.getSize() + border; y++) {
		for (int x = -border; x < qr.getSize() + border; x++) {
			FColor color = qr.getModule(x, y) ? black : white;
			pixels[(x + border) + (y + border) * size] = color;
		}
	}
	UTexture2D* texture = UTexture2D::CreateTransient(size, size, EPixelFormat::PF_B8G8R8A8);
#if ENGINE_MAJOR_VERSION >= 5
	void* data = texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
#else
	void* data = texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
#endif
	FMemory::Memcpy(data, pixels.GetData(), size * size * 4);
#if ENGINE_MAJOR_VERSION >= 5
	texture->GetPlatformData()->Mips[0].BulkData.Unlock();
#else
	texture->PlatformData->Mips[0].BulkData.Unlock();
#endif
	texture->UpdateResource();
	texture->Filter = TextureFilter::TF_Nearest;
	return texture;
}

#endif

