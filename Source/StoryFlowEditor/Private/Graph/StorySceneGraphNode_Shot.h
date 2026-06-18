// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "StorySceneGraphNode_Shot.generated.h"

class UStorySceneAsset;
class UStorySceneNodeData;

UCLASS()
class UStorySceneGraphNode_Shot : public UEdGraphNode
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UStorySceneNodeData> _ShotNodeData = nullptr;

public:
	virtual void AllocateDefaultPins() override;
	virtual void PostPlacedNewNode() override;
	virtual void PrepareForCopying() override;
	virtual void PostPasteNode() override;
	virtual void DestroyNode() override;
	virtual void AutowireNewNode(UEdGraphPin* _from_pin) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type _title_type) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool CanDuplicateNode() const override { return true; }

	UStorySceneNodeData* GetShotNodeData() const { return _ShotNodeData; }
	UStorySceneAsset* GetOwningSceneAsset() const;
	void PostCopyNode();
	void ClearCompileMessage();
	void SetCompileError(const FString& _error_message);

private:
	void ResetShotNodeDataOwner();
};
