// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Actor/TorchActor.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ATorchActor::ATorchActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // Create the flame particle system and attach to root
    FlameParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FlameParticle"));
    RootComponent = FlameParticle;

    // Set default values
    BurnoutTime = 10.0f; // Torch burns out in 10 seconds by default
    bIsDoused = false;
}

// Called when the game starts or when spawned
void ATorchActor::BeginPlay()
{
	Super::BeginPlay();
	
    // Start the burnout timer
    GetWorld()->GetTimerManager().SetTimer(BurnoutTimerHandle, this, &ATorchActor::BurnOut, BurnoutTime, false);
}

// Called every frame
void ATorchActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATorchActor::BurnOut()
{
    // Check if the torch has already been doused
    if (!bIsDoused)
    {
        // Deactivate the flame particle system
        if (FlameParticle)
        {
            FlameParticle->Deactivate();
        }
    }
}

void ATorchActor::AddFuel(float ExtraTime)
{
    // If torch is not doused, extend the burnout time
    if (!bIsDoused)
    {
        // Clear the current timer and start a new one with the extended time
        GetWorld()->GetTimerManager().ClearTimer(BurnoutTimerHandle);
        BurnoutTime += ExtraTime;
        GetWorld()->GetTimerManager().SetTimer(BurnoutTimerHandle, this, &ATorchActor::BurnOut, BurnoutTime, false);
    }
}

void ATorchActor::DouseWithWater()
{
    // Douse the torch and prevent it from being relit
    bIsDoused = true;

    // Deactivate the flame immediately
    if (FlameParticle)
    {
        FlameParticle->Deactivate();
    }

    // Clear the timer to stop further burns
    GetWorld()->GetTimerManager().ClearTimer(BurnoutTimerHandle);
}

