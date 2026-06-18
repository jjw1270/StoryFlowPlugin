// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "Graph/SStorySceneGraphNode_TemplateBase.h"

class UStorySceneGraphNode_Entry;

class SStorySceneGraphNode_Entry : public SStorySceneGraphNode_TemplateBase
{
public:
	SLATE_BEGIN_ARGS(SStorySceneGraphNode_Entry)
		: _GraphNodeObj(nullptr)
	{
	}

	SLATE_ARGUMENT(UStorySceneGraphNode_Entry*, GraphNodeObj)
	SLATE_END_ARGS()

	void Construct(const FArguments& _args);

protected:
	virtual UObject* GetTemplateObject() const override;

private:
	UStorySceneGraphNode_Entry* GetStorySceneGraphNode() const;
};
