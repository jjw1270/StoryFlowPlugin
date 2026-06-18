// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StoryFlowDefines.h"
#include "StorySceneRegistryAsset.generated.h"

class UStorySceneAsset;

USTRUCT(BlueprintType)
struct STORYFLOW_API FStorySceneReference
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStorySceneAsset> SceneAsset = nullptr;

public:
	bool IsValid() const
	{
		return SceneAsset.IsNull() == false;
	}

	UStorySceneAsset* ResolveSceneAsset() const;
	FStorySceneID GetSceneID() const;
	FText GetDisplayName() const;
};

UCLASS(BlueprintType)
class STORYFLOW_API UStorySceneRegistryAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StoryFlow")
	TArray<FStorySceneReference> _Scenes;

public:
	const FStorySceneReference* FindSceneReference(const FStorySceneID& _scene_id) const;
	UStorySceneAsset* FindSceneAsset(const FStorySceneID& _scene_id) const;

public:
	UFUNCTION(BlueprintPure)
	const TArray<FStorySceneReference>& GetScenes() const { return _Scenes; }
};
