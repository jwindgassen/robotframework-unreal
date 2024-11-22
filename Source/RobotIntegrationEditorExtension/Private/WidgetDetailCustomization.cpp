#include "WidgetDetailCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Components/Widget.h"
#include "Framework/MetaData/DriverIdMetaData.h"


TSharedRef<IDetailCustomization> FWidgetDetailCustomization::MakeInstance() {
    return MakeShareable(new FWidgetDetailCustomization);
}


void FWidgetDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {
    // Get the selected Widgets
    const TArray<TWeakObjectPtr<>>& SelectedWidgets = DetailBuilder.GetSelectedObjects();
    if (SelectedWidgets.Num() > 0) {
        SelectedWidget = Cast<UWidget>(SelectedWidgets[0].Get());
    }

    // Create a new "Metadata" Category for entering Tag and ID
    IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Metadata");
    
    // clang-format off
    // @formatter:off
    Category
        .AddCustomRow(FText::FromString("Tag"))
        .ValueContent()
        [
            SNew(SEditableTextBox).OnTextCommitted(this, &FWidgetDetailCustomization::OnEnterTag)
        ];

    Category
        .AddCustomRow(FText::FromString("ID"))
        .ValueContent()
        [
            SNew(SEditableTextBox).OnTextCommitted(this, &FWidgetDetailCustomization::OnEnterId)
        ];
    
    // clang-format on
    // @formatter:on
}

void FWidgetDetailCustomization::OnEnterTag(const FText& Tag, ETextCommit::Type) const {
    SelectedWidget->GetCachedWidget()->SetTag(FName(*Tag.ToString()));
}

void FWidgetDetailCustomization::OnEnterId(const FText& Id, ETextCommit::Type) const {
    const auto MetaData = MakeShared<FDriverIdMetaData>(*Id.ToString());
    SelectedWidget->GetCachedWidget()->AddMetadata(MetaData);
}