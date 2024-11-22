#include "Input/MouseInput.h"

TArray<FString> UScrollElementKeyword::GetArguments() {
    TArray<FString> Args = Super::GetArguments();
    Args.Add("Delta=2.5");

    return Args;
}

TMap<FString, FString> UScrollElementKeyword::GetArgumentTypes() {
    TMap<FString, FString> Types = Super::GetArgumentTypes();
    Types.Emplace("Delta", "float");

    return Types;
}

void UScrollElementKeyword::PerformAction(TSharedRef<IDriverElement> Element, const TArray<FString>& OtherArgs) {
    float Delta = FPlatformString::Atof(*OtherArgs[0]);
    Element->ScrollBy(Delta);
}


