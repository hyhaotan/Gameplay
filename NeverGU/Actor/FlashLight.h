#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FlashLight.generated.h"

UCLASS()
class NEVERGU_API AFlashLight : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	AFlashLight();

	// Hàm bật/tắt ánh sáng
	void ToggleLight();

	// Hàm kiểm tra đèn pin có được nhặt hay không
	bool IsPickedUp() const { return bIsPickedUp; }

	// Hàm để nhặt đèn pin
	void PickupFlashlight();

	// Set a timer to prevent toggling
	void SetTimerToggle();

	void EnableToggle();

	void DropFlashlight();

	// Timer handle for toggling light
	FTimerHandle ToggleTimerHandle;

protected:
	// Gọi khi game bắt đầu
	virtual void BeginPlay() override;

public:
	// Gọi mỗi frame
	virtual void Tick(float DeltaTime) override;

public:
	// Mesh cho đèn pin
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FlashLight")
	UStaticMeshComponent* FlashLightMesh;

	// Ánh sáng của đèn pin
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FlashLight")
	UPointLightComponent* Light;

	// Biến lưu trạng thái đèn pin (bật/tắt)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlashLight")
	bool bIsLightOn;

	// Biến lưu trạng thái đèn pin đã nhặt hay chưa
	bool bIsPickedUp;

	// Variable to prevent toggling for 2 seconds
	bool bCanToggle;

	//Animation Pick up
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PickupAnimation")
	UAnimMontage* PickupMontage;

	//Create sound click for Flash light
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FlashlightSound")
	USoundBase* FlashlightClick;
};

