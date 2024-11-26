#pragma once

#include <variant>
#include "CoreMinimal.h"
#include "HttpRouteHandle.h"
#include "XmlRpcServer.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogXmlRpcServer, Log, Log)


/**
 * Schema Definition: https://xmlrpc.com/spec.md
 *
 * Represents any valid value an RPC parameter / return type can have.
 * Unfortunately we can't use TVariant, as it is final, and we need to inherit because we need recursive types in it
 */
struct ROBOTINTEGRATION_API FRpcValue :
    std::variant<
        int32, bool, FString, double,        /* Simple scalar types */
        FDateTime, TArray<uint8>,            /* DateTime and Base64 binary data */
        TArray<TSharedPtr<FRpcValue>>,       /* XML-RPC Arrays */
        TMap<FString, TSharedPtr<FRpcValue>> /* XML-RPC Structs */
        > {
    using FBase = std::variant<
        int32, bool, FString, double, FDateTime, TArray<uint8>, TArray<TSharedPtr<FRpcValue>>,
        TMap<FString, TSharedPtr<FRpcValue>>>;
    using FBase::FBase;
    using FBase::operator=;
    using FBase::index;
};

struct ROBOTINTEGRATION_API FRpcMethodResponse :
    std::variant<
        TSharedPtr<FRpcValue>, /* Success */
        TTuple<int32, FString> /* Fault */
        > {
    using FBase = std::variant<TSharedPtr<FRpcValue>, TTuple<int32, FString>>;
    using FBase::FBase;
    using FBase::operator=;
    using FBase::index;
};

using FRemoteProcedure = TFunction<FRpcMethodResponse(const TArray<TSharedPtr<FRpcValue>>& Arguments)>;


USTRUCT()
struct ROBOTINTEGRATION_API FXmlRpcServer {
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

    TArray<TSharedPtr<FRpcValue>> ParseArguments(const class FXmlNode* Params);

    TSharedPtr<FRpcValue> ParseValue(const class FXmlNode* ValueNode);

    void BuildValueNode(FStringBuilderBase& Builder, const TSharedPtr<FRpcValue>& Value);

    FString BuildXmlResponse(FRpcMethodResponse Response);
};
