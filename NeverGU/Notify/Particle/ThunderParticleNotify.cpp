// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Notify/Particle/ThunderParticleNotify.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

void UThunderParticleNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp)
    {
        AActor* Owner = MeshComp->GetOwner();
        if (Owner)
        {

            FVector SpawnLocation = Owner->GetActorLocation() +  Owner->GetActorForwardVector() + 500.0f;;// Adjust the spawn location if necessary
            FRotator SpawnRotation = Owner->GetActorRotation();

            // Check if the particle system is valid
            if (ParticleEffect)
            {
                UGameplayStatics::SpawnEmitterAtLocation(Owner->GetWorld(), ParticleEffect, SpawnLocation, SpawnRotation, true);
            }
        }
    }
}
