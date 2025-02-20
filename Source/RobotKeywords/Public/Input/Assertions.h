#pragma once

#include "IDriverElement.h"
#include "InputKeyword.h"
#include "Assertions.generated.h"


UCLASS()
class ROBOTKEYWORDS_API UElementCanBeFocusedKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool CanFocus = Element->CanFocus();
        return Success(CanFocus);
    }
};


UCLASS()
class ROBOTKEYWORDS_API UElementIsFocusedKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool IsFocused = Element->IsFocused();
        return Success(IsFocused);
    }
};


UCLASS()
class ROBOTKEYWORDS_API UElementExistsKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool Exists = Element->Exists();
        return Success(Exists);
    }
};


UCLASS()
class ROBOTKEYWORDS_API UElementIsVisibleKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool Visible = Element->IsVisible();
        return Success(Visible);
    }
};


UCLASS()
class ROBOTKEYWORDS_API UElementIsCheckedKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool Checked = Element->IsChecked();
        return Success(Checked);
    }
};


UCLASS()
class ROBOTKEYWORDS_API UElementIsInteractableKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool Interactable = Element->IsInteractable();
        return Success(Interactable);
    }
};


UCLASS()
class ROBOTKEYWORDS_API UElementIsScrollableKeyword : public UInputKeyword {
    GENERATED_BODY()

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override {
        const bool Scrollable = Element->IsScrollable();
        return Success(Scrollable);
    }
};
