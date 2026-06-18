// Copyright (c) 2026 장윤제. All rights reserved.

#include "Graph/StorySceneGraphNode_Transition.h"

namespace
{
	static const FName StorySceneTransitionPinCategory(TEXT("StoryFlow"));
}

void UStorySceneGraphNode_Transition::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, StorySceneTransitionPinCategory, NAME_None, TEXT("In"));
}

FText UStorySceneGraphNode_Transition::GetNodeTitle(ENodeTitleType::Type) const
{
	if (_NextSceneID.IsValid())
	{
		return FText::Format(FText::FromString(TEXT("Transition\n{0}")), FText::FromName(_NextSceneID.Get()));
	}

	return FText::FromString(TEXT("Transition"));
}

FLinearColor UStorySceneGraphNode_Transition::GetNodeTitleColor() const
{
	return FLinearColor(0.50f, 0.28f, 0.72f);
}

void UStorySceneGraphNode_Transition::ClearCompileMessage()
{
	bHasCompilerMessage = false;
	ErrorType = 0;
	ErrorMsg.Empty();
}

void UStorySceneGraphNode_Transition::SetCompileError(const FString& _error_message)
{
	bHasCompilerMessage = true;
	ErrorType = 1;
	ErrorMsg = _error_message;
}
