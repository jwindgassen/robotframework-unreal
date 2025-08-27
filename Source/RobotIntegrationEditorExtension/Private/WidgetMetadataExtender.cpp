#include "WidgetMetadataExtender.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "NavigationWithMetadata.h"
#include "WidgetBlueprintEditor.h"
#include "Components/Widget.h"

#define LOCTEXT_NAMESPACE "RobotIntegrationEditorExtension"

void FWidgetMetadataExtender::CustomizeDetails(
    IDetailLayoutBuilder& InDetailLayout, const TArrayView<UWidget*>, const TSharedRef<FWidgetBlueprintEditor>& InWidgetBlueprintEditor
) {
    BlueprintEditor = InWidgetBlueprintEditor;

    // Create a new "Metadata" Category for entering Tag and ID
    IDetailCategoryBuilder& Category = InDetailLayout.EditCategory("Metadata");

    // clang-format off
    // @formatter:off
    const FText TagDetailsRowName = LOCTEXT("TagDetailsRowName", "Tag");
    Category
        .AddCustomRow(TagDetailsRowName)
        .NameContent()
        [
            SNew(STextBlock)
            .Text(TagDetailsRowName)
        ]
        .ValueContent()
        [
            SNew(SEditableTextBox)
            .Text_Raw(this, &FWidgetMetadataExtender::GetTag)
            .OnTextCommitted(this, &FWidgetMetadataExtender::OnEnterTag)
        ];

    const FText IdDetailsRowName = LOCTEXT("IdDetailsRowName", "ID");
    Category
        .AddCustomRow(IdDetailsRowName)
        .NameContent()
        [
            SNew(STextBlock)
            .Text(IdDetailsRowName)
        ]
        .ValueContent()
        [
            SNew(SEditableTextBox)
            .Text_Raw(this, &FWidgetMetadataExtender::GetId)
            .OnTextCommitted(this, &FWidgetMetadataExtender::OnEnterId)
        ];
    // clang-format on
    // @formatter:on
}

FText FWidgetMetadataExtender::GetTag() const {
    if (BlueprintEditor->GetSelectedWidgets().Num() != 1) {
        return FText::GetEmpty();
    }
    
    for (const auto& WidgetRef : BlueprintEditor->GetSelectedWidgets()) {
        const UNavigationWithMetadata* Navigation = Cast<UNavigationWithMetadata>(WidgetRef.GetTemplate()->Navigation);
        if (Navigation) {
            return Navigation->Tag;
        }
    }

    return FText::GetEmpty();
}

void FWidgetMetadataExtender::OnEnterTag(const FText& Tag, ETextCommit::Type CommitType) const {
    for (auto& WidgetReference : BlueprintEditor->GetSelectedWidgets()) {
        UWidget* Widget = WidgetReference.GetTemplate();
        GetNavigationWithMetadata(Widget)->Tag = Tag;
    }
}

FText FWidgetMetadataExtender::GetId() const {
    if (BlueprintEditor->GetSelectedWidgets().Num() != 1) {
        return FText::GetEmpty();
    }
    
    for (const auto& WidgetRef : BlueprintEditor->GetSelectedWidgets()) {
        const UNavigationWithMetadata* Navigation = Cast<UNavigationWithMetadata>(WidgetRef.GetTemplate()->Navigation);
        if (Navigation) {
            return Navigation->Id;
        }
    }

    return FText::GetEmpty();
}

void FWidgetMetadataExtender::OnEnterId(const FText& Id, ETextCommit::Type CommitType) const {
    for (auto& WidgetReference : BlueprintEditor->GetSelectedWidgets()) {
        UWidget* Widget = WidgetReference.GetTemplate();
        GetNavigationWithMetadata(Widget)->Id = Id;
    }
}

UNavigationWithMetadata* FWidgetMetadataExtender::GetNavigationWithMetadata(UWidget* Widget) {
    UWidgetNavigation* OldNavigation = Widget->Navigation;
    UNavigationWithMetadata* NewNavigation;

    // Create a new UNavigationWithMetadata if there is none
    if (!OldNavigation) {
        NewNavigation = NewObject<UNavigationWithMetadata>(Widget);
        Widget->Navigation = NewNavigation;
    } else {
        NewNavigation = Cast<UNavigationWithMetadata>(OldNavigation);
    }

    // Cast failed => this is only a UWidgetNavigation, not a UNavigationWithMetadata
    if (!NewNavigation) {
        NewNavigation = NewObject<UNavigationWithMetadata>(Widget);
        Widget->Navigation = NewNavigation;
        
        NewNavigation->Up = OldNavigation->Up;
        NewNavigation->Down = OldNavigation->Down;
        NewNavigation->Left = OldNavigation->Left;
        NewNavigation->Right = OldNavigation->Right;
        NewNavigation->Next = OldNavigation->Next;
        NewNavigation->Previous = OldNavigation->Previous;
    }

    return NewNavigation;
}
