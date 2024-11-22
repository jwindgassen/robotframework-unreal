#pragma once

#include "Keyword.h"
#include "PrintMessageOnScreen.generated.h"

UCLASS()
class UPrintMessageOnScreenKeyword : public UKeyword {
    GENERATED_BODY()
    
public:
    virtual FString GetName() override {
        return "Print Message On Screen";
    }

    virtual TArray<FString> GetArguments() override {
        return {"text", "duration=10"};
    }

    virtual TMap<FString, FString> GetArgumentTypes() override {
        return {
            {"text", "str"},
            {"duration", "float"},
        };
    }
    
    virtual void Execute(const TArray<FString>& Arguments) override;
};
