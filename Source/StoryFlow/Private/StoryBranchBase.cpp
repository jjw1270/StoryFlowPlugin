// Copyright (c) 2026 장윤제. All rights reserved.

#include "StoryBranchBase.h"

UWorld* UStoryBranchBase::GetWorld() const
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}

	const UObject* outer_object = GetOuter();
	return outer_object ? outer_object->GetWorld() : nullptr;
}

void UStoryBranchBase::InitializeBranch(const FStoryFlowRef& _story_flow_ref)
{
	_StartRef = _story_flow_ref;
}
