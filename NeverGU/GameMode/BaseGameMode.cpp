// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/GameMode/BaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"

void ABaseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ControllersRef.AddUnique(NewPlayer);

	if (ControllersRef.Num() >= MinimumNumberOfPlayers)
	{
		if (!HasMatchStarted())
		{
			StartMatch();
			GetWorld()->GetTimerManager().ClearTimer(MaxWaitingtimerHandle);
		}
	}
}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(MaxWaitingtimerHandle, this, &ABaseGameMode::StartMatch, MaxWaitingTime, false);
}

bool ABaseGameMode::ReadyToStartMatch_Implementation()
{
	if (ControllersRef.Num() >= MinimumNumberOfPlayers)
	{
		return true;
	}
	return false;
}

void ABaseGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	RestartPlayer(NewPlayer);
}

void ABaseGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	//Spawn the plane with a random position

	// Reference of all Start and End Points
	TArray<AActor*> StartPointArray;
	TArray<AActor*> EndPointArray;
	UGameplayStatics::GetAllActorsOfClass(this, PlaneStartClass, StartPointArray);
	UGameplayStatics::GetAllActorsOfClass(this, PlaneEndClass, EndPointArray);

	if (StartPointArray.Num() <= 0 || EndPointArray.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Either no Start Point is added or no End Point is added"));
	}

	//const FVector RandomStartPoint = StartPointArray[FMath::RandRange(0, StartPointArray.Num() - 1)]->GetActorLocation();
	//const FVector RandomEndPoint = EndPointArray[FMath::RandRange(0, EndPointArray.Num() - 1)]->GetActorLocation();
	//AAir_Plane* PlaneRef = GetWorld()->SpawnActor<AAir_Plane>(PlaneClass, RandomStartPoint, UKismetMathLibrary::FindLookAtRotation(RandomStartPoint,RandomEndPoint));
	//PlaneRef->PlaneEndPoint = RandomEndPoint;

	//Take all players to the Plane View
	for (auto& Controller : ControllersRef)
	{
		if (Controller)
		{
			if (AThirdPersonCharacter* CharacterRef = Cast<AThirdPersonCharacter>(Controller->GetPawn()))
			{
				CharacterRef->SrvSwitchPlayerViewToPlane();
			}
		}
	}

	FTimerHandle LocalHandle;
	GetWorld()->GetTimerManager().SetTimer(LocalHandle, this, &ABaseGameMode::SpawnZone, ZoneSpawnDelay, false);
}

void ABaseGameMode::SpawnZone()
{
	/*ZoneRef = GetWorld()->SpawnActor<AZone>(ZoneClass, ZoneTransform.GetLocation(), FRotator());

	if (ZoneRef)
	{
		ZoneRef->SetActorScale3D(ZoneTransform.GetScale3D());
	}
	ZoneRef->StartMovingZone();*/
}
