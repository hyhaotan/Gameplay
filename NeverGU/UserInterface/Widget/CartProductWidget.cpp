#include "NeverGU/UserInterface/Widget/CartProductWidget.h"
#include "NeverGU/Items/ProductBase.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "NeverGU/UserInterface/Widget/ProductWindowWidget.h"
#include "NeverGU/UserInterface/Widget/StoreItemWidget.h"

void UCartProductWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind the Remove button
    if (RemoveProductButton)
    {
        RemoveProductButton->OnClicked.AddDynamic(this, &UCartProductWidget::RemoveProduct);
        RemoveProductButton->SetCursor(EMouseCursor::Hand);
    }

    // Verify StoreItem and update details if present
    if (StoreItem && StoreItem->ProductReference)
    {
        UpdateProductDetails();
    }

    // Set ParentWidget if not already set
    if (!ParentWidget)
    {
        ParentWidget = Cast<UProductWindowWidget>(GetOuter());
    }
}

void UCartProductWidget::UpdateProductDetails()
{
    if (!StoreItem || !StoreItem->ProductReference) return;

    UProductBase* ProductBase = StoreItem->ProductReference;
    SetupProductWidget();
    UpdateProductVisuals(ProductBase->ProductQuality);
}

void UCartProductWidget::UpdateProductVisuals(EProductQuality ProductQuality)
{
    FLinearColor BorderColor;
    FLinearColor TextColor;

    switch (ProductQuality)
    {
    case EProductQuality::Shoddy:
        BorderColor = FLinearColor::Gray;
        TextColor = FLinearColor::Gray;
        break;
    case EProductQuality::Common:
        BorderColor = FLinearColor::Green;
        TextColor = FLinearColor::Green;
        break;
    case EProductQuality::Rare:
        BorderColor = FLinearColor::Blue;
        TextColor = FLinearColor::Blue;
        break;
    case EProductQuality::Epic:
        BorderColor = FLinearColor(1.0f, 0.7f, 0.6f);
        TextColor = FLinearColor(1.0f, 0.7f, 0.6f);
        break;
    case EProductQuality::Masterwork:
        BorderColor = FLinearColor(1.0f, 0.45f, 0.0f);
        TextColor = FLinearColor(1.0f, 0.45f, 0.0f);
        break;
    case EProductQuality::Grandmaster:
        BorderColor = FLinearColor::Red;
        TextColor = FLinearColor::Red;
        break;
    default:
        BorderColor = FLinearColor::White;
        TextColor = FLinearColor::White;
        break;
    }

    if (ProductBorder) ProductBorder->SetBrushColor(BorderColor);
    if (NameProduct) NameProduct->SetColorAndOpacity(TextColor);
}

void UCartProductWidget::RemoveProduct()
{
    if (!StoreItem || !StoreItem->CartProductSlot)
    {
        UE_LOG(LogTemp, Warning, TEXT("StoreItem or CartProductSlot is invalid!"));
        return;
    }

    // Find the product in ProductData to remove
    for (int32 i = 0; i < StoreItem->ProductData.Num(); ++i)
    {
        UProductBase* ExistingProduct = StoreItem->ProductData[i];

        // Match by name and ID (or any unique identifier)
        if (ExistingProduct->ProductTextData.Name.EqualTo(StoreItem->ProductReference->ProductTextData.Name) &&
            ExistingProduct->ID.IsEqual(StoreItem->ProductReference->ID))
        {
            // Remove product from ProductData array
            StoreItem->ProductData.RemoveAt(i);

            // Remove this widget from the CartProductSlot
            StoreItem->CartProductSlot->RemoveChild(this);

            // Hide the cart if no items are left
            if (StoreItem->ProductData.Num() == 0)
            {
                StoreItem->CartProductSlot->SetVisibility(ESlateVisibility::Hidden);
            }

            UE_LOG(LogTemp, Log, TEXT("Product removed from cart."));
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Product to remove not found in ProductData!"));
}

void UCartProductWidget::SetupProductWidget()
{
    if (NameProduct)
        NameProduct->SetText(FText(ProductData->ProductTextData.Name));

    if (PriceProduct)
        PriceProduct->SetText(FText::AsNumber(ProductData->ProductQuantityInfo.Price));

    if (AmountProduct)
        AmountProduct->SetText(FText::AsNumber(ProductData->ProductQuantityInfo.Quantity));

    if (ImageProduct)
        ImageProduct->SetBrushFromTexture(ProductData->ProductAssetData.Icon);
}
