#pragma once

#include "IDriverElement.h"
#include "InputKeyword.h"
#include "Assertions.generated.h"


/// Returns whether the Element can be focused
UCLASS(DisplayName = "Element can be focused")
class ROBOTKEYWORDS_API UElementCanBeFocusedKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool CanFocus = Element->CanFocus();
        return Success(CanFocus);
    }
};


/// Returns whether the Element is currently focused
UCLASS(DisplayName = "Element is focused")
class ROBOTKEYWORDS_API UElementIsFocusedKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool IsFocused = Element->IsFocused();
        return Success(IsFocused);
    }
};


/// Returns whether the Element exists
UCLASS(DisplayName = "Element exists")
class ROBOTKEYWORDS_API UElementExistsKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool Exists = Element->Exists();
        return Success(Exists);
    }
};


/// Returns whether the Element is visible
UCLASS(DisplayName = "Element is visible")
class ROBOTKEYWORDS_API UElementIsVisibleKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool Visible = Element->IsVisible();
        return Success(Visible);
    }
};


/// Returns whether the Element is checked
UCLASS(DisplayName = "Element is checked")
class ROBOTKEYWORDS_API UElementIsCheckedKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool Checked = Element->IsChecked();
        return Success(Checked);
    }
};


/// Returns whether the Element is interactable
UCLASS(DisplayName = "Element is interactable")
class ROBOTKEYWORDS_API UElementIsInteractableKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool Interactable = Element->IsInteractable();
        return Success(Interactable);
    }
};


/// Returns whether the Element is scrollable
UCLASS(DisplayName = "Element is scrollable")
class ROBOTKEYWORDS_API UElementIsScrollableKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool Scrollable = Element->IsScrollable();
        return Success(Scrollable);
    }
};
