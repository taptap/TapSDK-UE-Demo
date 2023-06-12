// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;

public class ActionRPGEditorTarget : TargetRules
{
	public ActionRPGEditorTarget(TargetInfo Target)
		: base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange(new string[] { "ActionRPG", "SDKUsage" });
		DefaultBuildSettings = BuildSettingsVersion.V2;
	}
}
