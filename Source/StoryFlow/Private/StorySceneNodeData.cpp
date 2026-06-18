// Copyright (c) 2026 장윤제. All rights reserved.

#include "StorySceneNodeData.h"
#include "StoryShotBase.h"
#if WITH_EDITOR
#include "StoryFlowDisplayNameUtils.h"
#endif
#include "CommonUtils.h"

#if WITH_EDITOR
void UStorySceneNodeData::PostEditChangeProperty(FPropertyChangedEvent& _property_changed_event)
{
	Super::PostEditChangeProperty(_property_changed_event);

	if (_property_changed_event.GetPropertyName() != GET_MEMBER_NAME_CHECKED(UStorySceneNodeData, _ShotTemplate))
	{
		return;
	}

	if (_DisplayName.IsEmpty() == false || IsInvalid(_ShotTemplate))
	{
		return;
	}

	Modify();
	_DisplayName = StoryFlowDisplayNameUtils::MakeDisplayNameFromTemplate(_ShotTemplate);
	MarkPackageDirty();
}

void UStorySceneNodeData::SetShotID(const FStoryShotID& _shot_id)
{
	if (_ShotID == _shot_id)
	{
		return;
	}

	Modify();
	_ShotID = _shot_id;
	MarkPackageDirty();
}

void UStorySceneNodeData::SetNextLink(const FStorySceneBranchLink& _next_link)
{
	if (_NextLink == _next_link)
	{
		return;
	}

	Modify();
	_NextLink = _next_link;
	MarkPackageDirty();
}
#endif

FText UStorySceneNodeData::GetDisplayNameText() const
{
	if (_DisplayName.IsEmpty() == false)
	{
		return _DisplayName;
	}

	if (_ShotID.IsValid())
	{
		return FText::FromName(_ShotID.Get());
	}

	return FText::FromString(TEXT("Shot"));
}
