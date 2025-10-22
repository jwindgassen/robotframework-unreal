#include "RobotIntegration.h"

#include "KeywordManager.h"
#include "XmlRpcServer.h"


IMPLEMENT_MODULE(FRobotIntegrationModule, RobotIntegration)


void FRobotIntegrationModule::StartupModule() {
    // Do not start an HTTP Server while Packaging
    if (IsRunningCommandlet()) {
        UE_LOG(LogTemp, Warning, TEXT("Detected running Commandlet. RobotIntegration RPC Server will not be started"))
        return;
    }
    
    Server.Start();

    Server.RegisterProcedure("get_library_information", [](const TArray<TSharedPtr<FRpcValue>>&) {
        const auto Information = FKeywordManager::Get().LibraryInformation();
        return MakeShared<FRpcMethodResponse>(Information);
    });

    Server.RegisterProcedure(
        "run_keyword",
        [](const TArray<TSharedPtr<FRpcValue>>& Arguments) {
            checkf(
                Arguments.Num() >= 1 && Arguments[0]->IsString(),
                TEXT("'run_keyword' need at least 1 String as Argument")
            )

            const FString& KeywordName = Arguments[0]->GetString();
            const FRpcValueList Args = Arguments.Num() == 2 ? Arguments[1]->GetList() : FRpcValueList{};
            const auto Result = FKeywordManager::Get().Execute(KeywordName, Args);
            
            return MakeShared<FRpcMethodResponse>(Result);
        }
    );

    Server.RegisterProcedure(
        "stop_remote_server",
        [](const TArray<TSharedPtr<FRpcValue>>&) {
            // We first need to finish the XML-RPC Request, so we queue the exit after a 0.5s delay
            Async(EAsyncExecution::ThreadPool, []() {
                FPlatformProcess::Sleep(0.5f);
                FPlatformMisc::RequestExit(false);
            });
            
            return MakeShared<FRpcMethodResponse>(MakeShared<FRpcValue>(true));
        }
    );
}

void FRobotIntegrationModule::ShutdownModule() {
    Server.Stop();
}
