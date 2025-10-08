#include "HttpTransport.h"

#include <functional>

#include "Common/TcpListener.h"
#include "Serialization/ArrayReader.h"

#define __GNUC__ 0
#define __SSE4_2__ 0
#include "XmlRpcServer.h"
#include "picohttpparser.h"


FAsyncHttpTransport::FAsyncHttpTransport(
    const FString& Path, const uint16 Port
) : Thread(nullptr), Listener(nullptr), Path(Path), Port(Port) {
    Thread = FRunnableThread::Create(this, TEXT("XML-RPC Transport"), 8 * 1024);
}

FAsyncHttpTransport::~FAsyncHttpTransport() {
    
}

bool FAsyncHttpTransport::Init() {
    if (!Listener) {
        const FIPv4Endpoint Endpoint{FIPv4Address::InternalLoopback, Port};
        Listener = new FTcpListener(Endpoint);
        Listener->OnConnectionAccepted().BindLambda([this](FSocket* Socket, const FIPv4Endpoint& Address) {
            UE_LOG(LogXmlRpcServer, Log, TEXT("New Connection: %s"), *Address.ToString())
            
            Clients.Add(Socket);
            return true;
        });
    }

    return true;
}

uint32 FAsyncHttpTransport::Run() {
    while (true) {
        // Clear Closed Connections
        for (int32 i = 0; i < Clients.Num(); ++i) {
            const auto State = Clients[i]->GetConnectionState();
            if (State != SCS_Connected) {
                Clients.RemoveAtSwap(i--);
            }
        }

        // Query incoming Requests
        for (int32 i = 0; i < Clients.Num(); ++i) {
            FSocket* Client = Clients[i];
            
            uint32 DataSize;
            if (Client->HasPendingData(DataSize)) {
                UE_LOG(LogXmlRpcServer, Log, TEXT("Client sent %d bytes"), DataSize)
                
                uint8* Buffer = (uint8*) malloc(sizeof(uint8) * DataSize);
                int32 BytesRead;
                if (Client->Recv(Buffer, DataSize, BytesRead)) {
                    const FString Data{BytesRead, (ANSICHAR*) Buffer};
                    UE_LOG(LogXmlRpcServer, Log, TEXT("Request: %s"), *Data)

                    // Execute Delegate
                    const TOptional<FRequest> Request = ParseRequest(Data);
                    if (Request.IsSet() && Request->Path == Path) {
                        FResponse Response = OnReceivedRequest.Execute(*Request);
                        SendResponse(Client, *Request, Response);
                    }
                }
            }
        }
    }
}

void FAsyncHttpTransport::Stop() {
    for (auto* Client : Clients) {
        Client->Close();
    }
}

TOptional<FRequest> FAsyncHttpTransport::ParseRequest(const FString& Data) {
    const char* Method = nullptr;
    const char* RequestPath = nullptr;
    int Version;
    size_t MethodLen, PathLen;

    size_t NumHeaders = 16;
    phr_header* Headers = (phr_header*) malloc(sizeof(phr_header) * NumHeaders);
    
    const char* Buffer = TCHAR_TO_UTF8(*Data);
    const int BytesParsed = phr_parse_request(Buffer, Data.Len(), &Method, &MethodLen, &RequestPath, &PathLen, &Version, Headers, &NumHeaders, 0);
    if (BytesParsed < 0) {
        return {};
    }

    TMap<FString, FString> HeaderMap;
    for (size_t i = 0; i < NumHeaders; ++i) {
        const phr_header Header = Headers[i];

        FString Name{static_cast<int32>(Header.name_len), Header.name};
        FString Value{static_cast<int32>(Header.value_len), Header.value};
        HeaderMap.Add(Name, Value);
    }

    UE_LOG(LogXmlRpcServer, Log, TEXT("Method: %s\nPath: %s\nBody: %s\n"), *FString(MethodLen, Method), *FString(PathLen, RequestPath), *Data.RightChop(BytesParsed))

    return FRequest{
        FString(MethodLen, Method),
        FString(PathLen, RequestPath),
        Version,
        HeaderMap,
        Data.RightChop(BytesParsed)
    };
}

void FAsyncHttpTransport::SendResponse(FSocket* Client, const FRequest& Request, const FResponse& Response) {
    FAnsiStringBuilderBase ResponseBuilder;

    ResponseBuilder.Appendf("HTTP/1.%d %d\r\n", Request.Version, Response.StatusCode);
    ResponseBuilder.Appendf("Content-Type: %s\r\n", *Response.ContentType);
    ResponseBuilder.Appendf("Content-Length: %d\r\n", Response.Content.Len());
    ResponseBuilder.Append("\r\n");
    ResponseBuilder.Append(*Response.Content);

    int32 BytesWritten;
    Client->Send((uint8*) ResponseBuilder.GetData(), ResponseBuilder.Len(), BytesWritten);
}
