// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"
#include "MainMenu.generated.h"

UCLASS()
class NEVERGU_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	AThirdPersonCharacter* PlayerCharacter;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
