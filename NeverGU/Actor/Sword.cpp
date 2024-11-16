// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Actor/Sword.h"
#include "Components/StaticMeshComponent.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASword::ASword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;
	SwordMesh->SetRelativeScale3D(FVector(0.5f));
	SwordMesh->GetSocketByName(FName("EquipSword"));

	Skill1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Skill1"));
	Skill1->SetupAttachment(RootComponent);
	Skill1->bAutoActivate = false; // Prevent auto activation
	Skill1->SetRelativeScale3D(FVector(2.0f));

	SwordBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordBox"));
	SwordBox->SetupAttachment(SwordMesh);
	SwordBox->SetRelativeScale3D(FVector(1.0f, 1.0f, 4.0f));

	bIsPickedUp = false;
}

// Called when the game starts or when spawned
void ASword::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASword::PickupSword()
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
void ASword::DropSword()
{
	if (bIsPickedUp)
	{
		// Re-enable collision so the flashlight becomes interactable again
		SetActorEnableCollision(true);

		// Enable physics so the flashlight can fall to the ground
		SwordMesh->SetSimulatePhysics(true);

		// Get the player character and the forward direction
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (PlayerCharacter)
		{
			// Calculate drop location in front of the player
			FVector DropLocation = PlayerCharacter->GetActorLocation() + PlayerCharacter->GetActorForwardVector() * 200.0f;
			SetActorLocation(DropLocation);

			// Optionally apply a small impulse for realism
			FVector ImpulseDirection = PlayerCharacter->GetActorForwardVector() * 500.0f;
			SwordMesh->AddImpulse(ImpulseDirection);
		}

		// Mark flashlight as no longer picked up
		bIsPickedUp = false;
	}
}

void ASword::SpawnSkill1(FVector SkillLocation, FRotator SkillRotation)
{
	if (Skill1)
	{
		// Set the particle system location and rotation passed by the character
		Skill1->SetWorldLocation(SkillLocation);
		Skill1->SetWorldRotation(SkillRotation);

		// Activate the particle system
		Skill1->ActivateSystem();

		UE_LOG(LogTemp, Log, TEXT("Skill 1 spawned at location: %s"), *SkillLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No particle system found for Skill1!"));
	}
}
