// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Actor/SpawnerComponent.h"

// Sets default values for this component's properties
USpawnerComponent::USpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < MaxChildren; i++)
	{
		SpawnChild();
	}
	
}


void USpawnerComponent::SpawnChild()
{
	if (ActorToSpawn && CurrentChildren < MaxChildren)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

		CurrentChildren++;
		GetWorld()->GetTimerManager().SetTimer(DespawnTimer, [this, SpawnedActor]() {
			DespawnChild(SpawnedActor);
			}, 5.0f, false);  // Tự hủy sau 5 giây
	}
}

void USpawnerComponent::DespawnChild(AActor* Child)
{
	if (Child)
	{
		Child->Destroy();
		CurrentChildren--;
	}
}

//void AttachActor(AActor* ParentActor, AActor* ChildActor, USceneComponent* AttachComponent)
//{
//	if (ParentActor && ChildActor && AttachComponent)
//	{
//		ChildActor->AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
//	}
//}
