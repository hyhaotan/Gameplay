// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pistol.generated.h"

UENUM(BlueprintType)
enum class EPistolTypes : uint8
{
	Other UMETA(DisplayName = "Other"),
	DesertEagle UMETA(DisplayName = "Desert Eagle"),
	Glock17 UMETA(DisplayName = "Glock 17")
};

UCLASS()
class NEVERGU_API APistol : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APistol();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void PickupPistol();
	bool IsPickedUp() const { return bIsPickedUp; }

	void DropPistol();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pistol")
	EPistolTypes PistolType = EPistolTypes::Other;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol")
	class UStaticMeshComponent* PistolMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pistol")
	class UBoxComponent* PistolBox;

	bool bIsPickedUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PickupAnimation")
	UAnimMontage* PickupMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* AimPistolCamera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pistol")
	UParticleSystem* MuzzleParticle;
};
