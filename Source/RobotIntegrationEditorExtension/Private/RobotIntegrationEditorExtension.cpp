#include "RobotIntegrationEditorExtension.h"
#include "DetailWidgetRow.h"
#include "WidgetDetailCustomization.h"
#include "Components/Widget.h"

IMPLEMENT_MODULE(FRobotIntegrationEditorExtensionModule, RobotIntegrationEditorExtension)

void FRobotIntegrationEditorExtensionModule::StartupModule() {
    auto& PropertyModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    (void)PropertyModule;

    PropertyModule.RegisterCustomClassLayout(
        UWidget::StaticClass()->GetFName(),
        FOnGetDetailCustomizationInstance::CreateStatic(&FWidgetDetailCustomization::MakeInstance)
    );
    PropertyModule.NotifyCustomizationModuleChanged();
}

void FRobotIntegrationEditorExtensionModule::ShutdownModule() {
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor")) {
        FPropertyEditorModule& PropertyModule =
            FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyModule.UnregisterCustomClassLayout(AActor::StaticClass()->GetFName());
    }
}
