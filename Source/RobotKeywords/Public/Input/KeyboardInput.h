#pragma once

#include "Input/InputKeyword.h"
#include "KeyboardInput.generated.h"


UCLASS()
class UEnterTextElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FString GetName() override {
        return "Enter Text on Element";
    }

    virtual TArray<FString> GetArguments() override;

    virtual TMap<FString, FString> GetArgumentTypes() override;

    virtual void PerformAction(TSharedRef<IDriverElement> Element, const TArray<FString>& OtherArgs) override;
};
