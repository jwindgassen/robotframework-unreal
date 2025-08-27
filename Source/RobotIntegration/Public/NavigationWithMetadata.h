#pragma once

#include "CoreMinimal.h"
#include "Blueprint/WidgetNavigation.h"
#include "NavigationWithMetadata.generated.h"


/**
 * Used to store the Tag and ID of an UWidget inside the UWidget::Navigation property.
 * Storing Tag/ID this way ensures they are serialized to disk and are stored persistently.
 *
 * Primarily used by our Editor Extension, which replaces the default UWidgetNavigation instance with this and stores
 * the Tag/ID set in the details panel into the instance.
 *
 * See the README of the Editor Extension for a more detailed explanation.
 */
UCLASS()
class ROBOTINTEGRATION_API UNavigationWithMetadata : public UWidgetNavigation {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Tag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Id;


    UNavigationWithMetadata();

    virtual void PostInitProperties() override;
    
    virtual void PostLoad() override;
};
