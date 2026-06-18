// Copyright (c) 2026 장윤제. All rights reserved.

#include "Graph/StorySceneSchemaAction_NewNode.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphNode_Comment.h"
#include "GraphEditor.h"

UEdGraphNode* FStorySceneSchemaAction_NewNode::PerformAction(UEdGraph* _parent_graph, UEdGraphPin* _from_pin, const FVector2f& _location, bool _is_select_new_node)
{
	if (_parent_graph == nullptr || _NodeClass == nullptr)
	{
		return nullptr;
	}

	UEdGraphNode* new_node = NewObject<UEdGraphNode>(_parent_graph, _NodeClass, NAME_None, RF_Transactional);
	_parent_graph->Modify();
	new_node->CreateNewGuid();
	new_node->NodePosX = static_cast<int32>(_location.X);
	new_node->NodePosY = static_cast<int32>(_location.Y);
	_parent_graph->AddNode(new_node, true, _is_select_new_node);
	new_node->PostPlacedNewNode();
	new_node->AllocateDefaultPins();

	if (_from_pin)
	{
		new_node->AutowireNewNode(_from_pin);
	}

	return new_node;
}

UEdGraphNode* FStorySceneSchemaAction_AddComment::PerformAction(UEdGraph* _parent_graph, UEdGraphPin* _from_pin, const FVector2f& _location, bool _is_select_new_node)
{
	if (_parent_graph == nullptr)
	{
		return nullptr;
	}

	UEdGraphNode_Comment* comment_template = NewObject<UEdGraphNode_Comment>();
	FVector2f spawn_location = _location;

	if (TSharedPtr<SGraphEditor> graph_editor = SGraphEditor::FindGraphEditorForGraph(_parent_graph))
	{
		FSlateRect selected_bounds;
		if (graph_editor->GetBoundsForSelectedNodes(selected_bounds, 50.0f))
		{
			comment_template->SetBounds(selected_bounds);
			spawn_location.X = comment_template->NodePosX;
			spawn_location.Y = comment_template->NodePosY;
		}
		else
		{
			spawn_location = graph_editor->GetPasteLocation2f();
		}
	}

	return FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(_parent_graph, comment_template, spawn_location, _is_select_new_node);
}
