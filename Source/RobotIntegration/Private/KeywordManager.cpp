#include "KeywordManager.h"

#include "Keyword.h"


FKeywordManager* FKeywordManager::Instance = nullptr;


FKeywordManager::FKeywordManager() {
    TArray<UClass*> Classes = {};
    GetDerivedClasses(UKeyword::StaticClass(), Classes, true);

    // Remove Abstract Classes
    Classes = Classes.FilterByPredicate([](const UClass* Class) {
        return !(Class->ClassFlags & CLASS_Abstract);
    });
    
    for (const auto* Class : Classes) {
        UKeyword* Keyword = NewObject<UKeyword>(GetTransientPackage(), Class);
        Keywords.Emplace(Keyword->GetName(), Keyword);
    }
}

FKeywordManager& FKeywordManager::Get() {
    if (!Instance) {
        Instance = new FKeywordManager();
    }

    return *Instance;
}

TArray<FString> FKeywordManager::KeywordNames() const {
    TArray<FString> Names;
    Keywords.GetKeys(Names);
    return Names;
}

bool FKeywordManager::HasKeyword(const FString& Name) const {
    return Keywords.Find(Name) != nullptr;
}

TArray<FString> FKeywordManager::KeywordArguments(const FString& Keyword) const {
    return Keywords[Keyword]->GetArguments();
}

TMap<FString, FString> FKeywordManager::KeywordTypes(const FString& Keyword) const {
    return Keywords[Keyword]->GetArgumentTypes();
}

void FKeywordManager::Execute(const FString& Keyword, const TArray<FString>& Arguments) const {
    Keywords[Keyword]->Execute(Arguments);
}
