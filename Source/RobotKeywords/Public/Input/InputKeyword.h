#pragma once

#include "Keyword.h"
#include "InputKeyword.generated.h"


class IAutomationDriver;
class IElementLocator;
class IDriverElement;


/**
 * An InputKeyword uses the AutomationDriver Module from Unreal to simulate user input on a UI Element
 *
 * The Locator is a string used locate the SWidget that the action should be performed on. You only need to
 * override "PerformAction" when subclassing an InputKeyword
 *
 * Uses the Path Locator by default.
 * @see https://dev.epicgames.com/documentation/en-us/unreal-engine/automation-driver-in-unreal-engine#by::path()
 */
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
