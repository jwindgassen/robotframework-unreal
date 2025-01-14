#pragma once

#include "IDriverElement.h"
#include "Input/InputKeyword.h"
#include "KeyboardInput.generated.h"


UCLASS()
class UEnterTextOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY()
    FString Text;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->Type(Text);
        return Success();
    }
};
