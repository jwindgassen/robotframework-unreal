#pragma once

#include "CoreMinimal.h"
#include "Components/ContentWidget.h"
#include "LocatorMarker.generated.h"


/**
 * The LocatorMarker can be used to annotate the child widget with an ID, which can be used in RobotFramework
 * test cases to locate and interact with widgets on the screen.
 */
UCLASS()
class ROBOTINTEGRATION_API ULocatorMarker : public UContentWidget {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Content")
    FName Id;
    
protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
};

