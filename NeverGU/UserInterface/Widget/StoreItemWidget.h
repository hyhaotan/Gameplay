#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NeverGU/Data/InfoProductData.h"
#include "StoreItemWidget.generated.h"

// Forward declarations
class UButton;
class UVerticalBox;
class UDataTable;
class UProductBase;
class UProductWindowWidget;
class UBorder;
class UImage;
class UCartProductWidget;
class UTextBlock;

// The StoreItemWidget class for managing the store UI
UCLASS()
class NEVERGU_API UStoreItemWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Cart Product")
    TSubclassOf<UCartProductWidget> CartProductClass;

    UPROPERTY(EditDefaultsOnly, Category = "Store")
    TSubclassOf<UProductWindowWidget> ProductWindowWidgetClass;

    // Buttons for product categories
    UPROPERTY(meta = (BindWidget))
    UButton* WeaponButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ItemButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ArmorButton;

    UPROPERTY(meta = (BindWidget))
    UButton* HealthButton;

    UPROPERTY(meta = (BindWidget))
    UButton* OtherButton;

    UPROPERTY(meta = (BindWidget))
    UButton* CancelButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* AmountProductText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SumPriceProductText;

    // UI elements
    UPROPERTY(meta = (BindWidget))
    UBorder* ProductBorder;

    UPROPERTY(meta = (BindWidget))
    UImage* ProductIcon;

    // Vertical boxes for product categories
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* WeaponProductBox;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* ItemProductBox;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* ArmorProductBox;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* HealthProductBox;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* OtherProductBox;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* CartProductSlot;

    // Reference to product data
    UPROPERTY(VisibleAnywhere, Category = "Info Product")
    UProductBase* ProductReference;

    FORCEINLINE UProductBase* GetProductBase() { return ProductReference; };

    // Track the visibility state of the ProductWindowWidget
    bool bIsProductWindowVisible = false;

    UPROPERTY(VisibleAnywhere, Category = "AmountProduct")
    int32 AmountProduct = 0;

    UPROPERTY(VisibleAnywhere, Category = "AmountProduct")
    int32 SumPriceProduct = 0;

    // Data table for product initialization
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Product Initialization")
    UDataTable* ProductDataClass;

    UPROPERTY()
    UProductWindowWidget* ProductWindowWidget;

    UPROPERTY(EditInstanceOnly, Category = "Product | Product Initialization")
    FDataTableRowHandle ProductRowHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cart")
    TArray<UProductBase*> ProductData;

    // Function declarations for handling button clicks
    UFUNCTION()
    void WeaponProduct();

    UFUNCTION()
    void ItemProduct();

    UFUNCTION()
    void ArmorProduct();

    UFUNCTION()
    void HealthProduct();

    UFUNCTION()
    void OtherProduct();

    UFUNCTION()
    void CancelStore();

    UFUNCTION()
    int32 CalculateTotalCost();

    // Function to show the specified product box
    void ShowProductBox(UVerticalBox* ProductBox);

    // Function to initialize product data
    void InitializeProduct();

    // Function to bind button handlers
    void BindButtonHandlers();

    // Function to set initial visibility of product boxes
    void SetInitialProductBoxVisibility();

    // Function to hide the product window
    void HideProductWindow();

    // Function to update the product border color based on quality
    void UpdateProductBorderColor();

private:
    // Override functions from UUserWidget
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    // Function to create and show the product window
    void CreateAndShowProductWindow();

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
