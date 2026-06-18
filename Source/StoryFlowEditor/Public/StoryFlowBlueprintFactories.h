// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "Factories/BlueprintFactory.h"
#include "StoryFlowBlueprintFactories.generated.h"

UCLASS()
class STORYFLOWEDITOR_API UStorySceneBaseBlueprintFactory : public UBlueprintFactory
{
	GENERATED_BODY()

public:
	UStorySceneBaseBlueprintFactory(const FObjectInitializer& _object_initializer);

	virtual bool ConfigureProperties() override { return true; }
	virtual FText GetDisplayName() const override;
	virtual uint32 GetMenuCategories() const override;
	virtual FName GetNewAssetIconOverride() const override;
	virtual FName GetNewAssetThumbnailOverride() const override;
};

UCLASS()
class STORYFLOWEDITOR_API UStoryShotBaseBlueprintFactory : public UBlueprintFactory
{
	GENERATED_BODY()

public:
	UStoryShotBaseBlueprintFactory(const FObjectInitializer& _object_initializer);

	virtual bool ConfigureProperties() override { return true; }
	virtual FText GetDisplayName() const override;
	virtual uint32 GetMenuCategories() const override;
	virtual FName GetNewAssetIconOverride() const override;
	virtual FName GetNewAssetThumbnailOverride() const override;
};

UCLASS()
class STORYFLOWEDITOR_API UStoryBranchBaseBlueprintFactory : public UBlueprintFactory
{
	GENERATED_BODY()

public:
	UStoryBranchBaseBlueprintFactory(const FObjectInitializer& _object_initializer);

	virtual bool ConfigureProperties() override { return true; }
	virtual FText GetDisplayName() const override;
	virtual uint32 GetMenuCategories() const override;
	virtual FName GetNewAssetIconOverride() const override;
	virtual FName GetNewAssetThumbnailOverride() const override;
};
