#pragma once

#include "IDriverElement.h"
#include "Input/InputKeyword.h"
#include "KeyboardInput.generated.h"


/// Types the given text into the Element
UCLASS(DisplayName = "Type Text on Element")
class UTypeTextOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (KeywordArgument))
    FString Text;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Presses and releases the given Key on the Element
UCLASS(DisplayName = "Type Key on Element")
class UTypeKeyOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (KeywordArgument))
    FString KeyName;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Presses and releases the given Keys on the Element
UCLASS(DisplayName = "Type Chord on Element")
class UTypeChordOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (KeywordArgument))
    TArray<FString> Keys;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/*************************************
 *               Press               *
 *************************************/


/// Presses the given Key on the Element
UCLASS(DisplayName = "Press Key on Element")
class UPressKeyOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (KeywordArgument))
    FString KeyName;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Presses Keys on the Element
UCLASS(DisplayName = "Press Chord on Element")
class UPressChordOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (KeywordArgument))
    TArray<FString> Keys;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/*************************************
 *              Release              *
 *************************************/


/// Releases the given Key on the Element
UCLASS(DisplayName = "Release Key on Element")
class UReleaseKeyOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (KeywordArgument))
    FString KeyName;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/// Releases the given Keys on the Element
UCLASS(DisplayName = "Release Chord on Element")
class UReleaseChordOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (KeywordArgument))
    TArray<FString> Keys;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};