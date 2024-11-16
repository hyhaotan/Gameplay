#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InfoProductData.generated.h"

UENUM(BlueprintType)
enum class EProductQuality : uint8
{
	Shoddy UMETA(DisplayName = "Shoddy"),
	Common UMETA(DisplayName = "Common"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Masterwork UMETA(DisplayName = "Masterwork"),
	Grandmaster UMETA(DisplayName = "Grandmaster")
};

UENUM(BlueprintType)
enum class EProductType : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
	Item UMETA(DisplayName = "Item"),
	Armor UMETA(DisplayName = "Armor"),
	Health UMETA(DisplayName = "Health"),
	Other UMETA(DisplayName = "Other")
};

USTRUCT(BlueprintType)
struct FProductTextData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Text Data")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Text Data")
	FText Description;
};

USTRUCT(BlueprintType)
struct FProductAssetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Asset Data")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Asset Data")
	UStaticMesh* Mesh;
};

USTRUCT(BlueprintType)
struct FProductQuantityInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Quantity Info")
	int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Quantity Info")
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Quantity Info")
	float Weight;
};

USTRUCT(BlueprintType)
struct FProductData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Data")
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Data")
	EProductType ProductType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Data")
	EProductQuality ProductQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Data")
	FProductTextData ProductTextData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Data")
	FProductAssetData ProductAssetData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Product Data")
	FProductQuantityInfo ProductQuantityInfo;
};
