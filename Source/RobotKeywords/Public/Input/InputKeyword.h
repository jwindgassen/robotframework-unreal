#pragma once

#include "Keyword.h"
#include "InputKeyword.generated.h"


class IAutomationDriver;
class IElementLocator;
class IDriverElement;


UCLASS(Abstract)
class ROBOTKEYWORDS_API UInputKeyword : public UKeyword {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (KeywordArgument))
    FString Locator;

    virtual FKeywordResponse Execute() override;

    virtual TSharedRef<IElementLocator> GetElementLocator();

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) {
        unimplemented();
        return Success();
    }

protected:
    static TSharedRef<IAutomationDriver> CreateDriver();
};
