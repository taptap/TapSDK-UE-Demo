// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TapAchievement : ModuleRules
{
	public TapAchievement(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"TapCommon",
				"TapBootstrap",
				"LeanCloud"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Json",
				"JsonUtilities",
				"HTTP",
				"UMG",
				"ImageWrapper",
			}
		);
	}
}