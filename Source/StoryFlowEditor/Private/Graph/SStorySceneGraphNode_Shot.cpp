// Copyright (c) 2026 장윤제. All rights reserved.

#include "Graph/SStorySceneGraphNode_Shot.h"
#include "Graph/StorySceneGraphNode_Shot.h"
#include "StoryShotBase.h"
#include "StorySceneNodeData.h"
#include "StoryFlowEditorModule.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"

void SStorySceneGraphNode_Shot::Construct(const FArguments& _args)
{
	ConstructBase(_args._GraphNodeObj);
}

UObject* SStorySceneGraphNode_Shot::GetTemplateObject() const
{
	const UStorySceneGraphNode_Shot* shot_node = GetStorySceneGraphNode();
	const UStorySceneNodeData* shot_node_data = IsValid(shot_node) ? shot_node->GetShotNodeData() : nullptr;
	return IsValid(shot_node_data) ? shot_node_data->GetShotTemplate() : nullptr;
}

void SStorySceneGraphNode_Shot::AddAdditionalNodeWidgets()
{
	GetOrAddSlot(ENodeZone::TopLeft)
		.SlotOffset2f(FVector2f(-32.0f, 4.0f))
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush(TEXT("NoBorder")))
			[
				CreatePlayButtonWidget()
			]
		];
}

TSharedRef<SWidget> SStorySceneGraphNode_Shot::CreatePlayButtonWidget()
{
	return SNew(SBox)
		.Cursor(EMouseCursor::Default)
		.WidthOverride(28.0f)
		.HeightOverride(28.0f)
		[
			SNew(SButton)
			.Cursor(EMouseCursor::Default)
			.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
			.ContentPadding(FMargin(0.0f))
			.Visibility(this, &SStorySceneGraphNode_Shot::GetPlayButtonVisibility)
			.ToolTipText(this, &SStorySceneGraphNode_Shot::GetPlayButtonTooltipText)
			.OnClicked(this, &SStorySceneGraphNode_Shot::OnClickPlayButton)
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush(TEXT("Icons.Play")))
				.ColorAndOpacity(FLinearColor(0.25f, 1.0f, 0.45f))
			]
		];
}

FReply SStorySceneGraphNode_Shot::OnClickPlayButton()
{
	FStoryFlowEditorModule::Get().RequestPlayFromShotNode(GetStorySceneGraphNode());
	return FReply::Handled();
}

FText SStorySceneGraphNode_Shot::GetPlayButtonTooltipText() const
{
	return FText::FromString(TEXT("이 Shot부터 PIE를 시작합니다."));
}

EVisibility SStorySceneGraphNode_Shot::GetPlayButtonVisibility() const
{
	return (IsValid(GEditor) && IsValid(GEditor->PlayWorld))
		? EVisibility::Collapsed
		: EVisibility::Visible;
}

UStorySceneGraphNode_Shot* SStorySceneGraphNode_Shot::GetStorySceneGraphNode() const
{
	return Cast<UStorySceneGraphNode_Shot>(GraphNode);
}
