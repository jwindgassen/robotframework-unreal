#pragma once

#include "Keyword.h"
#include "GetLevelName.generated.h"


/// Returns the name of the level currently open in the World
UCLASS(DisplayName = "Get Level Name")
class ROBOTKEYWORDS_API UGetLevelNameKeyword : public UKeyword {
    GENERATED_BODY()

public:
    virtual FKeywordResponse Execute() override;
};
