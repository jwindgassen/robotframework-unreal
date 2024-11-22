#include "RobotIntegration.h"
#include "Router.h"


IMPLEMENT_MODULE(FRobotIntegrationModule, RobotIntegration)


void FRobotIntegrationModule::StartupModule() {
    int32 Port;
    if (!FParse::Value(FCommandLine::Get(), TEXT("-robot-server-port="), Port)) {
        // Random open port
        Port = FMath::RandRange(1025, 65535);
    }
    UE_LOG(LogTemp, Log, TEXT("Starting listening for HTTP Requests on %d"), Port)
    
    RegisterRoutes(Port);
}

void FRobotIntegrationModule::ShutdownModule() {
    UnregisterRoutes();
}
