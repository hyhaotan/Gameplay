// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NeverGU/Data/InfoProductData.h"
#include "CartProductWidget.generated.h"

class UImage;
class UBorder;
class UButton;
class UTextBlock;

UCLASS()
class NEVERGU_API UCartProductWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // StoreItem representing the product in the cart
    UPROPERTY(VisibleAnywhere)
    class UStoreItemWidget* StoreItem;

    UPROPERTY(VisibleAnywhere)
    class UProductBase* ProductData;

    // Product window associated with this cart product
    UPROPERTY(VisibleAnywhere)
    class UProductWindowWidget* ParentWidget;

    // Sets up the product widget with the provided product data
    void SetupProductWidget();

public:
    // UI Elements bound in the Blueprint
    UPROPERTY(meta = (BindWidget))
    UBorder* ProductBorder;

    UPROPERTY(meta = (BindWidget))
    UImage* ImageProduct;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NameProduct;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* AmountProduct;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* PriceProduct;

    UPROPERTY(meta = (BindWidget))
    UButton* RemoveProductButton;

private:
    // Override for widget construction logic
    virtual void NativeConstruct() override;

    // Function to remove the product from the cart
    void RemoveProduct();

    // Update the visual representation of the product border based on quality
    void UpdateProductVisuals(EProductQuality ProductQuality);

    // Update product details in the widget
    void UpdateProductDetails();
};
