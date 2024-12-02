#include "Input/InputKeyword.h"

#include "IAutomationDriver.h"
#include "IAutomationDriverModule.h"
#include "IDriverElement.h"
#include "LocateBy.h"


FKeywordResponse UInputKeyword::Execute() {
    const auto Driver = CreateDriver();

    const auto ElementLocator = GetElementLocator();
    const auto Element = Driver->FindElement(ElementLocator);

    if (!Element->Exists()) {
        UE_LOG(LogTemp, Error, TEXT("Could not locate Element '%s'"), *Locator)
        return Error("Could not locate Element");
    }

    return PerformAction(Element);
}

TSharedRef<IElementLocator> UInputKeyword::GetElementLocator() {
    return By::Path(Locator);
}

TSharedRef<IAutomationDriver> UInputKeyword::CreateDriver() {
    IAutomationDriverModule::Get().Enable();

    return IAutomationDriverModule::Get().CreateDriver();
}
