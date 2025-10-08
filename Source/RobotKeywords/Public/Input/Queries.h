#pragma once

#include "InputKeyword.h"
#include "Queries.generated.h"


/// Returns whether the Element can be focused
UCLASS(DisplayName = "Element can be focused")
class ROBOTKEYWORDS_API UElementCanBeFocusedKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Returns whether the Element is currently focused
UCLASS(DisplayName = "Element is focused")
class ROBOTKEYWORDS_API UElementIsFocusedKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Returns whether the Element exists
UCLASS(DisplayName = "Element exists")
class ROBOTKEYWORDS_API UElementExistsKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Returns whether the Element is visible
UCLASS(DisplayName = "Element is visible")
class ROBOTKEYWORDS_API UElementIsVisibleKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Returns whether the Element is checked
UCLASS(DisplayName = "Element is checked")
class ROBOTKEYWORDS_API UElementIsCheckedKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Returns whether the Element is interactable
UCLASS(DisplayName = "Element is interactable")
class ROBOTKEYWORDS_API UElementIsInteractableKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Returns whether the Element is scrollable
UCLASS(DisplayName = "Element is scrollable")
class ROBOTKEYWORDS_API UElementIsScrollableKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};
