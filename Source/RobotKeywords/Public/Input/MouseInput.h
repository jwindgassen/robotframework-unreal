#pragma once

#include "CoreMinimal.h"
#include "IDriverElement.h"
#include "InputKeyword.h"
#include "MouseInput.generated.h"


/*************************************
 *               Click               *
 *************************************/


/// Hovers the mouse over the Element
UCLASS(DisplayName = "Hover Element")
class ROBOTKEYWORDS_API UHoverElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->Hover();
        return Success();
    }
};


/// Clicks on the Element
UCLASS(DisplayName = "Click Element")
class ROBOTKEYWORDS_API UClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->Click();
        return Success();
    }
};


/// Double-clicks on the Element
UCLASS(DisplayName = "Double-click Element")
class ROBOTKEYWORDS_API UDoubleClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->DoubleClick();
        return Success();
    }
};


/// Right-clicks on the Element
UCLASS(DisplayName = "Right-click Element")
class ROBOTKEYWORDS_API URightClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->Click(EMouseButtons::Right);
        return Success();
    }
};


/// Middle-clicks on the Element
UCLASS(DisplayName = "Middle-click Element")
class ROBOTKEYWORDS_API UMiddleClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->Click(EMouseButtons::Middle);
        return Success();
    }
};


/*************************************
 *               Scroll              *
 *************************************/


/// Scrolls the given Delta on the Element
UCLASS(DisplayName = "Scroll on Element")
class ROBOTKEYWORDS_API UScrollOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (KeywordArgument))
    double Delta = 2.5;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->ScrollBy(Delta);
        return Success();
    }
};


/// Scrolls to the beginning the Element
UCLASS(DisplayName = "Scroll to Beginning of Element")
class UScrollToBeginOfElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->ScrollToBeginning();
        return Success();
    }
};


/// Scrolls to the end the Element
UCLASS(DisplayName = "Scroll to End of Element")
class UScrollToEndOfElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->ScrollToEnd();
        return Success();
    }
};
