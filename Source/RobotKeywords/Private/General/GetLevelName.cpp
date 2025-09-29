#include "General/GetLevelName.h"


FKeywordResponse UGetLevelNameKeyword::Execute() {
    const UWorld* World = GEngine->GetCurrentPlayWorld();
    if (!World) {
        return Error("Could not get a Reference to the World");
    }

    return Success(World->GetMapName());
}
