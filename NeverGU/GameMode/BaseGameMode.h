// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NeverGU/Actor/Zone.h"
#include "NeverGU/Actor/Air_Plane.h"
#include "BaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NEVERGU_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Plane")
	TSubclassOf<AActor> PlaneStartClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Plane")
	TSubclassOf<AActor> PlaneEndClass;

	UPROPERTY()
	TArray<APlayerController*> ControllersRef;

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual bool ReadyToStartMatch_Implementation() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void HandleMatchHasStarted() override;
	void SpawnZone();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale")
	int32 MinimumNumberOfPlayers = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale")
	float MaxWaitingTime = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Zone")
	TSubclassOf<AZone> ZoneClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Zone")
	FTransform ZoneTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Zone")
	float ZoneSpawnDelay = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Royale | Plane")
	TSubclassOf<AAir_Plane> PlaneClass;

	FTimerHandle MaxWaitingtimerHandle;

	UPROPERTY()
	AZone* ZoneRef;
};
