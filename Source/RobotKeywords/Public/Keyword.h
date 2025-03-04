﻿#pragma once

#include "CoreMinimal.h"
#include "RpcValue.h"
#include "UObject/Object.h"
#include <variant>
#include "Keyword.generated.h"


class FXmlNode;
struct FRpcValue;

using FKeywordResponse = std::variant<TSharedPtr<FRpcValue>, FString>;


USTRUCT()
struct FKeywordArgument {
    GENERATED_BODY()

    UPROPERTY()
    FString Name;

    UPROPERTY()
    FString Type;

    UPROPERTY()
    FString DefaultValue = "";
};


USTRUCT()
struct FKeywordInformation {
    GENERATED_BODY()

    UPROPERTY()
    FString Name;

    UPROPERTY()
    TArray<FKeywordArgument> Arguments;

    UPROPERTY()
    FString Documentation = "";

    UPROPERTY()
    TArray<FString> Tags = {};
};


UCLASS(Abstract)
class ROBOTKEYWORDS_API UKeyword : public UObject {
    GENERATED_BODY()

    friend class UKeywordRunner;

    // Used to generate the output logs.
    FStringBuilderBase OutputBuilder;

public:
    virtual FKeywordInformation GetKeywordInformation() const;

    virtual FKeywordResponse Execute() {
        unimplemented();
        return Success();
    }

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

    template<typename FmtType, typename... Types>
    FKeywordResponse LogErrorAndReturn(const FmtType& Fmt, Types... Args) {
        Log("FAIL", Fmt, Forward<Types>(Args)...);
        return Error(FString::Printf(Fmt, Forward<Types>(Args)...));
    }
    

private:
    static TSharedPtr<FRpcValue> GenerateResponse(
        const FKeywordResponse& Response, const FStringBuilderBase& OutputBuilder
    );
};
