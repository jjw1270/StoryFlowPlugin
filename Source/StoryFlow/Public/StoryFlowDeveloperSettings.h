// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "StoryFlowDeveloperSettings.generated.h"

class UStorySceneRegistryAsset;
class UCurveFloat;
class UWorld;

UCLASS(Config = Game, DefaultConfig)
class STORYFLOW_API UStoryFlowDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config)
	TSoftObjectPtr<UStorySceneRegistryAsset> _StorySceneRegistry = nullptr;

	UPROPERTY(EditAnywhere, Config)
	TSoftObjectPtr<UWorld> _LoadingLevel = nullptr;

	UPROPERTY(EditAnywhere, Config, meta = (ClampMin = "0.0"))
	float _MinimumLoadingLevelDuration = 0.0f;

	UPROPERTY(EditAnywhere, Config)
	TSoftObjectPtr<UCurveFloat> _MinimumLoadingLevelProgressCurve = nullptr;
};
