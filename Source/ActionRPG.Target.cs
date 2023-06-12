// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionRPGTarget : TargetRules
{
	public ActionRPGTarget(TargetInfo Target)
		: base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.AddRange(new string[] { "ActionRPG", "SDKUsage" });
		DefaultBuildSettings = BuildSettingsVersion.V2;
	}
}
