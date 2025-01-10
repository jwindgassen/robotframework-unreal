#include "RobotIntegration.h"

#include "KeywordManager.h"
#include "XmlRpcServer.h"


IMPLEMENT_MODULE(FRobotIntegrationModule, RobotIntegration)


void FRobotIntegrationModule::StartupModule() {
    Server.Start();

    Server.RegisterProcedure("get_library_information", [](const TArray<TSharedPtr<FRpcValue>>&) {
        return MakeShared<FRpcMethodResponse>(FKeywordManager::Get().LibraryInformation());
    });

    Server.RegisterProcedure(
        "run_keyword",
        [](const TArray<TSharedPtr<FRpcValue>>& Arguments) {
            checkf(
                Arguments.Num() >= 1 && Arguments[0]->IsString(),
                TEXT("'run_keyword' need at least 1 String as Argument")
            )
            
            return MakeShared<FRpcMethodResponse>(FKeywordManager::Get().Execute(
                Arguments[0]->GetString(),
                Arguments.Num() == 2 ? Arguments[1]->GetList() : TArray<TSharedPtr<FRpcValue>>{}
            ));
        }
    );
}

void FRobotIntegrationModule::ShutdownModule() {
    Server.Stop();
}
