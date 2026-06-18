// Copyright (c) 2026 장윤제. All rights reserved.

#include "Graph/StorySceneGraphNode_Branch.h"
#include "StoryBranchBase.h"
#include "StoryBranchNodeData.h"
#include "StorySceneAsset.h"
#include "StoryFlowDefines.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphSchema.h"
#include "CommonUtils.h"

namespace
{
	static const FName StorySceneBranchPinCategory(TEXT("StoryFlow"));

	static bool TryGetNextPinIndex(const UEdGraphPin* _pin, int32& _out_index)
	{
		_out_index = INDEX_NONE;
		if (_pin == nullptr)
		{
			return false;
		}

		const FString pin_name = _pin->PinName.ToString();
		FString numeric_suffix;
		if (pin_name.Split(TEXT("Next_"), nullptr, &numeric_suffix) == false)
		{
			return false;
		}

		if (numeric_suffix.IsNumeric() == false)
		{
			return false;
		}

		_out_index = FCString::Atoi(*numeric_suffix);
		return true;
	}

	static FStoryBranchID MakeNextBranchID(const UStorySceneAsset* _scene_asset)
	{
		if (IsInvalid(_scene_asset))
		{
			return FStoryBranchID();
		}

		TSet<int32> used_indices;
		for (const TObjectPtr<UStoryBranchNodeData>& branch_node : _scene_asset->GetBranchNodes())
		{
			if (IsInvalid(branch_node) || branch_node->GetBranchID().IsValid() == false)
			{
				continue;
			}

			const FString id_string = branch_node->GetBranchID().Get().ToString();
			FString numeric_suffix;
			if (id_string.Split(TEXT("Branch_"), nullptr, &numeric_suffix))
			{
				used_indices.Add(FCString::Atoi(*numeric_suffix));
			}
		}

		int32 next_index = 1;
		while (used_indices.Contains(next_index))
		{
			++next_index;
		}

		return FStoryBranchID(*FString::Printf(TEXT("Branch_%03d"), next_index));
	}

	static TArray<FStoryBranchOutput> GetBranchOutputs(const UStoryBranchNodeData* _branch_node_data)
	{
		if (IsInvalid(_branch_node_data) || IsInvalid(_branch_node_data->GetBranchTemplate()))
		{
			return TArray<FStoryBranchOutput>();
		}

		return _branch_node_data->GetBranchTemplate()->GetBranchOutputs();
	}

	static FText GetNextPinDisplayName(const UEdGraphPin* _pin, const TArray<FStoryBranchOutput>& _branch_outputs)
	{
		int32 next_pin_index = INDEX_NONE;
		if (TryGetNextPinIndex(_pin, next_pin_index) && _branch_outputs.IsValidIndex(next_pin_index))
		{
			const FText& output_display_name = _branch_outputs[next_pin_index].DisplayName;
			if (output_display_name.IsEmpty() == false)
			{
				return output_display_name;
			}
		}

		return _pin ? FText::FromName(_pin->PinName) : FText::GetEmpty();
	}
}

void UStorySceneGraphNode_Branch::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, StorySceneBranchPinCategory, NAME_None, TEXT("In"));

	const TArray<FStoryBranchOutput> branch_outputs = GetBranchOutputs(_BranchNodeData);
	const int32 next_pin_count = branch_outputs.Num();
	for (int32 index = 0; index < next_pin_count; ++index)
	{
		UEdGraphPin* next_pin = CreatePin(EGPD_Output, StorySceneBranchPinCategory, NAME_None, *FString::Printf(TEXT("Next_%d"), index));
		if (next_pin)
		{
			next_pin->PinFriendlyName = GetNextPinDisplayName(next_pin, branch_outputs);
		}
	}
}

void UStorySceneGraphNode_Branch::PostLoad()
{
	Super::PostLoad();
	SyncNextPinsToNodeData();
}

void UStorySceneGraphNode_Branch::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

	if (IsValid(_BranchNodeData))
	{
		return;
	}

	UStorySceneAsset* scene_asset = GetOwningSceneAsset();
	if (IsInvalid(scene_asset))
	{
		return;
	}

	_BranchNodeData = scene_asset->CreateBranchNode();
	if (IsValid(_BranchNodeData))
	{
		_BranchNodeData->SetBranchID(MakeNextBranchID(scene_asset));
	}
}

void UStorySceneGraphNode_Branch::PrepareForCopying()
{
	Super::PrepareForCopying();

	if (IsValid(_BranchNodeData))
	{
		_BranchNodeData->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

void UStorySceneGraphNode_Branch::PostPasteNode()
{
	Super::PostPasteNode();
	ResetBranchNodeDataOwner();

	if (UStorySceneAsset* scene_asset = GetOwningSceneAsset())
	{
		if (IsInvalid(_BranchNodeData))
		{
			_BranchNodeData = scene_asset->CreateBranchNode();
		}
		else
		{
			scene_asset->AddBranchNode(_BranchNodeData);
		}

		if (IsValid(_BranchNodeData))
		{
			_BranchNodeData->SetBranchID(MakeNextBranchID(scene_asset));
			SyncNextPinsToNodeData();
		}
	}
}

void UStorySceneGraphNode_Branch::DestroyNode()
{
	if (UStorySceneAsset* scene_asset = GetOwningSceneAsset())
	{
		scene_asset->RemoveBranchNode(_BranchNodeData);
	}

	_BranchNodeData = nullptr;
	Super::DestroyNode();
}

void UStorySceneGraphNode_Branch::AutowireNewNode(UEdGraphPin* _from_pin)
{
	Super::AutowireNewNode(_from_pin);

	if (_from_pin == nullptr)
	{
		return;
	}

	const UEdGraphSchema* schema = GetSchema();
	if (schema == nullptr)
	{
		return;
	}

	UEdGraphPin* target_pin = (_from_pin->Direction == EGPD_Output) ? FindPin(TEXT("In")) : FindFirstAvailableNextPin();
	if (target_pin == nullptr)
	{
		return;
	}

	if (schema->TryCreateConnection(_from_pin, target_pin) == false)
	{
		schema->TryCreateConnection(target_pin, _from_pin);
	}

	if (UEdGraph* graph = GetGraph())
	{
		graph->NotifyGraphChanged();
	}
}

FText UStorySceneGraphNode_Branch::GetNodeTitle(ENodeTitleType::Type) const
{
	if (IsValid(_BranchNodeData))
	{
		return FText::Format(FText::FromString(TEXT("Branch\n{0}")), _BranchNodeData->GetDisplayNameText());
	}

	return FText::FromString(TEXT("Branch"));
}

FLinearColor UStorySceneGraphNode_Branch::GetNodeTitleColor() const
{
	return FLinearColor(0.78f, 0.40f, 0.10f);
}

UStorySceneAsset* UStorySceneGraphNode_Branch::GetOwningSceneAsset() const
{
	return GetGraph() ? Cast<UStorySceneAsset>(GetGraph()->GetOuter()) : nullptr;
}

TArray<UEdGraphPin*> UStorySceneGraphNode_Branch::GetNextPins() const
{
	TArray<UEdGraphPin*> next_pins;
	for (UEdGraphPin* pin : Pins)
	{
		if (pin && pin->Direction == EGPD_Output && pin->PinName.ToString().StartsWith(TEXT("Next_")))
		{
			next_pins.Add(pin);
		}
	}

	next_pins.Sort([](const UEdGraphPin& _lhs, const UEdGraphPin& _rhs)
		{
			int32 lhs_index = INDEX_NONE;
			int32 rhs_index = INDEX_NONE;
			const bool has_lhs_index = TryGetNextPinIndex(&_lhs, lhs_index);
			const bool has_rhs_index = TryGetNextPinIndex(&_rhs, rhs_index);
			if (has_lhs_index && has_rhs_index)
			{
				return lhs_index < rhs_index;
			}

			return _lhs.PinName.LexicalLess(_rhs.PinName);
		});

	return next_pins;
}

UEdGraphPin* UStorySceneGraphNode_Branch::FindFirstAvailableNextPin() const
{
	for (UEdGraphPin* next_pin : GetNextPins())
	{
		if (next_pin && next_pin->LinkedTo.Num() == 0)
		{
			return next_pin;
		}
	}

	TArray<UEdGraphPin*> next_pins = GetNextPins();
	return next_pins.Num() > 0 ? next_pins[0] : nullptr;
}

void UStorySceneGraphNode_Branch::SyncNextPinsToNodeData()
{
	const TArray<FStoryBranchOutput> branch_outputs = GetBranchOutputs(_BranchNodeData);
	const int32 desired_branch_count = branch_outputs.Num();
	TArray<UEdGraphPin*> next_pins = GetNextPins();
	const int32 current_branch_count = next_pins.Num();
	bool is_pin_display_name_dirty = false;
	for (UEdGraphPin* next_pin : next_pins)
	{
		if (next_pin == nullptr)
		{
			continue;
		}

		const FText desired_pin_display_name = GetNextPinDisplayName(next_pin, branch_outputs);
		if (next_pin->PinFriendlyName.EqualTo(desired_pin_display_name) == false)
		{
			next_pin->PinFriendlyName = desired_pin_display_name;
			is_pin_display_name_dirty = true;
		}
	}

	if (desired_branch_count == current_branch_count)
	{
		if (is_pin_display_name_dirty)
		{
			if (UEdGraph* graph = GetGraph())
			{
				graph->NotifyGraphChanged();
			}
		}

		return;
	}

	Modify();
	if (desired_branch_count > current_branch_count)
	{
		for (int32 index = current_branch_count; index < desired_branch_count; ++index)
		{
			UEdGraphPin* next_pin = CreatePin(EGPD_Output, StorySceneBranchPinCategory, NAME_None, *FString::Printf(TEXT("Next_%d"), index));
			if (next_pin)
			{
				next_pin->PinFriendlyName = GetNextPinDisplayName(next_pin, branch_outputs);
			}
		}
	}
	else
	{
		for (int32 index = current_branch_count - 1; index >= desired_branch_count; --index)
		{
			UEdGraphPin* pin_to_remove = next_pins.IsValidIndex(index) ? next_pins[index] : nullptr;
			if (pin_to_remove)
			{
				pin_to_remove->BreakAllPinLinks();
				RemovePin(pin_to_remove);
			}
		}
	}

	if (UEdGraph* graph = GetGraph())
	{
		graph->NotifyGraphChanged();
	}
}

void UStorySceneGraphNode_Branch::ClearCompileMessage()
{
	bHasCompilerMessage = false;
	ErrorType = 0;
	ErrorMsg.Empty();
}

void UStorySceneGraphNode_Branch::SetCompileError(const FString& _error_message)
{
	bHasCompilerMessage = true;
	ErrorType = 1;
	ErrorMsg = _error_message;
}

void UStorySceneGraphNode_Branch::PostCopyNode()
{
	ResetBranchNodeDataOwner();
}

void UStorySceneGraphNode_Branch::ResetBranchNodeDataOwner()
{
	if (IsValid(_BranchNodeData) == false)
	{
		return;
	}

	if (UStorySceneAsset* scene_asset = GetOwningSceneAsset())
	{
		if (_BranchNodeData->GetOuter() != scene_asset)
		{
			_BranchNodeData->Rename(nullptr, scene_asset, REN_DontCreateRedirectors | REN_DoNotDirty);
		}
	}
}
