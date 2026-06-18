// Copyright (c) 2026 장윤제. All rights reserved.

#include "Graph/StorySceneGraphNode_Entry.h"
#include "StorySceneAsset.h"
#include "EdGraph/EdGraph.h"

namespace
{
	static const FName StorySceneEntryPinCategory(TEXT("StoryFlow"));
}

void UStorySceneGraphNode_Entry::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, StorySceneEntryPinCategory, NAME_None, TEXT("Next"));
}

FText UStorySceneGraphNode_Entry::GetNodeTitle(ENodeTitleType::Type) const
{
	if (const UStorySceneAsset* scene_asset = GetOwningSceneAsset())
	{
		const FText& display_name = scene_asset->GetDisplayName();
		if (display_name.IsEmpty() == false)
		{
			return FText::Format(FText::FromString(TEXT("Entry\n{0}")), display_name);
		}
	}

	return FText::FromString(TEXT("Entry"));
}

FLinearColor UStorySceneGraphNode_Entry::GetNodeTitleColor() const
{
	return FLinearColor(0.18f, 0.48f, 0.22f);
}

UStorySceneAsset* UStorySceneGraphNode_Entry::GetOwningSceneAsset() const
{
	return GetGraph() ? Cast<UStorySceneAsset>(GetGraph()->GetOuter()) : nullptr;
}

void UStorySceneGraphNode_Entry::ClearCompileMessage()
{
	bHasCompilerMessage = false;
	ErrorType = 0;
	ErrorMsg.Empty();
}

void UStorySceneGraphNode_Entry::SetCompileError(const FString& _error_message)
{
	bHasCompilerMessage = true;
	ErrorType = 1;
	ErrorMsg = _error_message;
}
