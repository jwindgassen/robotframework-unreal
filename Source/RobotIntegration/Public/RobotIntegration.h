#pragma once

#include "CoreMinimal.h"
#include "XmlRpcServer.h"


class FRobotIntegrationModule final : public IModuleInterface {
    FXmlRpcServer Server;
    
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
