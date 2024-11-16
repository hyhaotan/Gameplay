#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NeverGU/Data/InfoProductData.h"
#include "ProductWindowWidget.generated.h"

class UProductBase;
class UButton;
class UBorder;
class UImage;
class USizeBox;
class UTextBlock;
class UCartProductWidget;
class UStoreItemWidget;

UCLASS()
class NEVERGU_API UProductWindowWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Reference to the current product
    UPROPERTY()
    UProductBase* ProductRef;

    // Store item for cart interaction
    UPROPERTY(VisibleAnywhere, Category = "Info Product")
    UStoreItemWidget* StoreItem;

    // Widget class for adding items to the cart
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cart Product")
    TSubclassOf<UCartProductWidget> CartProductWidgetClass;

    // UI Elements
    UPROPERTY(meta = (BindWidget))
    UBorder* ProductBorder;

    UPROPERTY(meta = (BindWidget))
    USizeBox* ProductSizeBox;

    UPROPERTY(meta = (BindWidget))
    UImage* ProductIcon;

    UPROPERTY(meta = (BindWidget))
    UButton* IncreaseButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ProductQuantityValue;

    UPROPERTY(meta = (BindWidget))
    UButton* DecreaseButton;

    UPROPERTY(meta = (BindWidget))
    UButton* BuyProductButton;

    UPROPERTY(meta = (BindWidget))
    UButton* AddProductButton;

    UPROPERTY(meta = (BindWidget))
    UButton* CancelProductButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ProductQuantity;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ProductText;

    UPROPERTY()
    UCartProductWidget* CartProduct;

    // Track the current quantity of the product
    int32 CurrentProductQuantity;

    //check add to cart
    bool bCanAddProduct;

    // Override to setup widget
    virtual void NativeConstruct() override;

    // Button click functions
    void OnIncreaseButtonClicked();
    void OnDecreaseButtonClicked();
    void BuyProduct();
    void CancelProduct();
    void AddToCart();
    void AllFunctionButton();

    // Helper functions
    void UpdateTextNumber();
    void UpdateProductVisuals();
    void SetProductAppearance(EProductQuality Quality);
    bool UpdateExistingProduct();
    void AddNewProductToCart();
    void AddCartProductWidget(UProductBase* NewProduct);
};

