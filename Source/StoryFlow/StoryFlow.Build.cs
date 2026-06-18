// Copyright (c) 2026 장윤제. All rights reserved.

using UnrealBuildTool;

public class StoryFlow : ModuleRules
{
	public StoryFlow(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"DeveloperSettings",
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CommonLibrary"
			});
	}
}
