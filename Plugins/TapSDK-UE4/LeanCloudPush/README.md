# LeanCloudPush

## 支持平台

* Android
* iOS

## 前提条件

* 安装 **UE 4.26** 及以上版本
* IOS **12** 或更高版本 
* Android MinSDK 为 **API21** 或更高版本


## 如何使用

### 安装插件

* 下载 **[TapSDK.zip](https://github.com/taptap/TapSDK-UE4/releases)** 解压后将 `LeanCloudPush`、`LeanCloud` 文件夹 `Copy` 到项目的 `Plugins` 目录中（如果项目中缺少关于 AndroidX 的配置，可以将 `AndroidX` 也 `Copy` 到项目中）
* 重启 Unreal Editor
* 打开 **编辑 > 插件 > 项目 > TapTap**，开启 `LeanCloudPush` 模块

### 依赖所需模块
在 **Project.Build.cs** 中添加所需模块:
```c#
PublicDependencyModuleNames.AddRange(new string[] { 
    "Core",
    "CoreUObject",
    "Engine",
    "Slate",
    "SlateCore",
    "Http",
    "Json",
    "JsonUtilities",
    "LeanCloud",
    "LeanCloudPush"
});
```

### 项目所需配置

#### iOS所需配置项
在文件 **DefaultEngine.ini** 添加如下配置

```ini
[/Script/IOSRuntimeSettings.IOSRuntimeSettings]
bEnableRemoteNotificationsSupport=True
```

然后确认iOS证书支持推送功能

#### Android所需配置项

* 新建一个名叫 **app** 的文件夹，将从华为开发者中心下载到的 **agconnect-services.json** 文件拷贝到文件夹里
* 在项目安卓的 UPL 文件中加入如下代码（没有 UPL 新建一个），填写正确的配置

```xml
<resourceCopies>
    <copyDir src="$S(PluginDir)/你的路径/app/" dst="$S(BuildDir)/gradle/app/" />
</resourceCopies>

<androidManifestUpdates>
    <addElements tag="application">
        <meta-data
                android:name="com.vivo.push.api_key"
                android:value="vivo的key"/>
        <meta-data
                android:name="com.vivo.push.app_id"
                android:value="vivo的appid"/>
        <meta-data
                android:name="com.hihonor.push.app_id"
                android:value="荣耀的appid" />

    </addElements>
</androidManifestUpdates>
 ```

### 导入头文件
```cpp
#if PLATFORM_IOS
#include "iOS/LCIOSPush.h"
#elif PLATFORM_ANDROID
#include "Android/LCAndroidPush.h"
#endif
```

### 如何使用

将开发者平台获得的配置填入下面接口中
```cpp
#if PLATFORM_IOS
	FLCIOSPush::Register("iOS Team ID");
#elif PLATFORM_ANDROID
	FString DeviceName = FLCAndroidPush::GetDeviceName().ToLower();
	if (DeviceName.Contains("huawei")) {
		FLCAndroidPush::RegisterHuaWei();
	} else if (DeviceName.Contains("oppo")) {
		FLCAndroidPush::RegisterOPPO("oppo的AppKey", "oppo的AppSecret");
	} else if (DeviceName.Contains("vivo")) {
		FLCAndroidPush::RegisterVIVO();
	} else if (DeviceName.Contains("meizu")) {
		FLCAndroidPush::RegisterMeiZu("meizu的AppId", "meizu的AppKey");
	} else if (DeviceName.Contains("honor")) {
		FLCAndroidPush::RegisterHonor();
	} else {
		FLCAndroidPush::RegisterXiaoMi("小米的AppId", "小米的AppKey");
	}
#endif
```

