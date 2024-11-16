// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Zone.generated.h"

USTRUCT(BlueprintType)
struct FZoneInfoStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Zone")
	float DelayStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Zone")
	float MovingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Zone")
	float ScaleToDecrease;
};

UCLASS()
class NEVERGU_API AZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZone();

	UFUNCTION(Server,Reliable)
	void StartMovingZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* ZoneMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Zone")
	TArray<FZoneInfoStruct> ZoneInfoArray;

	UPROPERTY()
	int32 CurrentStepNumber;

	void ReduceZoneSize();
	void StopZone();

	FTimerHandle MovingTimerHandle;
	FTimerHandle Localhandle1;
	FTimerHandle Localhandle2;

	
};
