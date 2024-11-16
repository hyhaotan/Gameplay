// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Actor/Air_Plane.h"
#include "NeverGU/GameMode/BaseGameMode.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"

// Sets default values
AAir_Plane::AAir_Plane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlaneMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlaneMesh"));
	RootComponent = PlaneMesh;
	bReplicates = true;
	AActor::SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AAir_Plane::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		return;
	}

	FTimerHandle LocalHandle1;
	GetWorld()->GetTimerManager().SetTimer(LocalHandle1, this, &AAir_Plane::MovePlaneTowardsEnd, 0.001, true);

}

// Called every frame
void AAir_Plane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAir_Plane::MovePlaneTowardsEnd()
{
	SetActorLocation(FMath::VInterpTo(GetActorLocation(), PlaneEndPoint, GetWorld()->DeltaTimeSeconds, PlaneSpeed));
	if (FVector::Dist(GetActorLocation(),PlaneEndPoint) < 6000)
	{
		Destroy();
	}
	if (FVector::Dist(GetActorLocation(), PlaneEndPoint) < 12000)
	{
		if (ABaseGameMode* BaseGameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode()))
		{
			for (auto& Controller : BaseGameMode->ControllersRef)
			{
				if (Controller)
				{
					if (AThirdPersonCharacter* CharacterRef = Cast<AThirdPersonCharacter>(Controller->GetPawn()))
					{
						CharacterRef->SrvJumpFromPlane();
					}
				}
			}
		}
	}
}

