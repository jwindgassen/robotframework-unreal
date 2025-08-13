#pragma once

#include "Keyword.h"
#include "PrintMessageOnScreen.generated.h"

UCLASS()
class ROBOTKEYWORDS_API UPrintMessageOnScreenKeyword : public UKeyword {
    GENERATED_BODY()
    
public:
    UPROPERTY()
    FString Message;
    
    UPROPERTY()
    double Duration = 10;

    virtual FKeywordResponse Execute() override {
        LogInfo(TEXT("Some Number: %d"), 42);

        GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Purple, Message);

        return Success();
    }
};
