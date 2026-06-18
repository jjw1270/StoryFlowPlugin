// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "Graph/SStorySceneGraphNode_TemplateBase.h"

class UStorySceneGraphNode_Branch;

class SStorySceneGraphNode_Branch : public SStorySceneGraphNode_TemplateBase
{
public:
	SLATE_BEGIN_ARGS(SStorySceneGraphNode_Branch)
		: _GraphNodeObj(nullptr)
	{
	}

	SLATE_ARGUMENT(UStorySceneGraphNode_Branch*, GraphNodeObj)
	SLATE_END_ARGS()

	void Construct(const FArguments& _args);

protected:
	virtual UObject* GetTemplateObject() const override;
	virtual void OnTemplatePropertyValueChanged() override;

private:
	UStorySceneGraphNode_Branch* GetStorySceneGraphNode() const;
};
