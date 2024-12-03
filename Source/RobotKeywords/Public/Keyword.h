#pragma once

#include "CoreMinimal.h"
#include "RpcValue.h"
#include "UObject/Object.h"
#include <variant>
#include "Keyword.generated.h"


class FXmlNode;
struct FRpcValue;
using FKeywordResponse = std::variant<TSharedPtr<FRpcValue>, FString>;


UCLASS(Abstract)
class ROBOTKEYWORDS_API UKeyword : public UObject {
    GENERATED_BODY()

    friend class UKeywordRunner;
    
    // Used to generate the output logs.
    FStringBuilderBase OutputBuilder;

public:
    virtual FKeywordResponse Execute() {
        unimplemented();
        return Success();
    }

    static FString GetKeywordName(TSubclassOf<UKeyword> KeywordClass);

    static TArray<FString> GetArguments(TSubclassOf<UKeyword> KeywordClass);
    
    static TArray<FString> GetTypes(TSubclassOf<UKeyword> KeywordClass);
    
    static TSharedPtr<FRpcValue> Run(TSubclassOf<UKeyword> KeywordClass, const TArray<TSharedPtr<FRpcValue>>& Arguments);

protected:
    /**
     * Logging Routines
     */

    template<typename FmtType, typename... Types>
    void Log(const FString& Level, const FmtType& Fmt, Types... Args) {
        const FString Message = FString::Printf(Fmt, Forward<Types>(Args)...);
        OutputBuilder.Appendf(TEXT("*%s* %s\\n"), *Level, *Message);
    }

    template<typename FmtType, typename... Types>
    void LogWarning(const FmtType& Fmt, Types... Args) {
        Log("WARN", Fmt, Forward<Types>(Args)...);
    }

    template<typename FmtType, typename... Types>
    void LogInfo(const FmtType& Fmt, Types... Args) {
        Log("INFO", Fmt, Forward<Types>(Args)...);
    }

    template<typename FmtType, typename... Types>
    void LogDebug(const FmtType& Fmt, Types... Args) {
        Log("DEBUG", Fmt, Forward<Types>(Args)...);
    }

    /**
     * Response Generation
     */
    FKeywordResponse Success();
    FKeywordResponse Success(const int32& Value);
    FKeywordResponse Success(const bool& Value);
    FKeywordResponse Success(const FString& Value);
    FKeywordResponse Success(const double& Value);
    FKeywordResponse Success(const FDateTime& Value);
    FKeywordResponse Success(const TArray<uint8>& Value);
    FKeywordResponse Success(const TArray<TSharedPtr<FRpcValue>>& Value);
    FKeywordResponse Success(const TMap<FString, TSharedPtr<FRpcValue>>& Value);

    FKeywordResponse Error(const FString& Message);
    
private:
    static TSharedPtr<FRpcValue> GenerateResponse(
        const FKeywordResponse& Response, const FStringBuilderBase& OutputBuilder
    );
};
