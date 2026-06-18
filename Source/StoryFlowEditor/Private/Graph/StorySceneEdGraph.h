// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "EdGraph/EdGraph.h"
#include "StorySceneEdGraph.generated.h"

class UStorySceneAsset;
class UStorySceneGraphNode_Entry;

UCLASS()
class UStorySceneEdGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	virtual void PostLoad() override;

	UStorySceneAsset* GetOwningSceneAsset() const;
	UStorySceneGraphNode_Entry* FindEntryNode() const;
	void EnsureEntryNode();
	void GetReachableNodes(TSet<UEdGraphNode*>& _out_nodes) const;
	void RebuildRuntimeData();
};
