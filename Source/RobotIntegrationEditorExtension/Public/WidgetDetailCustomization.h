// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class FWidgetDetailCustomization : public IDetailCustomization {
    TWeakObjectPtr<class UWidget> SelectedWidget;
    
public:
    static TSharedRef<IDetailCustomization> MakeInstance();

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

    void OnEnterTag(const FText& Tag, ETextCommit::Type CommitType) const;

    void OnEnterId(const FText& Id, ETextCommit::Type CommitType) const;
};
