#include "XmlRpcServer.h"
#include "HttpServerModule.h"
#include "XmlFile.h"
#include "IHttpRouter.h"


DEFINE_LOG_CATEGORY(LogXmlRpcServer)


bool FXmlRpcServer::Start(const FString& Path, const int32 Port) {
    UE_LOG(LogXmlRpcServer, Log, TEXT("Starting XmlRpcServer"))

    if (RouteHandle) {
        UE_LOG(LogXmlRpcServer, Warning, TEXT("XmlRpcServer already running"))
        return true;
    }

    FHttpServerModule& HttpServerModule = FHttpServerModule::Get();

    Router = HttpServerModule.GetHttpRouter(Port);
    if (!Router) {
        UE_LOG(LogXmlRpcServer, Error, TEXT("Could not create HTTP Router on Port %d"), Port)
        return false;
    }

    RouteHandle = Router->BindRoute(
        Path, EHttpServerRequestVerbs::VERB_POST,
        [this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) -> bool {
            return this->ProcessHttpRequest(Request, OnComplete);
        }
    );
    UE_LOG(LogXmlRpcServer, Log, TEXT("XmlRpc Route: %s"), *RouteHandle->Path)

    HttpServerModule.StartAllListeners();
    return true;
}

void FXmlRpcServer::RegisterProcedure(FString Name, FRemoteProcedure Procedure) {
    Procedures.Emplace(Name, Procedure);
}

void FXmlRpcServer::Stop() const {
    UE_LOG(LogXmlRpcServer, Log, TEXT("Stopping XmlRpcServer"))

    if (RouteHandle.IsValid()) {
        Router->UnbindRoute(RouteHandle);
    }
}

bool FXmlRpcServer::ProcessHttpRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) {
    // Convert body to text
    TArray RequestBody{Request.Body, 1};
    RequestBody.Add(0);  // Add Null-Terminator
    const FString XmlText = UTF8_TO_TCHAR(RequestBody.GetData());
    UE_LOG(LogXmlRpcServer, Verbose, TEXT("Received XML-RPC: '%s'"), *XmlText)

    // Parse XML from body
    const FXmlFile XmlFile{XmlText, EConstructMethod::ConstructFromBuffer};
    if (!XmlFile.IsValid()) {
        UE_LOG(LogXmlRpcServer, Error, TEXT("Could not parse XML from Request Body: %s"), *XmlFile.GetLastError())

        OnComplete(FHttpServerResponse::Error(EHttpServerResponseCodes::BadRequest, TEXT(""), TEXT("Invalid XML")));
        return false;
    }

    const FString ProcedureName = XmlFile.GetRootNode()->FindChildNode("methodName")->GetContent();
    UE_LOG(LogXmlRpcServer, Log, TEXT("Trying to find remote procedure '%s'"), *ProcedureName)
    
    const FRemoteProcedure* Procedure = Procedures.Find(ProcedureName);
    const TArray<TSharedPtr<FRpcValue>> Arguments = ParseArguments(XmlFile.GetRootNode()->FindChildNode("params"));
    TFuture<TSharedPtr<FRpcMethodResponse>> Result = ExecuteProcedure(Procedure, Arguments);

    // Finish the Request when the future is fulfilled
    Result.Next([OnComplete](const TSharedPtr<FRpcMethodResponse>& Response) {
        const FString ResponseXml = Response->ParseToXmlString();
        UE_LOG(LogXmlRpcServer, Verbose, TEXT("Response XML: '%s'"), *ResponseXml)

        UE_LOG(LogXmlRpcServer, Log, TEXT("Finishing XmlRpc Request"))
        OnComplete(FHttpServerResponse::Create(ResponseXml, "text/xml")); 
    });
    return true;
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
