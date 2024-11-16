// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Items/Item.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	
	RootComponent = BoxCollision;

	StaticMesh->SetupAttachment(BoxCollision);
	SkeletalMesh->SetupAttachment(BoxCollision);

	bReplicates = true;

	bIsPickup = false;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		return;
	}
	if (bIsItem)
	{
		if (UKismetMathLibrary::RandomBool())
		{
			const TSubclassOf<AItem> RandomItemClassToSpawn = AllChildClasses[UKismetMathLibrary::RandomIntegerInRange(0, AllChildClasses.Num() - 1)];
			AItem* ItemRef = GetWorld()->SpawnActor<AItem>(RandomItemClassToSpawn, GetActorTransform());
			if (ItemRef)
			{
				ItemRef->bIsItem = false;
			}
			Destroy();
		}
		else
		{
			Destroy();
		}
	}
	else
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AItem::ItemOverlapped);
	}
}


void AItem::ItemOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if (AThirdPersonCharacter* CharacterRef = Cast<AThirdPersonCharacter>(OtherActor))
	//{
	//	if (bHoldable)
	//	{
	//		// Ẩn vật phẩm khỏi thế giới game
	//		SetActorEnableCollision(false);

	//		if (StaticMesh->GetStaticMesh())
	//		{
	//			StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//		}
	//		AttachToComponent(CharacterRef->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, EquippedSocketName);
	//		CharacterRef->CurrentEquippedItem = ItemType;
	//		CharacterRef->EquippedItem = this;
	//	}
	//	else
	//	{
	//		//Increase count of the item in inventory
	//	}
	//}
}

void AItem::DropItem()
{
	if (bIsPickup)
	{
		// Re-enable collision so the flashlight becomes interactable again
		SetActorEnableCollision(true);

		// Enable physics so the flashlight can fall to the ground
		StaticMesh->SetSimulatePhysics(true);
		SkeletalMesh->SetSimulatePhysics(true);

		// Get the player character and the forward direction
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (PlayerCharacter)
		{
			// Calculate drop location in front of the player
			FVector DropLocation = PlayerCharacter->GetActorLocation() + PlayerCharacter->GetActorForwardVector() * 200.0f;
			SetActorLocation(DropLocation);

			// Optionally apply a small impulse for realism
			FVector ImpulseDirection = PlayerCharacter->GetActorForwardVector() * 500.0f;
			StaticMesh->AddImpulse(ImpulseDirection);
			SkeletalMesh->AddImpulse(ImpulseDirection);
		}

		// Mark flashlight as no longer picked up
		bIsPickup = false;
	}
}

void AItem::PickupItem()
{
	if (!bIsPickup)
	{
		bIsPickup = true;

		// Disable collision so the Item is no longer interactable
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


// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FVector NewLocation = GetActorLocation();
	//FRotator NewRotation = GetActorRotation();
	//float RunningTime = GetGameTimeSinceCreation();
	//float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
	//NewLocation.Z += DeltaHeight * FloatSpeed;			//Scale our height by FloatSpeed
	//float DeltaRotation = DeltaTime * RotationSpeed;	//Rotate by a number of degrees equal to RotationSpeed each second
	//NewRotation.Yaw += DeltaRotation;
	//SetActorLocationAndRotation(NewLocation, NewRotation);

}

