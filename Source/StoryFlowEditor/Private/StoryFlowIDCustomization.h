// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

class FStoryFlowIDCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FStoryFlowIDCustomization>();
	}

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> _struct_property_handle, FDetailWidgetRow& _header_row, IPropertyTypeCustomizationUtils& _customization_utils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> _struct_property_handle, IDetailChildrenBuilder& _child_builder, IPropertyTypeCustomizationUtils& _customization_utils) override {}
};
