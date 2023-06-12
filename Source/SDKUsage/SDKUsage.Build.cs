// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;


public class SDKUsage : ModuleRules
{
	public SDKUsage(ReadOnlyTargetRules Target)
		: base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"AntiAddiction"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"DeveloperSettings",
			"Json",
			"JsonUtilities",
			
			"ActionRPG",

			"LeanCloud",
			"LeanCloudPush",
			"TapAchievement",
			"TapBillboard",
			"TapBootstrap",
			"TapCommon",
			"TapDB",
			"TapLogin",
			"TapSupport",
			"TapMoment", 
		});
	}
}
