// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Items/ProductBase.h"

UProductBase* UProductBase::CreateProductCopy() const
{
	UProductBase* ProductCopy = NewObject<UProductBase>(StaticClass());
	ProductCopy->ID = this->ID;
	ProductCopy->ProductQuality = this->ProductQuality;
	ProductCopy->ProductAssetData = this->ProductAssetData;
	ProductCopy->ProductQuantityInfo = this->ProductQuantityInfo;
	ProductCopy->ProductTextData = this->ProductTextData;
	ProductCopy->ProductType = this->ProductType;
	return ProductCopy;
}

