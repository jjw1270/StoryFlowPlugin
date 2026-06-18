// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StoryFlowDefines.h"
#include "StorySceneNodeData.h"
#include "StorySceneAsset.generated.h"

class UStorySceneBase;
class UStoryBranchNodeData;
class UStorySceneNodeData;
class UEdGraph;

UCLASS(BlueprintType)
class STORYFLOW_API UStorySceneAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FStorySceneID _SceneID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText _DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText _Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> _TargetLevel = nullptr;

	UPROPERTY()
	FStorySceneBranchLink _EntryLink;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly)
	TObjectPtr<UStorySceneBase> _SceneTemplate = nullptr;

	UPROPERTY(Instanced, BlueprintReadOnly)
	TArray<TObjectPtr<UStorySceneNodeData>> _ShotNodes;

	UPROPERTY(Instanced, BlueprintReadOnly)
	TArray<TObjectPtr<UStoryBranchNodeData>> _BranchNodes;

public:
	UFUNCTION(BlueprintPure)
	UStorySceneNodeData* FindShotNode(const FStoryShotID& _shot_id) const;

	UFUNCTION(BlueprintPure)
	UStoryBranchNodeData* FindBranchNode(const FStoryBranchID& _branch_id) const;

public:
	UFUNCTION(BlueprintPure)
	const FStorySceneID& GetSceneID() const { return _SceneID; }

	UFUNCTION(BlueprintPure)
	const FText& GetDisplayName() const { return _DisplayName; }

	UFUNCTION(BlueprintPure)
	const FText& GetDescriptionText() const { return _Description; }

	UFUNCTION(BlueprintPure)
	const TSoftObjectPtr<UWorld>& GetTargetLevel() const { return _TargetLevel; }

	const FStorySceneBranchLink& GetEntryLink() const { return _EntryLink; }

	UFUNCTION(BlueprintPure)
	UStorySceneBase* GetSceneTemplate() const { return _SceneTemplate; }

	const TArray<TObjectPtr<UStorySceneNodeData>>& GetShotNodes() const { return _ShotNodes; }
	const TArray<TObjectPtr<UStoryBranchNodeData>>& GetBranchNodes() const { return _BranchNodes; }

#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY()
	TObjectPtr<UEdGraph> _EditorGraph = nullptr;
#endif

#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& _property_changed_event) override;

	void SetEditorGraph(UEdGraph* _editor_graph);
	UEdGraph* GetEditorGraph() const { return _EditorGraph; }

	void SetSceneID(const FStorySceneID& _scene_id);
	void SetEntryLink(const FStorySceneBranchLink& _entry_link);

	UStorySceneNodeData* CreateShotNode();
	UStoryBranchNodeData* CreateBranchNode();
	void AddShotNode(UStorySceneNodeData* _shot_node);
	void AddBranchNode(UStoryBranchNodeData* _branch_node);
	void RemoveShotNode(UStorySceneNodeData* _shot_node);
	void RemoveBranchNode(UStoryBranchNodeData* _branch_node);
#endif
};
