// Copyright (c) 2026 장윤제. All rights reserved.

#include "StoryFlowIDCustomization.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"

void FStoryFlowIDCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> _struct_property_handle, FDetailWidgetRow& _header_row, IPropertyTypeCustomizationUtils& _customization_utils)
{
	TSharedPtr<IPropertyHandle> id_property_handle = _struct_property_handle->GetChildHandle(TEXT("ID"));
	if (id_property_handle.IsValid() == false)
	{
		_header_row.NameContent()
		[
			_struct_property_handle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			_struct_property_handle->CreatePropertyValueWidget()
		];
		return;
	}

	_struct_property_handle->MarkResetToDefaultCustomized(true);

	_header_row
	.OverrideResetToDefault(FResetToDefaultOverride::Hide())
	.IsValueEnabled(TAttribute<bool>::CreateLambda([_struct_property_handle]()
	{
		return _struct_property_handle->IsEditable();
	}))
	.NameContent()
	[
		_struct_property_handle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(150.0f)
	.MaxDesiredWidth(400.0f)
	[
		id_property_handle->CreatePropertyValueWidget()
	];
}
