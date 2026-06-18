// Copyright (c) 2026 장윤제. All rights reserved.

#include "StoryFlowBlueprintFactories.h"
#include "StoryFlowEditorModule.h"
#include "StorySceneBase.h"
#include "StoryShotBase.h"
#include "StoryBranchBase.h"

UStorySceneBaseBlueprintFactory::UStorySceneBaseBlueprintFactory(const FObjectInitializer& _object_initializer)
	: Super(_object_initializer)
{
	SupportedClass = UBlueprint::StaticClass();
	ParentClass = UStorySceneBase::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

FText UStorySceneBaseBlueprintFactory::GetDisplayName() const
{
	return FText::FromString(TEXT("StoryFlow Scene"));
}

uint32 UStorySceneBaseBlueprintFactory::GetMenuCategories() const
{
	return FStoryFlowEditorModule::GetAssetCategory();
}

FName UStorySceneBaseBlueprintFactory::GetNewAssetIconOverride() const
{
	return TEXT("ClassIcon.StorySceneBase");
}

FName UStorySceneBaseBlueprintFactory::GetNewAssetThumbnailOverride() const
{
	return TEXT("ClassThumbnail.StorySceneBase");
}

UStoryShotBaseBlueprintFactory::UStoryShotBaseBlueprintFactory(const FObjectInitializer& _object_initializer)
	: Super(_object_initializer)
{
	SupportedClass = UBlueprint::StaticClass();
	ParentClass = UStoryShotBase::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

FText UStoryShotBaseBlueprintFactory::GetDisplayName() const
{
	return FText::FromString(TEXT("StoryFlow Shot"));
}

uint32 UStoryShotBaseBlueprintFactory::GetMenuCategories() const
{
	return FStoryFlowEditorModule::GetAssetCategory();
}

FName UStoryShotBaseBlueprintFactory::GetNewAssetIconOverride() const
{
	return TEXT("ClassIcon.StoryShotBase");
}

FName UStoryShotBaseBlueprintFactory::GetNewAssetThumbnailOverride() const
{
	return TEXT("ClassThumbnail.StoryShotBase");
}

UStoryBranchBaseBlueprintFactory::UStoryBranchBaseBlueprintFactory(const FObjectInitializer& _object_initializer)
	: Super(_object_initializer)
{
	SupportedClass = UBlueprint::StaticClass();
	ParentClass = UStoryBranchBase::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

FText UStoryBranchBaseBlueprintFactory::GetDisplayName() const
{
	return FText::FromString(TEXT("StoryFlow Branch"));
}

uint32 UStoryBranchBaseBlueprintFactory::GetMenuCategories() const
{
	return FStoryFlowEditorModule::GetAssetCategory();
}

FName UStoryBranchBaseBlueprintFactory::GetNewAssetIconOverride() const
{
	return TEXT("ClassIcon.StoryBranchBase");
}

FName UStoryBranchBaseBlueprintFactory::GetNewAssetThumbnailOverride() const
{
	return TEXT("ClassThumbnail.StoryBranchBase");
}
