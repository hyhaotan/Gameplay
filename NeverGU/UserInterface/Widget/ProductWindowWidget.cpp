#include "NeverGU/UserInterface/Widget/ProductWindowWidget.h"
#include "NeverGU/Items/ProductBase.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "NeverGU/UserInterface/Widget/CartProductWidget.h"
#include "NeverGU/UserInterface/Widget/StoreItemWidget.h"
#include "Components/VerticalBox.h"

void UProductWindowWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!ProductRef)
    {
        UE_LOG(LogTemp, Warning, TEXT("ProductReference is null in UProductWindowWidget::NativeConstruct"));
        return;
    }

    CurrentProductQuantity = ProductRef->ProductQuantityInfo.Quantity;

    UpdateProductVisuals();

    if (ProductText)
    {
        ProductText->SetText(ProductRef->ProductTextData.Name);
    }

    if (ProductIcon && ProductRef->ProductAssetData.Icon)
    {
        ProductIcon->SetBrushFromTexture(ProductRef->ProductAssetData.Icon);
    }

    UpdateTextNumber();

    AllFunctionButton();

    bCanAddProduct = false;
}

void UProductWindowWidget::OnIncreaseButtonClicked()
{
    if (CurrentProductQuantity < 100)
    {
        ++CurrentProductQuantity;
        UpdateTextNumber();
    }
}

void UProductWindowWidget::OnDecreaseButtonClicked()
{
    if (CurrentProductQuantity > 0)
    {
        --CurrentProductQuantity;
        UpdateTextNumber();
    }
}

void UProductWindowWidget::UpdateTextNumber()
{
    if (ProductQuantityValue)
    {
        ProductQuantityValue->SetText(FText::AsNumber(CurrentProductQuantity));
        ProductQuantityValue->SetVisibility(CurrentProductQuantity > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UProductWindowWidget::BuyProduct()
{
    // Implement product buying logic
}

void UProductWindowWidget::CancelProduct()
{
    SetVisibility(ESlateVisibility::Collapsed);
}

void UProductWindowWidget::AddToCart()
{
    if (!StoreItem || !StoreItem->ProductReference)
    {
        UE_LOG(LogTemp, Warning, TEXT("StoreItem or ProductReference is invalid in UProductWindowWidget::AddToCart"));
        return;
    }

    if (!UpdateExistingProduct() && !bCanAddProduct)
    {
        AddNewProductToCart();
    }
}

bool UProductWindowWidget::UpdateExistingProduct()
{
    for (UProductBase* ExistingProduct : StoreItem->ProductData)
    {
        if (ExistingProduct->ProductTextData.Name.EqualTo(StoreItem->ProductReference->ProductTextData.Name) &&
            ExistingProduct->ID.IsEqual(StoreItem->ProductReference->ID))
        {
            ExistingProduct->ProductQuantityInfo.Quantity += StoreItem->ProductReference->ProductQuantityInfo.Quantity;
            ExistingProduct->ProductQuantityInfo.Price += StoreItem->ProductReference->ProductQuantityInfo.Price;
            return true;
        }
    }
    return false;
}

void UProductWindowWidget::AddNewProductToCart()
{
    UProductBase* ProductCopy = StoreItem->ProductReference->CreateProductCopy();

    if (!ProductCopy)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create a product copy in UProductWindowWidget::AddNewProductToCart"));
        return;
    }

    StoreItem->ProductData.Add(ProductCopy);
    AddCartProductWidget(ProductCopy);
    bCanAddProduct = true;
}

void UProductWindowWidget::AddCartProductWidget(UProductBase* NewProduct)
{
    if (!StoreItem->CartProductSlot || !CartProductWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("CartProductSlot or CartProductWidgetClass is null in UProductWindowWidget::AddCartProductWidget"));
        return;
    }

    UCartProductWidget* NewCartProductWidget = CreateWidget<UCartProductWidget>(this, CartProductWidgetClass);
    if (NewCartProductWidget)
    {
        NewCartProductWidget->SetupProductWidget();
        StoreItem->CartProductSlot->AddChild(NewCartProductWidget);
        CartProduct->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create CartProductWidget in UProductWindowWidget::AddCartProductWidget"));
    }
}

void UProductWindowWidget::AllFunctionButton()
{
    if (BuyProductButton)
    {
        BuyProductButton->OnClicked.AddDynamic(this, &UProductWindowWidget::BuyProduct);
        BuyProductButton->SetCursor(EMouseCursor::Hand);
    }

    if (CancelProductButton)
    {
        CancelProductButton->OnClicked.AddDynamic(this, &UProductWindowWidget::CancelProduct);
        CancelProductButton->SetCursor(EMouseCursor::Hand);
    }

    if (AddProductButton)
    {
        AddProductButton->OnClicked.AddDynamic(this, &UProductWindowWidget::AddToCart);
        AddProductButton->SetCursor(EMouseCursor::Hand);
    }

    if (IncreaseButton)
    {
        IncreaseButton->OnClicked.AddDynamic(this, &UProductWindowWidget::OnIncreaseButtonClicked);
        IncreaseButton->SetCursor(EMouseCursor::Hand);
    }

    if (DecreaseButton)
    {
        DecreaseButton->OnClicked.AddDynamic(this, &UProductWindowWidget::OnDecreaseButtonClicked);
        DecreaseButton->SetCursor(EMouseCursor::Hand);
    }
}

void UProductWindowWidget::UpdateProductVisuals()
{
    if (!ProductBorder || !ProductText || !ProductRef)
    {
        UE_LOG(LogTemp, Warning, TEXT("One of ProductBorder, ProductText, or ProductRef is null in UProductWindowWidget::UpdateProductVisuals"));
        return;
    }

    SetProductAppearance(ProductRef->ProductQuality);
}

void UProductWindowWidget::SetProductAppearance(EProductQuality Quality)
{
    FLinearColor BorderColor;
    FSlateColor TextColor;

    switch (Quality)
    {
    case EProductQuality::Shoddy:
        BorderColor = FLinearColor::Gray;
        TextColor = FSlateColor(FLinearColor::Gray);
        break;
    case EProductQuality::Common:
        BorderColor = FLinearColor::Green;
        TextColor = FSlateColor(FLinearColor::Green);
        break;
    case EProductQuality::Rare:
        BorderColor = FLinearColor::Blue;
        TextColor = FSlateColor(FLinearColor::Blue);
        break;
    case EProductQuality::Epic:
        BorderColor = FLinearColor(1.0f, 0.7f, 0.6f);
        TextColor = FSlateColor(FLinearColor(1.0f, 0.7f, 0.6f));
        break;
    case EProductQuality::Masterwork:
        BorderColor = FLinearColor(1.0f, 0.45f, 0.0f);
        TextColor = FSlateColor(FLinearColor(1.0f, 0.45f, 0.0f));
        break;
    case EProductQuality::Grandmaster:
        BorderColor = FLinearColor::Red;
        TextColor = FSlateColor(FLinearColor::Red);
        break;
    default:
        BorderColor = FLinearColor::White;
        TextColor = FSlateColor(FLinearColor::White);
        break;
    }

    ProductBorder->SetBrushColor(BorderColor);
    ProductText->SetColorAndOpacity(TextColor);
}
