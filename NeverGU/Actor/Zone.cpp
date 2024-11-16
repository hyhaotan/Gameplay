// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Actor/Zone.h"

// Sets default values
AZone::AZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ZoneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Zone Mesh"));
	RootComponent = ZoneMesh;
	bReplicates = true;
}

void AZone::StartMovingZone_Implementation()
{
	if (CurrentStepNumber < ZoneInfoArray.Num())
	{
		//Start reducing the zone size
		GetWorld()->GetTimerManager().SetTimer(MovingTimerHandle, this, &AZone::ReduceZoneSize, 0.01f, true);
		//Stop the movement of zone after the moving time is reached
		GetWorld()->GetTimerManager().SetTimer(Localhandle1, this, &AZone::StopZone, ZoneInfoArray[CurrentStepNumber].MovingTime, false);
		
		const float TotalTimeForNextStartMovingEvent = ZoneInfoArray[CurrentStepNumber].DelayStart + ZoneInfoArray[CurrentStepNumber].MovingTime;
		//Call StartMovingZone again after the total time
		GetWorld()->GetTimerManager().SetTimer(Localhandle2, this, &AZone::StartMovingZone, TotalTimeForNextStartMovingEvent, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning,TEXT("Current Step Number is less then the num"));
	}
}

// Called when the game starts or when spawned
void AZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AZone::ReduceZoneSize()
{
	const FVector LocalCal = GetActorScale3D() - FVector(ZoneInfoArray[CurrentStepNumber].ScaleToDecrease, ZoneInfoArray[CurrentStepNumber].ScaleToDecrease, 0);
	/*UE_LOG(LogTemp, Warning, TEXT("Current Scale: %s"), *LocalCal.ToString());*/
	SetActorScale3D(LocalCal);
}

void AZone::StopZone()
{
	if (CurrentStepNumber < ZoneInfoArray.Num() - 1)
	{
		CurrentStepNumber++;
	}
	else
	{
		K2_DestroyActor();
	}
	GetWorld()->GetTimerManager().ClearTimer(MovingTimerHandle);
	MovingTimerHandle.Invalidate();
}

