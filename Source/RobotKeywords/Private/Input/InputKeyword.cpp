#include "Input/InputKeyword.h"

#include "AutomatedApplication.h"
#include "AutomationDriver.h"
#include "IAutomationDriver.h"
#include "IAutomationDriverModule.h"
#include "IDriverElement.h"
#include "LocateBy.h"
#include "PassThroughMessageHandler.h"


// The Application the AutomationDriver will run on
static TSharedPtr<GenericApplication> RealApplication;

// Wrapper of RealApplication for forwarding input
static TSharedPtr<FAutomatedApplication> AutomatedApplication;


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
    // We cannot use IAutomationDriverModule::Get(), so we do it manually
    // See `FAutomationDriverModule::Enable()` in `AutomationDriverModule.cpp`
    // ToDo: Also put Cleanup Code somewhere?
    if (!AutomatedApplication.IsValid()) {
        RealApplication = FSlateApplication::Get().GetPlatformApplication();
        AutomatedApplication = FAutomatedApplicationFactory::Create(
            RealApplication.ToSharedRef(), FPassThroughMessageHandlerFactoryFactory::Create()
        );

        if (AutomatedApplication.IsValid()) {
            FSlateApplication::Get().SetPlatformApplication(AutomatedApplication.ToSharedRef());
            AutomatedApplication->AllowPlatformMessageHandling();
        }
    }

    return FAutomationDriverFactory::Create(AutomatedApplication.ToSharedRef());
}
