#include "Keyword.h"
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
                LogTemp, Error, TEXT("Unknown Keyword Argument Type for Property '%s::%s'"), *KeywordClass->GetName(),
                *Iter->GetName()
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

FString UKeyword::GetDocumentation(TSubclassOf<UKeyword> KeywordClass){
    return KeywordClass->GetDescription();
}

TArray<FString> UKeyword::GetTags(TSubclassOf<UKeyword> KeywordClass) {
    const FString TagString = KeywordClass->GetMetaData("KeywordTags");

    // Split by commas
    TArray<FString> Tags;
    TagString.ParseIntoArray(Tags, TEXT(","));

    // Trim whitespaces
    for (auto& Tag : Tags) {
        Tag.TrimStartAndEndInline();
    }

    return Tags;
}

FKeywordInformation UKeyword::GetKeywordInformation(TSubclassOf<UKeyword> KeywordClass) {
    return FKeywordInformation{
        .Arguments = GetArguments(KeywordClass),
        .Types = GetTypes(KeywordClass),
        .Documentation =  GetDocumentation(KeywordClass),
        .Tags = GetTags(KeywordClass),
    };
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

        if (!Arguments[ArgumentIndex]->ParseIntoProperty(*Iter, Iter->ContainerPtrToValuePtr<void>(Keyword))) {
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