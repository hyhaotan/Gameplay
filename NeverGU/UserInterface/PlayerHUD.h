// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

struct FInteractableData;
class UInteractionWidget;
class UMainMenu;

UCLASS()
class NEVERGU_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	//UPROPERTIES AND VARIABLES
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UMainMenu> MainMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	//Sound Open Inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	USoundBase* OpenInventorySound;

	//Sound Close Inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	USoundBase* CloseInventorySound;

	bool bIsMenuVisible;

	//FUNCTION
	APlayerHUD();

	void DisplayMenu();
	void HideMenu();
	void ToggleMenu();

	void ShowCrosshair();
	void HideCrosshair();

	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;

protected:
	//UPROPERTIES AND VARIABLES
	UPROPERTY()
	UMainMenu* MainMenuWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;

	UPROPERTY()
	UUserWidget* CrosshairWidget;

	//FUNCTION
	virtual void BeginPlay() override;
};
