#pragma once

#include "CoreMinimal.h"


class UKeyword;
struct FRpcValue;


class ROBOTINTEGRATION_API FKeywordManager {
    TMap<FString, TSubclassOf<UKeyword>> Keywords;
    
    static FKeywordManager* Instance;
    
    FKeywordManager();
    
public:
    static FKeywordManager& Get();

    TArray<FString> KeywordNames() const;

    bool HasKeyword(const FString& Name) const;
    
    TArray<FString> KeywordArguments(const FString& Keyword) const;

    TMap<FString, FString> KeywordTypes(const FString& Keyword) const;

    void Execute(const FString& Keyword, const TArray<TSharedPtr<FRpcValue>>& Arguments) const;
};
