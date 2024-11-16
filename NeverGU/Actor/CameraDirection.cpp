#include "NeverGU/Actor/CameraDirection.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACameraDirection::ACameraDirection()
{
	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
	CurrentCameraIndex = 0;
	TimeToNextCameraChange = 0.0f;
}

// Called when the game starts or when spawned
void ACameraDirection::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACameraDirection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Decrease the time to the next camera change
	TimeToNextCameraChange -= DeltaTime;

	// If it's time to switch cameras
	if (TimeToNextCameraChange <= 0.0f && CameraArray.Num() > 0)
	{
		// Get the player controller
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			// Switch to the next camera
			SwitchCamera(PlayerController);
		}
	}
}

void ACameraDirection::SwitchCamera(APlayerController* PlayerController)
{
	if (CameraArray.Num() == 0)
	{
		return;
	}

	// Get the current camera data
	const FCameraData& CameraData = CameraArray[CurrentCameraIndex];

	// If the current camera exists, switch to it
	if (CameraData.CameraActor)
	{
		// Blend to the new camera if there's a blend time
		if (CameraData.BlendTime > 0.0f)
		{
			PlayerController->SetViewTargetWithBlend(CameraData.CameraActor, CameraData.BlendTime);
		}
		else
		{
			PlayerController->SetViewTarget(CameraData.CameraActor);
		}
	}

	// Set the time for the next camera switch
	TimeToNextCameraChange = CameraData.TimeBeforeSwitch;

	// Move to the next camera in the array
	CurrentCameraIndex = (CurrentCameraIndex + 1) % CameraArray.Num();
}
