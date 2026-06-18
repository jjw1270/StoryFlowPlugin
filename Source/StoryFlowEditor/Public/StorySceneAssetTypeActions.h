// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "AssetTypeActions_Base.h"

class FStorySceneAssetTypeActions : public FAssetTypeActions_Base
{
private:
	EAssetTypeCategories::Type _Category;

public:
	explicit FStorySceneAssetTypeActions(EAssetTypeCategories::Type _category)
		: _Category(_category)
	{
	}

	virtual FText GetName() const override { return FText::FromString(TEXT("StoryFlow Scene Asset")); }
	virtual FColor GetTypeColor() const override { return FLinearColor(0.35f, 0.55f, 0.9f).ToFColor(true); }
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& _objects, TSharedPtr<IToolkitHost> _edit_within_level_editor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override { return _Category; }
};
