#include "Router.h"

#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "JsonObjectConverter.h"
#include "KeywordManager.h"
#include "String/Join.h"


static TSharedPtr<IHttpRouter> Router;
static TArray<FHttpRouteHandle> RouteHandles = {};


#define BEGIN_ROUTE(Method, Path) \
    Handle = Router->BindRoute(                                                     \
    FHttpPath(TEXT(Path)),                                                          \
    EHttpServerRequestVerbs::VERB_##Method,                                         \
    [](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) {  \
        UE_LOG(LogTemp, Log, TEXT("Received " #Method " Request on '"  Path  "'"))    


#define END_ROUTE() \
        return true;          \
    });                       \
    RouteHandles.Add(Handle); \


#define QUERY_PARAMETER(Identifier, Name) \
    if (!Request.QueryParams.Contains(Name)) {                                                \
        UE_LOG(LogTemp, Error, TEXT("Could not get Query Parameter '" Name "'"))              \
        OnComplete(FHttpServerResponse::Error(                                                \
            EHttpServerResponseCodes::BadRequest, "400", "Missing Query Parameter '" Name "'" \
        ));                                                                                   \
    }                                                                                         \
    const FString& Identifier = *Request.QueryParams.Find(Name);


void RegisterRoutes(const uint32 Port) {
    FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
    
    Router = HttpServerModule.GetHttpRouter(Port);
    FHttpRouteHandle Handle;
    
    BEGIN_ROUTE(GET, "/keywords/names")
        const TArray<FString>& Names = FKeywordManager::Get().KeywordNames();
    
        const FString Content = FStringBuilderBase{}.Append("[").JoinQuoted(Names, ",", "\"").Append("]").ToString();
        OnComplete(FHttpServerResponse::Create(Content, "text/plain"));
    END_ROUTE()

    BEGIN_ROUTE(GET, "/keywords/arguments")
        QUERY_PARAMETER(Keyword, "keyword")
    
        if (!FKeywordManager::Get().HasKeyword(Keyword)) {
            OnComplete(FHttpServerResponse::Error(
                EHttpServerResponseCodes::BadRequest, "400", FString::Printf(TEXT("Unknown Keyword '%s'"), *Keyword)
            ));
        }

        const TArray<FString> Arguments = FKeywordManager::Get().KeywordArguments(Keyword);
        const FString Content = FStringBuilderBase{}.Append("[").JoinQuoted(Arguments, ",", "\"").Append("]").ToString();
        OnComplete(FHttpServerResponse::Create(Content, "text/plain"));
    END_ROUTE()

    BEGIN_ROUTE(GET, "/keywords/types")
        QUERY_PARAMETER(Keyword, "keyword")
        
        if (!FKeywordManager::Get().HasKeyword(Keyword)) {
            OnComplete(FHttpServerResponse::Error(
                EHttpServerResponseCodes::BadRequest, "400", FString::Printf(TEXT("Unknown Keyword '%s'"), *Keyword)
            ));
        }

        FStringBuilderBase Builder;
        Builder.Append("{");
        for (const auto& [Key, Value] : FKeywordManager::Get().KeywordTypes(Keyword)) {
            Builder.Appendf(TEXT("\"%s\": \"%s\","), *Key, *Value);
        }
        Builder.RemoveSuffix(1);  // Remove Trailing Comma
        Builder.Append("}");
    
        OnComplete(FHttpServerResponse::Create(Builder.ToString(), "text/plain"));
    END_ROUTE()

    BEGIN_ROUTE(POST, "/keywords/run")
        QUERY_PARAMETER(Keyword, "keyword")

        if (!FKeywordManager::Get().HasKeyword(Keyword)) {
            OnComplete(FHttpServerResponse::Error(
                EHttpServerResponseCodes::BadRequest, "400", FString::Printf(TEXT("Unknown Keyword '%s'"), *Keyword)
            ));
        }

        FKeywordManager::Get().Execute(Keyword, {{"force", "false"}});
        OnComplete(FHttpServerResponse::Ok());
    END_ROUTE()
    
    HttpServerModule.StartAllListeners();
}


void UnregisterRoutes() {
    for (const auto& Handle : RouteHandles) {
        Router->UnbindRoute(Handle);
    }
}

