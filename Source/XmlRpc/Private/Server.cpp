﻿#include "Server.h"
#include "HttpServerModule.h"
#include "XmlFile.h"
#include "Runtime/Online/HTTPServer/Public/IHttpRouter.h"


DEFINE_LOG_CATEGORY(LogXmlRpcServer)


FXmlRpcServer::FXmlRpcServer(const int32 Port, const FString& Path) {
    FHttpServerModule& HttpServerModule = FHttpServerModule::Get();

    Router = HttpServerModule.GetHttpRouter(Port);
    RouteHandle = Router->BindRoute(
        Path, EHttpServerRequestVerbs::VERB_POST,
        [this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) -> bool {
            return this->ProcessHttpRequest(Request, OnComplete);
        }
    );

    HttpServerModule.StartAllListeners();
}

FXmlRpcServer::~FXmlRpcServer() {
    Stop();
}

void FXmlRpcServer::Stop() const {
    if (RouteHandle.IsValid()) {
        Router->UnbindRoute(RouteHandle);
    }
}

bool FXmlRpcServer::ProcessHttpRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) {
    // Convert body to text
    const FString XmlText = BytesToString(Request.Body.GetData(), Request.Body.Num());
    UE_LOG(LogXmlRpcServer, Verbose, TEXT("Received XML-RPC: '%s'"), *XmlText)

    // Parse XML from body
    const FXmlFile XmlFile{XmlText, EConstructMethod::ConstructFromBuffer};
    if (!XmlFile.IsValid()) {
        UE_LOG(LogXmlRpcServer, Error, TEXT("Could not parse XML from Request Body: %s"), *XmlFile.GetLastError())

        OnComplete(FHttpServerResponse::Error(EHttpServerResponseCodes::BadRequest, TEXT(""), TEXT("Invalid XML")));
        return false;
    }

    const FXmlNode* MethodCall = XmlFile.GetRootNode()->FindChildNode("methodCall");
    const FString ProcedureName = MethodCall->FindChildNode("methodName")->GetContent();
    const FRemoteProcedure* Procedure = Procedures.Find(ProcedureName);

    FRpcMethodResponse Result;
    if (Procedure) {
        const TArray<TSharedPtr<FRpcValue>> Arguments = ParseArguments(MethodCall->FindChildNode("params"));
        Result = (*Procedure)(Arguments);
    } else {
        UE_LOG(LogXmlRpcServer, Error, TEXT("Unknown Procedure: '%s'"), *ProcedureName)
        Result = TTuple<int32, FString>{400, FString::Printf(TEXT("Unknown Procedure: '%s'"), *ProcedureName)};
    }

    const FString ResponseXml = BuildXmlResponse(Result);
    UE_LOG(LogXmlRpcServer, Verbose, TEXT("Response XML: '%s'"), *ResponseXml)
    
    OnComplete(FHttpServerResponse::Create(ResponseXml, "text/xml"));
    return true;
}

TArray<TSharedPtr<FRpcValue>> FXmlRpcServer::ParseArguments(const FXmlNode* Params) {
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

void FXmlRpcServer::BuildValueNode(FStringBuilderBase& Builder, const TSharedPtr<FRpcValue>& Value) {
    if (!Value.IsValid()) {
        return;
    }
    
    Builder.Append("<value>");
    
    switch (Value->index()) {
        case 0: Builder.Appendf(TEXT("<i4>%d</i4>"), std::get<int32>(*Value)); break;
        case 1: Builder.Appendf(TEXT("<bool>%s</bool>"), std::get<bool>(*Value) ? TEXT("1") : TEXT("0")); break;
        case 2: Builder.Appendf(TEXT("<string>%f</string>"), *std::get<FString>(*Value)); break;
        case 3: Builder.Appendf(TEXT("<double>%s</double>"), std::get<double>(*Value)); break;
        case 4: {
            Builder.Appendf(TEXT("<dateTime.iso8601>%s</dateTime.iso8601>"), *std::get<FDateTime>(*Value).ToIso8601());
            break;
        }
        case 5: {
            Builder.Appendf(TEXT("<base64>%s<base64>"), *FBase64::Encode(std::get<TArray<uint8>>(*Value)));
            break;
        }
        case 6: {
            Builder.Append("<array><data>");
            for (const auto& Val : std::get<TArray<TSharedPtr<FRpcValue>>>(*Value)) {
                BuildValueNode(Builder, Val);
            }
            Builder.Append("</data></array>");
            
            break;
        }
        case 7: {
            Builder.Append("<struct>");
            for (const auto& [Name, Val] : std::get<TMap<FString, TSharedPtr<FRpcValue>>>(*Value)) {
                Builder.Appendf(TEXT("<member><name>%s</name>"), *Name);
                BuildValueNode(Builder, Val);
                Builder.Append("</member>");
            }
        }
        default: {
            UE_LOG(LogXmlRpcServer, Error, TEXT("Unknown FRpcValue Index"))
        }
    }

    Builder.Append("</value>");
}

FString FXmlRpcServer::BuildXmlResponse(FRpcMethodResponse Response) {
    FStringBuilderBase ResponseXml;
    ResponseXml.Append("<?xml version=\"1.0\"?>");
    ResponseXml.Append("<methodResponse>");

    if (std::holds_alternative<TSharedPtr<FRpcValue>>(Response)) {
        const TSharedPtr<FRpcValue> Value = std::get<0>(Response);
        
        ResponseXml.Append("<params><param>");
        BuildValueNode(ResponseXml, Value);
        ResponseXml.Append("</param></params>");
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
