// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsBarInventory.generated.h"

class UButton;
class AThirdPersonCharacter;

UCLASS()
class NEVERGU_API UOptionsBarInventory : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton* UseOption;

	UPROPERTY(meta = (BindWidget))
	UButton* DropOption;

	UPROPERTY(meta = (BindWidget))
	UButton* SellOption;

	UPROPERTY(meta = (BindWidget))
	UButton* DeleteOption;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelOption;

};
