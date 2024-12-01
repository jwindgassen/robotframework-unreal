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
    
    for (auto* Class : Classes) {
        Keywords.Emplace(UKeyword::GetKeywordName(Class), Class);
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
    check(HasKeyword(Keyword));
    return UKeyword::GetArguments(Keywords[Keyword]);
}

TArray<FString> FKeywordManager::KeywordTypes(const FString& Keyword) const {
    check(HasKeyword(Keyword));
    return UKeyword::GetTypes(Keywords[Keyword]);
}

TSharedPtr<FRpcValue> FKeywordManager::Execute(const FString& Keyword, const TArray<TSharedPtr<FRpcValue>>& Arguments) const {
    check(HasKeyword(Keyword));
    return UKeyword::Run(Keywords[Keyword], Arguments);
}
