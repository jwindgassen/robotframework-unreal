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

    Server.RegisterProcedure(
        "get_keyword_arguments",
        [](const TArray<TSharedPtr<FRpcValue>>& Arguments) -> FRpcMethodResponse {
            checkf(
                Arguments.Num() == 1 && std::holds_alternative<FString>(*Arguments[0]),
                TEXT("'get_keywords_arguments' expects exactly 1 String as Argument")
            )
            const FString& Keyword = std::get<FString>(*Arguments[0]);

            TArray<TSharedPtr<FRpcValue>> Response;
            for (const FString& Arg : FKeywordManager::Get().KeywordArguments(Keyword)) {
                Response.Add(MakeShared<FRpcValue>(Arg));
            }
            return MakeShared<FRpcValue>(Response);
        }
    );

    Server.RegisterProcedure(
        "get_keyword_types",
        [](const TArray<TSharedPtr<FRpcValue>>& Arguments) -> FRpcMethodResponse {
            checkf(
                Arguments.Num() == 1 && std::holds_alternative<FString>(*Arguments[0]),
                TEXT("'get_keyword_types' expects exactly 1 String as Argument")
            )
            const FString& Keyword = std::get<FString>(*Arguments[0]);

            TArray<TSharedPtr<FRpcValue>> Response;
            for (const auto& Type : FKeywordManager::Get().KeywordTypes(Keyword)) {
                Response.Add(MakeShared<FRpcValue>(Type));
            }
            return MakeShared<FRpcValue>(Response);
        }
    );

    Server.RegisterProcedure(
        "run_keyword",
        [](const TArray<TSharedPtr<FRpcValue>>& Arguments) -> FRpcMethodResponse {
            checkf(
                Arguments.Num() >= 1 && std::holds_alternative<FString>(*Arguments[0]),
                TEXT("'run_keyword' need at least 1 String as Argument")
            )
            
            return FKeywordManager::Get().Execute(
                std::get<FString>(*Arguments[0]),
                Arguments.Num() == 2 ? std::get<TArray<TSharedPtr<FRpcValue>>>(*Arguments[1])
                                     : TArray<TSharedPtr<FRpcValue>>{}
            );
        }
    );
}

void FRobotIntegrationModule::ShutdownModule() {
    Server.Stop();
}
