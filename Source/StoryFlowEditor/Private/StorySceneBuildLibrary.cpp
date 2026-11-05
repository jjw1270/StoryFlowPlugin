// Copyright (c) 2026 장윤제. All rights reserved.

#include "StorySceneBuildLibrary.h"
#include "StorySceneAsset.h"
#include "StorySceneNodeData.h"
#include "StoryBranchNodeData.h"
#include "StoryShotBase.h"
#include "Graph/StorySceneEdGraph.h"
#include "Graph/StorySceneGraphSchema.h"
#include "Graph/StorySceneGraphNode_Entry.h"
#include "Graph/StorySceneGraphNode_Shot.h"
#include "Graph/StorySceneGraphNode_Transition.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "CommonUtils.h"

namespace
{
	static const int32 BuildGraphColumnCount = 6;
	static const int32 BuildGraphStepX = 360;
	static const int32 BuildGraphStepY = 260;

	static void PlaceGraphNode(UEdGraphNode* _node, const UEdGraphNode* _entry_node, int32 _index)
	{
		_node->NodePosX = _entry_node->NodePosX + BuildGraphStepX * (1 + _index % BuildGraphColumnCount);
		_node->NodePosY = _entry_node->NodePosY + BuildGraphStepY * (_index / BuildGraphColumnCount);
	}
}

UStorySceneAsset* UStorySceneBuildLibrary::CreateSceneAsset(const FString& _package_path, const FString& _asset_name, FName _scene_id)
{
	const FString package_name = _package_path / _asset_name;
	if (FPackageName::IsValidLongPackageName(package_name) == false)
	{
		TRACE_ERROR(TEXT("Invalid package name: %s"), *package_name);
		return nullptr;
	}

	UPackage* package = FindPackage(nullptr, *package_name);
	if (package == nullptr && FPackageName::DoesPackageExist(package_name))
	{
		package = LoadPackage(nullptr, *package_name, LOAD_None);
	}

	if (package != nullptr)
	{
		if (UStorySceneAsset* existing_asset = FindObject<UStorySceneAsset>(package, *_asset_name))
		{
			return existing_asset;
		}

		TRACE_ERROR(TEXT("Package exists but has no StorySceneAsset named %s"), *_asset_name);
		return nullptr;
	}

	package = CreatePackage(*package_name);
	UStorySceneAsset* scene_asset = NewObject<UStorySceneAsset>(package, UStorySceneAsset::StaticClass(), FName(*_asset_name), RF_Public | RF_Standalone | RF_Transactional);
	if (IsInvalid(scene_asset))
	{
		return nullptr;
	}

	scene_asset->SetSceneID(FStorySceneID(_scene_id.IsNone() ? FName(*_asset_name) : _scene_id));
	FAssetRegistryModule::AssetCreated(scene_asset);
	scene_asset->MarkPackageDirty();
	return scene_asset;
}

TArray<UStoryShotBase*> UStorySceneBuildLibrary::RebuildLinearSceneGraph(UStorySceneAsset* _scene_asset, const TArray<FStorySceneBuildShot>& _shots, FName _next_scene_id)
{
	TArray<UStoryShotBase*> shot_templates;
	if (IsInvalid(_scene_asset))
	{
		TRACE_ERROR(TEXT("Scene asset is invalid."));
		return shot_templates;
	}

	TSet<FName> used_shot_ids;
	for (const FStorySceneBuildShot& build_shot : _shots)
	{
		if (build_shot.ShotID.IsNone() || used_shot_ids.Contains(build_shot.ShotID))
		{
			TRACE_ERROR(TEXT("ShotID is empty or duplicated: %s"), *build_shot.ShotID.ToString());
			return shot_templates;
		}

		UClass* shot_class = build_shot.ShotClass.Get();
		if (IsInvalid(shot_class) || shot_class->HasAnyClassFlags(CLASS_Abstract))
		{
			TRACE_ERROR(TEXT("ShotClass is invalid or abstract: %s"), *build_shot.ShotID.ToString());
			return shot_templates;
		}

		used_shot_ids.Add(build_shot.ShotID);
	}

	_scene_asset->Modify();

	UStorySceneEdGraph* graph = Cast<UStorySceneEdGraph>(_scene_asset->GetEditorGraph());
	if (IsInvalid(graph))
	{
		graph = NewObject<UStorySceneEdGraph>(_scene_asset, UStorySceneEdGraph::StaticClass(), NAME_None, RF_Transactional);
		graph->Schema = UStorySceneGraphSchema::StaticClass();
		_scene_asset->SetEditorGraph(graph);
	}

	graph->Modify();
	graph->EnsureEntryNode();

	// Entry를 뺀 기존 노드를 지운다. Shot/Branch 노드는 DestroyNode에서 자기 런타임 데이터도 에셋에서 뺀다.
	TArray<UEdGraphNode*> old_nodes;
	for (UEdGraphNode* node : graph->Nodes)
	{
		if (IsValid(node) && Cast<UStorySceneGraphNode_Entry>(node) == nullptr)
		{
			old_nodes.Add(node);
		}
	}

	for (UEdGraphNode* node : old_nodes)
	{
		node->Modify();
		node->BreakAllNodeLinks();
		node->DestroyNode();
	}

	// 그래프 노드 없이 남은 런타임 데이터도 정리한다.
	const TArray<TObjectPtr<UStorySceneNodeData>> stale_shot_nodes = _scene_asset->GetShotNodes();
	for (UStorySceneNodeData* stale_shot_node : stale_shot_nodes)
	{
		_scene_asset->RemoveShotNode(stale_shot_node);
	}

	const TArray<TObjectPtr<UStoryBranchNodeData>> stale_branch_nodes = _scene_asset->GetBranchNodes();
	for (UStoryBranchNodeData* stale_branch_node : stale_branch_nodes)
	{
		_scene_asset->RemoveBranchNode(stale_branch_node);
	}

	UStorySceneGraphNode_Entry* entry_node = graph->FindEntryNode();
	if (IsInvalid(entry_node))
	{
		TRACE_ERROR(TEXT("Entry node is missing."));
		return shot_templates;
	}

	entry_node->BreakAllNodeLinks();
	UEdGraphPin* previous_next_pin = entry_node->FindPin(TEXT("Next"));

	for (int32 shot_index = 0; shot_index < _shots.Num(); ++shot_index)
	{
		const FStorySceneBuildShot& build_shot = _shots[shot_index];

		UStorySceneGraphNode_Shot* shot_graph_node = NewObject<UStorySceneGraphNode_Shot>(graph, UStorySceneGraphNode_Shot::StaticClass(), NAME_None, RF_Transactional);
		shot_graph_node->CreateNewGuid();
		PlaceGraphNode(shot_graph_node, entry_node, shot_index);
		graph->AddNode(shot_graph_node, false, false);
		shot_graph_node->PostPlacedNewNode();
		shot_graph_node->AllocateDefaultPins();

		UStorySceneNodeData* shot_node_data = shot_graph_node->GetShotNodeData();
		if (IsInvalid(shot_node_data))
		{
			TRACE_ERROR(TEXT("Failed to create shot node data: %s"), *build_shot.ShotID.ToString());
			shot_templates.Reset();
			return shot_templates;
		}

		shot_node_data->SetShotID(FStoryShotID(build_shot.ShotID));
		UStoryShotBase* shot_template = NewObject<UStoryShotBase>(shot_node_data, build_shot.ShotClass.Get(), NAME_None, RF_Transactional);
		shot_node_data->SetShotTemplate(shot_template);
		shot_node_data->SetEditorInfo(build_shot.DisplayName, build_shot.Description, build_shot.IsCheckpoint);

		if (previous_next_pin != nullptr)
		{
			previous_next_pin->MakeLinkTo(shot_graph_node->FindPin(TEXT("In")));
		}

		previous_next_pin = shot_graph_node->FindPin(TEXT("Next"));
		shot_templates.Add(shot_template);
	}

	if (_next_scene_id.IsNone() == false)
	{
		UStorySceneGraphNode_Transition* transition_node = NewObject<UStorySceneGraphNode_Transition>(graph, UStorySceneGraphNode_Transition::StaticClass(), NAME_None, RF_Transactional);
		transition_node->CreateNewGuid();
		PlaceGraphNode(transition_node, entry_node, _shots.Num());
		graph->AddNode(transition_node, false, false);
		transition_node->SetNextSceneID(FStorySceneID(_next_scene_id));
		transition_node->PostPlacedNewNode();
		transition_node->AllocateDefaultPins();

		if (previous_next_pin != nullptr)
		{
			previous_next_pin->MakeLinkTo(transition_node->FindPin(TEXT("In")));
		}
	}

	graph->RebuildRuntimeData();
	graph->NotifyGraphChanged();
	_scene_asset->MarkPackageDirty();
	return shot_templates;
}
