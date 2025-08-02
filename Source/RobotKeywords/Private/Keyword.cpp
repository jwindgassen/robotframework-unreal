#include "Keyword.h"
#include <variant>


FKeywordInformation UKeyword::GetKeywordInformation() const {
    FString KeywordName = GetClass()->GetName();
    KeywordName = KeywordName.Left(KeywordName.Len() - 7).TrimEnd();  // Remove Suffix

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
    for (TFieldIterator<FProperty> Iter(GetClass()); Iter; ++Iter) {
        // Determine Type
        FString InnerType;
        FString* Type = CppTypesToRobotTypes.Find(Iter->GetCPPType(&InnerType));
        if (!Type) {
            UE_LOG(
                LogTemp, Error, TEXT("Unknown Keyword Argument Type for Property '%s::%s'"),
                *GetClass()->GetName(), *Iter->GetName()
            )
        }

        // Special Case for byte arrays
        if (*Type == "list" && InnerType == "uint8") {
            *Type = "bytes";
        }

        // ToDo: Default Values
        Arguments.Emplace(Iter->GetName(), *Type, "");
    }
    
    return {
        .Name = KeywordName,
        .Arguments = Arguments,
        .Documentation = "",
        .Tags = {},
    };
}


TSharedPtr<FRpcValue> UKeyword::Run(
    const TSubclassOf<UKeyword> KeywordClass, const TArray<TSharedPtr<FRpcValue>>& Arguments
) {
    check(KeywordClass);
    UKeyword* Keyword = NewObject<UKeyword>(GetTransientPackage(), KeywordClass);

    // Find Properties via KeywordInformation and fill Values
    int32 ArgumentIndex = 0;
    for (const FKeywordArgument& Argument : Keyword->GetKeywordInformation().Arguments) {
        FProperty* Property = KeywordClass->FindPropertyByName(FName(*Argument.Name));

        // Ensure Field exists if no default is given
        if (!Property && Argument.DefaultValue == "") {
            UE_LOG(
                LogTemp, Error,
                TEXT("Could not find Property '%s::%s', which doesn't have a default value"),
                *Argument.Name, *KeywordClass->GetName()
            )
            continue;
        }

        // Parse Argument into Property
        if (!Arguments[ArgumentIndex++]->ParseIntoProperty(Property, Property->ContainerPtrToValuePtr<void>(Keyword))) {
            UE_LOG(
                LogTemp, Error, TEXT("Could not set property '%s' on '%s'"),
                *Property->GetName(), *KeywordClass->GetName()
            )
        }
    }

    if (ArgumentIndex < Arguments.Num()) {
        UE_LOG(
            LogTemp, Warning, TEXT("Could not process all given Arguments (got %d, processed %d)"),
            Arguments.Num(), ArgumentIndex
        )
    }

    // Execute Keyword
    const auto Response = Keyword->Execute();

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

    // ToDo: 
    // if (Success) {
    //     JsonBuilder.Appendf(TEXT("\"return\": \"%s\""))
    // } else {
    //     ...
    // }
    
    return MakeShared<FRpcValue>(Xml);
}