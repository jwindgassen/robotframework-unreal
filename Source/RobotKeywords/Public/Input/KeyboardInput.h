#pragma once

#include "IDriverElement.h"
#include "Input/InputKeyword.h"
#include "KeyboardInput.generated.h"


UCLASS()
class UTypeTextOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY()
    FString Text;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


UCLASS()
class UTypeKeyOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY()
    FString KeyName;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


UCLASS()
class UTypeChordOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<FString> Keys;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/*************************************
 *               Press               *
 *************************************/


UCLASS()
class UPressKeyOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY()
    FString KeyName;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


UCLASS()
class UPressChordOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<FString> Keys;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


/*************************************
 *              Release              *
 *************************************/


UCLASS()
class UReleaseKeyOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY()
    FString KeyName;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};


UCLASS()
class UReleaseChordOnElementKeyword : public UInputKeyword {
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<FString> Keys;

    virtual FKeywordResponse PerformAction(TSharedRef<IDriverElement> Element) override;
};