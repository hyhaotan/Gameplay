// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/Actor/Door.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ADoor::ADoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorFrame = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Door Frame"));
	DoorFrame->SetupAttachment(RootComponent);

	Door = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(DoorFrame);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	BoxCollision->SetupAttachment(DoorFrame);

}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &ADoor::OpenDoor);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Timeline.TickTimeline(DeltaTime);
}

void ADoor::OnInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact with Door!"));

	if (bIsDoorClose)
	{
		SetDoorSameSide();
		Timeline.Play(); // Open the Door
	}
	else {
		Timeline.Reverse();//Close the Door
	}

	bIsDoorClose = !bIsDoorClose;//Flip Flop
}

void ADoor::OpenDoor(float Value)
{
	float Angle = bDoorOnSameSide ? -DoorRotateAngle : DoorRotateAngle;

	FRotator Rot = FRotator(0.f, Angle * Value, 0.f);

	Door->SetRelativeRotation(Rot);
}

void ADoor::SetDoorSameSide()
{
	if (ThirdPerson)
	{
		//set vector forward character
		FVector CharacterFV = ThirdPerson->GetActorForwardVector();
		//set vector forward door
		FVector DoorFV = GetActorForwardVector();
		bDoorOnSameSide = (FVector::DotProduct(CharacterFV, DoorFV) >= 0);
	}
}
