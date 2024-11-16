// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TorchActor.generated.h"

UCLASS()
class NEVERGU_API ATorchActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATorchActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    // Particle system component for the flame
    UPROPERTY(VisibleAnywhere)
    class UParticleSystemComponent* FlameParticle;

    // Timer for burning time
    FTimerHandle BurnoutTimerHandle;

    // Burnout time in seconds
    float BurnoutTime;

    // Flag for checking if torch is doused
    bool bIsDoused;

    // Method to turn off the flame
    void BurnOut();

public:
    // Event to add fuel
    UFUNCTION(BlueprintCallable, Category = "Torch")
    void AddFuel(float ExtraTime);

    // Event to douse the torch
    UFUNCTION(BlueprintCallable, Category = "Torch")
    void DouseWithWater();
};
