#include "RobotIntegrationEditorExtension.h"
#include "UMGEditorModule.h"
#include "WidgetMetadataExtender.h"


IMPLEMENT_MODULE(FRobotIntegrationEditorExtensionModule, RobotIntegrationEditorExtension)


void FRobotIntegrationEditorExtensionModule::StartupModule() {
    IUMGEditorModule& EditorModule = FModuleManager::GetModuleChecked<IUMGEditorModule>("UMGEditor");

    MetadataExtender = MakeShared<FWidgetMetadataExtender>();
    EditorModule.AddWidgetCustomizationExtender(MetadataExtender.ToSharedRef());
}

void FRobotIntegrationEditorExtensionModule::ShutdownModule() {
    if (FModuleManager::Get().IsModuleLoaded("UMGEditor")) {
        IUMGEditorModule& EditorModule = FModuleManager::GetModuleChecked<IUMGEditorModule>("UMGEditor");
        EditorModule.RemoveWidgetCustomizationExtender(MetadataExtender.ToSharedRef());
    }
}
