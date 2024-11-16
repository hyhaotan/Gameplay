// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/World/InterfaceActor.h"

// Sets default values
AInterfaceActor::AInterfaceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

}

// Called when the game starts or when spawned
void AInterfaceActor::BeginPlay()
{
	Super::BeginPlay();
	
	InteractableData = InstanceInteractableData;
}

// Called every frame
void AInterfaceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AInterfaceActor::BeginFocus()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(true);
	}
	UE_LOG(LogTemp, Warning, TEXT("Calling BeginFocus override on interface test actor."));
}

void AInterfaceActor::EndFocus()
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(false);
	}
	UE_LOG(LogTemp, Warning, TEXT("Calling EndFocus override on interface test actor."));
}

void AInterfaceActor::BeginInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling BeginInteract override on interface actor."));
}

void AInterfaceActor::EndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling EndInteract override on interface actor."));
}

void AInterfaceActor::InteractItem(AThirdPersonCharacter* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact override on interface actor."));
}

