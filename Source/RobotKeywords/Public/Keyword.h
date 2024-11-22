#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Keyword.generated.h"


UCLASS(Abstract)
class ROBOTKEYWORDS_API UKeyword : public UObject {
    GENERATED_BODY()

public:
    UFUNCTION()
    virtual FString GetName() {
        unimplemented();
        return "";
    }

    UFUNCTION()
    virtual TArray<FString> GetArguments() {
        return {};
    }

    UFUNCTION()
    virtual TMap<FString, FString> GetArgumentTypes() {
        return {};
    }

    UFUNCTION()
    virtual void Execute(const TArray<FString>& Arguments) {
        unimplemented();
    }
};
