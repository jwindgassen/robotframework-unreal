#include "General/LevelHasActorOfClass.h"
#include "Kismet/GameplayStatics.h"


FKeywordResponse ULevelHasActorOfClassKeyword::Execute() {
    return Async(EAsyncExecution::TaskGraphMainThread, [&]() {
        const TSubclassOf<AActor> Class = StaticLoadClass(AActor::StaticClass(), nullptr, *ClassName);
        if (!Class) {
            return Error("Could not find class with name " + ClassName);
        }

        const AActor* Actor = UGameplayStatics::GetActorOfClass(GEngine->GetCurrentPlayWorld(), Class);
        return Actor ? Success() : Error("Could not find an Actor of class " + ClassName);
    }).Get();
}
