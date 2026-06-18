// Copyright (c) 2026 장윤제. All rights reserved.

#include "StorySceneAsset.h"
#include "StoryBranchNodeData.h"
#include "StorySceneBase.h"
#include "StorySceneNodeData.h"
#if WITH_EDITOR
#include "StoryFlowDisplayNameUtils.h"
#endif
#include "CommonUtils.h"

UStorySceneNodeData* UStorySceneAsset::FindShotNode(const FStoryShotID& _shot_id) const
{
	if (_shot_id.IsValid() == false)
	{
		return nullptr;
	}

	for (const TObjectPtr<UStorySceneNodeData>& shot_node : _ShotNodes)
	{
		if (IsValid(shot_node) && shot_node->GetShotID() == _shot_id)
		{
			return shot_node;
		}
	}

	return nullptr;
}

UStoryBranchNodeData* UStorySceneAsset::FindBranchNode(const FStoryBranchID& _branch_id) const
{
	if (_branch_id.IsValid() == false)
	{
		return nullptr;
	}

	for (const TObjectPtr<UStoryBranchNodeData>& branch_node : _BranchNodes)
	{
		if (IsValid(branch_node) && branch_node->GetBranchID() == _branch_id)
		{
			return branch_node;
		}
	}

	return nullptr;
}

#if WITH_EDITOR
void UStorySceneAsset::PostEditChangeProperty(FPropertyChangedEvent& _property_changed_event)
{
	Super::PostEditChangeProperty(_property_changed_event);

	if (_property_changed_event.GetPropertyName() != GET_MEMBER_NAME_CHECKED(UStorySceneAsset, _SceneTemplate))
	{
		return;
	}

	if (_DisplayName.IsEmpty() == false || IsInvalid(_SceneTemplate))
	{
		return;
	}

	Modify();
	_DisplayName = StoryFlowDisplayNameUtils::MakeDisplayNameFromTemplate(_SceneTemplate);
	MarkPackageDirty();
}

void UStorySceneAsset::SetEditorGraph(UEdGraph* _editor_graph)
{
	if (_EditorGraph == _editor_graph)
	{
		return;
	}

	Modify();
	_EditorGraph = _editor_graph;
	MarkPackageDirty();
}

void UStorySceneAsset::SetSceneID(const FStorySceneID& _scene_id)
{
	if (_SceneID == _scene_id)
	{
		return;
	}

	Modify();
	_SceneID = _scene_id;
	MarkPackageDirty();
}

void UStorySceneAsset::SetEntryLink(const FStorySceneBranchLink& _entry_link)
{
	if (_EntryLink == _entry_link)
	{
		return;
	}

	Modify();
	_EntryLink = _entry_link;
	MarkPackageDirty();
}

UStorySceneNodeData* UStorySceneAsset::CreateShotNode()
{
	Modify();

	UStorySceneNodeData* shot_node = NewObject<UStorySceneNodeData>(this, UStorySceneNodeData::StaticClass(), NAME_None, RF_Transactional);
	_ShotNodes.Add(shot_node);
	MarkPackageDirty();
	return shot_node;
}

UStoryBranchNodeData* UStorySceneAsset::CreateBranchNode()
{
	Modify();

	UStoryBranchNodeData* branch_node = NewObject<UStoryBranchNodeData>(this, UStoryBranchNodeData::StaticClass(), NAME_None, RF_Transactional);
	_BranchNodes.Add(branch_node);
	MarkPackageDirty();
	return branch_node;
}

void UStorySceneAsset::AddShotNode(UStorySceneNodeData* _shot_node)
{
	if (IsInvalid(_shot_node) || _ShotNodes.Contains(_shot_node))
	{
		return;
	}

	Modify();
	_ShotNodes.Add(_shot_node);
	MarkPackageDirty();
}

void UStorySceneAsset::AddBranchNode(UStoryBranchNodeData* _branch_node)
{
	if (IsInvalid(_branch_node) || _BranchNodes.Contains(_branch_node))
	{
		return;
	}

	Modify();
	_BranchNodes.Add(_branch_node);
	MarkPackageDirty();
}

void UStorySceneAsset::RemoveShotNode(UStorySceneNodeData* _shot_node)
{
	if (IsInvalid(_shot_node))
	{
		return;
	}

	Modify();
	_ShotNodes.Remove(_shot_node);
	MarkPackageDirty();
}

void UStorySceneAsset::RemoveBranchNode(UStoryBranchNodeData* _branch_node)
{
	if (IsInvalid(_branch_node))
	{
		return;
	}

	Modify();
	_BranchNodes.Remove(_branch_node);
	MarkPackageDirty();
}
#endif
