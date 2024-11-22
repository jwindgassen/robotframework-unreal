#pragma once

#include "CoreMinimal.h"


class UKeyword;


class ROBOTINTEGRATION_API FKeywordManager {
    TMap<FString, UKeyword*> Keywords;
    
    static FKeywordManager* Instance;
    
    FKeywordManager();
    
public:
    static FKeywordManager& Get();

    TArray<FString> KeywordNames() const;

    bool HasKeyword(const FString& Name) const;
    
    TArray<FString> KeywordArguments(const FString& Keyword) const;

    TMap<FString, FString> KeywordTypes(const FString& Keyword) const;

    void Execute(const FString& Keyword, const TArray<FString>& Arguments) const;
};
