// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DPLM : ModuleRules
{
	public DPLM(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
