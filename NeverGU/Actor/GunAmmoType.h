// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunAmmoType.generated.h"

UENUM(BlueprintType)
enum class EAmmoTypes : uint8
{
	Other UMETA(Display = "Other"),
	AKM  UMETA(DisplayName = "AKM"),
	DesertEagle UMETA(DisplayName = "Desert Eagle"),
	Glock17 UMETA(DisplayName = "Glock 17")
};

UCLASS()
class NEVERGU_API AGunAmmoType : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunAmmoType();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void PickupPistol();
	bool IsPickedUp() const { return bIsPickedUp; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PickupAnimation")
	UAnimMontage* PickupMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ammo")
	EAmmoTypes AmmoType = EAmmoTypes::Other;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo")
	class UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	class UBoxComponent* AmmoBox;

	bool bIsPickedUp;
};
