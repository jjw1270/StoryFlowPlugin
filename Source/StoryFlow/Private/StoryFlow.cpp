// Copyright (c) 2026 장윤제. All rights reserved.

#include "StoryFlow.h"
#include "StoryFlowSubsystem.h"
#include "CommonUtils.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"

#define LOCTEXT_NAMESPACE "FStoryFlowModule"

namespace
{
	TUniquePtr<FAutoConsoleCommandWithWorldAndArgs> DebugStoryFlowCommand;

	static UWorld* ResolveDebugWorld(UWorld* _world)
	{
		if (IsValid(_world))
		{
			return _world;
		}

		if (IsInvalid(GEngine))
		{
			return nullptr;
		}

		for (const FWorldContext& world_context : GEngine->GetWorldContexts())
		{
			UWorld* world = world_context.World();
			if (IsValid(world) && (world->IsGameWorld() || world->WorldType == EWorldType::PIE))
			{
				return world;
			}
		}

		return nullptr;
	}

	static void ShowDebugMessage(const FString& _message, const FColor& _color = FColor::Cyan)
	{
		CUSTOM_LOG(Display, TEXT("\n%s"), *_message);
	}

	static void ExecuteDebugStoryFlow(const TArray<FString>& _args, UWorld* _world)
	{
		if (_args.Num() == 0 || _args[0].Equals(TEXT("StoryFlow"), ESearchCase::IgnoreCase) == false)
		{
			ShowDebugMessage(TEXT("[StoryFlow Debug]\nUsage: Debug StoryFlow"), FColor::Yellow);
			return;
		}

		UWorld* world = ResolveDebugWorld(_world);
		if (IsInvalid(world) || IsInvalid(world->GetGameInstance()))
		{
			ShowDebugMessage(TEXT("[StoryFlow Debug]\nWorld or GameInstance is not available."), FColor::Yellow);
			return;
		}

		UStoryFlowSubsystem* story_flow_subsystem = world->GetGameInstance()->GetSubsystem<UStoryFlowSubsystem>();
		if (IsInvalid(story_flow_subsystem))
		{
			ShowDebugMessage(TEXT("[StoryFlow Debug]\nStoryFlowSubsystem is not available."), FColor::Yellow);
			return;
		}

		const bool should_enable = !story_flow_subsystem->IsDebugOverlayEnabled();
		story_flow_subsystem->SetDebugOverlayEnabled(should_enable);
		ShowDebugMessage(FString::Printf(TEXT("[StoryFlow Debug]\nOverlay: %s\n%s"),
			should_enable ? TEXT("On") : TEXT("Off"),
			*story_flow_subsystem->BuildDebugSummary()),
			should_enable ? FColor::Cyan : FColor::Silver);
	}
}

void FStoryFlowModule::StartupModule()
{
	DebugStoryFlowCommand = MakeUnique<FAutoConsoleCommandWithWorldAndArgs>(
		TEXT("Debug"),
		TEXT("Usage: Debug StoryFlow. Toggles the StoryFlow runtime debug overlay."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&ExecuteDebugStoryFlow),
		ECVF_Cheat);
}

void FStoryFlowModule::ShutdownModule()
{
	DebugStoryFlowCommand.Reset();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStoryFlowModule, StoryFlow)
