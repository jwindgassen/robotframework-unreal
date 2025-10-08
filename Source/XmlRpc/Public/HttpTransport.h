#pragma once

#include "CoreMinimal.h"


class FSocket;
class FTcpListener;


struct FRequest {
    FString Method;
    FString Path;
    int32 Version;
    TMap<FString, FString> Headers;
    FString Body;
};

struct FResponse {
    int32 StatusCode;
    FString Content;
    FString ContentType;
};

DECLARE_DELEGATE_RetVal_OneParam(FResponse, FOnReceivedRequest, const FRequest& /* Request */)

/**
 * Transport Layer for the XML-RPC Server, which uses a TCP Socket for listening to HTTP Requests.
 *
 * Implementation inspired from Moritz Wundke: https://github.com/moritz-wundke/UnrealRemote/blob/master/Source/UnrealEdRemote/Public/UnrealEdRemoteServer.h
 */
class XMLRPC_API FAsyncHttpTransport : public FRunnable {
    FRunnableThread* Thread;
    FTcpListener* Listener;

    TArray<FSocket*> Clients;
    
public:
    FString Path;
    uint16 Port;

    FOnReceivedRequest OnReceivedRequest;
    
    FAsyncHttpTransport(const FString& Path, const uint16 Port);
    ~FAsyncHttpTransport();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

private:
    TOptional<FRequest> ParseRequest(const FString& Data);
    void SendResponse(FSocket* Client, const FRequest& Request, const FResponse& Response);
};
