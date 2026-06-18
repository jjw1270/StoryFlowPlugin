// Copyright (c) 2026 장윤제. All rights reserved.

#include "StorySceneAssetTypeActions.h"
#include "StorySceneAsset.h"
#include "StorySceneEditor.h"

UClass* FStorySceneAssetTypeActions::GetSupportedClass() const
{
	return UStorySceneAsset::StaticClass();
}

void FStorySceneAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& _objects, TSharedPtr<IToolkitHost> _edit_within_level_editor)
{
	const EToolkitMode::Type mode = _edit_within_level_editor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* object : _objects)
	{
		UStorySceneAsset* scene_asset = Cast<UStorySceneAsset>(object);
		if (IsValid(scene_asset))
		{
			TSharedRef<FStorySceneEditor> scene_editor = MakeShared<FStorySceneEditor>();
			scene_editor->InitEditor(mode, _edit_within_level_editor, scene_asset);
		}
	}
}
