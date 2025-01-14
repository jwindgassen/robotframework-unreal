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
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->Hover();
        return Success();
    }
};


UCLASS()
class ROBOTKEYWORDS_API UClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->Click();
        return Success();
    }
};


UCLASS()
class ROBOTKEYWORDS_API UDoubleClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->DoubleClick();
        return Success();
    }
};


UCLASS()
class ROBOTKEYWORDS_API URightClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->Click(EMouseButtons::Right);
        return Success();
    }
};


UCLASS()
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


UCLASS()
class ROBOTKEYWORDS_API UScrollOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY()
    double Delta = 2.5;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->ScrollBy(Delta);
        return Success();
    }
};


UCLASS()
class UScrollToBeginOfElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->ScrollToBeginning();
        return Success();
    }
};


UCLASS()
class UScrollToEndOfElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        Element->ScrollToEnd();
        return Success();
    }
};
