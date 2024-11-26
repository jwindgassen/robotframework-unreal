#include "RobotIntegration.h"

#include "KeywordManager.h"
#include "XmlRpcServer.h"


IMPLEMENT_MODULE(FRobotIntegrationModule, RobotIntegration)


void FRobotIntegrationModule::StartupModule() {
    // int32 Port;
    // if (!FParse::Value(FCommandLine::Get(), TEXT("-robot-server-port="), Port)) {
    //     // Random open port
    //     Port = FMath::RandRange(1025, 65535);
    // }
    // UE_LOG(LogTemp, Log, TEXT("Starting listening for HTTP Requests on %d"), Port)
    
    Server.Start();

    Server.RegisterProcedure("get_keyword_names", [](const TArray<TSharedPtr<FRpcValue>>&) -> FRpcMethodResponse {
        const TArray<FString> Keywords = FKeywordManager::Get().KeywordNames();

        TArray<TSharedPtr<FRpcValue>> Response;
        for (const FString& Name : Keywords) {
            Response.Add(MakeShared<FRpcValue>(Name));
        }
        return MakeShared<FRpcValue>(Response);
    });
}

void FRobotIntegrationModule::ShutdownModule() {
    Server.Stop();
}
