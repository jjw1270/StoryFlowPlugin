// Copyright (c) 2026 장윤제. All rights reserved.

#include "Graph/StorySceneEdGraph.h"
#include "Graph/StorySceneGraphNode_Entry.h"
#include "Graph/StorySceneGraphNode_Branch.h"
#include "Graph/StorySceneGraphNode_Shot.h"
#include "Graph/StorySceneGraphNode_Transition.h"
#include "StorySceneAsset.h"
#include "StoryBranchNodeData.h"
#include "StorySceneNodeData.h"
#include "CommonUtils.h"

namespace
{
	static FStorySceneBranchLink MakeBranchLinkFromTargetNode(const UEdGraphNode* _target_node)
	{
		FStorySceneBranchLink branch_link;

		if (const UStorySceneGraphNode_Shot* target_shot_node = Cast<UStorySceneGraphNode_Shot>(_target_node))
		{
			if (IsValid(target_shot_node->GetShotNodeData()))
			{
				branch_link.NextShotID = target_shot_node->GetShotNodeData()->GetShotID();
			}
		}
		else if (const UStorySceneGraphNode_Branch* target_branch_node = Cast<UStorySceneGraphNode_Branch>(_target_node))
		{
			if (IsValid(target_branch_node->GetBranchNodeData()))
			{
				branch_link.NextBranchID = target_branch_node->GetBranchNodeData()->GetBranchID();
			}
		}
		else if (const UStorySceneGraphNode_Transition* target_transition_node = Cast<UStorySceneGraphNode_Transition>(_target_node))
		{
			branch_link.NextSceneID = target_transition_node->GetNextSceneID();
		}

		return branch_link;
	}

	static void CollectReachableNodes(UEdGraphNode* _node, TSet<UEdGraphNode*>& _out_nodes)
	{
		if (IsInvalid(_node) || _out_nodes.Contains(_node))
		{
			return;
		}

		_out_nodes.Add(_node);

		for (UEdGraphPin* pin : _node->Pins)
		{
			if (pin == nullptr || pin->Direction != EGPD_Output)
			{
				continue;
			}

			for (UEdGraphPin* linked_pin : pin->LinkedTo)
			{
				if (linked_pin == nullptr)
				{
					continue;
				}

				CollectReachableNodes(linked_pin->GetOwningNode(), _out_nodes);
			}
		}
	}
}

void UStorySceneEdGraph::PostLoad()
{
	Super::PostLoad();
	EnsureEntryNode();
}

UStorySceneAsset* UStorySceneEdGraph::GetOwningSceneAsset() const
{
	return Cast<UStorySceneAsset>(GetOuter());
}

UStorySceneGraphNode_Entry* UStorySceneEdGraph::FindEntryNode() const
{
	for (UEdGraphNode* node : Nodes)
	{
		UStorySceneGraphNode_Entry* entry_node = Cast<UStorySceneGraphNode_Entry>(node);
		if (IsValid(entry_node))
		{
			return entry_node;
		}
	}

	return nullptr;
}

void UStorySceneEdGraph::EnsureEntryNode()
{
	if (FindEntryNode())
	{
		return;
	}

	UStorySceneGraphNode_Entry* entry_node = NewObject<UStorySceneGraphNode_Entry>(this, UStorySceneGraphNode_Entry::StaticClass(), NAME_None, RF_Transactional);
	entry_node->CreateNewGuid();
	entry_node->NodePosX = -400;
	entry_node->NodePosY = 0;
	AddNode(entry_node, true, false);
	entry_node->AllocateDefaultPins();
}

void UStorySceneEdGraph::GetReachableNodes(TSet<UEdGraphNode*>& _out_nodes) const
{
	_out_nodes.Reset();

	UStorySceneGraphNode_Entry* entry_node = FindEntryNode();
	if (IsInvalid(entry_node))
	{
		return;
	}

	CollectReachableNodes(entry_node, _out_nodes);
}

void UStorySceneEdGraph::RebuildRuntimeData()
{
	UStorySceneAsset* scene_asset = GetOwningSceneAsset();
	if (IsInvalid(scene_asset))
	{
		return;
	}

	FStorySceneBranchLink rebuilt_entry_link;
	TMap<UStorySceneNodeData*, FStorySceneBranchLink> rebuilt_next_links;
	TMap<UStoryBranchNodeData*, TMap<int32, FStorySceneBranchLink>> rebuilt_branch_next_links_by_pin_index;
	TSet<UEdGraphNode*> reachable_nodes;
	GetReachableNodes(reachable_nodes);

	for (UEdGraphNode* node : Nodes)
	{
		UStorySceneGraphNode_Shot* shot_node = Cast<UStorySceneGraphNode_Shot>(node);
		if (IsInvalid(shot_node) || IsInvalid(shot_node->GetShotNodeData()))
		{
			continue;
		}

		rebuilt_next_links.FindOrAdd(shot_node->GetShotNodeData());
	}

	if (UStorySceneGraphNode_Entry* entry_node = FindEntryNode())
	{
		if (reachable_nodes.Contains(entry_node) == false)
		{
			scene_asset->SetEntryLink(rebuilt_entry_link);
		}
		else if (UEdGraphPin* next_pin = entry_node->FindPin(TEXT("Next")))
		{
			if (next_pin->LinkedTo.Num() > 0)
			{
				UEdGraphNode* target_node = next_pin->LinkedTo[0]->GetOwningNode();
				if (IsValid(target_node) && reachable_nodes.Contains(target_node))
				{
					rebuilt_entry_link = MakeBranchLinkFromTargetNode(target_node);
				}
			}
		}
	}

	for (UEdGraphNode* node : Nodes)
	{
		UStorySceneGraphNode_Shot* shot_node = Cast<UStorySceneGraphNode_Shot>(node);
		if (IsInvalid(shot_node) || IsInvalid(shot_node->GetShotNodeData()))
		{
			continue;
		}

		FStorySceneBranchLink& next_link = rebuilt_next_links.FindOrAdd(shot_node->GetShotNodeData());
		next_link = FStorySceneBranchLink();
		if (reachable_nodes.Contains(shot_node) == false)
		{
			continue;
		}

		UEdGraphPin* next_pin = shot_node->FindPin(TEXT("Next"));
		if (next_pin == nullptr || next_pin->LinkedTo.Num() == 0)
		{
			continue;
		}

		const FStorySceneBranchLink branch_link = MakeBranchLinkFromTargetNode(next_pin->LinkedTo[0]->GetOwningNode());

		if (branch_link.IsValid())
		{
			next_link = branch_link;
		}
	}

	for (UEdGraphNode* node : Nodes)
	{
		UStorySceneGraphNode_Branch* branch_node = Cast<UStorySceneGraphNode_Branch>(node);
		if (IsInvalid(branch_node) || IsInvalid(branch_node->GetBranchNodeData()))
		{
			continue;
		}

		TMap<int32, FStorySceneBranchLink>& next_links_by_pin_index = rebuilt_branch_next_links_by_pin_index.FindOrAdd(branch_node->GetBranchNodeData());
		next_links_by_pin_index.Reset();
		if (reachable_nodes.Contains(branch_node) == false)
		{
			continue;
		}

		const TArray<UEdGraphPin*> next_pins = branch_node->GetNextPins();
		for (int32 next_pin_index = 0; next_pin_index < next_pins.Num(); ++next_pin_index)
		{
			UEdGraphPin* next_pin = next_pins[next_pin_index];
			if (next_pin == nullptr || next_pin->LinkedTo.Num() == 0)
			{
				continue;
			}

			const FStorySceneBranchLink branch_link = MakeBranchLinkFromTargetNode(next_pin->LinkedTo[0]->GetOwningNode());

			if (branch_link.IsValid())
			{
				next_links_by_pin_index.Add(next_pin_index, branch_link);
			}
		}
	}

	scene_asset->SetEntryLink(rebuilt_entry_link);

	for (const TPair<UStorySceneNodeData*, FStorySceneBranchLink>& rebuilt_pair : rebuilt_next_links)
	{
		if (IsValid(rebuilt_pair.Key))
		{
			rebuilt_pair.Key->SetNextLink(rebuilt_pair.Value);
		}
	}

	for (const TPair<UStoryBranchNodeData*, TMap<int32, FStorySceneBranchLink>>& rebuilt_pair : rebuilt_branch_next_links_by_pin_index)
	{
		if (IsValid(rebuilt_pair.Key))
		{
			rebuilt_pair.Key->SetNextLinksByPinIndex(rebuilt_pair.Value);
		}
	}
}
