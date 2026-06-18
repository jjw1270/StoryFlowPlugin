// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "StorySceneGraphSchema.generated.h"

UCLASS()
class UStorySceneGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& _context_menu_builder) const override;
	virtual TSharedPtr<FEdGraphSchemaAction> GetCreateCommentAction() const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* _a, const UEdGraphPin* _b) const override;
	virtual bool TryCreateConnection(UEdGraphPin* _a, UEdGraphPin* _b) const override;
	virtual void BreakPinLinks(UEdGraphPin& _target_pin, bool _is_sends_node_notifcation) const override;
	virtual void BreakSinglePinLink(UEdGraphPin* _source_pin, UEdGraphPin* _target_pin) const override;
};
