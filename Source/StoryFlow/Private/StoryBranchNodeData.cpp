// Copyright (c) 2026 장윤제. All rights reserved.

#include "StoryBranchNodeData.h"
#include "StoryBranchBase.h"
#if WITH_EDITOR
#include "StoryFlowDisplayNameUtils.h"
#endif
#include "CommonUtils.h"

#if WITH_EDITOR
void UStoryBranchNodeData::PostEditChangeProperty(FPropertyChangedEvent& _property_changed_event)
{
	Super::PostEditChangeProperty(_property_changed_event);

	if (_property_changed_event.GetPropertyName() != GET_MEMBER_NAME_CHECKED(UStoryBranchNodeData, _BranchTemplate))
	{
		return;
	}

	if (_DisplayName.IsEmpty() == false || IsInvalid(_BranchTemplate))
	{
		return;
	}

	Modify();
	_DisplayName = StoryFlowDisplayNameUtils::MakeDisplayNameFromTemplate(_BranchTemplate);
	MarkPackageDirty();
}

void UStoryBranchNodeData::SetBranchID(const FStoryBranchID& _branch_id)
{
	if (_BranchID == _branch_id)
	{
		return;
	}

	Modify();
	_BranchID = _branch_id;
	MarkPackageDirty();
}

void UStoryBranchNodeData::SetNextLinksByPinIndex(const TMap<int32, FStorySceneBranchLink>& _next_links_by_pin_index)
{
	if (_NextLinksByPinIndex.OrderIndependentCompareEqual(_next_links_by_pin_index))
	{
		return;
	}

	Modify();
	_NextLinksByPinIndex = _next_links_by_pin_index;
	MarkPackageDirty();
}
#endif

FText UStoryBranchNodeData::GetDisplayNameText() const
{
	if (_DisplayName.IsEmpty() == false)
	{
		return _DisplayName;
	}

	if (_BranchID.IsValid())
	{
		return FText::FromName(_BranchID.Get());
	}

	return FText::FromString(TEXT("Branch"));
}
