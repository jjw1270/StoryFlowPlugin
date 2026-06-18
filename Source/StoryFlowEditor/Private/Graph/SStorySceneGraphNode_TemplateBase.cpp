// Copyright (c) 2026 장윤제. All rights reserved.

#include "Graph/SStorySceneGraphNode_TemplateBase.h"
#include "EdGraph/EdGraph.h"
#include "ISinglePropertyView.h"
#include "Modules/ModuleManager.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "UObject/FieldIterator.h"
#include "UObject/UnrealType.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
	static const FName HideInStoryFlowNodeMetaName(TEXT("HideInStoryFlowNode"));
}

void SStorySceneGraphNode_TemplateBase::ConstructBase(UEdGraphNode* _graph_node)
{
	SGraphNodeDefault::Construct(SGraphNodeDefault::FArguments().GraphNodeObj(_graph_node));
}

void SStorySceneGraphNode_TemplateBase::UpdateGraphNode()
{
	SGraphNodeDefault::UpdateGraphNode();

	AddAdditionalNodeWidgets();
}

void SStorySceneGraphNode_TemplateBase::CreateBelowWidgetControls(TSharedPtr<SVerticalBox> _main_box)
{
	if (_main_box.IsValid() == false)
	{
		return;
	}

	TSharedRef<SWidget> template_properties_widget = CreateTemplatePropertiesWidget();
	if (template_properties_widget != SNullWidget::NullWidget)
	{
		_main_box->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.Padding(FMargin(8.0f, 4.0f, 8.0f, 8.0f))
			[
				template_properties_widget
			];
	}
}

bool SStorySceneGraphNode_TemplateBase::ShouldShowPropertyInNode(const UObject* _template_object, const FProperty* _property) const
{
	if (_template_object == nullptr || _property == nullptr)
	{
		return false;
	}

	if (_property->HasMetaData(HideInStoryFlowNodeMetaName))
	{
		return false;
	}

	if (_property->HasAnyPropertyFlags(CPF_Edit) == false)
	{
		return false;
	}

	if (_property->HasAnyPropertyFlags(CPF_EditConst | CPF_DisableEditOnInstance | CPF_Deprecated))
	{
		return false;
	}

	if (_template_object->CanEditChange(_property) == false)
	{
		return false;
	}

	return true;
}

bool SStorySceneGraphNode_TemplateBase::ShouldUseCompactObjectPicker(const FProperty* _property) const
{
	if (_property == nullptr)
	{
		return false;
	}

	if (CastField<const FClassProperty>(_property) || CastField<const FSoftClassProperty>(_property))
	{
		return false;
	}

	return CastField<const FObjectPropertyBase>(_property) != nullptr;
}

TSharedRef<SWidget> SStorySceneGraphNode_TemplateBase::CreateTemplatePropertiesWidget()
{
	_TemplatePropertyViews.Reset();

	UObject* template_object = GetTemplateObject();
	if (IsValid(template_object) == false)
	{
		return SNullWidget::NullWidget;
	}

	UClass* template_class = template_object->GetClass();
	if (IsValid(template_class) == false)
	{
		return SNullWidget::NullWidget;
	}

	FPropertyEditorModule& property_editor_module = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	TSharedRef<SVerticalBox> property_box = SNew(SVerticalBox);

	FSinglePropertyParams property_params;
	property_params.NamePlacement = EPropertyNamePlacement::Hidden;
	property_params.bHideAssetThumbnail = true;
	property_params.bHideResetToDefault = true;

	for (TFieldIterator<FProperty> property_it(template_class, EFieldIteratorFlags::IncludeSuper); property_it; ++property_it)
	{
		FProperty* property = *property_it;
		if (ShouldShowPropertyInNode(template_object, property) == false)
		{
			continue;
		}

		TSharedPtr<ISinglePropertyView> property_view = property_editor_module.CreateSingleProperty(template_object, property->GetFName(), property_params);
		if (property_view.IsValid() == false || property_view->HasValidProperty() == false)
		{
			continue;
		}

		const bool use_compact_object_picker = ShouldUseCompactObjectPicker(property);
		TSharedPtr<IPropertyHandle> property_handle = property_view->GetPropertyHandle();
		if (use_compact_object_picker && property_handle.IsValid())
		{
			property_handle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &SStorySceneGraphNode_TemplateBase::HandleTemplatePropertyValueChanged));
		}
		else
		{
			property_view->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &SStorySceneGraphNode_TemplateBase::HandleTemplatePropertyValueChanged));
		}

		_TemplatePropertyViews.Add(property_view);

		property_box->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			.Padding(3.0f, 1.0f)
			[
				CreatePropertyRowWidget(property, property_view)
			];
	}

	if (_TemplatePropertyViews.Num() == 0)
	{
		return SNullWidget::NullWidget;
	}

	return SNew(SBorder)
		.Cursor(EMouseCursor::Default)
		.BorderImage(FAppStyle::GetBrush(TEXT("Graph.Node.Body")))
		.Padding(FMargin(6.0f, 4.0f))
		.HAlign(HAlign_Fill)
		[
			property_box
		];
}

TSharedRef<SWidget> SStorySceneGraphNode_TemplateBase::CreatePropertyRowWidget(FProperty* _property, const TSharedPtr<ISinglePropertyView>& _property_view)
{
	TSharedPtr<IPropertyHandle> property_handle = _property_view.IsValid() ? _property_view->GetPropertyHandle() : nullptr;

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(FMargin(0.0f, 0.0f, 0.0f, 1.0f))
		[
			SNew(STextBlock)
			.Text(_property ? _property->GetDisplayNameText() : FText::GetEmpty())
			.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
			.ColorAndOpacity(FSlateColor::UseSubduedForeground())
			.Cursor(EMouseCursor::Default)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		[
			CreatePropertyValueWidget(_property, property_handle, _property_view)
		];
}

TSharedRef<SWidget> SStorySceneGraphNode_TemplateBase::CreatePropertyValueWidget(FProperty* _property, const TSharedPtr<IPropertyHandle>& _property_handle, const TSharedPtr<ISinglePropertyView>& _property_view)
{
	if (ShouldUseCompactObjectPicker(_property) && _property_handle.IsValid())
	{
		const FObjectPropertyBase* object_property = CastField<const FObjectPropertyBase>(_property);
		const UClass* allowed_class = object_property && IsValid(object_property->PropertyClass)
			? object_property->PropertyClass.Get()
			: UObject::StaticClass();

		return SNew(SObjectPropertyEntryBox)
			.Cursor(EMouseCursor::Default)
			.PropertyHandle(_property_handle)
			.AllowedClass(allowed_class)
			.AllowClear(false)
			.DisplayUseSelected(false)
			.DisplayBrowse(false)
			.DisplayThumbnail(false)
			.DisplayCompactSize(true)
			.EnableContentPicker(true);
	}

	if (_property_view.IsValid())
	{
		return SNew(SBox)
			.Cursor(EMouseCursor::Default)
			.HAlign(HAlign_Fill)
			[
				_property_view.ToSharedRef()
			];
	}

	return SNullWidget::NullWidget;
}

void SStorySceneGraphNode_TemplateBase::HandleTemplatePropertyValueChanged()
{
	UObject* template_object = GetTemplateObject();
	if (IsValid(template_object))
	{
		template_object->Modify();
		template_object->MarkPackageDirty();
	}

	OnTemplatePropertyValueChanged();

	if (GraphNode != nullptr)
	{
		GraphNode->Modify();

		if (UEdGraph* graph = GraphNode->GetGraph())
		{
			graph->Modify();
			graph->NotifyGraphChanged();
		}
	}
}
