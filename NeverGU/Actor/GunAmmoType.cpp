// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Actor/GunAmmoType.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGunAmmoType::AGunAmmoType()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	RootComponent = AmmoMesh;
	AmmoMesh->SetRelativeScale3D(FVector(1.0f));

	AmmoBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AmmoBox"));
	AmmoBox->SetupAttachment(AmmoMesh);
	AmmoBox->SetRelativeScale3D(FVector(1.0f));

	bIsPickedUp = false;
}

// Called when the game starts or when spawned
void AGunAmmoType::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGunAmmoType::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGunAmmoType::PickupPistol()
{
	if (!bIsPickedUp)
	{
		bIsPickedUp = true;
		// Disable collision so the flashlight is no longer interactable
		SetActorEnableCollision(false);

		// Check if the pickup animation montage is set
		if (PickupMontage)
		{
			// Get the player character and cast it to your custom character class
			AThirdPersonCharacter* PlayerCharacter = Cast<AThirdPersonCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());

			// If the cast is successful and the player character exists
			if (PlayerCharacter)
			{
				// Play the pickup montage on the player character
				PlayerCharacter->PlayAnimMontage(PickupMontage);
			}
		}
	}
}
