// Copyright (c) 2026 장윤제. All rights reserved.

#include "Graph/SStorySceneGraphNode_Branch.h"
#include "Graph/StorySceneGraphNode_Branch.h"
#include "StoryBranchBase.h"
#include "StoryBranchNodeData.h"

void SStorySceneGraphNode_Branch::Construct(const FArguments& _args)
{
	ConstructBase(_args._GraphNodeObj);
}

UObject* SStorySceneGraphNode_Branch::GetTemplateObject() const
{
	const UStorySceneGraphNode_Branch* branch_node = GetStorySceneGraphNode();
	const UStoryBranchNodeData* branch_node_data = IsValid(branch_node) ? branch_node->GetBranchNodeData() : nullptr;
	return IsValid(branch_node_data) ? branch_node_data->GetBranchTemplate() : nullptr;
}

void SStorySceneGraphNode_Branch::OnTemplatePropertyValueChanged()
{
	if (UStorySceneGraphNode_Branch* branch_node = GetStorySceneGraphNode())
	{
		branch_node->SyncNextPinsToNodeData();
	}
}

UStorySceneGraphNode_Branch* SStorySceneGraphNode_Branch::GetStorySceneGraphNode() const
{
	return Cast<UStorySceneGraphNode_Branch>(GraphNode);
}
