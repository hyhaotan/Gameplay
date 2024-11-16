// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEVERGU_API USpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpawnerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "Sinh đối tượng")
	TSubclassOf<AActor> ActorToSpawn;

	void SpawnChild();

private:
	int32 MaxChildren = 3;
	int32 CurrentChildren = 0;

	void DespawnChild(AActor* Child);

	FTimerHandle DespawnTimer;

	/*void AttachActor(AActor* ParentActor, AActor* ChildActor, USceneComponent* AttachComponent);*/
};
