#pragma once

#include "CoreMinimal.h"
#include "HttpRouteHandle.h"
#include "RpcValue.h"
#include "XmlRpcServer.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogXmlRpcServer, Log, Log)


using FRemoteProcedure = TFunction<TSharedPtr<FRpcMethodResponse>(const TArray<TSharedPtr<FRpcValue>>& Arguments)>;


USTRUCT()
struct XMLRPC_API FXmlRpcServer {
    GENERATED_BODY()

private:
    // Underlying HTTP Router
    TSharedPtr<class IHttpRouter> Router;

    // Handle to the Route we are listening on
    FHttpRouteHandle RouteHandle;

    // Available Procedures to call. Key is the Name the Function was registered with
    TMap<FString, FRemoteProcedure> Procedures;

public:
    virtual ~FXmlRpcServer() {
        Stop();
    }

    bool Start(const FString& Path = "/rpc", int32 Port = 8270);

    void RegisterProcedure(FString Name, FRemoteProcedure Procedure);

    void Stop() const;

private:
    bool ProcessHttpRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

    TFuture<TSharedPtr<FRpcMethodResponse>> ExecuteProcedure(
        const FRemoteProcedure* Procedure, const TArray<TSharedPtr<FRpcValue>>& Arguments
    );

    TArray<TSharedPtr<FRpcValue>> ParseArguments(const class FXmlNode* Params);
};
