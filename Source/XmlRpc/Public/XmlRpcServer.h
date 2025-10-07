#pragma once

#include "CoreMinimal.h"
#include "HttpRouteHandle.h"
#include "RpcValue.h"
#include "XmlRpcServer.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogXmlRpcServer, Log, Log)


using FRemoteProcedure = TFunction<TSharedPtr<FRpcMethodResponse>(const TArray<TSharedPtr<FRpcValue>>& Arguments)>;


/**
 * An XML-RPC Server accepting and processing XML-RPC requests via HTTP.
 *
 * Listens for HTTP requests on a port (default: 8270) on a given endpoint (default: /rpc).
 * Procedures can be registered to the server using a name and a callback accepting the arguments of the call.
 * Procedures are executed on a background thread (ThreadPool).
 *
 * Supported values:
 *   - Scalars: Integers, Doubles, Booleans, Strings, DateTime, Binary (Bytes)
 *   - Compound: Arrays and Structs (using TArray and TMap)
 *   - NIL (Not part of the official spec)
 *
 * @see https://xmlrpc.com/spec.md
 */
USTRUCT()
struct XMLRPC_API FXmlRpcServer {
    GENERATED_BODY()

private:
    /// Underlying HTTP Router
    TSharedPtr<class IHttpRouter> Router;

    /// Handle to the Route we are listening on
    FHttpRouteHandle RouteHandle;

    /// Available Procedures to call. Key is the Name the Function was registered with
    TMap<FString, FRemoteProcedure> Procedures;

public:
    virtual ~FXmlRpcServer() {
        Stop();
    }

    /**
     * Starts the underlying HTTP Server.
     *
     * @param Path The HTTP Path where RPCs can be received
     * @param Port The Port where the HTTP Server will listen on
     * @return Whether the HTTP Server started successfully
     */
    bool Start(const FString& Path = "/rpc", int32 Port = 8270);

    /**
     * Registers a new remote procedure. Registering a procedure again will override the previous one.
     *
     * @param Name The name of the procedure
     * @param Procedure A callback for processing the RPC, passing in the arguments from the received RPC and returning
     *   the response (A value on success or an error code and message on fault)
     */
    void RegisterProcedure(FString Name, FRemoteProcedure Procedure);

    /// Stops the underlying HTTP Server.
    void Stop() const;

private:
    /**
     * Parses the HTTP Request into XML, calls the corresponding procedure (if there is one for this request)
     * and completes the request with the generated XML response.
     */
    bool ProcessHttpRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

    /// Executes a remote procedure on a background tread
    TFuture<TSharedPtr<FRpcMethodResponse>> ExecuteProcedure(
        const FRemoteProcedure* Procedure, const TArray<TSharedPtr<FRpcValue>>& Arguments
    );

    /// Parses the arguments of the RPC from XML
    TArray<TSharedPtr<FRpcValue>> ParseArguments(const class FXmlNode* Params);
};
