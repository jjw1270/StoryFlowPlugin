// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "EdGraph/EdGraphSchema.h"

struct FStorySceneSchemaAction_NewNode : public FEdGraphSchemaAction
{
	UClass* _NodeClass = nullptr;

	FStorySceneSchemaAction_NewNode(const FText& _category, const FText& _menu_desc, const FText& _tooltip, int32 _grouping)
		: FEdGraphSchemaAction(_category, _menu_desc, _tooltip, _grouping)
	{
	}

	virtual UEdGraphNode* PerformAction(UEdGraph* _parent_graph, UEdGraphPin* _from_pin, const FVector2f& _location, bool _is_select_new_node = true) override;
};

struct FStorySceneSchemaAction_AddComment : public FEdGraphSchemaAction
{
	FStorySceneSchemaAction_AddComment()
		: FEdGraphSchemaAction(FText::FromString(TEXT("StoryFlow")), FText::FromString(TEXT("Add Comment")), FText::FromString(TEXT("Create a comment box.")), 0)
	{
	}

	virtual UEdGraphNode* PerformAction(UEdGraph* _parent_graph, UEdGraphPin* _from_pin, const FVector2f& _location, bool _is_select_new_node = true) override;
};
