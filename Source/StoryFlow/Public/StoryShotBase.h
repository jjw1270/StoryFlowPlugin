// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StoryFlowDefines.h"
#include "StoryShotBase.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class STORYFLOW_API UStoryShotBase : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "StoryFlow")
	FStoryShotID _ShotID;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "StoryFlow")
	bool _IsRunning = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "StoryFlow")
	bool _IsFinished = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "StoryFlow")
	float _ElapsedTime = 0.0f;

public:
	virtual UWorld* GetWorld() const override final;

public:
	void InitializeShot(const FStoryShotID& _shot_id);

	void EnterShot();
	void TickShot(float _delta_time);
	void ExitShot();

protected:
	UFUNCTION(BlueprintCallable, Category = "StoryFlow")
	void FinishShot();

	UFUNCTION(BlueprintNativeEvent)
	void OnEnterShot();
	virtual void OnEnterShot_Implementation() {};

	UFUNCTION(BlueprintNativeEvent)
	void OnTickShot(float _delta_time);
	virtual void OnTickShot_Implementation(float _delta_time) {};

	UFUNCTION(BlueprintNativeEvent)
	void OnExitShot();
	virtual void OnExitShot_Implementation() {};

public:
	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	const FStoryShotID& GetShotID() const { return _ShotID; }

	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	bool IsRunning() const { return _IsRunning; }

	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	bool IsFinished() const { return _IsFinished; }

	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	float GetElapsedTime() const { return _ElapsedTime; }
};
