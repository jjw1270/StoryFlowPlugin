// Copyright (c) 2026 장윤제. All rights reserved.


#include "StoryFlowSubsystem.h"
#include "StorySceneAsset.h"
#include "StoryBranchBase.h"
#include "StoryBranchNodeData.h"
#include "StorySceneBase.h"
#include "StorySceneNodeData.h"
#include "StoryFlowDeveloperSettings.h"
#include "StorySceneRegistryAsset.h"
#include "StoryShotBase.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Curves/CurveFloat.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
#include "Subsystems/SubsystemCollection.h"
#include "UObject/UObjectGlobals.h"
#include "CommonUtils.h"

namespace
{
	const FLinearColor DEBUG_KEY_COLOR = FLinearColor::White;
	const FLinearColor DEBUG_VALUE_COLOR(0.0f, 1.0f, 1.0f, 1.0f);
	const FLinearColor DEBUG_ID_VALUE_COLOR(1.0f, 0.84f, 0.0f, 1.0f);
	const FLinearColor DEBUG_SHADOW_COLOR = FLinearColor::Black;
	const FVector2D DEBUG_SHADOW_OFFSET(1.0f, 1.0f);
	const float DEBUG_X_MARGIN = 24.0f;
	const float DEBUG_Y_MARGIN = 32.0f;
	const float DEBUG_LINE_PADDING = 2.0f;
	const float DEBUG_TEXT_SCALE = 1.0f;

#if WITH_EDITOR
	bool GIsEditorPlayFromShotSession = false;
#endif

	static FString ToDebugString(const FStorySceneID& _scene_id)
	{
		return _scene_id.IsValid() ? _scene_id.Get().ToString() : TEXT("None");
	}

	static FString ToDebugString(const FStoryShotID& _shot_id)
	{
		return _shot_id.IsValid() ? _shot_id.Get().ToString() : TEXT("None");
	}

	static FString ToDebugString(const FStoryBranchID& _branch_id)
	{
		return _branch_id.IsValid() ? _branch_id.Get().ToString() : TEXT("None");
	}

	static FString ToDebugString(EStoryFlowPendingTravelPhase _pending_travel_phase)
	{
		switch (_pending_travel_phase)
		{
		case EStoryFlowPendingTravelPhase::None:
			return TEXT("None");
		case EStoryFlowPendingTravelPhase::LoadingLevel:
			return TEXT("LoadingLevel");
		case EStoryFlowPendingTravelPhase::AsyncLoadingTargetLevel:
			return TEXT("AsyncLoadingTargetLevel");
		case EStoryFlowPendingTravelPhase::OpeningTargetLevel:
			return TEXT("OpeningTargetLevel");
		default:
			return TEXT("Unknown");
		}
	}

	static FString ToDebugString(const FStorySceneBranchLink& _next_link)
	{
		if (_next_link.IsShotLink())
		{
			return FString::Printf(TEXT("Shot -> %s"), *ToDebugString(_next_link.NextShotID));
		}

		if (_next_link.IsBranchLink())
		{
			return FString::Printf(TEXT("Branch -> %s"), *ToDebugString(_next_link.NextBranchID));
		}

		if (_next_link.IsSceneLink())
		{
			return FString::Printf(TEXT("Scene -> %s"), *ToDebugString(_next_link.NextSceneID));
		}

		return TEXT("None");
	}

	static FString ToDebugLevelName(const TSoftObjectPtr<UWorld>& _level)
	{
		if (_level.IsNull())
		{
			return TEXT("None");
		}

		const FString level_package_name = FPackageName::ObjectPathToPackageName(_level.ToSoftObjectPath().ToString());
		return FPackageName::GetShortName(level_package_name);
	}

	static FLinearColor GetDebugValueColor(const FString& _line)
	{
		if (_line.StartsWith(TEXT("SceneID:")) || _line.StartsWith(TEXT("ShotID:")))
		{
			return DEBUG_ID_VALUE_COLOR;
		}

		return DEBUG_VALUE_COLOR;
	}

	static float DrawDebugTextSegment(UCanvas* _canvas, UFont* _font, const FString& _text, const FVector2D& _position, const FLinearColor& _color, float _scale)
	{
		if (_text.IsEmpty())
		{
			return 0.0f;
		}

		_canvas->K2_DrawText(
			_font,
			_text,
			_position,
			FVector2D(_scale, _scale),
			_color,
			0.0f,
			DEBUG_SHADOW_COLOR,
			DEBUG_SHADOW_OFFSET,
			false,
			false,
			false,
			DEBUG_SHADOW_COLOR);

		float text_width = 0.0f;
		float text_height = 0.0f;
		_canvas->TextSize(_font, _text, text_width, text_height, _scale, _scale);
		return text_width;
	}

	static bool DrawDebugKeyValueLine(UCanvas* _canvas, UFont* _font, const FString& _line, const FVector2D& _position, const FLinearColor& _value_color, float _scale)
	{
		const int32 value_separator_index = _line.Find(TEXT(": "), ESearchCase::CaseSensitive);
		if (value_separator_index == INDEX_NONE)
		{
			return false;
		}

		const FString label = _line.Left(value_separator_index + 2);
		const FString value = _line.Mid(value_separator_index + 2);

		float x = _position.X;
		x += DrawDebugTextSegment(_canvas, _font, label, FVector2D(x, _position.Y), DEBUG_KEY_COLOR, _scale);
		DrawDebugTextSegment(_canvas, _font, value, FVector2D(x, _position.Y), _value_color, _scale);
		return true;
	}

	static bool DrawPendingStartDebugLine(UCanvas* _canvas, UFont* _font, const FString& _line, const FVector2D& _position, float _scale)
	{
		const FString scene_marker = TEXT("SceneID=");
		const FString shot_marker = TEXT(" ShotID=");
		int32 scene_marker_index = INDEX_NONE;
		int32 shot_marker_index = INDEX_NONE;
		scene_marker_index = _line.Find(scene_marker, ESearchCase::CaseSensitive);
		if (scene_marker_index == INDEX_NONE || _line.Find(shot_marker, ESearchCase::CaseSensitive, ESearchDir::FromStart, scene_marker_index) == INDEX_NONE)
		{
			return false;
		}

		shot_marker_index = _line.Find(shot_marker, ESearchCase::CaseSensitive, ESearchDir::FromStart, scene_marker_index);
		const int32 scene_value_index = scene_marker_index + scene_marker.Len();
		const int32 shot_value_index = shot_marker_index + shot_marker.Len();

		const FString prefix = _line.Left(scene_value_index);
		const FString scene_id = _line.Mid(scene_value_index, shot_marker_index - scene_value_index);
		const FString middle = _line.Mid(shot_marker_index, shot_value_index - shot_marker_index);
		const FString shot_id = _line.Mid(shot_value_index);

		float x = _position.X;
		x += DrawDebugTextSegment(_canvas, _font, prefix, FVector2D(x, _position.Y), DEBUG_KEY_COLOR, _scale);
		x += DrawDebugTextSegment(_canvas, _font, scene_id, FVector2D(x, _position.Y), DEBUG_ID_VALUE_COLOR, _scale);
		x += DrawDebugTextSegment(_canvas, _font, middle, FVector2D(x, _position.Y), DEBUG_KEY_COLOR, _scale);
		DrawDebugTextSegment(_canvas, _font, shot_id, FVector2D(x, _position.Y), DEBUG_ID_VALUE_COLOR, _scale);
		return true;
	}

	static float GetDebugLineHeight(UCanvas* _canvas, UFont* _font, const FString& _line, float _scale)
	{
		if (_line.IsEmpty())
		{
			return _font->GetMaxCharHeight();
		}

		float text_width = 0.0f;
		float text_height = 0.0f;
		_canvas->TextSize(_font, _line, text_width, text_height, _scale, _scale);
		return text_height;
	}

	static void DrawBottomLeftDebugText(UCanvas* _canvas, const FString& _message)
	{
		if (IsInvalid(_canvas))
		{
			return;
		}

		UFont* font = UEngine::GetSmallFont();
		if (IsInvalid(font))
		{
			return;
		}

		TArray<FString> lines;
		_message.ParseIntoArrayLines(lines, false);

		float total_height = 0.0f;

		for (const FString& line : lines)
		{
			total_height += GetDebugLineHeight(_canvas, font, line, DEBUG_TEXT_SCALE) + DEBUG_LINE_PADDING;
		}

		float y = FMath::Max(0.0f, _canvas->ClipY - DEBUG_Y_MARGIN - total_height);

		for (const FString& line : lines)
		{
			if (line.IsEmpty())
			{
				y += font->GetMaxCharHeight() + DEBUG_LINE_PADDING;
				continue;
			}

			const float text_height = GetDebugLineHeight(_canvas, font, line, DEBUG_TEXT_SCALE);

			const float x = DEBUG_X_MARGIN;
			bool drawn = false;
			if (line.StartsWith(TEXT("[")))
			{
				DrawDebugTextSegment(_canvas, font, line, FVector2D(x, y), DEBUG_KEY_COLOR, DEBUG_TEXT_SCALE);
				drawn = true;
			}
			else if (line.StartsWith(TEXT("PendingStart:")))
			{
				drawn = DrawPendingStartDebugLine(_canvas, font, line, FVector2D(x, y), DEBUG_TEXT_SCALE);
			}
			else
			{
				drawn = DrawDebugKeyValueLine(_canvas, font, line, FVector2D(x, y), GetDebugValueColor(line), DEBUG_TEXT_SCALE);
			}

			if (drawn == false)
			{
				DrawDebugTextSegment(_canvas, font, line, FVector2D(x, y), DEBUG_VALUE_COLOR, DEBUG_TEXT_SCALE);
			}

			y += text_height + DEBUG_LINE_PADDING;
		}
	}
}

void UStoryFlowSubsystem::Initialize(FSubsystemCollectionBase& _collection)
{
	Super::Initialize(_collection);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UStoryFlowSubsystem::HandlePostLoadMap);
	_DebugDrawDelegateHandle = UDebugDrawService::Register(TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &UStoryFlowSubsystem::DrawDebugOverlay));
}

void UStoryFlowSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	if (_DebugDrawDelegateHandle.IsValid())
	{
		UDebugDrawService::Unregister(_DebugDrawDelegateHandle);
		_DebugDrawDelegateHandle.Reset();
	}

	StopScene();
	ClearPendingSceneStart();

	Super::Deinitialize();
}

const UStoryFlowDeveloperSettings* UStoryFlowSubsystem::GetStoryFlowDeveloperSettings() const
{
	return GetDefault<UStoryFlowDeveloperSettings>();
}

TStatId UStoryFlowSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UStoryFlowSubsystem, STATGROUP_Tickables);
}

bool UStoryFlowSubsystem::IsTickable() const
{
	return IsValid(_CurrentShotInstance) || _PendingTravelPhase == EStoryFlowPendingTravelPhase::AsyncLoadingTargetLevel;
}

void UStoryFlowSubsystem::Tick(float _delta_time)
{
	if (_PendingTravelPhase == EStoryFlowPendingTravelPhase::AsyncLoadingTargetLevel)
	{
		if (_PendingTargetLevelLoadCompleted && GetMinimumLoadingTimeProgress() >= 1.0f)
		{
			_PendingTravelPhase = EStoryFlowPendingTravelPhase::OpeningTargetLevel;
			CUSTOM_LOG(Display, TEXT("Async load + minimum duration satisfied. Open TargetLevel=%s"), *GetLevelPackageName(_PendingTargetLevel));
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, _PendingTargetLevel);
		}
	}

	if (IsInvalid(_CurrentShotInstance))
		return;

	_CurrentShotInstance->TickShot(_delta_time);
}

bool UStoryFlowSubsystem::StartFromScene(const FStorySceneID& _scene_id)
{
	if (_scene_id.IsValid() == false)
		return false;

	FStoryFlowRef story_flow_ref;
	story_flow_ref.SceneID = _scene_id;

	return StartFromRef(story_flow_ref);
}

bool UStoryFlowSubsystem::StartFromRef(const FStoryFlowRef& _story_flow_ref)
{
	UStorySceneAsset* scene_asset = FindSceneAssetBySceneID(_story_flow_ref.SceneID);

	if (IsInvalid(scene_asset))
		return false;

	if (ShouldOpenTargetLevel(scene_asset))
	{
		return BeginPendingSceneTravel(scene_asset, _story_flow_ref);
	}

	return StartResolvedScene(scene_asset, _story_flow_ref);
}

void UStoryFlowSubsystem::StopScene()
{
	_FinishingShotInstance = nullptr;
	ClearPendingSceneStart();
	ClearCurrentShot();
	ClearCurrentBranch();
	ClearCurrentScene();
	_CurrentSceneAsset = nullptr;
}

void UStoryFlowSubsystem::FinishCurrentShot(UStoryShotBase* _shot_instance)
{
	if (IsInvalid(_shot_instance) || _shot_instance != _CurrentShotInstance || _FinishingShotInstance == _shot_instance)
	{
		return;
	}

	_FinishingShotInstance = _shot_instance;
	const bool is_moved_to_next = MoveToNextShot();
	_FinishingShotInstance = nullptr;

	if (is_moved_to_next == false)
	{
		StopScene();
	}
}

#if WITH_EDITOR
void UStoryFlowSubsystem::SetEditorPlayFromShotSession(bool _is_active)
{
	GIsEditorPlayFromShotSession = _is_active;
}

bool UStoryFlowSubsystem::IsEditorPlayFromShotSession()
{
	return GIsEditorPlayFromShotSession;
}
#endif

UStorySceneAsset* UStoryFlowSubsystem::FindSceneAssetBySceneID(const FStorySceneID& _scene_id) const
{
	if (_scene_id.IsValid() == false)
		return nullptr;

	const UStoryFlowDeveloperSettings* dev_settings = GetStoryFlowDeveloperSettings();
	if (IsInvalid(dev_settings) || dev_settings->_StorySceneRegistry.IsNull())
		return nullptr;

	UStorySceneRegistryAsset* scene_registry = dev_settings->_StorySceneRegistry.LoadSynchronous();
	if (IsInvalid(scene_registry))
		return nullptr;

	return scene_registry->FindSceneAsset(_scene_id);
}

FStoryFlowRef UStoryFlowSubsystem::MakeResolvedStartRef(UStorySceneAsset* _scene_asset, const FStoryFlowRef& _story_flow_ref) const
{
	FStoryFlowRef start_ref = _story_flow_ref;
	if (IsValid(_scene_asset))
	{
		start_ref.SceneID = _scene_asset->GetSceneID();
	}

	return start_ref;
}

bool UStoryFlowSubsystem::BeginPendingSceneTravel(UStorySceneAsset* _scene_asset, const FStoryFlowRef& _story_flow_ref)
{
	if (IsInvalid(_scene_asset))
	{
		return false;
	}

	StopScene();

	_PendingStartRef = MakeResolvedStartRef(_scene_asset, _story_flow_ref);
	_PendingTargetLevel = _scene_asset->GetTargetLevel();

	const UStoryFlowDeveloperSettings* dev_settings = GetStoryFlowDeveloperSettings();
	if (IsInvalid(dev_settings) || dev_settings->_LoadingLevel.IsNull())
	{
		TRACE_ERROR(TEXT("BeginPendingSceneTravel failed: LoadingLevel is required but missing."));
		ClearPendingSceneStart();
		return false;
	}

	if (GetLevelPackageName(dev_settings->_LoadingLevel) == GetLevelPackageName(_PendingTargetLevel))
	{
		TRACE_ERROR(TEXT("BeginPendingSceneTravel failed: LoadingLevel must be different from TargetLevel."));
		ClearPendingSceneStart();
		return false;
	}

	_PendingTravelPhase = EStoryFlowPendingTravelPhase::LoadingLevel;
	RequestOpenLoadingLevel();
	return true;
}

bool UStoryFlowSubsystem::StartResolvedScene(UStorySceneAsset* _scene_asset, const FStoryFlowRef& _story_flow_ref)
{
	if (IsInvalid(_scene_asset))
		return false;

	StopScene();
	_CurrentSceneAsset = _scene_asset;

	const FStoryFlowRef start_ref = MakeResolvedStartRef(_CurrentSceneAsset, _story_flow_ref);

	if (EnterScene(start_ref) == false)
	{
		StopScene();
		return false;
	}

	if (start_ref.ShotID.IsValid())
	{
		if (MoveToShot(start_ref.ShotID) == false)
		{
			StopScene();
			return false;
		}

		return true;
	}

	if (MoveToLink(_CurrentSceneAsset->GetEntryLink(), start_ref) == false)
	{
		StopScene();
		return false;
	}

	return true;
}

bool UStoryFlowSubsystem::ShouldOpenTargetLevel(UStorySceneAsset* _scene_asset) const
{
	if (IsInvalid(_scene_asset) || _scene_asset->GetTargetLevel().IsNull())
		return false;

	return GetCurrentLevelPackageName() != GetLevelPackageName(_scene_asset->GetTargetLevel());
}

bool UStoryFlowSubsystem::BeginAsyncLoadTargetLevel()
{
	if (_PendingTargetLevel.IsNull())
	{
		TRACE_WARNING(TEXT("BeginAsyncLoadTargetLevel failed: target level missing."));
		return false;
	}

	const FSoftObjectPath target_level_path = _PendingTargetLevel.ToSoftObjectPath();
	if (target_level_path.IsValid() == false)
	{
		TRACE_WARNING(TEXT("BeginAsyncLoadTargetLevel failed: target level path invalid."));
		return false;
	}

	_PendingTargetLevelLoadHandle.Reset();
	_PendingTravelPhase = EStoryFlowPendingTravelPhase::AsyncLoadingTargetLevel;
	_PendingTargetLevelLoadCompleted = false;
	FStreamableDelegate on_loaded = FStreamableDelegate::CreateUObject(this, &UStoryFlowSubsystem::HandleTargetLevelAsyncLoaded);
	_PendingTargetLevelLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(target_level_path, MoveTemp(on_loaded));
	if (IsInvalid(_PendingTargetLevelLoadHandle))
	{
		TRACE_WARNING(TEXT("Async load request failed immediately. TargetLevel=%s"), *target_level_path.ToString());
		_PendingTravelPhase = EStoryFlowPendingTravelPhase::None;
		return false;
	}

	return true;
}

void UStoryFlowSubsystem::RequestOpenLoadingLevel()
{
	const UStoryFlowDeveloperSettings* dev_settings = GetStoryFlowDeveloperSettings();
	check(IsValid(dev_settings));
	check(dev_settings->_LoadingLevel.IsNull() == false);

	UGameplayStatics::OpenLevelBySoftObjectPtr(this, dev_settings->_LoadingLevel);
}

void UStoryFlowSubsystem::HandleTargetLevelAsyncLoaded()
{
	if (_PendingTravelPhase != EStoryFlowPendingTravelPhase::AsyncLoadingTargetLevel)
		return;

	if (_PendingTargetLevel.IsNull())
	{
		TRACE_WARNING(TEXT("Async load callback failed: pending target level missing."));
		ClearPendingSceneStart();
		return;
	}

	_PendingTargetLevelLoadCompleted = true;
}

void UStoryFlowSubsystem::ClearPendingSceneStart()
{
	if (_PendingTargetLevelLoadHandle.IsValid())
	{
		_PendingTargetLevelLoadHandle->CancelHandle();
		_PendingTargetLevelLoadHandle.Reset();
	}

	_PendingStartRef.Reset();
	_PendingTargetLevel = nullptr;
	_PendingTravelPhase = EStoryFlowPendingTravelPhase::None;
	_PendingTargetLevelLoadCompleted = false;
	_PendingLoadingLevelEnterTime = 0.0;
}

void UStoryFlowSubsystem::HandlePostLoadMap(UWorld* _loaded_world)
{
	if (_PendingTravelPhase == EStoryFlowPendingTravelPhase::None || IsInvalid(_loaded_world))
	{
		return;
	}

	const UStoryFlowDeveloperSettings* dev_settings = GetStoryFlowDeveloperSettings();
	const FString loaded_world_package_name = GetWorldPackageName(_loaded_world);

	if (_PendingTravelPhase == EStoryFlowPendingTravelPhase::LoadingLevel)
	{
		const FString loading_level_package_name = IsValid(dev_settings) ? GetLevelPackageName(dev_settings->_LoadingLevel) : FString();
		if (loading_level_package_name.IsEmpty() || loaded_world_package_name != loading_level_package_name)
		{
			return;
		}

		_PendingLoadingLevelEnterTime = FPlatformTime::Seconds();
		if (BeginAsyncLoadTargetLevel() == false)
		{
			ClearPendingSceneStart();
		}
		return;
	}

	if (_PendingTravelPhase != EStoryFlowPendingTravelPhase::OpeningTargetLevel)
	{
		return;
	}

	if (loaded_world_package_name != GetLevelPackageName(_PendingTargetLevel))
	{
		return;
	}

	const FStoryFlowRef pending_start_ref = _PendingStartRef;
	ClearPendingSceneStart();

	UStorySceneAsset* scene_asset = FindSceneAssetBySceneID(pending_start_ref.SceneID);
	if (IsInvalid(scene_asset))
	{
		return;
	}

	StartResolvedScene(scene_asset, pending_start_ref);
}

FString UStoryFlowSubsystem::GetCurrentLevelPackageName() const
{
	const UGameInstance* game_instance = GetGameInstance();
	const UWorld* world = IsValid(game_instance) ? game_instance->GetWorld() : nullptr;
	return GetWorldPackageName(world);
}

FString UStoryFlowSubsystem::GetLevelPackageName(const TSoftObjectPtr<UWorld>& _level)
{
	if (_level.IsNull())
	{
		return FString();
	}

	return FPackageName::ObjectPathToPackageName(_level.ToSoftObjectPath().ToString());
}

FString UStoryFlowSubsystem::GetWorldPackageName(const UWorld* _world)
{
	if (IsInvalid(_world))
	{
		return FString();
	}

	const ULevel* current_level = _world->GetCurrentLevel();
	if (IsValid(current_level))
	{
		return UWorld::RemovePIEPrefix(current_level->GetOutermost()->GetName());
	}

	return UWorld::RemovePIEPrefix(_world->GetOutermost()->GetName());
}

float UStoryFlowSubsystem::GetMinimumLoadingTimeProgress() const
{
	if (_PendingLoadingLevelEnterTime <= 0.0)
	{
		return 1.0f;
	}

	const UStoryFlowDeveloperSettings* dev_settings = GetStoryFlowDeveloperSettings();
	const float minimum_loading_duration = IsValid(dev_settings) ? dev_settings->_MinimumLoadingLevelDuration : 0.0f;
	if (minimum_loading_duration <= KINDA_SMALL_NUMBER)
	{
		return 1.0f;
	}

	const double elapsed_time = FPlatformTime::Seconds() - _PendingLoadingLevelEnterTime;
	const float normalized_time_progress = FMath::Clamp(static_cast<float>(elapsed_time / minimum_loading_duration), 0.0f, 1.0f);

	if (IsValid(dev_settings) && dev_settings->_MinimumLoadingLevelProgressCurve.IsNull() == false)
	{
		if (UCurveFloat* progress_curve = dev_settings->_MinimumLoadingLevelProgressCurve.LoadSynchronous())
		{
			return FMath::Clamp(progress_curve->GetFloatValue(normalized_time_progress), 0.0f, 1.0f);
		}
	}

	return normalized_time_progress;
}

bool UStoryFlowSubsystem::EnterScene(const FStoryFlowRef& _story_flow_ref)
{
	if (IsInvalid(_CurrentSceneAsset))
	{
		return false;
	}

	UStorySceneBase* scene_template = _CurrentSceneAsset->GetSceneTemplate();
	if (IsInvalid(scene_template))
	{
		return true;
	}

	_CurrentSceneInstance = DuplicateObject<UStorySceneBase>(scene_template, this);
	if (IsInvalid(_CurrentSceneInstance))
	{
		return false;
	}

	_CurrentSceneInstance->InitializeScene(_story_flow_ref);
	_CurrentSceneInstance->EnterScene();
	return true;
}

bool UStoryFlowSubsystem::MoveToShot(const FStoryShotID& _shot_id)
{
	if (IsInvalid(_CurrentSceneAsset) || _shot_id.IsValid() == false)
	{
		return false;
	}

	UStorySceneNodeData* shot_node = _CurrentSceneAsset->FindShotNode(_shot_id);
	if (IsInvalid(shot_node))
	{
		return false;
	}

	UStoryShotBase* shot_template = shot_node->GetShotTemplate();
	if (IsInvalid(shot_template))
	{
		return false;
	}

	ClearCurrentShot();

	_CurrentShotNode = shot_node;
	_CurrentShotInstance = DuplicateObject<UStoryShotBase>(shot_template, this);
	if (IsInvalid(_CurrentShotInstance))
	{
		_CurrentShotNode = nullptr;
		return false;
	}

	_CurrentShotInstance->InitializeShot(_shot_id);
	_CurrentShotInstance->EnterShot();
	return true;
}

bool UStoryFlowSubsystem::MoveToLink(const FStorySceneBranchLink& _next_link, const FStoryFlowRef& _story_flow_ref)
{
	if (_next_link.IsValid() == false)
	{
		return false;
	}

	if (_next_link.IsShotLink())
	{
		return MoveToShot(_next_link.NextShotID);
	}

	if (_next_link.IsBranchLink())
	{
		return EvaluateBranch(_next_link.NextBranchID, _story_flow_ref);
	}

	if (_next_link.IsSceneLink())
	{
		return StartFromScene(_next_link.NextSceneID);
	}

	return false;
}

bool UStoryFlowSubsystem::EvaluateBranch(const FStoryBranchID& _branch_id, const FStoryFlowRef& _story_flow_ref)
{
	if (IsInvalid(_CurrentSceneAsset) || _branch_id.IsValid() == false)
	{
		return false;
	}

	UStoryBranchNodeData* branch_node = _CurrentSceneAsset->FindBranchNode(_branch_id);
	if (IsInvalid(branch_node))
	{
		return false;
	}

	UStoryBranchBase* branch_template = branch_node->GetBranchTemplate();
	if (IsInvalid(branch_template))
	{
		return false;
	}

	const TArray<FStoryBranchOutput> branch_outputs = branch_template->GetBranchOutputs();
	const int32 branch_output_count = branch_outputs.Num();
	if (branch_output_count == 0)
	{
		return false;
	}

	_CurrentBranchNode = branch_node;
	UStoryBranchBase* branch_instance = DuplicateObject<UStoryBranchBase>(branch_template, this);
	if (IsInvalid(branch_instance))
	{
		_CurrentBranchNode = nullptr;
		return false;
	}

	branch_instance->InitializeBranch(_story_flow_ref);

	const TMap<int32, FStorySceneBranchLink>& next_links_by_pin_index = branch_node->GetNextLinksByPinIndex();
	if (next_links_by_pin_index.Num() == 0)
	{
		_CurrentBranchNode = nullptr;
		return false;
	}

	int32 next_index = 0;
	if (branch_output_count > 1)
	{
		next_index = branch_instance->SelectNextIndex(branch_output_count);
	}

	next_index = FMath::Clamp(next_index, 0, branch_output_count - 1);
	const FStorySceneBranchLink* next_link = next_links_by_pin_index.Find(next_index);
	_CurrentBranchNode = nullptr;

	if (next_link == nullptr || next_link->IsValid() == false)
	{
		return false;
	}

	return MoveToLink(*next_link, _story_flow_ref);
}

bool UStoryFlowSubsystem::MoveToNextShot()
{
	if (IsInvalid(_CurrentShotNode))
	{
		return false;
	}

	const FStoryFlowRef current_ref = GetCurrentRef();
	const FStorySceneBranchLink next_link = _CurrentShotNode->GetNextLink();

	UStoryShotBase* current_shot = _CurrentShotInstance;
	if (IsValid(current_shot))
	{
		current_shot->ExitShot();
	}

	if (next_link.IsValid() == false)
	{
		_CurrentShotInstance = nullptr;
		_CurrentShotNode = nullptr;
		return false;
	}

	_CurrentShotInstance = nullptr;
	_CurrentShotNode = nullptr;

	return MoveToLink(next_link, current_ref);
}

void UStoryFlowSubsystem::ClearCurrentScene()
{
	if (IsValid(_CurrentSceneInstance))
	{
		_CurrentSceneInstance->ExitScene();
	}

	_CurrentSceneInstance = nullptr;
}

void UStoryFlowSubsystem::ClearCurrentBranch()
{
	_CurrentBranchNode = nullptr;
}

void UStoryFlowSubsystem::ClearCurrentShot()
{
	if (IsValid(_CurrentShotInstance))
	{
		_CurrentShotInstance->ExitShot();
	}

	_CurrentShotInstance = nullptr;
	_CurrentShotNode = nullptr;
}

void UStoryFlowSubsystem::DrawDebugOverlay(UCanvas* _canvas, APlayerController* _player_controller)
{
	if (_IsDebugOverlayEnabled == false)
	{
		return;
	}

	if (IsValid(_player_controller) && _player_controller->GetGameInstance() != GetGameInstance())
	{
		return;
	}

	DrawBottomLeftDebugText(_canvas, BuildDebugSummary());
}

FStoryFlowRef UStoryFlowSubsystem::GetCurrentRef() const
{
	FStoryFlowRef current_ref;
	if (IsValid(_CurrentSceneAsset))
	{
		current_ref.SceneID = _CurrentSceneAsset->GetSceneID();
	}

	if (IsValid(_CurrentShotNode))
	{
		current_ref.ShotID = _CurrentShotNode->GetShotID();
	}

	return current_ref;
}

float UStoryFlowSubsystem::GetTargetLevelLoadingProgressRate() const
{
	if (_PendingTravelPhase == EStoryFlowPendingTravelPhase::None)
	{
		return 0.0f;
	}

	if (_PendingTravelPhase == EStoryFlowPendingTravelPhase::OpeningTargetLevel)
	{
		return 1.0f;
	}

	float target_level_load_progress = 0.0f;
	if (_PendingTargetLevelLoadCompleted)
	{
		target_level_load_progress = 1.0f;
	}
	if (_PendingTargetLevelLoadHandle.IsValid())
	{
		target_level_load_progress = _PendingTargetLevelLoadHandle->GetLoadProgress();
	}

	return FMath::Min(target_level_load_progress, GetMinimumLoadingTimeProgress());
}

FString UStoryFlowSubsystem::BuildDebugSummary() const
{
	const FStoryFlowRef current_ref = GetCurrentRef();
	const FString next_link = IsValid(_CurrentShotNode)
		? ToDebugString(_CurrentShotNode->GetNextLink())
		: TEXT("None");

	return FString::Printf(
		TEXT("[StoryFlow Debug]\n")
		TEXT("SceneID: %s\n")
		TEXT("ShotID: %s\n")
		TEXT("NextLink: %s\n")
		TEXT("\n")
		TEXT("TravelPhase: %s\n")
		TEXT("PendingStart: SceneID=%s ShotID=%s\n")
		TEXT("PendingTargetLevel: %s\n")
		TEXT("TargetLoadProgress: %.0f%%"),
		*ToDebugString(current_ref.SceneID),
		*ToDebugString(current_ref.ShotID),
		*next_link,
		*ToDebugString(_PendingTravelPhase),
		*ToDebugString(_PendingStartRef.SceneID),
		*ToDebugString(_PendingStartRef.ShotID),
		*ToDebugLevelName(_PendingTargetLevel),
		GetTargetLevelLoadingProgressRate() * 100.0f);
}

void UStoryFlowSubsystem::SetDebugOverlayEnabled(bool _is_enabled)
{
	_IsDebugOverlayEnabled = _is_enabled;
}
