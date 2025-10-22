// Copyright (c) 2021-2025, Forschungszentrum Jülich GmbH. All rights reserved.


#include "General/StopRemoteServer.h"

FKeywordResponse UStopRemoteServerKeyword::Execute() {
    // We first need to finish the XML-RPC Request, so we queue the exit after a 0.5s delay
    LogInfo(TEXT("Exiting Application in 0.5s"));
    Async(EAsyncExecution::ThreadPool, []() {
        FPlatformProcess::Sleep(0.5f);
        FPlatformMisc::RequestExit(false);
    });
            
    return Success(true);
}