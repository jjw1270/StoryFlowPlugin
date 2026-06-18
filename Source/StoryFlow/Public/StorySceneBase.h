// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StoryFlowDefines.h"
#include "StorySceneBase.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class STORYFLOW_API UStorySceneBase : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "StoryFlow")
	FStoryFlowRef _StartRef;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "StoryFlow")
	bool _IsRunning = false;

public:
	virtual UWorld* GetWorld() const override final;

public:
	void InitializeScene(const FStoryFlowRef& _story_flow_ref);

	void EnterScene();
	void ExitScene();

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnEnterScene();
	virtual void OnEnterScene_Implementation() {}

	UFUNCTION(BlueprintNativeEvent)
	void OnExitScene();
	virtual void OnExitScene_Implementation() {}

public:
	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	const FStoryFlowRef& GetStartRef() const { return _StartRef; }

	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	bool IsRunning() const { return _IsRunning; }
};
