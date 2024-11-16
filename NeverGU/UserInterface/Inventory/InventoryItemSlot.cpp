// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/UserInterface/Inventory/InventoryItemSlot.h"
#include "NeverGU/UserInterface/Inventory/InventoryToolTip.h"
#include "NeverGU/Items/ItemBase.h"
#include "NeverGU/UserInterface/Inventory/DragItemVisual.h"
#include "NeverGU/UserInterface/Inventory/ItemDragDropOperation.h"
#include "NeverGU/UserInterface/Inventory/OptionsBarInventory.h"
#include "NeverGU/UserInterface/Inventory/Inventorypanel.h"


#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ToolTipClass)
	{
		UInventoryToolTip* ToolTip = CreateWidget<UInventoryToolTip>(this, ToolTipClass);
		ToolTip->InventorySlotBeingHovered = this;
		SetToolTip(ToolTip);
	}
}

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemReference)
	{
		//rarity of the item
		switch (ItemReference->ItemQuality)
		{
		case EItemQuality::Shoddy:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
		case EItemQuality::Common:
			ItemBorder->SetBrushColor(FLinearColor::White);
			break;
		case EItemQuality::Rare:
			ItemBorder->SetBrushColor(FLinearColor::Green);
			break;
		case EItemQuality::Epic:
			ItemBorder->SetBrushColor(FLinearColor::Blue);
			break;
		case EItemQuality::Quality:
			ItemBorder->SetBrushColor(FLinearColor(0.4f,0.2f,0.6f)); // Purple
			break;
		case EItemQuality::Masterwork:
			ItemBorder->SetBrushColor(FLinearColor(1.0f, 0.45f, 0.0f)); // Orange
			break;
		case EItemQuality::Grandmaster:
			ItemBorder->SetBrushColor(FLinearColor::Red);
			break;
		default:
			break;
		}
		ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);

		if (ItemReference->NumericData.bIsStackable)
		{
			ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	//if the player character Detect Drag the item
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	//if the player character press right mouse button the item
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		if (OptionBarClass)
		{
			if (!ItemOptionWidgetInstance)
			{
				ItemOptionWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), OptionBarClass);

				if (ItemOptionWidgetInstance && ItemBorder)
				{
					// Thêm vào viewport của Inventory Widget (bên trong)
					ItemBorder->AddChild(ItemOptionWidgetInstance);
				}
			}

			if (ItemOptionWidgetInstance)  // Kiểm tra lại trước khi sử dụng
			{
				// Lấy vị trí của vật phẩm (vị trí chuột hoặc widget tương ứng với vật phẩm)
				FVector2D ItemPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

				// Đặt vị trí của `ItemOptionWidgetInstance` theo vị trí chuột
				FVector2D OptionWidgetPosition = ItemPosition - FVector2D(0, ItemOptionWidgetInstance->GetDesiredSize().Y);
				ItemOptionWidgetInstance->SetPositionInViewport(OptionWidgetPosition, true);

				// Hiển thị widget (nếu nó bị ẩn)
				ItemOptionWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			}
		}
		return FReply::Handled();
	}

	return Reply;
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (DragItemVisualClass)
	{
		//when the player character drag item and show item 
		const TObjectPtr<UDragItemVisual> DragVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
		DragVisual->ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());

		ItemReference->NumericData.bIsStackable
			? DragVisual->ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity))
			: DragVisual->ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);

		UItemDragDropOperation* DragItemOperation = NewObject<UItemDragDropOperation>();
		DragItemOperation->SourceItem = ItemReference;
		DragItemOperation->SourceInventory = ItemReference->OwningInventory;

		DragItemOperation->DefaultDragVisual = DragVisual;
		DragItemOperation->Pivot = EDragPivot::TopLeft;

		OutOperation = DragItemOperation;
	}
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry,InDragDropEvent,InOperation);
}
