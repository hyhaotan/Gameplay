// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum EItemtypes
{
	None,
	Weapon,
	HealthPotion,
	Throwable,
	Armor,
	Backpack,
	Ammo,
	Others
};

UCLASS()
class NEVERGU_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items")
	bool bIsItem = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items")
	bool bIsPickup;

	// Hàm kiểm tra đèn pin có được nhặt hay không
	bool IsPickedUpItem() const { return bIsPickup; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items")
	TEnumAsByte<EItemtypes> ItemType = None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items")
	TArray<TSubclassOf<AItem>> AllChildClasses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items")
	bool bHoldable = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items")
	FName EquippedSocketName = "";

	UFUNCTION()
	void ItemOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	void DropItem();

	void PickupItem();

	//Animation Pick up
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PickupAnimation")
	UAnimMontage* PickupMontage;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Item")
	float FloatSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Item")
	float RotationSpeed = 20.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UStaticMeshComponent* GetStaticMesh()
	{
		return StaticMesh;
	}

	USkeletalMeshComponent* GetSkeletalMesh()
	{
		return SkeletalMesh;
	}


};
