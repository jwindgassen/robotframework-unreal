#include "Input/KeyboardInput.h"
#include "IDriverElement.h"


TArray<FString> UEnterTextElementKeyword::GetArguments() {
    TArray<FString> Args = Super::GetArguments();
    Args.Add("Text");

    return Args;
}

TMap<FString, FString> UEnterTextElementKeyword::GetArgumentTypes() {
    TMap<FString, FString> Types = Super::GetArgumentTypes();
    Types.Emplace("Text", "str");

    return Types;
}

void UEnterTextElementKeyword::PerformAction(TSharedRef<IDriverElement> Element, const TArray<FString>& OtherArgs) {
    Element->Type(OtherArgs[0]);
}
