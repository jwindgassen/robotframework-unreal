#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FRobotIntegrationEditorExtensionModule : public IModuleInterface {
    TSharedPtr<class FWidgetMetadataExtender> MetadataExtender;
    
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    virtual bool SupportsDynamicReloading() override {
        return false;
    }
};
