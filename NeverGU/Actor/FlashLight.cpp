#include "FlashLight.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"
#include "Kismet/GameplayStatics.h"

// Constructor
AFlashLight::AFlashLight()
{
	PrimaryActorTick.bCanEverTick = false;

	FlashLightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashLightMesh"));
	RootComponent = FlashLightMesh;

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(RootComponent);
	Light->Intensity = 15.0f;
	Light->IntensityUnits = ELightUnits::Candelas;
	Light->AttenuationRadius = 1200.0f;
	Light->SourceRadius = 30.f;

	// Đèn pin mặc định tắt khi khởi tạo
	Light->SetVisibility(false);
	bIsLightOn = false;

	// Mặc định đèn pin chưa được nhặt
	bIsPickedUp = false;

	// Flashlight can be toggled at start
	bCanToggle = true;
}

// Gọi khi game bắt đầu
void AFlashLight::BeginPlay()
{
	Super::BeginPlay();
}

// Gọi mỗi frame
void AFlashLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Hàm bật/tắt đèn pin
void AFlashLight::ToggleLight()
{
	// Check if we can toggle the flashlight
	if (bCanToggle)
	{
		bIsLightOn = !bIsLightOn;
		Light->SetVisibility(bIsLightOn);
		UGameplayStatics::PlaySoundAtLocation(this, FlashlightClick, GetActorLocation());
		
		// If we just turned it on, disable toggling for 2 seconds
		if (bIsLightOn)
		{
			bCanToggle = false;  // Disable toggling
			SetTimerToggle();    // Start the timer to re-enable toggling
		}
	}
}

// Hàm nhặt đèn pin
void AFlashLight::PickupFlashlight()
{
	if (!bIsPickedUp)
	{
		bIsPickedUp = true;
		// Disable collision so the flashlight is no longer interactable
		SetActorEnableCollision(false);

		// Check if the pickup animation montage is set
		if (PickupMontage)
		{
			// Get the player character and cast it to your custom character class
			AThirdPersonCharacter* PlayerCharacter = Cast<AThirdPersonCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());

			// If the cast is successful and the player character exists
			if (PlayerCharacter)
			{
				// Play the pickup montage on the player character
				PlayerCharacter->PlayAnimMontage(PickupMontage);
			}
		}
	}
}

// Set a timer to re-enable toggling
void AFlashLight::SetTimerToggle()
{
	// Set a timer for 2 seconds to re-enable toggling
	GetWorldTimerManager().SetTimer(ToggleTimerHandle, this, &AFlashLight::EnableToggle, 2.0f, false);
}

// Function to re-enable toggling after 2 seconds
void AFlashLight::EnableToggle()
{
	bCanToggle = true;
}

// Method to drop the flashlight
void AFlashLight::DropFlashlight()
{
	if (bIsPickedUp)
	{
		// Re-enable collision so the flashlight becomes interactable again
		SetActorEnableCollision(true);

		// Enable physics so the flashlight can fall to the ground
		FlashLightMesh->SetSimulatePhysics(true);

		// Get the player character and the forward direction
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (PlayerCharacter)
		{
			// Calculate drop location in front of the player
			FVector DropLocation = PlayerCharacter->GetActorLocation() + PlayerCharacter->GetActorForwardVector() * 200.0f;
			SetActorLocation(DropLocation);

			// Optionally apply a small impulse for realism
			FVector ImpulseDirection = PlayerCharacter->GetActorForwardVector() * 500.0f;
			FlashLightMesh->AddImpulse(ImpulseDirection);
		}

		// Mark flashlight as no longer picked up
		bIsPickedUp = false;
	}
}