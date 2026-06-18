// Copyright (c) 2026 장윤제. All rights reserved.

#include "StorySceneFactory.h"
#include "StorySceneAsset.h"

UStorySceneFactory::UStorySceneFactory(const FObjectInitializer& _object_initializer)
	: Super(_object_initializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UStorySceneAsset::StaticClass();
}

UObject* UStorySceneFactory::FactoryCreateNew(UClass* _class, UObject* _parent, FName _name, EObjectFlags _flags, UObject* _context, FFeedbackContext* _warn)
{
	UStorySceneAsset* story_scene_asset = NewObject<UStorySceneAsset>(_parent, _class, _name, _flags | RF_Transactional);
	if (IsValid(story_scene_asset))
	{
		story_scene_asset->SetSceneID(FStorySceneID(_name));
	}

	return story_scene_asset;
}

FText UStorySceneFactory::GetDisplayName() const
{
	return FText::FromString(TEXT("StoryFlow Scene Asset"));
}
