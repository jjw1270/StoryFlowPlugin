// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "Factories/Factory.h"
#include "StorySceneFactory.generated.h"

UCLASS()
class STORYFLOWEDITOR_API UStorySceneFactory : public UFactory
{
	GENERATED_BODY()

public:
	UStorySceneFactory(const FObjectInitializer& _object_initializer);

	virtual UObject* FactoryCreateNew(UClass* _class, UObject* _parent, FName _name, EObjectFlags _flags, UObject* _context, FFeedbackContext* _warn) override;
	virtual FText GetDisplayName() const override;
	virtual bool ShouldShowInNewMenu() const override { return true; }
};
