// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NeverGU/Items/Item.h"
#include "health.generated.h"

/**
 * 
 */
UCLASS()
class NEVERGU_API Ahealth : public AItem
{
	GENERATED_BODY()
	
public:
	Ahealth();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items")
	UParticleSystem* HealthParticle;
};
