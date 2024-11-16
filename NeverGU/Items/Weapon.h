// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NeverGU/Items/Item.h"
#include "Camera/CameraComponent.h"
#include "Weapon.generated.h"


/**
 * 
 */
UCLASS()
class NEVERGU_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "Items")
	UParticleSystem* MuzzleParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* AimDownSightCamera;
};
