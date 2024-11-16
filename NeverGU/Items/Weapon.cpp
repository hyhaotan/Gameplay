// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Items/Weapon.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	ItemType = Weapon;
	bIsItem = false;
	bHoldable = true;
	EquippedSocketName = "EquipGun";
	AimDownSightCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("AimDownSightsCamera"));
	AimDownSightCamera->SetupAttachment(GetSkeletalMesh());
	AimDownSightCamera->SetWorldScale3D(FVector(0.1, 0.1, 0.1));
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}
