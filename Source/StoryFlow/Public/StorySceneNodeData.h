// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StoryFlowDefines.h"
#include "StorySceneNodeData.generated.h"

class UStoryShotBase;

USTRUCT(BlueprintType)
struct STORYFLOW_API FStorySceneBranchLink
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StoryFlow")
	FStoryShotID NextShotID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StoryFlow")
	FStoryBranchID NextBranchID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StoryFlow")
	FStorySceneID NextSceneID;

	bool IsValid() const
	{
		return NextShotID.IsValid() || NextBranchID.IsValid() || NextSceneID.IsValid();
	}

	bool IsShotLink() const
	{
		return NextShotID.IsValid();
	}

	bool IsBranchLink() const
	{
		return NextBranchID.IsValid();
	}

	bool IsSceneLink() const
	{
		return NextSceneID.IsValid();
	}

	bool operator==(const FStorySceneBranchLink& _other) const
	{
		return NextShotID == _other.NextShotID
			&& NextBranchID == _other.NextBranchID
			&& NextSceneID == _other.NextSceneID;
	}
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class STORYFLOW_API UStorySceneNodeData : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StoryFlow")
	FStoryShotID _ShotID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StoryFlow")
	FText _DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StoryFlow", meta = (MultiLine = true))
	FText _Description;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "StoryFlow")
	TObjectPtr<UStoryShotBase> _ShotTemplate = nullptr;

	UPROPERTY()
	FStorySceneBranchLink _NextLink;

#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& _property_changed_event) override;

	void SetShotID(const FStoryShotID& _shot_id);
	void SetNextLink(const FStorySceneBranchLink& _next_link);
#endif

public:
	UFUNCTION(BlueprintPure)
	const FStoryShotID& GetShotID() const { return _ShotID; }

	UFUNCTION(BlueprintPure)
	FText GetDisplayNameText() const;

	UFUNCTION(BlueprintPure)
	const FText& GetDisplayName() const { return _DisplayName; }

	UFUNCTION(BlueprintPure)
	const FText& GetDescriptionText() const { return _Description; }

	UFUNCTION(BlueprintPure)
	UStoryShotBase* GetShotTemplate() const { return _ShotTemplate; }

	UFUNCTION(BlueprintPure)
	const FStorySceneBranchLink& GetNextLink() const { return _NextLink; }
};
