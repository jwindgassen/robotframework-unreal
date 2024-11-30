#pragma once

#include "CoreMinimal.h"
#include "RpcTypes.h"
#include "UObject/Object.h"
#include "Keyword.generated.h"


class FXmlNode;
struct FRpcValue;
struct FRpcMethodResponse;


UCLASS(Abstract)
class ROBOTKEYWORDS_API UKeyword : public UObject {
    GENERATED_BODY()

    friend class UKeywordRunner;
    
    // Used to generate the output logs.
    FStringBuilderBase OutputBuilder;

public:
    virtual TSharedPtr<FRpcMethodResponse> Execute() {
        unimplemented();
        return nullptr;
    }

    static FString GetKeywordName(TSubclassOf<UKeyword> KeywordClass);

    static TArray<FString> GetArguments(TSubclassOf<UKeyword> KeywordClass);
    
    static TMap<FString, FString> GetArgumentTypes(TSubclassOf<UKeyword> KeywordClass);
    
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
    TSharedPtr<FRpcMethodResponse> Success();
    TSharedPtr<FRpcMethodResponse> Success(const int32& Value);
    TSharedPtr<FRpcMethodResponse> Success(const bool& Value);
    TSharedPtr<FRpcMethodResponse> Success(const FString& Value);
    TSharedPtr<FRpcMethodResponse> Success(const double& Value);
    TSharedPtr<FRpcMethodResponse> Success(const FDateTime& Value);
    TSharedPtr<FRpcMethodResponse> Success(const TArray<uint8>& Value);
    TSharedPtr<FRpcMethodResponse> Success(const TArray<TSharedPtr<FRpcValue>>& Value);
    TSharedPtr<FRpcMethodResponse> Success(const TMap<FString, TSharedPtr<FRpcValue>>& Value);

    TSharedPtr<FRpcMethodResponse> Error(const int32& Code, const FString& Message);
    
private:
    static bool SetPropertyValue(const TSharedPtr<FRpcValue>& Element, FProperty const* Property, void* PropertyValue);
    
    static TSharedPtr<FRpcValue> GenerateResponse(
        const TSharedPtr<FRpcMethodResponse>& Response, const FStringBuilderBase& OutputBuilder
    );
};
