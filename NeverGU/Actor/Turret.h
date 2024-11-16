// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Turret.generated.h"

UCLASS()
class NEVERGU_API ATurret : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Beam;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Target1;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Target2;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* FollowTarget;

	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	UParticleSystemComponent* P_MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	USoundBase* ShootingSound;

	UPROPERTY()
	FTimerHandle ChangeTargetTimerHandle;

	UPROPERTY()
	FTimerHandle TraceTimerHandle;

	UPROPERTY()
	FTimerHandle ShootTimerHandle;

	//Rotation related variables
	int TimerCount = 0;
	FRotator LookAtRotation;
	FRotator TargetRotation;
	FRotator RotationDelta;

	UPROPERTY(EditAnywhere)
	float ChangeTargetDelay = 5.0f;

	UPROPERTY(EditAnywhere)
	float RotationRateMultiplier = 1.f;

	UPROPERTY()
	AActor* Enemy = nullptr;

	UFUNCTION()
	void UpdateLookAtTarget(float DeltaTime);

	UFUNCTION()
	void ChangeBeamTarget();

	UFUNCTION(BlueprintCallable)
	void SetBeamLength(float Length);

	UFUNCTION()
	void TraceBeam();

	UFUNCTION()
	void CheckEnemy(AActor* HitActor);

	UFUNCTION()
	void FollowEnemy(float DeltaTime);

	UFUNCTION()
	void Shooting();

public:	
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	// beam length
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float BeamLength = 1000.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
