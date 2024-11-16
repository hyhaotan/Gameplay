#include "NeverGU/UserInterface/Widget/StoreItemWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "NeverGU/Items/ProductBase.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "NeverGU/UserInterface/Widget/ProductWindowWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "NeverGU/UserInterface/Widget/CartProductWidget.h"
#include "Components/TextBlock.h"

void UStoreItemWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    InitializeProduct();
    SetInitialProductBoxVisibility();
    // Set initial visibility for product boxes
}

void UStoreItemWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind button actions and set the mouse cursor
    BindButtonHandlers();
    CalculateTotalCost();
    // Update UI based on the product reference if it exists

    UpdateProductBorderColor();
    if(ProductReference)
    {
        ProductIcon->SetBrushFromTexture(ProductReference->ProductAssetData.Icon);
    }
}

void UStoreItemWidget::SetInitialProductBoxVisibility()
{
    // Show the weapon product box by default and hide others
    WeaponProductBox->SetVisibility(ESlateVisibility::Visible);
    ItemProductBox->SetVisibility(ESlateVisibility::Collapsed);
    ArmorProductBox->SetVisibility(ESlateVisibility::Collapsed);
    HealthProductBox->SetVisibility(ESlateVisibility::Collapsed);
    OtherProductBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UStoreItemWidget::BindButtonHandlers()
{
    if (WeaponButton)
    {
        WeaponButton->OnClicked.AddDynamic(this, &UStoreItemWidget::WeaponProduct);
        WeaponButton->SetCursor(EMouseCursor::Hand);
    }

    if (ItemButton)
    {
        ItemButton->OnClicked.AddDynamic(this, &UStoreItemWidget::ItemProduct);
        ItemButton->SetCursor(EMouseCursor::Hand);
    }

    if (ArmorButton)
    {
        ArmorButton->OnClicked.AddDynamic(this, &UStoreItemWidget::ArmorProduct);
        ArmorButton->SetCursor(EMouseCursor::Hand);
    }

    if (HealthButton)
    {
        HealthButton->OnClicked.AddDynamic(this, &UStoreItemWidget::HealthProduct);
        HealthButton->SetCursor(EMouseCursor::Hand);
    }

    if (OtherButton)
    {
        OtherButton->OnClicked.AddDynamic(this, &UStoreItemWidget::OtherProduct);
        OtherButton->SetCursor(EMouseCursor::Hand);
    }

    if (CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &UStoreItemWidget::CancelStore);
        CancelButton->SetCursor(EMouseCursor::Hand);
    }
}

void UStoreItemWidget::UpdateProductBorderColor()
{
    // Set the border color based on the product's quality
    if (ProductReference)
    {
        switch (ProductReference->ProductQuality)
        {
        case EProductQuality::Shoddy:
            ProductBorder->SetBrushColor(FLinearColor::Gray);
            break;
        case EProductQuality::Common:
            ProductBorder->SetBrushColor(FLinearColor::Green);
            break;
        case EProductQuality::Rare:
            ProductBorder->SetBrushColor(FLinearColor::Blue);
            break;
        case EProductQuality::Epic:
            ProductBorder->SetBrushColor(FLinearColor(1.0f, 0.7f, 0.6f));
            break;
        case EProductQuality::Masterwork:
            ProductBorder->SetBrushColor(FLinearColor(1.0f, 0.45f, 0.0f));
            break;
        case EProductQuality::Grandmaster:
            ProductBorder->SetBrushColor(FLinearColor::Red);
            break;
        default:
            ProductBorder->SetBrushColor(FLinearColor::White);
            break;
        }
    }
}

void UStoreItemWidget::InitializeProduct()
{
    if (!ProductRowHandle.IsNull())
    {
        // Ensure ProductDataClass is already a UDataTable* and not a UClass*
        if (ProductDataClass) // Assuming ProductDataClass is a UDataTable*
        {
            // Use the ProductDataClass directly as a UDataTable*
            FProductData* ProductDatas = ProductRowHandle.GetRow<FProductData>(ProductRowHandle.RowName.ToString());

            if (ProductDatas)
            {
                ProductReference = NewObject<UProductBase>(this, UProductBase::StaticClass());

                ProductReference->ID = ProductDatas->ID;
                ProductReference->ProductType = ProductDatas->ProductType;
                ProductReference->ProductQuality = ProductDatas->ProductQuality;
                ProductReference->ProductTextData = ProductDatas->ProductTextData;
                ProductReference->ProductAssetData = ProductDatas->ProductAssetData;
                ProductReference->ProductQuantityInfo = ProductDatas->ProductQuantityInfo;
            }
        }
    }
}

FReply UStoreItemWidget::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    // Check if the left mouse button was clicked
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // Check if the ProductBorder is hovered over
        if (ProductBorder && ProductBorder->IsHovered())
        {
            // If the product window is not currently visible, create and show it
            if (!bIsProductWindowVisible)
            {
                CreateAndShowProductWindow();
            }
            return FReply::Handled(); // Indicate that the event was handled
        }
        // If the product window is visible and the user clicks outside of it
        else if (bIsProductWindowVisible)
        {
            // Hide the product window
            HideProductWindow();
            return FReply::Handled(); // Indicate that the event was handled
        }
    }

    // If the event was not handled, pass it to the base class
    return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
}

void UStoreItemWidget::CreateAndShowProductWindow()
{
    if (UWorld* World = GetWorld())
    {
        // Only create a new widget if it doesn't already exist
        if (!ProductWindowWidget && ProductWindowWidgetClass)
        {
            // Create the widget instance
            ProductWindowWidget = CreateWidget<UProductWindowWidget>(World, ProductWindowWidgetClass);
            if (ProductWindowWidget)
            {
                // Add the widget to the viewport
                ProductWindowWidget->AddToViewport();
                FVector2D Position(800, 200); // Adjust as necessary for layout
                ProductWindowWidget->SetPositionInViewport(Position, true);
                bIsProductWindowVisible = true; // Set the visibility flag
            }
        }
    }
}

void UStoreItemWidget::HideProductWindow()
{
    if (ProductWindowWidget)
    {
        ProductWindowWidget->RemoveFromParent(); // Remove the widget from the viewport
        ProductWindowWidget = nullptr;
        bIsProductWindowVisible = false;
    }
}

// Show the appropriate product box when a button is clicked
void UStoreItemWidget::WeaponProduct() { ShowProductBox(WeaponProductBox); }
void UStoreItemWidget::ItemProduct() { ShowProductBox(ItemProductBox); }
void UStoreItemWidget::ArmorProduct() { ShowProductBox(ArmorProductBox); }
void UStoreItemWidget::HealthProduct() { ShowProductBox(HealthProductBox); }
void UStoreItemWidget::OtherProduct() { ShowProductBox(OtherProductBox); }

void UStoreItemWidget::CancelStore()
{
    // Collapse the store widget when the cancel button is clicked
    this->SetVisibility(ESlateVisibility::Collapsed);
    UGameplayStatics::GetPlayerController(this, 0)->SetShowMouseCursor(false);
}

int32 UStoreItemWidget::CalculateTotalCost()
{
    // Ensure SumPriceProduct is initialized
    SumPriceProduct = 0; // Reset the sum before calculation

    // Ensure ProductData has elements
    if (ProductData.Num() > 0)
    {
        for (UProductBase* Product : ProductData) // Iterate over the ProductData array
        {
            if (Product) // Check if the product pointer is valid (non-null)
            {
                SumPriceProduct += Product->ProductQuantityInfo.Price; // Sum the prices
            }
        }
    }

    // Update the text display of SumPriceProduct
    SumPriceProductText->SetText(FText::AsNumber(SumPriceProduct));

    // Return the total sum
    return SumPriceProduct;
}



void UStoreItemWidget::ShowProductBox(UVerticalBox* ProductBox)
{
    if (ProductBox)
    {
        // Hide all product boxes
        WeaponProductBox->SetVisibility(ESlateVisibility::Collapsed);
        ItemProductBox->SetVisibility(ESlateVisibility::Collapsed);
        ArmorProductBox->SetVisibility(ESlateVisibility::Collapsed);
        HealthProductBox->SetVisibility(ESlateVisibility::Collapsed);
        OtherProductBox->SetVisibility(ESlateVisibility::Collapsed);

        // Show the selected product box
        ProductBox->SetVisibility(ESlateVisibility::Visible);
    }
}

#if WITH_EDITOR
void UStoreItemWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(FDataTableRowHandle, RowName))
    {
        if (!ProductRowHandle.IsNull())
        {
            if (ProductDataClass)
            {
                UDataTable* ProductDataTable = ProductDataClass;

                FProductData* ProductDataRef = ProductRowHandle.GetRow<FProductData>(ProductRowHandle.RowName.ToString());
                if (ProductDataRef)
                {
                
                }
            }
        }
    }
}
#endif
