#pragma once

#include "CoreMinimal.h"
#include "Keyword.h"


class ROBOTINTEGRATION_API FKeywordManager {
    using FKeywordCacheType = TTuple<TSubclassOf<UKeyword>, FKeywordInformation>;

    // Store all known Keywords and their corresponding FKeywordInformation
    TMap<FString, FKeywordCacheType> Keywords;
    
    static FKeywordManager* Instance;
    
    FKeywordManager();
    
public:
    static FKeywordManager& Get();

    TArray<FString> KeywordNames() const;

    bool HasKeyword(const FString& Name) const;

    TSharedPtr<FRpcValue> LibraryInformation() const;

    TSharedPtr<FRpcValue> Execute(const FString& Keyword, const TArray<TSharedPtr<FRpcValue>>& Arguments) const;
};
