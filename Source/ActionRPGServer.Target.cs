// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGServerTarget : TargetRules
{
	public ActionRPGServerTarget(TargetInfo Target)
		: base(Target)
	{
		Type = TargetType.Server;
		ExtraModuleNames.AddRange(new string[] { "ActionRPG" });
		DefaultBuildSettings = BuildSettingsVersion.V2;
	}
}
