#pragma once

#include "Keyword.h"
#include "InputKeyword.generated.h"


class IAutomationDriver;
class IElementLocator;
class IDriverElement;

/**
 * Simple macro for automatically generating a subclass of UInputKeyword, which will just call a
 * specific method on the IDriverElement. We cannot use UCLASS in a macro, so we only generate the body.
 */
#define SIMPLE_INPUT_KEYWORD_BODY(Name, Method, ...)                                                         \
    virtual FString GetName() override {                                                              \
        return Name;                                                                                  \
    }                                                                                                 \
    virtual void PerformAction(TSharedRef<IDriverElement> Element, const TArray<FString>&) override { \
        Element->##Method##(__VA_ARGS__);                                                             \
    }


UCLASS(Abstract)
class ROBOTKEYWORDS_API UInputKeyword : public UKeyword {
    GENERATED_BODY()

public:
    virtual TArray<FString> GetArguments() override {
        return {"Locator"};
    }

    virtual TMap<FString, FString> GetArgumentTypes() override {
        return {{"Locator", "str"}};
    }

    virtual void Execute(const TArray<FString>& Arguments) override;

    virtual TSharedRef<IElementLocator> GetLocator(const FString& Path);

    virtual void PerformAction(TSharedRef<IDriverElement> Element, const TArray<FString>& OtherArgs) {
        unimplemented();
    }

protected:
    static TSharedRef<IAutomationDriver> CreateDriver();
};
