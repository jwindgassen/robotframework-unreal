#include "XmlRpcServer.h"
#include "XmlFile.h"


DEFINE_LOG_CATEGORY(LogXmlRpcServer)


bool FXmlRpcServer::Start(const FString& Path, const int32 Port) {
    UE_LOG(LogXmlRpcServer, Log, TEXT("Starting XmlRpcServer"))

//     if (RouteHandle) {
//         UE_LOG(LogXmlRpcServer, Warning, TEXT("XmlRpcServer already running"))
//         return true;
//     }
//
//     FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
//
//     Router = HttpServerModule.GetHttpRouter(Port);
//     if (!Router) {
//         UE_LOG(LogXmlRpcServer, Error, TEXT("Could not create HTTP Router on Port %d"), Port)
//         return false;
//     }
//
//     RouteHandle = Router->BindRoute(
//         Path, EHttpServerRequestVerbs::VERB_POST,
// #if UE_VERSION_OLDER_THAN(5, 4, 0)
//         [this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) -> bool {
//             return this->ProcessHttpRequest(Request, OnComplete);
//         }
// #else
//         FHttpRequestHandler::CreateRaw(this, &FXmlRpcServer::ProcessHttpRequest)
// #endif
//     );
//     UE_LOG(LogXmlRpcServer, Log, TEXT("XmlRpc Route: %s"), *RouteHandle->Path)
//
//     HttpServerModule.StartAllListeners();
    Transport = MakeShared<FAsyncHttpTransport>(Path, Port);
    Transport->OnReceivedRequest.BindRaw(this, &FXmlRpcServer::ProcessHttpRequest);

    TransportThread = FRunnableThread::Create(Transport.Get(), TEXT("XML-RPC Server"));

    return true;
}

void FXmlRpcServer::RegisterProcedure(FString Name, FRemoteProcedure Procedure) {
    Procedures.Emplace(Name, Procedure);
}

void FXmlRpcServer::Stop() const {
    UE_LOG(LogXmlRpcServer, Log, TEXT("Stopping XmlRpcServer"))

    if (TransportThread) {
        TransportThread->Kill(false);
    }
    // if (RouteHandle.IsValid()) {
    //     Router->UnbindRoute(RouteHandle);
    // }
}

FResponse FXmlRpcServer::ProcessHttpRequest(const FRequest& Request) {
    UE_LOG(LogXmlRpcServer, Verbose, TEXT("Received XML-RPC: '%s'"), *Request.Body)

    // Parse XML from body
    const FXmlFile XmlFile{Request.Body, EConstructMethod::ConstructFromBuffer};
    if (!XmlFile.IsValid()) {
        UE_LOG(LogXmlRpcServer, Error, TEXT("Could not parse XML from Request Body: %s"), *XmlFile.GetLastError())

        FString Error = FString::Printf(TEXT("Invalid XML: %s"), *XmlFile.GetLastError());
        return FResponse(400, Error, "text/plain");
    }

    const FString ProcedureName = XmlFile.GetRootNode()->FindChildNode("methodName")->GetContent();
    UE_LOG(LogXmlRpcServer, Log, TEXT("Trying to find remote procedure '%s'"), *ProcedureName)
    
    const FRemoteProcedure* Procedure = Procedures.Find(ProcedureName);
    const TArray<TSharedPtr<FRpcValue>> Arguments = ParseArguments(XmlFile.GetRootNode()->FindChildNode("params"));
    TSharedPtr<FRpcMethodResponse> Result = ExecuteProcedure(Procedure, Arguments).Get();

    // Finish the Request when the future is fulfilled
    const FString ResponseXml = Result->ParseToXmlString();
    UE_LOG(LogXmlRpcServer, Verbose, TEXT("Response XML: '%s'"), *ResponseXml)

    UE_LOG(LogXmlRpcServer, Log, TEXT("Finishing XmlRpc Request"))
    return FResponse(200, ResponseXml, "text/xml");
}

TFuture<TSharedPtr<FRpcMethodResponse>> FXmlRpcServer::ExecuteProcedure(
    const FRemoteProcedure* Procedure, const TArray<TSharedPtr<FRpcValue>>& Arguments
) {
    if (!Procedure) {
        UE_LOG(LogXmlRpcServer, Error, TEXT("Unknown Procedure"))
        return MakeFulfilledPromise<TSharedPtr<FRpcMethodResponse>>(
            MakeShared<FRpcMethodResponse>(400, TEXT("Unknown Procedure"))
        ).GetFuture();
    }

    // Execute Procedure in ThreadPool
    return Async(EAsyncExecution::ThreadPool, [Procedure, Arguments]() {
        return (*Procedure)(Arguments);
    });
}

TArray<TSharedPtr<FRpcValue>> FXmlRpcServer::ParseArguments(const FXmlNode* Params) {
    if (!Params) {
        return {};
    }

    TArray<TSharedPtr<FRpcValue>> Arguments;
    for (const FXmlNode* Param : Params->GetChildrenNodes()) {
        const FXmlNode* Value = Param->GetFirstChildNode();
        Arguments.Push(FRpcValue::FromXml(Value));
    }

    return Arguments;
}
