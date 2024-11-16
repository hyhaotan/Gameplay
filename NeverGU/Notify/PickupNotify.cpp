// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Notify/PickupNotify.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"

void UPickupNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		// Cast to your character class
		AThirdPersonCharacter* Character = Cast<AThirdPersonCharacter>(MeshComp->GetOwner());
		if (Character)
		{
			// Call the PickupSword function
			Character->PickupSword();
		}
	}
}