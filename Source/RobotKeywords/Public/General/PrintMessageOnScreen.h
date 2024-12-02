#pragma once

#include "Keyword.h"
#include "PrintMessageOnScreen.generated.h"

UCLASS()
class UPrintMessageOnScreenKeyword : public UKeyword {
    GENERATED_BODY()
    
public:
    UPROPERTY(meta = (KeywordArgument))
    FString Message;
    
    UPROPERTY(meta = (KeywordArgument))
    double Duration = 10;

    virtual FKeywordResponse Execute() override {
        LogInfo(TEXT("Some Number: %d"), 42);

        GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Purple, Message);

        return Success();
    }
};
