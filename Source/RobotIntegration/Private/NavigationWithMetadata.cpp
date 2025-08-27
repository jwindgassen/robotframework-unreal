#include "NavigationWithMetadata.h"

#include "Components/Widget.h"
#include "Framework/MetaData/DriverIdMetaData.h"


UNavigationWithMetadata::UNavigationWithMetadata() {
    UE_LOG(LogTemp, Warning, TEXT("%s->UNavigationWithMetadata()"), *GetName())
}

void UNavigationWithMetadata::PostInitProperties() {
    Super::PostInitProperties();
    UE_LOG(LogTemp, Warning, TEXT("%s->PostInitProperties()"), *GetName())
}

void UNavigationWithMetadata::PostLoad() {
    Super::PostLoad();
    UE_LOG(LogTemp, Warning, TEXT("%s->PostLoad()"), *GetName())

    const UWidget* Widget = Cast<UWidget>(GetOuter());
    checkf(
        Widget, TEXT("UNavigationWithMetadata's Outer (%s) is not a UWidget but %s"), *Widget->GetName(),
        *GetOuter()->GetClass()->GetName()
    )
    UE_LOG(LogTemp, Warning, TEXT("Widget: %s"), *Widget->GetName())

    // While the UWidget has been created, the underlying Slate Widget might not have been created yet.
    // So we will start an Async Task to wait until the Slate Widget has been created to set the Tag/ID on it.
    // ToDo: Is there a better way to do this?
    Async(EAsyncExecution::ThreadPool, [&Widget, this]() {
        // Wait until the Slate Widget is valid
        FGenericPlatformProcess::ConditionalSleep([&Widget]() {
            return Widget->GetCachedWidget().IsValid();
        }, 0.1);

        UE_LOG(LogTemp, Warning, TEXT("Underlying SWidget has been created!"))
        
        // Set Tag and ID on it
        const TSharedPtr<SWidget> SlateWidget = Widget->GetCachedWidget();
        SlateWidget->SetTag(FName{Tag.ToString()});

        const auto Metadata = MakeShared<FDriverIdMetaData>(FName{Id.ToString()});
        SlateWidget->AddMetadata(Metadata);
    });
}