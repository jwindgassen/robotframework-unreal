// Copyright (c) 2021-2025, Forschungszentrum Jülich GmbH. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Keyword.h"
#include "StopRemoteServer.generated.h"


/**
 * Stop the Unreal Engine Application after a delay of 0.5s
 */
UCLASS(DisplayName="Stop Remote Server")
class ROBOTKEYWORDS_API UStopRemoteServerKeyword : public UKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse Execute() override;
};
