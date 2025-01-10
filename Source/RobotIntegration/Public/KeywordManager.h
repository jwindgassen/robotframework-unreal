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

    TSharedPtr<FRpcValue> LibraryInformation() const;

    TSharedPtr<FRpcValue> Execute(const FString& Keyword, const TArray<TSharedPtr<FRpcValue>>& Arguments) const;
};
