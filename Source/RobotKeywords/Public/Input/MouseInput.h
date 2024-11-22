#pragma once

#include "CoreMinimal.h"
#include "IDriverElement.h"
#include "InputKeyword.h"
#include "MouseInput.generated.h"


/*************************************
 *               Click               *
 *************************************/


UCLASS()
class ROBOTKEYWORDS_API UHoverElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    SIMPLE_INPUT_KEYWORD_BODY("Hover on Element", Hover);
};


UCLASS()
class ROBOTKEYWORDS_API UClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    SIMPLE_INPUT_KEYWORD_BODY("Click on Element", Click);
};


UCLASS()
class ROBOTKEYWORDS_API UDoubleClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    SIMPLE_INPUT_KEYWORD_BODY("Double Click on Element", DoubleClick);
};


UCLASS()
class ROBOTKEYWORDS_API URightClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    SIMPLE_INPUT_KEYWORD_BODY("Right Click on Element", Click, EMouseButtons::Right);
};


UCLASS()
class ROBOTKEYWORDS_API UMiddleClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    SIMPLE_INPUT_KEYWORD_BODY("Middle Click on Element", Click, EMouseButtons::Middle);
};


/*************************************
 *               Scroll              * 
 *************************************/


UCLASS()
class ROBOTKEYWORDS_API UScrollElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FString GetName() override {
        return "Scroll on Element";
    }

    virtual TArray<FString> GetArguments() override;

    virtual TMap<FString, FString> GetArgumentTypes() override;

    virtual void PerformAction(TSharedRef<IDriverElement> Element, const TArray<FString>& OtherArgs) override;
};


UCLASS()
class UScrollToBeginElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    SIMPLE_INPUT_KEYWORD_BODY("Scroll to beginning on Element", ScrollToBeginning)
};


UCLASS()
class UScrollToEndElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    SIMPLE_INPUT_KEYWORD_BODY("Scroll to end on Element", ScrollToEnd)
};
