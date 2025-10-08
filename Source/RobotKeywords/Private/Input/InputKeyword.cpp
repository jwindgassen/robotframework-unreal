#include "Input/InputKeyword.h"

#include "IAutomationDriverModule.h"
#include "IAutomationDriver.h"
#include "IDriverElement.h"
#include "LocateBy.h"
#include "RobotKeywords.h"


FKeywordResponse UInputKeyword::Execute() {
    // Create the Driver and Locator
    const TSharedRef<IAutomationDriver> Driver = CreateDriver();
    const TSharedRef<IElementLocator> ElementLocator = GetElementLocator();

    // Locate the element
    const TSharedRef<IDriverElement> Element = Driver->FindElement(ElementLocator);

    if (!Element->Exists()) {
        UE_LOG(LogRobotKeywords, Error, TEXT("Could not locate Element '%s'"), *Locator)
        return Error("Could not locate Element");
    }

    // Perform an action on the element
    FKeywordResponse Response = PerformAction(Element);

    CleanupDriver();

    return Response;
}

TSharedRef<IElementLocator> UInputKeyword::GetElementLocator() {
    return By::Path(Locator);
}

TSharedRef<IAutomationDriver> UInputKeyword::CreateDriver() {
    // Module initialization and driver creation must be executed on the GameThread.
    // Since this is executed on the ThreadPool, we can just wait for it to finish
    return Async(EAsyncExecution::TaskGraphMainThread, []() {
        // Enable simulated input
        IAutomationDriverModule& AutomationDriverModule = IAutomationDriverModule::Get();
        AutomationDriverModule.Enable();

        // Create the Driver and Locate the Element
        return AutomationDriverModule.CreateDriver();
    }).Get();
}

void UInputKeyword::CleanupDriver() {
    Async(EAsyncExecution::TaskGraphMainThread, []() {
        // Disable simulated input
        IAutomationDriverModule::Get().Disable();
    }).Wait();
}
