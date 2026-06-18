// Copyright (c) 2026 장윤제. All rights reserved.

#include "StorySceneBase.h"

UWorld* UStorySceneBase::GetWorld() const
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}

	const UObject* outer_object = GetOuter();
	return outer_object ? outer_object->GetWorld() : nullptr;
}

void UStorySceneBase::InitializeScene(const FStoryFlowRef& _story_flow_ref)
{
	_StartRef = _story_flow_ref;
	_IsRunning = false;
}

void UStorySceneBase::EnterScene()
{
	if (_IsRunning)
	{
		return;
	}

	_IsRunning = true;
	OnEnterScene();
}

void UStorySceneBase::ExitScene()
{
	if (_IsRunning == false)
	{
		return;
	}

	OnExitScene();
	_IsRunning = false;
}
