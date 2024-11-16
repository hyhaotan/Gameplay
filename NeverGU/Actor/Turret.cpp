// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Actor/Turret.h"
#include "NeverGU/Interfaces/CharacterInterface.h"
#include "NeverGU/Interfaces/TurretAnimInterface.h"

#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

#define OUT
// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(Root);

	Beam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam"));
	Beam->SetupAttachment(TurretMesh, TEXT("BeamSocket"));

	Target1 = CreateDefaultSubobject<USceneComponent>(TEXT("Target1"));
	Target1->SetupAttachment(Root);

	Target2 = CreateDefaultSubobject<USceneComponent>(TEXT("Target2"));
	Target2->SetupAttachment(Root);

	FollowTarget = CreateDefaultSubobject<USceneComponent>(TEXT("FollowTarget"));
	FollowTarget->SetupAttachment(Root);

	SetBeamLength(BeamLength);

	P_MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Muzzle Flash"));
	P_MuzzleFlash->SetupAttachment(TurretMesh,TEXT("BeamSocket"));
	P_MuzzleFlash->SetAutoActivate(false);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(ChangeTargetTimerHandle, this, &ATurret::ChangeBeamTarget, ChangeTargetDelay, true,1.f);
	GetWorldTimerManager().SetTimer(TraceTimerHandle, this, &ATurret::TraceBeam, 0.1f, true, 0.1f);
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*TraceBeam();*/

	if (Enemy)
	{
		FollowEnemy(DeltaTime);
	}
	else
	{
		UpdateLookAtTarget(DeltaTime);
	}
}


void ATurret::UpdateLookAtTarget(float DeltaTime)
{
	if (LookAtRotation.Equals(TargetRotation,1.f))
	{
		return;
	}

	LookAtRotation += RotationDelta * RotationRateMultiplier * DeltaTime;

	if (TurretMesh->GetAnimInstance()->Implements<UTurretAnimInterface>())
	{
		ITurretAnimInterface::Execute_UpdateLookAtRotation(TurretMesh->GetAnimInstance(), LookAtRotation);
	}

}

void ATurret::ChangeBeamTarget()
{
	TimerCount++;

	if (TimerCount % 2 ==0)
	{
		FollowTarget->SetWorldLocation(Target1->GetComponentLocation());
	}
	else
	{
		FollowTarget->SetWorldLocation(Target2->GetComponentLocation());
	}

	FVector Start = TurretMesh->GetSocketLocation("BeamSocket");
	FVector End = FollowTarget->GetComponentLocation();
	TargetRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

	RotationDelta = TargetRotation - LookAtRotation;
	RotationDelta.Normalize();
}

void ATurret::SetBeamLength(float Length)
{
	Beam->SetRelativeScale3D(FVector(Length / 400, Beam->GetRelativeScale3D().Y, Beam->GetRelativeScale3D().Z));
	Beam->SetRelativeLocation(FVector(Length / (-8), 0, 0));
}

void ATurret::TraceBeam()
{
	FHitResult HitResult;
	FVector Start = TurretMesh->GetSocketLocation("BeamSocket");
	FVector End = Start + Beam->GetForwardVector() * BeamLength;
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OUT HitResult, Start, End, ECollisionChannel::ECC_Camera, QueryParams);

	if (bHit)
	{
		SetBeamLength(HitResult.Distance);
		CheckEnemy(HitResult.GetActor());
	}
	else
	{
		SetBeamLength(BeamLength);
	}
}

void ATurret::CheckEnemy(AActor* HitActor)
{
	if (HitActor->Implements<UCharacterInterface>())
	{
		bool bEnemy = ICharacterInterface::Execute_IsEnemy(HitActor);
		if (bEnemy)
		{
			if (!Enemy)
			{
				Enemy = HitActor;
				//Start Shooting
				GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &ATurret::Shooting, 0.4f, true, 0.5f);
				/*UE_LOG(LogTemp, Warning, TEXT("Enemy Detected"));*/
			}
		}
	}
	else
	{
		if (Enemy)
		{
			Enemy = nullptr;
			GetWorldTimerManager().ClearTimer(ShootTimerHandle);
			//Stop shooting
		}
	}
}

void ATurret::FollowEnemy(float DeltaTime)
{
	FVector Start = TurretMesh->GetSocketLocation("BeamSocket");
	FVector End = Enemy->GetActorLocation();

	FRotator RotationToEnemy = UKismetMathLibrary::FindLookAtRotation(Start, End);

	LookAtRotation = FMath::RInterpTo(LookAtRotation, RotationToEnemy, DeltaTime, 10);

	if (TurretMesh->GetAnimInstance()->Implements<UTurretAnimInterface>())
	{
		ITurretAnimInterface::Execute_UpdateLookAtRotation(TurretMesh->GetAnimInstance(), LookAtRotation);
	}
}

void ATurret::Shooting()
{
	UGameplayStatics::PlaySoundAtLocation(this, ShootingSound, P_MuzzleFlash->GetComponentLocation());
	P_MuzzleFlash->Activate(true);

	FHitResult HitResult;
	FVector Start = TurretMesh->GetSocketLocation("BeamSocket");
	FVector End = Start + Beam->GetForwardVector() * BeamLength;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OUT HitResult, Start, End, ECollisionChannel::ECC_Camera, QueryParams);

	if (bHit)
	{
		FPointDamageEvent DamageEvent(10.f, HitResult, Beam->GetForwardVector(), nullptr);
		HitResult.GetActor()->TakeDamage(10.f, DamageEvent, GetInstigatorController(), this);
	}
}
