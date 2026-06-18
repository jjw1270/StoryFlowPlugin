// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "Toolkits/AssetEditorToolkit.h"
#include "EditorUndoClient.h"

class UStorySceneAsset;
class UStoryBranchNodeData;
class UStorySceneEdGraph;
class UStorySceneGraphNode_Branch;
class UStorySceneGraphNode_Shot;
class UStorySceneGraphNode_Transition;
class SWidget;
class FReply;
struct FSlateBrush;

class FStorySceneEditor : public FAssetEditorToolkit, public FEditorUndoClient
{
private:
	enum class ECompileStatus : uint8
	{
		Dirty,
		Good,
		Error,
	};

	TObjectPtr<UStorySceneAsset> _StorySceneAsset = nullptr;

	static const FName _GraphTabID;
	static const FName _DetailsTabID;

	TSharedPtr<FWorkspaceItem> _WorkspaceMenuCategory;
	TSharedPtr<class SGraphEditor> _GraphEditorWidget;
	TSharedPtr<class IDetailsView> _DetailsView;
	TSharedPtr<class FUICommandList> _GraphEditorCommands;
	TSharedPtr<class FUICommandList> _EditorCommands;

	FDelegateHandle _OnGraphChangedHandle;
	TWeakObjectPtr<UStorySceneEdGraph> _ObservedGraph;

	static const FName _CompileMessageLogName;
	static TArray<FStorySceneEditor*> _ActiveEditors;

	ECompileStatus _CompileStatus = ECompileStatus::Dirty;
	TArray<FString> _CompileErrors;
	bool _IsCompiling = false;

public:
	virtual ~FStorySceneEditor() override;
	static void GetActiveEditors(TArray<FStorySceneEditor*>& _out_editors);
	static void ClearCompileMessageLog();

	void InitEditor(const EToolkitMode::Type _mode, const TSharedPtr<IToolkitHost>& _toolkit_host, UStorySceneAsset* _story_scene_asset);
	bool ValidateForPIE(FString& _out_denied_reason);

	virtual FName GetToolkitFName() const override { return FName(TEXT("StorySceneEditor")); }
	virtual FText GetBaseToolkitName() const override { return FText::FromString(TEXT("Story Scene Editor")); }
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("Story Scene"); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor(0.35f, 0.55f, 0.9f, 1.0f); }

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& _tab_manager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& _tab_manager) override;

	virtual void PostUndo(bool _is_success) override;
	virtual void PostRedo(bool _is_success) override { PostUndo(_is_success); }

private:
	TSharedRef<SDockTab> SpawnGraphTab(const FSpawnTabArgs& _args);
	TSharedRef<SDockTab> SpawnDetailsTab(const FSpawnTabArgs& _args);
	void FillToolbar(class FToolBarBuilder& _toolbar_builder);
	TSharedRef<class SWidget> BuildCompileToolbarWidget();
	FReply OnClickCompileButton();
	const FSlateBrush* GetCompileBaseBrush() const;
	const FSlateBrush* GetCompileStatusBrush() const;
	FText GetCompileButtonTooltipText() const;
	void OnGraphSelectionChanged(const TSet<UObject*>& _selection);
	void OnGraphNodeDoubleClicked(UEdGraphNode* _node) const;
	void OnDetailsFinishedChangingProperties(const FPropertyChangedEvent& _property_changed_event);
	void OnGraphChanged(const struct FEdGraphEditAction& _graph_edit_action);
	void BindGraphCommands();
	void BindEditorCommands();
	void CompileScene();
	bool CompileSceneInternal();
	void CopySelectedNodes();
	bool CanCopySelectedNodes() const;
	void CutSelectedNodes();
	bool CanCutSelectedNodes() const;
	void PasteNodes();
	bool CanPasteNodes() const;
	void PasteNodesHere(const FVector2D& _location);
	void CreateCommentNode();
	bool CanCreateCommentNode() const;
	void ReportCompileFailureForPIE() const;
	void RefreshShotIDsForCompile(UStorySceneEdGraph* _graph) const;
	void RefreshGraphNodeComments(UStorySceneEdGraph* _graph) const;
	bool ValidateCompiledScene(UStorySceneEdGraph* _graph, TArray<FString>& _out_errors);
	void ValidateSceneMetadata(UStorySceneEdGraph* _graph, TArray<FString>& _out_errors) const;
	void ValidateBranchNode(UStorySceneGraphNode_Branch* _branch_node, TSet<FName>& _used_branch_ids, TArray<FString>& _out_errors) const;
	void ValidateShotNode(UStorySceneGraphNode_Shot* _shot_node, TSet<FName>& _used_shot_ids, TArray<FString>& _out_errors) const;
	void ValidateTransitionNode(UStorySceneGraphNode_Transition* _transition_node, TArray<FString>& _out_errors) const;
	void ClearNodeCompileMessages(UStorySceneEdGraph* _graph) const;
	void SyncGraphNodePins(UStorySceneEdGraph* _graph) const;
	void MarkCompileDirty();
	void DeleteSelectedNodes();
	bool CanDeleteSelectedNodes() const;
	void UndoGraphAction();
	void RedoGraphAction();
	bool CanUndoGraphAction() const;
	bool CanRedoGraphAction() const;
	UStorySceneEdGraph* GetOrCreateGraph() const;
};
