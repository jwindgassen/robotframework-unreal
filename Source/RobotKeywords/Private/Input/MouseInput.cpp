#include "Input/MouseInput.h"
#include "IDriverElement.h"


FKeywordResponse UHoverElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    Element->Hover();
    return Success();
}

FKeywordResponse UClickElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    Element->Click();
    return Success();
}

FKeywordResponse UDoubleClickElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    Element->DoubleClick();
    return Success();
}

FKeywordResponse URightClickElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    Element->Click(EMouseButtons::Right);
    return Success();
}

FKeywordResponse UMiddleClickElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    Element->Click(EMouseButtons::Middle);
    return Success();
}

FKeywordResponse UScrollOnElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    Element->ScrollBy(Delta);
    return Success();
}

FKeywordResponse UScrollToBeginOfElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    Element->ScrollToBeginning();
    return Success();
}

FKeywordResponse UScrollToEndOfElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    Element->ScrollToEnd();
    return Success();
}
