// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "SGraphNodeDefault.h"

class ISinglePropertyView;
class IPropertyHandle;

class SStorySceneGraphNode_TemplateBase : public SGraphNodeDefault
{
public:
	void ConstructBase(UEdGraphNode* _graph_node);
	virtual void UpdateGraphNode() override;

protected:
	virtual void CreateBelowWidgetControls(TSharedPtr<SVerticalBox> _main_box) override;

	virtual UObject* GetTemplateObject() const = 0;
	virtual void AddAdditionalNodeWidgets() {}
	virtual void OnTemplatePropertyValueChanged() {}

private:
	bool ShouldShowPropertyInNode(const UObject* _template_object, const FProperty* _property) const;
	bool ShouldUseCompactObjectPicker(const FProperty* _property) const;
	TSharedRef<SWidget> CreateTemplatePropertiesWidget();
	TSharedRef<SWidget> CreatePropertyRowWidget(FProperty* _property, const TSharedPtr<ISinglePropertyView>& _property_view);
	TSharedRef<SWidget> CreatePropertyValueWidget(FProperty* _property, const TSharedPtr<IPropertyHandle>& _property_handle, const TSharedPtr<ISinglePropertyView>& _property_view);
	void HandleTemplatePropertyValueChanged();

	TArray<TSharedPtr<ISinglePropertyView>> _TemplatePropertyViews;
};
