// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NeverGU/Data/InfoProductData.h"
#include "ProductBase.generated.h"

UCLASS()
class NEVERGU_API UProductBase : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, Category = "Product Data")
	FName ID;

	UPROPERTY(VisibleAnywhere, Category = "Product Data")
	EProductType ProductType;

	UPROPERTY(VisibleAnywhere, Category = "Product Data")
	EProductQuality ProductQuality;

	UPROPERTY(VisibleAnywhere, Category = "Product Data")
	FProductTextData ProductTextData;

	UPROPERTY(VisibleAnywhere, Category = "Product Data")
	FProductAssetData ProductAssetData;

	UPROPERTY(VisibleAnywhere, Category = "Product Data")
	FProductQuantityInfo ProductQuantityInfo;

	UFUNCTION(Category = "Product")
	UProductBase* CreateProductCopy() const;

protected:
	bool operator == (const FName& OtherID) const
	{
		return this->ID == OtherID;
	}
};
