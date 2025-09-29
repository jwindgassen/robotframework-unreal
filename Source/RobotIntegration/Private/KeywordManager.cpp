#include "KeywordManager.h"


FKeywordManager* FKeywordManager::Instance = nullptr;


FKeywordManager::FKeywordManager() {
    TArray<UClass*> Classes = {};
    GetDerivedClasses(UKeyword::StaticClass(), Classes, true);

    // Remove Abstract Classes
    Classes = Classes.FilterByPredicate([](const UClass* Class) {
        return !(Class->ClassFlags & CLASS_Abstract);
    });
    
    for (auto* Class : Classes) {
        const FKeywordInformation Information = UKeyword::GetKeywordInformation(Class);
        FKeywordCacheType Cache = MakeTuple(Class, Information);
        Keywords.Emplace(Information.Name, Cache);
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
    
    for (const auto& [KeywordName, Tuple] : Keywords) {
        const FKeywordInformation& KeywordInformation = Tuple.Value;
        
        FRpcValueStruct Information;
        Information.Add("docs", MakeShared<FRpcValue>(KeywordInformation.Documentation));

        // Convert Arguments & Types
        FRpcValueList Args, Types;
        for (const auto& Arg : KeywordInformation.Arguments) {
            Args.Add(MakeShared<FRpcValue>(
                Arg.DefaultValue.IsEmpty() ? FString::Printf(TEXT("%s=%s"), *Arg.Name, *Arg.DefaultValue) : Arg.Name
            ));
            Types.Add(MakeShared<FRpcValue>(Arg.Type));
        }
        
        Information.Add("args", MakeShared<FRpcValue>(Args));
        Information.Add("types", MakeShared<FRpcValue>(Types));
    
        // Convert Tags
        FRpcValueList TagList;
        for (const auto& Tag : KeywordInformation.Tags) {
            TagList.Push(MakeShared<FRpcValue>(Tag));
        }
        Information.Add("tags", MakeShared<FRpcValue>(TagList));
        
        LibraryInformation.Add(KeywordName, MakeShared<FRpcValue>(Information));
    }
    
    return MakeShared<FRpcValue>(LibraryInformation);
}

TSharedPtr<FRpcValue> FKeywordManager::Execute(const FString& Keyword, const TArray<TSharedPtr<FRpcValue>>& Arguments) const {
    check(HasKeyword(Keyword));
    return UKeyword::Run(Keywords[Keyword].Key, Arguments);
}
