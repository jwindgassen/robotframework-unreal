#pragma once

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
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Clicks on the Element
UCLASS(DisplayName = "Click Element")
class ROBOTKEYWORDS_API UClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Double-clicks on the Element
UCLASS(DisplayName = "Double-click Element")
class ROBOTKEYWORDS_API UDoubleClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Right-clicks on the Element
UCLASS(DisplayName = "Right-click Element")
class ROBOTKEYWORDS_API URightClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Middle-clicks on the Element
UCLASS(DisplayName = "Middle-click Element")
class ROBOTKEYWORDS_API UMiddleClickElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
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

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Scrolls to the beginning the Element
UCLASS(DisplayName = "Scroll to Beginning of Element")
class UScrollToBeginOfElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Scrolls to the end the Element
UCLASS(DisplayName = "Scroll to End of Element")
class UScrollToEndOfElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};
