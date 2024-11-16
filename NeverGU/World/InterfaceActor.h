// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NeverGU/Interfaces/InteractionInterface.h"
#include "InterfaceActor.generated.h"

class AThirdPersonCharacter;

UCLASS()
class NEVERGU_API AInterfaceActor : public AActor,public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInterfaceActor();

protected:

	UPROPERTY(EditAnywhere, Category = "Actor")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditInstanceOnly, Category = "Actor")
	FInteractableData InstanceInteractableData;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void InteractItem(AThirdPersonCharacter* PlayerCharacter) override;
};
