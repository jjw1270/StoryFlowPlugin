// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUtils.h"

#if WITH_EDITOR
namespace StoryFlowDisplayNameUtils
{
	FORCEINLINE FText MakeDisplayNameFromTemplate(const UObject* _template)
	{
		if (IsInvalid(_template))
		{
			return FText::GetEmpty();
		}

		FString template_name;
		const UClass* template_class = _template->GetClass();
		if (IsValid(template_class) && IsValid(template_class->ClassGeneratedBy))
		{
			template_name = template_class->ClassGeneratedBy->GetName();
		}
		else
		{
			template_name = _template->GetName();
		}

		return FText::FromString(FName::NameToDisplayString(template_name, false));
	}
}
#endif
