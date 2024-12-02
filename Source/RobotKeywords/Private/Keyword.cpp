#include "Keyword.h"
#include "RpcTypes.h"
#include <variant>


FString UKeyword::GetKeywordName(TSubclassOf<UKeyword> KeywordClass) {
    FString Name = KeywordClass->GetDisplayNameText().ToString();
    
    if (!Name.EndsWith("Keyword")) {
        UE_LOG(LogTemp, Warning, TEXT("Keyword Subclasses should end with 'Keyword': %s"), *Name);
        return Name;
    }

    // Remove Suffix
    Name = Name.Left(Name.Len() - 7);
    Name.TrimEndInline();

    return Name;
}

TArray<FString> UKeyword::GetArguments(TSubclassOf<UKeyword> KeywordClass) {
    TArray<FString> Names;

    for (TFieldIterator<FProperty> Iter(KeywordClass); Iter; ++Iter) {
        if (!Iter->HasMetaData(TEXT("KeywordArgument"))) {
            continue;
        }

        // ToDo: Default Values
        Names.Add(Iter->GetName());
    }

    return Names;
}

TArray<FString> UKeyword::GetTypes(TSubclassOf<UKeyword> KeywordClass) {
    TArray<FString> Types;

    static TMap<FString, FString> CppTypesToRobotTypes = {
        {"int32", "int"},
        {"bool", "bool"},
        {"FString", "str"},
        {"double", "double"},
        {"FDateTime", "datetime.iso8601"},
        {"TArray", "list"},
        {"TMap", "dict"},
    };

    for (TFieldIterator<FProperty> Iter(KeywordClass); Iter; ++Iter) {
        if (!Iter->HasMetaData(TEXT("KeywordArgument"))) {
            continue;
        }

        FString InnerType;
        FString* Type = CppTypesToRobotTypes.Find(Iter->GetCPPType(&InnerType));
        if (!Type) {
            UE_LOG(
                LogTemp, Error, TEXT("Unknown Keyword Argument Type for Property '%s::%s'"),
                *KeywordClass->GetName(), *Iter->GetName()
            )
        }

        // Special Case for byte arrays
        if (*Type == "list" && InnerType == "uint8") {
            *Type = "bytes";
        }
        
        Types.Add(*Type);
    }

    return Types;
}

TSharedPtr<FRpcValue> UKeyword::Run(
    const TSubclassOf<UKeyword> KeywordClass, const TArray<TSharedPtr<FRpcValue>>& Arguments
) {
    check(KeywordClass);
    UKeyword* Keyword = NewObject<UKeyword>(GetTransientPackage(), KeywordClass);

    // Generate Properties
    int32 ArgumentIndex = 0;
    for (TFieldIterator<FProperty> Iter(KeywordClass); Iter; ++Iter, ++ArgumentIndex) {
        if (!Iter->HasMetaData(TEXT("KeywordArgument"))) {
            continue;
        }

        if (!SetPropertyValue(Arguments[ArgumentIndex], *Iter, Iter->ContainerPtrToValuePtr<void>(Keyword))) {
            UE_LOG(
                LogTemp, Error, TEXT("Could not set property '%s' on '%s'"), *Iter->GetName(),
                *KeywordClass->GetName()
            )
        }
    }

    if (ArgumentIndex < Arguments.Num()) {
        UE_LOG(LogTemp, Warning, TEXT("Could not process all given Arguments (got %d, processed %d)"), Arguments.Num(), ArgumentIndex)
    }

    // Execute Keyword
    const auto Response = Keyword->Execute();

    return GenerateResponse(Response, Keyword->OutputBuilder);
}


FKeywordResponse UKeyword::Success() {
    return nullptr;
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


bool UKeyword::SetPropertyValue(const TSharedPtr<FRpcValue>& Element, FProperty const* Property, void* PropertyValue) {
    if (const auto* NumericProperty = CastField<FNumericProperty>(Property)) {
        if (NumericProperty->IsInteger()) {
            check(std::holds_alternative<int32>(*Element))

            NumericProperty->SetIntPropertyValue(PropertyValue, static_cast<int64>(std::get<int32>(*Element)));
            return true;
        }

        if (NumericProperty->IsFloatingPoint()) {
            check(std::holds_alternative<double>(*Element))

            NumericProperty->SetFloatingPointPropertyValue(PropertyValue, std::get<double>(*Element));
            return true;
        }

        return false;
    }

    if (const auto* BoolProperty = CastField<FBoolProperty>(Property)) {
        check(std::holds_alternative<bool>(*Element))

        BoolProperty->SetPropertyValue(PropertyValue, std::get<bool>(*Element));
        return true;
    }

    if (const auto* StringProperty = CastField<FStrProperty>(Property)) {
        check(std::holds_alternative<FString>(*Element))

        StringProperty->SetPropertyValue(PropertyValue, std::get<FString>(*Element));
        return true;
    }

    if (const auto* ArrayProperty = CastField<FArrayProperty>(Property)) {
        FScriptArrayHelper Helper{ArrayProperty, PropertyValue};

        if (CastField<FByteProperty>(ArrayProperty->Inner)) {
            check(std::holds_alternative<TArray<uint8>>(*Element))

            auto& Array = std::get<TArray<uint8>>(*Element);
            Helper.MoveAssign(&Array);

            return true;
        }

        if (CastField<FObjectPtrProperty>(ArrayProperty->Inner)) {
            check(std::holds_alternative<TArray<TSharedPtr<FRpcValue>>>(*Element))

            const auto& Array = std::get<TArray<TSharedPtr<FRpcValue>>>(*Element);
            Helper.AddValues(Array.Num());

            for (int32 i = 0; i < Array.Num(); ++i) {
                SetPropertyValue(Array[i], ArrayProperty->Inner, Helper.GetRawPtr(i));
            }

            return true;
        }

        return false;
    }

    if (const auto* MapProperty = CastField<FMapProperty>(Property)) {
        using FRpcValueMap = TMap<FString, TSharedPtr<FRpcValue>>;
        
        check(std::holds_alternative<FRpcValueMap>(*Element))
        check(CastField<FStrProperty>(MapProperty->KeyProp))
        check(CastField<FObjectPtrProperty>(MapProperty->ValueProp))

        // const auto& Map = std::get<TMap<FString, TSharedPtr<FRpcValue>>>(*Element);
        // FScriptMapHelper Helper{MapProperty, PropertyValue};
        checkf(false, TEXT("ToDo"))
        return false;
    }

    return false;
}

TSharedPtr<FRpcValue> UKeyword::GenerateResponse(
    const FKeywordResponse& Response, const FStringBuilderBase& OutputBuilder
) {
    TMap<FString, TSharedPtr<FRpcValue>> Xml;

    const bool Success = std::holds_alternative<TSharedPtr<FRpcValue>>(Response);
    Xml.Emplace("status", MakeShared<FRpcValue>(Success ? "PASS" : "FAIL"));
    Xml.Emplace("output", MakeShared<FRpcValue>(OutputBuilder.ToString()));

    // ToDo: 
    // if (Success) {
    //     JsonBuilder.Appendf(TEXT("\"return\": \"%s\""))
    // } else {
    //     ...
    // }
    
    return MakeShared<FRpcValue>(Xml);
}