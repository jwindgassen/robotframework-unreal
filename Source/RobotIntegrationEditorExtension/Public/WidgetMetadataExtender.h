#pragma once

#include "Customizations/IBlueprintWidgetCustomizationExtender.h"


class UNavigationWithMetadata;


class FWidgetMetadataExtender : public IBlueprintWidgetCustomizationExtender {
    TSharedPtr<FWidgetBlueprintEditor> BlueprintEditor;
    
public:
    virtual void CustomizeDetails(
        IDetailLayoutBuilder& InDetailLayout, const TArrayView<UWidget*> InWidgets,
        const TSharedRef<FWidgetBlueprintEditor>& InWidgetBlueprintEditor
    ) override;

    FText GetTag() const;
    
    void OnEnterTag(const FText& Tag, ETextCommit::Type CommitType) const;

    FText GetId() const;

    void OnEnterId(const FText& Id, ETextCommit::Type CommitType) const;

private:
    /** Creates or Returns the NavigationWithMetadata from a Widget */
    static UNavigationWithMetadata* GetNavigationWithMetadata(UWidget* Widget);
};
