#include "Parse.h"
#include "General/PrintMessageOnScreen.h"
#include "Kismet/GameplayStatics.h"

void UPrintMessageOnScreenKeyword::Execute(const TArray<FString>& Arguments) {
    if (!Arguments[1].IsNumeric()) {
        UE_LOG(LogTemp, Error, TEXT("2nd Argument to 'Print Message On Screen' is no float"))
    }
    const float Duration = FCString::Atof(*Arguments[1]);
    
    GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Purple, Arguments[0]);
}

