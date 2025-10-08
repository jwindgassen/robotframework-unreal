#include "Input/Queries.h"
#include "IDriverElement.h"


inline FKeywordResponse UElementCanBeFocusedKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    const bool CanFocus = Element->CanFocus();
    return Success(CanFocus);
}

FKeywordResponse UElementIsFocusedKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    const bool IsFocused = Element->IsFocused();
    return Success(IsFocused);
}

FKeywordResponse UElementExistsKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    const bool Exists = Element->Exists();
    return Success(Exists);
}

FKeywordResponse UElementIsVisibleKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    const bool Visible = Element->IsVisible();
    return Success(Visible);
}

FKeywordResponse UElementIsCheckedKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    const bool Checked = Element->IsChecked();
    return Success(Checked);
}

FKeywordResponse UElementIsInteractableKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    const bool Interactable = Element->IsInteractable();
    return Success(Interactable);
}

FKeywordResponse UElementIsScrollableKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    const bool Scrollable = Element->IsScrollable();
    return Success(Scrollable);
}
