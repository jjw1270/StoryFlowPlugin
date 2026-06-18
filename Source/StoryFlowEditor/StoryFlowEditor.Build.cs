// Copyright (c) 2026 장윤제. All rights reserved.

using UnrealBuildTool;

public class StoryFlowEditor : ModuleRules
{
	public StoryFlowEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CommonLibrary",
				"StoryFlow",
				"UnrealEd",
				"GraphEditor",
				"EditorFramework",
				"AssetTools",
				"PropertyEditor",
				"Slate",
				"SlateCore",
				"InputCore",
				"ApplicationCore"
			});
	}
}
