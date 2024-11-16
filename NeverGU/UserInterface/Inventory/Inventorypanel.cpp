// Fill out your copyright notice in the Description page of Project Settings.

#include "NeverGU/UserInterface/Inventory/Inventorypanel.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"
#include "NeverGU/Component/InventoryComponent.h"
#include "NeverGU/UserInterface/Inventory/InventoryItemSlot.h"
#include "NeverGU/UserInterface/Inventory/ItemDragDropOperation.h"
#include "NeverGU/Items/Backpack.h"

#include "Components/GridPanel.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

void UInventorypanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerCharacter = Cast<AThirdPersonCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		InventoryReference = PlayerCharacter->GetInventory();
		if (InventoryReference)
		{
			//set slot capacity and weight capacity for the player character
			InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventorypanel::RefreshInventory);
			InventoryReference->SetSlotsCapacity(20);
			InventoryReference->SetWeightCapacity(50.0f);
			SetInfoText();

			//if the player character take backpack then upgrade backpack of player 
			if (PlayerCharacter->EquippedItem && PlayerCharacter->EquippedItem->ItemType == EItemtypes::Backpack)
			{
				PlayerCharacter->bIsBackpack = true;
				int32 AdditionalSlots = InventoryReference->GetSlotsCapacity();
				float AdditionalWeightCapacity = InventoryReference->GetWeightCapacity();

				InventoryReference->SetSlotsCapacity(AdditionalSlots + 100);
				InventoryReference->SetWeightCapacity(AdditionalWeightCapacity + 400.f);
			}
		}
	}
}

void UInventorypanel::SetInfoText() const
{
	//set up weight for user widget
	const FString WeightInfoValue{
		FString::SanitizeFloat(InventoryReference->GetInventoryTotalWeight()) + "/"
		+ FString::SanitizeFloat(InventoryReference->GetWeightCapacity())
	};

	//set up capacity for user widget
	const FString CapacityInfoValue{
		FString::FromInt(InventoryReference->GetInventoryContents().Num()) + "/"
		+ FString::FromInt(InventoryReference->GetSlotsCapacity())
	};

	//set up text for widget
	WeightInfo->SetText(FText::FromString(WeightInfoValue));
	CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}

void UInventorypanel::RefreshInventory()
{
	if (InventoryReference && InventorySlotClass)
	{
		InventoryWrapBox->ClearChildren();

		for (UItemBase* const& InventoryItem : InventoryReference->GetInventoryContents())
		{
			UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
			ItemSlot->SetItemReference(InventoryItem);

			InventoryWrapBox->AddChildToWrapBox(ItemSlot);
		}

		SetInfoText();
	}
}

bool UInventorypanel::NativeOnDrop(const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (ItemDragDrop->SourceItem && InventoryReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected an item drop on InventoryPanel."))

			// returning true will stop the drop operation at this widget
			return true;
	}
	// returning false will cause the drop operation to fall through to underlying widgets (if any)
	return false;
}
