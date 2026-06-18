// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StoryFlowDefines.h"
#include "StorySceneNodeData.h"
#include "StoryBranchNodeData.generated.h"

class UStoryBranchBase;

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class STORYFLOW_API UStoryBranchNodeData : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FStoryBranchID _BranchID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StoryFlow")
	FText _DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StoryFlow", meta = (MultiLine = true))
	FText _Description;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "StoryFlow")
	TObjectPtr<UStoryBranchBase> _BranchTemplate = nullptr;

	UPROPERTY()
	TMap<int32, FStorySceneBranchLink> _NextLinksByPinIndex;

#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& _property_changed_event) override;

	void SetBranchID(const FStoryBranchID& _branch_id);
	void SetNextLinksByPinIndex(const TMap<int32, FStorySceneBranchLink>& _next_links_by_pin_index);
#endif

public:
	UFUNCTION(BlueprintPure)
	const FStoryBranchID& GetBranchID() const { return _BranchID; }

	UFUNCTION(BlueprintPure)
	FText GetDisplayNameText() const;

	UFUNCTION(BlueprintPure)
	const FText& GetDescriptionText() const { return _Description; }

	UFUNCTION(BlueprintPure)
	UStoryBranchBase* GetBranchTemplate() const { return _BranchTemplate; }

	UFUNCTION(BlueprintPure)
	const TMap<int32, FStorySceneBranchLink>& GetNextLinksByPinIndex() const { return _NextLinksByPinIndex; }
};
