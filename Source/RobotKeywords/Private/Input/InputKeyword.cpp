#include "Input/InputKeyword.h"

#include "IAutomationDriver.h"
#include "IAutomationDriverModule.h"
#include "IDriverElement.h"
#include "LocateBy.h"


void UInputKeyword::Execute(const TArray<FString>& Arguments) {
    const auto Driver = CreateDriver();

    const FString& Path = Arguments[0];
    const auto Locator = GetLocator(Path);
    const auto Element = Driver->FindElement(Locator);

    if (!Element->Exists()) {
        UE_LOG(LogTemp, Error, TEXT("Could not find Element for Path '%s'"), *Path)
    }

    const TArray<FString> OtherArgs =
        Arguments.Num() > 1 ? TArray{&Arguments[1], Arguments.Num() - 1} : TArray<FString>{};
    PerformAction(Element, OtherArgs);
}

TSharedRef<IElementLocator> UInputKeyword::GetLocator(const FString& Path) {
    return By::Path(Path);
}

TSharedRef<IAutomationDriver> UInputKeyword::CreateDriver() {
    IAutomationDriverModule::Get().Enable();

    return IAutomationDriverModule::Get().CreateDriver();
}
