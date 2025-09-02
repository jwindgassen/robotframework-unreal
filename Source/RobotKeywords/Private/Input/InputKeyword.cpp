#include "Input/InputKeyword.h"

#include "AutomatedApplication.h"
#include "AutomationDriver.h"
#include "IAutomationDriver.h"
#include "IAutomationDriverModule.h"
#include "IDriverElement.h"
#include "LocateBy.h"
#include "PassThroughMessageHandler.h"
#include "RobotKeywords.h"


// The Application the AutomationDriver will run on
static TSharedPtr<GenericApplication> RealApplication;

// Wrapper of RealApplication for forwarding input
static TSharedPtr<FAutomatedApplication> AutomatedApplication;


FKeywordResponse UInputKeyword::Execute() {
    const auto Driver = CreateDriver();

    const auto ElementLocator = GetElementLocator();
    const auto Element = Driver->FindElement(ElementLocator);

    if (!Element->Exists()) {
        UE_LOG(LogRobotKeywords, Error, TEXT("Could not locate Element '%s'"), *Locator)
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
        // SlateApplication must be retrieved on the GameThread, so we will query it and wait
        // We run on in the ThreadPool, so waiting should not be a problem
        TPromise<FSlateApplication&> SlateApplicationFuture;
        AsyncTask(ENamedThreads::GameThread, [&SlateApplicationFuture]() {
            SlateApplicationFuture.SetValue(FSlateApplication::Get());
        });
        
        FSlateApplication& SlateApplication = SlateApplicationFuture.GetFuture().Get();
        RealApplication = SlateApplication.GetPlatformApplication();
        AutomatedApplication = FAutomatedApplicationFactory::Create(
            RealApplication.ToSharedRef(), FPassThroughMessageHandlerFactoryFactory::Create()
        );

        if (AutomatedApplication.IsValid()) {
            SlateApplication.SetPlatformApplication(AutomatedApplication.ToSharedRef());
            AutomatedApplication->AllowPlatformMessageHandling();
        }
    }

    return FAutomationDriverFactory::Create(AutomatedApplication.ToSharedRef());
}
