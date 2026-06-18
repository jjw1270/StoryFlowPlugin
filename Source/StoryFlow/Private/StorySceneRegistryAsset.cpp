// Copyright (c) 2026 장윤제. All rights reserved.

#include "StorySceneRegistryAsset.h"
#include "CommonUtils.h"
#include "StorySceneAsset.h"

UStorySceneAsset* FStorySceneReference::ResolveSceneAsset() const
{
	if (SceneAsset.IsNull())
	{
		return nullptr;
	}

	return SceneAsset.LoadSynchronous();
}

FStorySceneID FStorySceneReference::GetSceneID() const
{
	const UStorySceneAsset* scene_asset = ResolveSceneAsset();
	return ::IsValid(scene_asset) ? scene_asset->GetSceneID() : FStorySceneID();
}

FText FStorySceneReference::GetDisplayName() const
{
	const UStorySceneAsset* scene_asset = ResolveSceneAsset();
	return ::IsValid(scene_asset) ? scene_asset->GetDisplayName() : FText::GetEmpty();
}

const FStorySceneReference* UStorySceneRegistryAsset::FindSceneReference(const FStorySceneID& _scene_id) const
{
	if (_scene_id.IsValid() == false)
	{
		return nullptr;
	}

	for (const auto& scene_reference : _Scenes)
	{
		if (scene_reference.GetSceneID() == _scene_id)
		{
			return &scene_reference;
		}
	}

	return nullptr;
}

UStorySceneAsset* UStorySceneRegistryAsset::FindSceneAsset(const FStorySceneID& _scene_id) const
{
	const FStorySceneReference* scene_reference = FindSceneReference(_scene_id);
	if (IsInvalid(scene_reference))
	{
		return nullptr;
	}

	return scene_reference->ResolveSceneAsset();
}
