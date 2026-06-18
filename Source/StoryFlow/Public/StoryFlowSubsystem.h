// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "StoryFlowDefines.h"
#include "StoryFlowSubsystem.generated.h"

class UStorySceneAsset;
class UStorySceneBase;
class UStoryBranchNodeData;
class UStorySceneNodeData;
class UStoryShotBase;
class UCanvas;
class UWorld;
class APlayerController;
struct FStorySceneBranchLink;

UENUM()
enum class EStoryFlowPendingTravelPhase : uint8
{
	None,
	LoadingLevel,
	AsyncLoadingTargetLevel,
	OpeningTargetLevel,
};

UCLASS()
class STORYFLOW_API UStoryFlowSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient)
	TObjectPtr<UStorySceneAsset> _CurrentSceneAsset = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UStorySceneNodeData> _CurrentShotNode = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UStoryBranchNodeData> _CurrentBranchNode = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UStorySceneBase> _CurrentSceneInstance = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UStoryShotBase> _CurrentShotInstance = nullptr;

	UPROPERTY(Transient)
	FStoryFlowRef _PendingStartRef;

	UPROPERTY(Transient)
	TSoftObjectPtr<UWorld> _PendingTargetLevel = nullptr;

	UPROPERTY(Transient)
	EStoryFlowPendingTravelPhase _PendingTravelPhase = EStoryFlowPendingTravelPhase::None;

	TSharedPtr<FStreamableHandle> _PendingTargetLevelLoadHandle;
	FDelegateHandle _DebugDrawDelegateHandle;
	UStoryShotBase* _FinishingShotInstance = nullptr;
	bool _PendingTargetLevelLoadCompleted = false;
	double _PendingLoadingLevelEnterTime = 0.0;
	bool _IsDebugOverlayEnabled = false;

public:
	virtual void Initialize(FSubsystemCollectionBase& _collection) override;
	virtual void Deinitialize() override;

	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual void Tick(float _delta_time) override;

public:
	UFUNCTION(BlueprintCallable)
	bool StartFromScene(const FStorySceneID& _scene_id);

	UFUNCTION(BlueprintCallable)
	bool StartFromRef(const FStoryFlowRef& _story_flow_ref);

	UFUNCTION(BlueprintCallable)
	void StopScene();

	void FinishCurrentShot(UStoryShotBase* _shot_instance);

#if WITH_EDITOR
	static void SetEditorPlayFromShotSession(bool _is_active);
	static bool IsEditorPlayFromShotSession();
#endif

protected:
	const class UStoryFlowDeveloperSettings* GetStoryFlowDeveloperSettings() const;
	UStorySceneAsset* FindSceneAssetBySceneID(const FStorySceneID& _scene_id) const;

	FStoryFlowRef MakeResolvedStartRef(UStorySceneAsset* _scene_asset, const FStoryFlowRef& _story_flow_ref) const;
	bool BeginPendingSceneTravel(UStorySceneAsset* _scene_asset, const FStoryFlowRef& _story_flow_ref);
	bool StartResolvedScene(UStorySceneAsset* _scene_asset, const FStoryFlowRef& _story_flow_ref);
	bool ShouldOpenTargetLevel(UStorySceneAsset* _scene_asset) const;
	void RequestOpenLoadingLevel();
	bool BeginAsyncLoadTargetLevel();
	void HandleTargetLevelAsyncLoaded();
	void ClearPendingSceneStart();
	void HandlePostLoadMap(UWorld* _loaded_world);

	FString GetCurrentLevelPackageName() const;
	static FString GetLevelPackageName(const TSoftObjectPtr<UWorld>& _level);
	static FString GetWorldPackageName(const UWorld* _world);
	float GetMinimumLoadingTimeProgress() const;

	bool EnterScene(const FStoryFlowRef& _story_flow_ref);
	bool MoveToLink(const FStorySceneBranchLink& _next_link, const FStoryFlowRef& _story_flow_ref);
	bool EvaluateBranch(const FStoryBranchID& _branch_id, const FStoryFlowRef& _story_flow_ref);
	bool MoveToShot(const FStoryShotID& _shot_id);
	bool MoveToNextShot();

	void ClearCurrentScene();
	void ClearCurrentBranch();
	void ClearCurrentShot();
	void DrawDebugOverlay(UCanvas* _canvas, APlayerController* _player_controller);

public:
	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	UStorySceneAsset* GetCurrentSceneAsset() const { return _CurrentSceneAsset; }

	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	UStoryShotBase* GetCurrentShot() const { return _CurrentShotInstance; }

	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	UStorySceneBase* GetCurrentScene() const { return _CurrentSceneInstance; }

	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	FStoryFlowRef GetCurrentRef() const;

	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	FStoryFlowRef GetPendingStartRef() const { return _PendingStartRef; }

	UFUNCTION(BlueprintPure, Category = "StoryFlow")
	float GetTargetLevelLoadingProgressRate() const;

	FString BuildDebugSummary() const;
	void SetDebugOverlayEnabled(bool _is_enabled);
	bool IsDebugOverlayEnabled() const { return _IsDebugOverlayEnabled; }

};
