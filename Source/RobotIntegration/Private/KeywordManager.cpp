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

TSharedPtr<FRpcValue> FKeywordManager::LibraryInformation() const {
    FRpcValueStruct LibraryInformation;

    // Use Game name for __intro__/__init__ for now
    // ToDo: Custom __intro__?
    FRpcValueStruct Intro;
    const FString GameName = FApp::GetProjectName();
    Intro.Add("docs", MakeShared<FRpcValue>(GameName));
    LibraryInformation.Emplace("__intro__", MakeShared<FRpcValue>(Intro));
    LibraryInformation.Emplace("__init__", MakeShared<FRpcValue>(Intro));
    
    for (const auto& [Name, Keyword] : Keywords) {
        const auto& [Arguments, Types, Documentation, Tags] = UKeyword::GetKeywordInformation(Keyword);
        
        FRpcValueStruct Information;
        Information.Add("docs", MakeShared<FRpcValue>(Documentation));

        // Convert Arguments
        FRpcValueList Args;
        for (const auto& Argument : Arguments) {
            Args.Push(MakeShared<FRpcValue>(Argument));
        }
        Information.Add("args", MakeShared<FRpcValue>(Args));

        // Convert Types
        FRpcValueList TypeList;
        for (const auto& Type : Types) {
            TypeList.Push(MakeShared<FRpcValue>(Type));
        }
        Information.Add("types", MakeShared<FRpcValue>(TypeList));
    
        // Convert Tags
        FRpcValueList TagList;
        for (const auto& Tag : Tags) {
            TagList.Push(MakeShared<FRpcValue>(Tag));
        }
        Information.Add("tags", MakeShared<FRpcValue>(TagList));
        
        LibraryInformation.Add(Name, MakeShared<FRpcValue>(Information));
    }
    
    return MakeShared<FRpcValue>(LibraryInformation);
}

TSharedPtr<FRpcValue> FKeywordManager::Execute(const FString& Keyword, const TArray<TSharedPtr<FRpcValue>>& Arguments) const {
    check(HasKeyword(Keyword));
    return UKeyword::Run(Keywords[Keyword], Arguments);
}
