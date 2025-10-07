#include "Keyword.h"
#include "RobotKeywords.h"

#include <variant>


FKeywordInformation UKeyword::GetKeywordInformation(const TSubclassOf<UKeyword> KeywordClass) {
    checkf(
        KeywordClass->HasMetaData(TEXT("DisplayName")), TEXT("Please provide a DisplayName for %s"),
        *KeywordClass->GetName()
    )

    static TMap<FString, FString> CppTypesToRobotTypes = {
        {"int32", "int"},
        {"bool", "bool"},
        {"FString", "str"},
        {"double", "double"},
        {"FDateTime", "datetime.iso8601"},
        {"TArray", "list"},
        {"TMap", "dict"},
    };

    TArray<FKeywordArgument> Arguments;
    for (TFieldIterator<FProperty> Iter(KeywordClass); Iter; ++Iter) {
        if (!Iter->HasMetaData(TEXT("KeywordArgument"))) {
            continue;
        }

        // Determine Type
        FString InnerType;
        FString* Type = CppTypesToRobotTypes.Find(Iter->GetCPPType(&InnerType));
        if (!Type) {
            UE_LOG(
                LogRobotKeywords, Error, TEXT("Unknown Keyword Argument Type for Property '%s::%s'"),
                *KeywordClass->GetName(), *Iter->GetName()
            )
        }

        // Special Case for byte arrays
        if (*Type == "list" && InnerType == "uint8") {
            *Type = "bytes";
        }

        // ToDo: Default Values
        Arguments.Emplace(Iter->GetName(), *Type, "");
    }

    TArray<FString> Tags;
    KeywordClass->GetMetaData(TEXT("KeywordTags")).ParseIntoArray(Tags, TEXT(","));

    return {
        .Name = KeywordClass->GetDisplayNameText().ToString(),
        .Arguments = Arguments,
        .Documentation = KeywordClass->GetToolTipText().ToString(),
        .Tags = Tags,
    };
}


TSharedPtr<FRpcValue> UKeyword::Run(
    const TSubclassOf<UKeyword> KeywordClass, const TArray<TSharedPtr<FRpcValue>>& Arguments
) {
    check(KeywordClass);
    UE_LOG(LogRobotKeywords, Log, TEXT("Executing Keyword '%s'"), *KeywordClass->GetName())
    UKeyword* Keyword = NewObject<UKeyword>(GetTransientPackage(), KeywordClass);

    // Find Properties via KeywordInformation and fill Values
    int32 ArgumentIndex = 0;
    for (TFieldIterator<FProperty> Iter(KeywordClass); Iter; ++Iter) {
        if (!Iter->HasMetaData(TEXT("KeywordArgument"))) {
            continue;
        }

        // Parse Argument into Property
        if (!Arguments[ArgumentIndex++]->ParseIntoProperty(*Iter, Iter->ContainerPtrToValuePtr<void>(Keyword))) {
            UE_LOG(
                LogRobotKeywords, Error, TEXT("Could not set property '%s' on '%s'"), *Iter->GetName(),
                *KeywordClass->GetName()
            )
        }
    }

    if (ArgumentIndex < Arguments.Num()) {
        UE_LOG(
            LogRobotKeywords, Warning, TEXT("Could not process all given Arguments (got %d, processed %d)"),
            Arguments.Num(), ArgumentIndex
        )
    }

    // Execute Keyword
    const double StartTime = FGameTime::GetTimeSinceAppStart().GetRealTimeSeconds();

    const auto Response = Keyword->Execute();

    const double ExecutionTime = FGameTime::GetTimeSinceAppStart().GetRealTimeSeconds() - StartTime;
    UE_LOG(LogRobotKeywords, Log, TEXT("Keyword execution took %fs"), ExecutionTime)
    Keyword->LogInfo(TEXT("Keyword execution took %fs"), ExecutionTime);

    return GenerateResponse(Response, Keyword->OutputBuilder);
}


FKeywordResponse UKeyword::Success() {
    return MakeShared<FRpcValue>();
}

FKeywordResponse UKeyword::Success(const int32& Value) {
    return MakeShared<FRpcValue>(Value);
}

FKeywordResponse UKeyword::Success(const bool& Value) {
    return MakeShared<FRpcValue>(Value);
}

FKeywordResponse UKeyword::Success(const FString& Value) {
    return MakeShared<FRpcValue>(Value);
}

FKeywordResponse UKeyword::Success(const double& Value) {
    return MakeShared<FRpcValue>(Value);
}

FKeywordResponse UKeyword::Success(const FDateTime& Value) {
    return MakeShared<FRpcValue>(Value);
}

FKeywordResponse UKeyword::Success(const TArray<uint8>& Value) {
    return MakeShared<FRpcValue>(Value);
}

FKeywordResponse UKeyword::Success(const TArray<TSharedPtr<FRpcValue>>& Value) {
    return MakeShared<FRpcValue>(Value);
}

FKeywordResponse UKeyword::Success(const TMap<FString, TSharedPtr<FRpcValue>>& Value) {
    return MakeShared<FRpcValue>(Value);
}

FKeywordResponse UKeyword::Error(const FString& Message) {
    return Message;
}

TSharedPtr<FRpcValue> UKeyword::GenerateResponse(
    const FKeywordResponse& Response, const FStringBuilderBase& OutputBuilder
) {
    TMap<FString, TSharedPtr<FRpcValue>> Xml;

    const bool Success = std::holds_alternative<TSharedPtr<FRpcValue>>(Response);
    Xml.Emplace("status", MakeShared<FRpcValue>(FString{Success ? "PASS" : "FAIL"}));
    Xml.Emplace("output", MakeShared<FRpcValue>(FString{OutputBuilder.ToString()}));

    if (Success) {
        Xml.Emplace("return", std::get<TSharedPtr<FRpcValue>>(Response));
    } else {
        Xml.Emplace("error", MakeShared<FRpcValue>(std::get<FString>(Response)));
    }

    return MakeShared<FRpcValue>(Xml);
}