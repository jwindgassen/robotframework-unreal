#include "Parse.h"

bool ParseBool(const FString& Argument) {
    const FString Lower = Argument.ToLower();
    return Lower == "true" | Lower == "t" | Lower == "1" | Lower == "yes";
}
