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
    
    FRpcMethodResponse Result;
    const FRemoteProcedure* Procedure = Procedures.Find(ProcedureName);
    if (Procedure) {
        const TArray<TSharedPtr<FRpcValue>> Arguments = ParseArguments(XmlFile.GetRootNode()->FindChildNode("params"));
        Result = (*Procedure)(Arguments);
    } else {
        UE_LOG(LogXmlRpcServer, Error, TEXT("Unknown Procedure: '%s'"), *ProcedureName)
        Result = TTuple<int32, FString>{400, FString::Printf(TEXT("Unknown Procedure: '%s'"), *ProcedureName)};
    }

    const FString ResponseXml = BuildXmlResponse(Result);
    UE_LOG(LogXmlRpcServer, Verbose, TEXT("Response XML: '%s'"), *ResponseXml)

    UE_LOG(LogXmlRpcServer, Log, TEXT("Finishing XmlRpc Request"))
    OnComplete(FHttpServerResponse::Create(ResponseXml, "text/xml"));
    return true;
}

TArray<TSharedPtr<FRpcValue>> FXmlRpcServer::ParseArguments(const FXmlNode* Params) {
    if (!Params) {
        return {};
    }
    
    TArray<TSharedPtr<FRpcValue>> Arguments;
    for (const FXmlNode* Param : Params->GetChildrenNodes()) {
        const FXmlNode* Value = Param->GetFirstChildNode();
        Arguments.Push(ParseValue(Value));
    }

    return Arguments;
}

TSharedPtr<FRpcValue> FXmlRpcServer::ParseValue(const FXmlNode* ValueNode) {
    // If no type is defined, this is a String
    if (!ValueNode->GetFirstChildNode()) {
        return MakeShared<FRpcValue>(ValueNode->GetContent());
    }

    const FString& Type = ValueNode->GetFirstChildNode()->GetTag();
    const FString& Content = ValueNode->GetFirstChildNode()->GetContent();
    
    if (Type == "i4") {
        return MakeShared<FRpcValue>(FPlatformString::Atoi(*Content));
    }
    if (Type == "bool") {
        return MakeShared<FRpcValue>(Content == "1");
    }
    if (Type == "string") {
        return MakeShared<FRpcValue>(Content);
    }
    if (Type == "double") {
        return MakeShared<FRpcValue>(FPlatformString::Atod(*Content));
    }
    if (Type == "dateTime.iso8601") {
        FDateTime DateTime;
        if (!FDateTime::ParseIso8601(*Content, DateTime)) {
            UE_LOG(LogXmlRpcServer, Error, TEXT("Could not parse ISO-8601 timestamp"))
        }
        return MakeShared<FRpcValue>(DateTime);
    }
    if (Type == "base64") {
        TArray<uint8> Base64;
        if (!FBase64::Decode(Content, Base64)) {
            UE_LOG(LogXmlRpcServer, Error, TEXT("Could not decode Base64 String"))
        }
        return MakeShared<FRpcValue>(Base64);
    }
    if (Type == "array") {
        TArray<TSharedPtr<FRpcValue>> Array;
        for (const FXmlNode* Node : ValueNode->GetFirstChildNode()->GetFirstChildNode()->GetChildrenNodes()) {
            Array.Push(ParseValue(Node));
        }
        return MakeShared<FRpcValue>(Array);
    }
    if (Type == "struct") {
        TMap<FString, TSharedPtr<FRpcValue>> Struct;
        for (const FXmlNode* MemberNode : ValueNode->GetFirstChildNode()->GetChildrenNodes()) {
            Struct.Emplace(
                MemberNode->FindChildNode("name")->GetContent(), ParseValue(MemberNode->FindChildNode("value"))
            );
        }
        return MakeShared<FRpcValue>(Struct);
    }
    
    UE_LOG(LogXmlRpcServer, Error, TEXT("Unknown Value Type: '%s'"), *Type)
    return nullptr;
}

FString FXmlRpcServer::BuildXmlResponse(FRpcMethodResponse Response) {
    FStringBuilderBase ResponseXml;
    ResponseXml.Append("<?xml version=\"1.0\"?>");
    ResponseXml.Append("<methodResponse>");

    if (std::holds_alternative<TSharedPtr<FRpcValue>>(Response)) {
        ResponseXml.Appendf(TEXT("<params><param>%s</param></params>"), *std::get<0>(Response)->ParseToXmlString());
    } else {
        const TTuple<int32, FString> Fault = std::get<1>(Response);
        
        ResponseXml.Append("<fault><value><struct>");
        ResponseXml.Appendf(
            TEXT("<member><name>faultCode</name><value><i4>%d</i4></value></member>"), Fault.Key
        );
        ResponseXml.Appendf(
            TEXT("<member><name>faultString</name><value><string>%s</string></value></member>"), *Fault.Value
        );
        ResponseXml.Append("</struct></value></fault>");
    }

    ResponseXml.Append("</methodResponse>");
    return ResponseXml.ToString();
}
