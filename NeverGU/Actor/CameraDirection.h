#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraDirection.generated.h"

USTRUCT(BlueprintType)
struct FCameraData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> CameraActor;  // Pointer to the camera actor

	UPROPERTY(EditAnywhere)
	float TimeBeforeSwitch = 2.0f;   // Time to wait before switching to this camera

	UPROPERTY(EditAnywhere)
	float BlendTime = 0.75f;         // Time to blend into this camera
};

UCLASS()
class NEVERGU_API ACameraDirection : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACameraDirection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Current index of the active camera
	int32 CurrentCameraIndex;

	// Time until the next camera switch
	float TimeToNextCameraChange;

	// Array of camera structures
	UPROPERTY(EditAnywhere)
	TArray<FCameraData> CameraArray;

	// Function to switch to the next camera in the array
	void SwitchCamera(APlayerController* PlayerController);
};

