// Copyright (c) 2026 장윤제. All rights reserved.

#include "Graph/SStorySceneGraphNode_Entry.h"
#include "Graph/StorySceneGraphNode_Entry.h"
#include "StorySceneBase.h"
#include "StorySceneAsset.h"

void SStorySceneGraphNode_Entry::Construct(const FArguments& _args)
{
	ConstructBase(_args._GraphNodeObj);
}

UObject* SStorySceneGraphNode_Entry::GetTemplateObject() const
{
	const UStorySceneGraphNode_Entry* entry_node = GetStorySceneGraphNode();
	UStorySceneAsset* scene_asset = IsValid(entry_node) ? entry_node->GetOwningSceneAsset() : nullptr;
	return IsValid(scene_asset) ? scene_asset->GetSceneTemplate() : nullptr;
}

UStorySceneGraphNode_Entry* SStorySceneGraphNode_Entry::GetStorySceneGraphNode() const
{
	return Cast<UStorySceneGraphNode_Entry>(GraphNode);
}
