#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WidgetUtils.generated.h"


class UWidget;


UCLASS()
class ROBOTINTEGRATION_API UWidgetUtils : public UBlueprintFunctionLibrary {
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    static void SetTag(const UWidget* Widget, FString Tag);

    UFUNCTION(BlueprintCallable)
    static void SetId(const UWidget* Widget, FString Id);
};
