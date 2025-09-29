#pragma once

#include "Keyword.h"
#include "LevelHasActorOfClass.generated.h"

/**
 * Checks whether the currently open Level has an Actor of the given class
 */
UCLASS(DisplayName="Level has Actor of Class")
class ULevelHasActorOfClassKeyword : public UKeyword {
    GENERATED_BODY()

public:
    UPROPERTY(meta=(KeywordArgument))
    FString ClassName;
    
    virtual FKeywordResponse Execute() override;
};
