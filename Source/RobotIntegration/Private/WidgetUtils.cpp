#include "WidgetUtils.h"

#include "Components/Widget.h"
#include "Framework/MetaData/DriverIdMetaData.h"


void UWidgetUtils::SetId(const UWidget* Widget, FString Id) {
    const TSharedPtr<SWidget> SlateWidget = Widget->GetCachedWidget();
    if (!SlateWidget.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("Could not set Id on UWidget '%s'. Widget is invalid"), *Widget->GetName())
        return;
    }

    SlateWidget->AddMetadata(MakeShared<FDriverIdMetaData>(FName{Id}));
}

void UWidgetUtils::SetTag(const UWidget* Widget, FString Tag) {
    const TSharedPtr<SWidget> SlateWidget = Widget->GetCachedWidget();
    if (!SlateWidget.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("Could not set Tag on UWidget '%s'. Widget is invalid"), *Widget->GetName())
        return;
    }

    SlateWidget->SetTag(FName{Tag});
}


