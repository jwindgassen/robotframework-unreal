#include "Input/KeyboardInput.h"
#include "IDriverElement.h"


#define PARSE_KEY(Identifier, Value)                                \
    const FKey Identifier(*Value);                                  \
    if (!Identifier.IsValid()) {                                    \
        return LogErrorAndReturn(TEXT("Unknown Key: %s"), *Value);  \
    }

FKeywordResponse UTypeTextOnElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    Element->Type(Text);
    return Success();
}

FKeywordResponse UTypeKeyOnElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    PARSE_KEY(Key, KeyName);

    Element->Type(Key);
    return Success();
}

FKeywordResponse UTypeChordOnElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    if (Keys.Num() < 2) {
        return LogErrorAndReturn(TEXT("Chords must be at least 2 Keys."));
    }
    
    if (Keys.Num() > 3) {
        LogWarning(TEXT("Chords can consist of 3 Keys at most. Ignoring anything above that."));
    }

    PARSE_KEY(KeyA, Keys[0]);
    PARSE_KEY(KeyB, Keys[1]);

    if (Keys.Num() == 2) {
        Element->TypeChord(KeyA, KeyB);
    } else {
        PARSE_KEY(KeyC, Keys[2]);
        Element->TypeChord(KeyA, KeyB, KeyC);
    }
    
    return Success();
}

FKeywordResponse UPressKeyOnElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    PARSE_KEY(Key, KeyName);

    Element->Press(Key);
    return Success();
}

FKeywordResponse UPressChordOnElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    if (Keys.Num() < 2) {
        return LogErrorAndReturn(TEXT("Chords must be at least 2 Keys."));
    }
    
    if (Keys.Num() > 3) {
        LogWarning(TEXT("Chords can consist of 3 Keys at most. Ignoring anything above that."));
    }

    PARSE_KEY(KeyA, Keys[0]);
    PARSE_KEY(KeyB, Keys[1]);

    if (Keys.Num() == 2) {
        Element->PressChord(KeyA, KeyB);
    } else {
        PARSE_KEY(KeyC, Keys[2]);
        Element->PressChord(KeyA, KeyB, KeyC);
    }
    
    return Success();
}

FKeywordResponse UReleaseKeyOnElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    PARSE_KEY(Key, KeyName);

    Element->Release(Key);
    return Success();
}

FKeywordResponse UReleaseChordOnElementKeyword::PerformAction(TSharedRef<IDriverElement> Element) {
    if (Keys.Num() < 2) {
        return LogErrorAndReturn(TEXT("Chords must be at least 2 Keys."));
    }
    
    if (Keys.Num() > 3) {
        LogWarning(TEXT("Chords can consist of 3 Keys at most. Ignoring anything above that."));
    }

    PARSE_KEY(KeyA, Keys[0]);
    PARSE_KEY(KeyB, Keys[1]);

    if (Keys.Num() == 2) {
        Element->ReleaseChord(KeyA, KeyB);
    } else {
        PARSE_KEY(KeyC, Keys[2]);
        Element->ReleaseChord(KeyA, KeyB, KeyC);
    }
    
    return Success();
}