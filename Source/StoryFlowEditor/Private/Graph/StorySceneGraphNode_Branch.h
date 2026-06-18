// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "StorySceneGraphNode_Branch.generated.h"

class UStoryBranchNodeData;
class UStorySceneAsset;

UCLASS()
class UStorySceneGraphNode_Branch : public UEdGraphNode
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UStoryBranchNodeData> _BranchNodeData = nullptr;

public:
	virtual void AllocateDefaultPins() override;
	virtual void PostLoad() override;
	virtual void PostPlacedNewNode() override;
	virtual void PrepareForCopying() override;
	virtual void PostPasteNode() override;
	virtual void DestroyNode() override;
	virtual void AutowireNewNode(UEdGraphPin* _from_pin) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type _title_type) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool CanDuplicateNode() const override { return true; }

	UStoryBranchNodeData* GetBranchNodeData() const { return _BranchNodeData; }
	UStorySceneAsset* GetOwningSceneAsset() const;
	TArray<UEdGraphPin*> GetNextPins() const;
	UEdGraphPin* FindFirstAvailableNextPin() const;
	void PostCopyNode();
	void SyncNextPinsToNodeData();
	void ClearCompileMessage();
	void SetCompileError(const FString& _error_message);

private:
	void ResetBranchNodeDataOwner();
};
