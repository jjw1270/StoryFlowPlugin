// Copyright (c) 2026 장윤제. All rights reserved.

#include "Graph/StorySceneGraphSchema.h"
#include "Graph/StorySceneSchemaAction_NewNode.h"
#include "Graph/StorySceneGraphNode_Entry.h"
#include "Graph/StorySceneGraphNode_Branch.h"
#include "Graph/StorySceneGraphNode_Shot.h"
#include "Graph/StorySceneGraphNode_Transition.h"

namespace
{
	static const FName StorySceneSchemaPinCategory(TEXT("StoryFlow"));
}

void UStorySceneGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& _context_menu_builder) const
{
	TSharedPtr<FStorySceneSchemaAction_NewNode> action = MakeShared<FStorySceneSchemaAction_NewNode>(
		FText::FromString(TEXT("StoryFlow")),
		FText::FromString(TEXT("Add Shot")),
		FText::FromString(TEXT("Create a new shot node.")),
		0);

	action->_NodeClass = UStorySceneGraphNode_Shot::StaticClass();
	_context_menu_builder.AddAction(action);

	TSharedPtr<FStorySceneSchemaAction_NewNode> branch_action = MakeShared<FStorySceneSchemaAction_NewNode>(
		FText::FromString(TEXT("StoryFlow")),
		FText::FromString(TEXT("Add Branch")),
		FText::FromString(TEXT("Create a branch node.")),
		0);

	branch_action->_NodeClass = UStorySceneGraphNode_Branch::StaticClass();
	_context_menu_builder.AddAction(branch_action);

	TSharedPtr<FStorySceneSchemaAction_NewNode> transition_action = MakeShared<FStorySceneSchemaAction_NewNode>(
		FText::FromString(TEXT("StoryFlow")),
		FText::FromString(TEXT("Add Transition")),
		FText::FromString(TEXT("Create a transition node.")),
		0);

	transition_action->_NodeClass = UStorySceneGraphNode_Transition::StaticClass();
	_context_menu_builder.AddAction(transition_action);
}

TSharedPtr<FEdGraphSchemaAction> UStorySceneGraphSchema::GetCreateCommentAction() const
{
	return MakeShared<FStorySceneSchemaAction_AddComment>();
}

const FPinConnectionResponse UStorySceneGraphSchema::CanCreateConnection(const UEdGraphPin* _a, const UEdGraphPin* _b) const
{
	if (_a == nullptr || _b == nullptr)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid pins."));
	}

	if (_a->Direction == _b->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Same direction."));
	}

	if (_a->PinType.PinCategory != StorySceneSchemaPinCategory || _b->PinType.PinCategory != StorySceneSchemaPinCategory)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Only StoryFlow pins are supported."));
	}

	const UEdGraphPin* output_pin = (_a->Direction == EGPD_Output) ? _a : _b;
	const UEdGraphPin* input_pin = (_a->Direction == EGPD_Input) ? _a : _b;
	const UEdGraphNode* output_node = output_pin->GetOwningNode();
	const UEdGraphNode* input_node = input_pin->GetOwningNode();

	if (Cast<UStorySceneGraphNode_Transition>(output_node))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Transition cannot connect to other nodes."));
	}

	if (output_pin->LinkedTo.Num() > 0)
	{
		const ECanCreateConnectionResponse response = (output_pin == _a) ? CONNECT_RESPONSE_BREAK_OTHERS_A : CONNECT_RESPONSE_BREAK_OTHERS_B;
		return FPinConnectionResponse(response, TEXT("Output supports only one connection."));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

bool UStorySceneGraphSchema::TryCreateConnection(UEdGraphPin* _a, UEdGraphPin* _b) const
{
	const bool is_success = Super::TryCreateConnection(_a, _b);
	if (is_success)
	{
		if (UEdGraphNode* node = _a ? _a->GetOwningNode() : nullptr)
		{
			if (UEdGraph* graph = node->GetGraph())
			{
				graph->NotifyGraphChanged();
			}
		}
	}

	return is_success;
}

void UStorySceneGraphSchema::BreakPinLinks(UEdGraphPin& _target_pin, bool _is_sends_node_notifcation) const
{
	Super::BreakPinLinks(_target_pin, _is_sends_node_notifcation);

	if (UEdGraphNode* node = _target_pin.GetOwningNode())
	{
		if (UEdGraph* graph = node->GetGraph())
		{
			graph->NotifyGraphChanged();
		}
	}
}

void UStorySceneGraphSchema::BreakSinglePinLink(UEdGraphPin* _source_pin, UEdGraphPin* _target_pin) const
{
	Super::BreakSinglePinLink(_source_pin, _target_pin);

	if (UEdGraphNode* node = _source_pin ? _source_pin->GetOwningNode() : (_target_pin ? _target_pin->GetOwningNode() : nullptr))
	{
		if (UEdGraph* graph = node->GetGraph())
		{
			graph->NotifyGraphChanged();
		}
	}
}
