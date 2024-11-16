// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Air_Plane.generated.h"

UCLASS()
class NEVERGU_API AAir_Plane : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAir_Plane();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	USkeletalMeshComponent* PlaneMesh;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector PlaneEndPoint;

	void MovePlaneTowardsEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Plane")
	float PlaneSpeed;
};
