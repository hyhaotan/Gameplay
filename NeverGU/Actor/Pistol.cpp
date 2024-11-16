// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Actor/Pistol.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APistol::APistol()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PistolMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PistolMesh"));
	RootComponent = PistolMesh;
	PistolMesh->SetRelativeScale3D(FVector(0.1f));
	PistolMesh->GetSocketByName(FName("EquipPistol"));

	PistolBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PistolBox"));
	PistolBox->SetupAttachment(PistolMesh);
	PistolBox->SetRelativeScale3D(FVector(6.0f, 11.0f, 6.0f));

	AimPistolCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("AimPistolCamera"));
	AimPistolCamera->SetupAttachment(PistolMesh);
	AimPistolCamera->SetWorldScale3D(FVector(1.0f));

	bIsPickedUp = false;
}

// Called when the game starts or when spawned
void APistol::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APistol::PickupPistol()
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

// Method to drop the flashlight
void APistol::DropPistol()
{
	if (bIsPickedUp)
	{
		// Re-enable collision so the flashlight becomes interactable again
		SetActorEnableCollision(true);

		// Enable physics so the flashlight can fall to the ground
		PistolMesh->SetSimulatePhysics(true);

		// Get the player character and the forward direction
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (PlayerCharacter)
		{
			// Calculate drop location in front of the player
			FVector DropLocation = PlayerCharacter->GetActorLocation() + PlayerCharacter->GetActorForwardVector() * 200.0f;
			SetActorLocation(DropLocation);

			// Optionally apply a small impulse for realism
			FVector ImpulseDirection = PlayerCharacter->GetActorForwardVector() * 500.0f;
			PistolMesh->AddImpulse(ImpulseDirection);
		}

		// Mark flashlight as no longer picked up
		bIsPickedUp = false;
	}
}

