// Fill out your copyright notice in the Description page of Project Settings.

//source
#include "NeverGU/Input/ThirdPersonCharacter.h"
#include "NeverGU/Actor/Door.h"
#include "NeverGU/UserInterface/PlayerHUD.h"
#include "NeverGU/Component/InventoryComponent.h"
#include "NeverGU/World/Pickup.h"
#include "NeverGU/Interfaces/InteractionInterface.h"
#include "NeverGU/Input/ThirdPersonPlayerController.h"
#include "NeverGU/Actor/Air_Plane.h"
#include "NeverGU/Items/Weapon.h"
#include "NeverGU/Items/Item.h"
#include "NeverGU/Items/health.h"
#include "NeverGU/Items/Armor.h"
#include "NeverGU/Items/Backpack.h"
#include "NeverGU/Actor/GunAmmo.h"
#include "NeverGU/AI/AICharacterBase.h"
#include "NeverGU/Actor/FlashLight.h"
#include "NeverGU/Data/InfoPlayer.h"
#include "NeverGU/Actor/Sword.h"
#include "NeverGU/UserInterface/Widget/CooldownWidget.h"
#include "NeverGU/Character/TradeCharacter.h"
#include "NeverGU/UserInterface/Widget/StoreItemWidget.h"
#include "NeverGU/UserInterface/Widget/ProductWindowWidget.h"
#include "NeverGU/UserInterface/Widget/DialogWidget.h"
#include "NeverGU/Actor/Pistol.h"
#include "NeverGU/Actor/GunAmmoType.h"

//engine
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "NiagaraComponent.h"
#include "NiagaraCommon.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/NavMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	AimingCameraTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AimingCameraTimeline"));
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	InfoPlayer = CreateDefaultSubobject<UInfoPlayer>(TEXT("InfoPlayer"));
	 
	//Setting Default Variables and Behavior of the SpringArmComponent
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->CameraLagSpeed = 12.f;

	//Third Person Camera
	ThirdPersonCamera->SetupAttachment(SpringArm);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	//First Person Camera
	FirstPersonCamera->SetupAttachment(RootComponent,FName("head"));
	FirstPersonCamera->bUsePawnControlRotation = true; // Rotate the camera based on controller
	FirstPersonCamera->SetRelativeLocation(FVector(30.0f, 0.0f, 70.0f)); // Position the camera

	// Initially enable the third-person camera and disable the first-person camera
	ThirdPersonCamera->bAutoActivate = true;
	FirstPersonCamera->bAutoActivate = false;

	// Initialize the view mode to third-person
	bIsFirstPerson = false;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	// Enable flying
	GetCharacterMovement()->MaxFlySpeed = 600.f;
	GetCharacterMovement()->BrakingDecelerationFlying = 1000.f;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	
	// Enable swimming
	GetCharacterMovement()->MaxSwimSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationSwimming = 1000.f;

	CrouchEyeOffset = FVector(0.f);
	CrouchSpeed = 12.f;

	InteractionCheckFrequency = 0.1;
	InteractionCheckDistance = 200.0f;

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);
	BaseEyeHeight = 76.0f;

	//Set AimingCamera Location
	DefaultCameraLocation = FVector{ 0.0f,0.0f,65.0f };
	AimingCameraLocation = FVector{ 175.0f,50.0f,55.0f };
	SpringArm->SocketOffset = DefaultCameraLocation;

	SetupStimulusSource();

	//Set Stamina
	CurrentStamina = 1.0f;
	MaxStamina = 1.0f;
	StaminaSpringUsageRate = 0.05f;
	StaminaRechargeRate = 0.1f;
	StaminaJumpUsageRate = 0.1f;

	CanStaminaRecharge = true;
	DelayForStaminaRecharge = 2.0f;

	// Initialize the flashlight reference as nullptr
	EquippedFlashlight = nullptr;
	NearbyFlashlight = nullptr;
	HeldFlashlight = nullptr;

	FireRate = 0.12f; // Fire rate: 0.12 seconds between shots
	bCanFire = true; /*Player can shoot initially*/
	bCanShootPistol = true;

	RecoilPitch = 1.0f; // Tùy chỉnh giá trị độ giật dọc
	RecoilYaw = 0.5f;   // Tùy chỉnh giá trị độ giật ngang
	RecoilResetSpeed = 5.0f; // Tốc độ hồi về trạng thái bình thường

	bIsAttacking = false;
	ComboCount = 0;
	ComboResetTime = 1.5f;

	// Proper initialization of NamePlayer (e.g., "Player" as a default name).
	NamePlayer = FText::FromString("KYAN");

	// Initialize health and armor with default values.
	Health = 100.0f;
	Armor = 100.0f;

	// Set age default to 19.
	AgePlayer = 19;

}

void AThirdPersonCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeighAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}

	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeighAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight + HalfHeightAdjust;
	ThirdPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);

}

void AThirdPersonCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeighAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}

	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeighAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight - HalfHeightAdjust;
	ThirdPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);

}

void AThirdPersonCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (ThirdPersonCamera)
	{
		ThirdPersonCamera->GetCameraView(DeltaTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AThirdPersonCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AThirdPersonCharacter::OnOverlapEnd);

	HUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	//Aim Timeline
	FOnTimelineFloat AimLerpAlphaValue;
	FOnTimelineEvent TimelineFinishedEvent;
	AimLerpAlphaValue.BindUFunction(this, FName("UpdateCameraTimeLine"));
	TimelineFinishedEvent.BindUFunction(this, FName("CameraTimelineEnd"));

	if (AimingCameraTimeline && AimingCameraCurve)
	{
		AimingCameraTimeline->AddInterpFloat(AimingCameraCurve, AimLerpAlphaValue);
		AimingCameraTimeline->SetTimelineFinishedFunc(TimelineFinishedEvent);
	}

	if (!HasAuthority())
	{
		return;
	}
	//FTimerHandle ZoneTimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(ZoneTimerHandle, this, &AThirdPersonCharacter::DamagePlayerifnotinZone, 0.1f, true);

	 // Khởi tạo widget nếu nó đã được thiết lập trong editor
	if (CooldownWidgetClass)
	{
		CooldownWidget = CreateWidget<UCooldownWidget>(GetWorld(), CooldownWidgetClass);
		if (CooldownWidget)
		{
			CooldownWidget->AddToViewport();
		}
	}

	if (ComboMontage1 || ComboMontage2 || ComboMontage3)
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AThirdPersonCharacter::OnMontageEnded);
	}
}

// Called every frame
void AThirdPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Smoothly interpolate the camera distance based on crouching status
	float TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, CurrentCameraDistance, DeltaTime, 5.0f);
	SpringArm->TargetArmLength = TargetArmLength;

	float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaTime);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
	/*ReplicateControlRotation();*/

	if (bIsRunning)
	{
		CurrentStamina = FMath::FInterpConstantTo(CurrentStamina, 0.0f, DeltaTime, StaminaSpringUsageRate);
	
		if (CurrentStamina <= 0.0f)
		{
			DepletedAllStamina();
		}
	}
	else
	{
		if (CurrentStamina < MaxStamina)
		{
			if (CanStaminaRecharge)
			{
				CurrentStamina = FMath::FInterpConstantTo(CurrentStamina, MaxStamina, DeltaTime, StaminaRechargeRate);
			}
		}
	}	

	FlashlightActor();
	ItemActor();
	SwordActor();
	PistolActor();
	AmmoActor();

	//Update Cooldown time widget
	UpdateCooldownTime(DeltaTime);

	/*Zoom in if ZoomIn button is down, zoom back out if it's not*/
	if (bZoomingIn)
	{
		ZoomFactor += DeltaTime / 0.5f; //Zoom out over a quarter of a second
	}
	else
	{
		ZoomFactor -= DeltaTime / 0.25f; //Zoom in over half a second
	}
	ZoomFactor = FMath::Clamp<float>(ZoomFactor, 0.0f, 1.0f);
	//Blend our camera's FOV and our SpringArm's length based on ZoomFactor
	ThirdPersonCamera->FieldOfView = FMath::Lerp<float>(90.0f, 60.0f, ZoomFactor);
}

// Called to bind functionality to input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		//lấy hệ thống con trình phát cục bộ
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//them dau vao boi canh

			Subsystem->AddMappingContext(InputMapping, 0);

			if (UEnhancedInputComponent* Input = CastChecked <UEnhancedInputComponent>(PlayerInputComponent))
			{
				//Move & Look & Jump
				Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Move);
				Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Look);
				Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Jump);
				Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &AThirdPersonCharacter::StopJumping);

				//Crouch
				Input->BindAction(CrouchAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::ToggleCrouch);

				//Run
				Input->BindAction(RunAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::StartRunning);
				Input->BindAction(RunAction, ETriggerEvent::Completed, this, &AThirdPersonCharacter::EndRunning);

				//Interact
				Input->BindAction(InteractAction, ETriggerEvent::Completed, this, &AThirdPersonCharacter::Interact);
				Input->BindAction(InteractItemAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::BeginInteract);
				Input->BindAction(InteractItemAction, ETriggerEvent::Completed, this, &AThirdPersonCharacter::EndInteract);

				//Toggle Menu
				Input->BindAction(ToggleAction, ETriggerEvent::Completed, this, &AThirdPersonCharacter::ToggleMenu);
			
				//Aim
				Input->BindAction(AimAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::Aim);
				Input->BindAction(AimAction, ETriggerEvent::Completed, this, &AThirdPersonCharacter::StopAiming);
			
				//Drop Item
				Input->BindAction(DropItemAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::DropCurrentItem);
				Input->BindAction(DropItemAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::DropHeldFlashlight);
				Input->BindAction(DropItemAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::DropSword);
				Input->BindAction(DropItemAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::DropPistol);

				//Reload Ammo for gun
				Input->BindAction(ReloadAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::ReloadGun);

				//Dodge
				Input->BindAction(DodgeAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::DodgeForward);

				//Scream
				Input->BindAction(ScreamAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::ScreamPlayerCharacter);

				//Target Lock
				Input->BindAction(TargetLockAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::TargetLockEnemy);

				//Pickup
				Input->BindAction(PickupFlashlightAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::PickupFlashlight);
				Input->BindAction(PickupFlashlightAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::PickupPistol);
				Input->BindAction(PickupFlashlightAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::PickupAmmoType);

				Input->BindAction(AttackAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::AttackSword);
				Input->BindAction(InteractItemAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::PickupItem);

				//Switch Camera Player
				Input->BindAction(SwitchCameraAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::SwitchCameraPlayer);

				//Toggle
				Input->BindAction(ToggleSwordAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::ToggleWeaponEquip);
				Input->BindAction(ToggleGunAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::ToggleGun);
				Input->BindAction(TogglePistolAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::TogglePistol);
				Input->BindAction(ToggleFlashlightAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::ToggleFlashlight);
				Input->BindAction(ToggleFlashlightsAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::ToggleFlashlights);

				//Skill
				Input->BindAction(SkillAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::SpawnSkill);

				Input->BindAction(InteractAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::InteractDialog);
			}
		}
	}
}

void AThirdPersonCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		//lay phuong huong phia truoc
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//di chuyển bình thường
		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

void AThirdPersonCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
	}
}

void AThirdPersonCharacter::StartRunning()
{
	if (CurrentStamina > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("We are new running."));
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = 600.f;

		//Stop Stamina from recharing for a period of time
		CanStaminaRecharge = false;
		GetWorld()->GetTimerManager().ClearTimer(StaminaRechargeTimerHandle);
	}
}

void AThirdPersonCharacter::EndRunning()
{
	if (bIsRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("We have stopped running."));
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = 200.f;

		//Start the timer to rechage stamina when the character has stopped running
		GetWorld()->GetTimerManager().SetTimer(StaminaRechargeTimerHandle, this, &AThirdPersonCharacter::EnableStaminaGain, DelayForStaminaRecharge, false);
	}
}

void AThirdPersonCharacter::Jump()
{
	// Check if the character is already jumping to prevent stamina reduction multiple times
	if (!bIsJumping)
	{
		// Set jumping state and disable stamina recharge while jumping
		bIsJumping = true;
		CanStaminaRecharge = false;
		CurrentStamina = FMath::Clamp(CurrentStamina - StaminaJumpUsageRate, 0.0f, MaxStamina);

		// Clear any stamina recharge timers while jumping
		GetWorld()->GetTimerManager().ClearTimer(StaminaRechargeTimerHandle);
	}

	// Call the base class Jump() functionality
	Super::Jump();
}

void AThirdPersonCharacter::StopJumping()
{
	// Set jumping state to false and allow stamina recharge after a delay
	bIsJumping = false;

	// Set a timer to allow stamina recharge after a delay
	GetWorld()->GetTimerManager().SetTimer(StaminaRechargeTimerHandle, this, &AThirdPersonCharacter::EnableStaminaGain, DelayForStaminaRecharge, false);

	// Call the base class StopJumping() functionality
	Super::StopJumping();
}

void AThirdPersonCharacter::ResetJumping()
{
	bIsJumping = false;
}

void AThirdPersonCharacter::Interact()
{
	if (ThirdPersonCamera == nullptr)
		return;

	//set HitResult Door of vector Start and End
	FHitResult HitResult;
	FVector Start = ThirdPersonCamera->GetComponentLocation();
	FVector End = Start + ThirdPersonCamera->GetForwardVector() * InteractLineTraceLength;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

	// Cast to DoorActor
	ADoor* Door = Cast<ADoor>(HitResult.GetActor());
	if (Door)
	{
		Door->ThirdPerson = this;
		Door->OnInteract();
		UGameplayStatics::PlaySoundAtLocation(this, DoorSound, Door->GetActorLocation());
	}
}

void AThirdPersonCharacter::Aim()
{
	if (!HUD->bIsMenuVisible)
	{
		bAiming = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->MaxWalkSpeed = 150.f;

		if (AimingCameraTimeline)
			AimingCameraTimeline->PlayFromStart();
	}
}

void AThirdPersonCharacter::StopAiming()
{
	if (bAiming)
	{
		bAiming = false;
		bUseControllerRotationYaw = false;
		HUD->HideCrosshair();
		GetCharacterMovement()->MaxWalkSpeed = 200.f;

		if (AimingCameraTimeline)
			AimingCameraTimeline->Reverse();
	}
}

void AThirdPersonCharacter::UpdateCameraTimeLine(const float TimelineValue) const
{
	const FVector CameraLocation = FMath::Lerp(DefaultCameraLocation, AimingCameraLocation, TimelineValue);
	SpringArm->SocketOffset = CameraLocation;
}

void AThirdPersonCharacter::CameraTimelineEnd()
{
	if (AimingCameraTimeline)
	{
		if (AimingCameraTimeline->GetPlaybackPosition() != 0.0f)
		{
			HUD->ShowCrosshair();
		}
	}
}

void AThirdPersonCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart;

	if (!bAiming)
	{
		InteractionCheckDistance = 200.0f;
		TraceStart = GetPawnViewLocation();
	}
	else
	{
		InteractionCheckDistance = 250.0f;
		TraceStart = ThirdPersonCamera->GetComponentLocation();
	}

	FVector TraceEnd{ TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance) };

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	if (LookDirection > 0)
	{
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 2.0f);
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult TraceHit;

		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			if (TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				if (TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}

				if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}
	NoInteractableFound();
}

void AThirdPersonCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);

	TargetInteractable->BeginFocus();
}

void AThirdPersonCharacter::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
			EndInteract();
		}

		HUD->HideInteractionWidget();

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void AThirdPersonCharacter::BeginInteract()
{
	// verify nothing has changed with the interactable state since beginning interaction
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration,0.1f))
			{
				InteractItem();
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction,
					this,
					&AThirdPersonCharacter::InteractItem,
					TargetInteractable->InteractableData.InteractionDuration,
					false);
			}
		}
	}
}

void AThirdPersonCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void AThirdPersonCharacter::InteractItem()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->InteractItem(this);
	}
}

void AThirdPersonCharacter::DodgeForward()
{
	// Check if the character is already dodging or if they are in the air (not on the ground)
	if (!bIsDodge && GetCharacterMovement()->IsMovingOnGround())
	{
		bIsDodge = true;
		GetCharacterMovement()->DisableMovement();

		// Play the dodge roll animation
		float AnimationPlayRate = 1.0f; // Default play rate
		float DodgeTime = 1.0f; // Default dodge time
		float DodgeLenght = 10.0f;

		if (DodgeMontage)
		{
			// Calculate Dodge Time based on animation length and play rate
			DodgeTime = DodgeMontage->GetPlayLength();

			this->PlayAnimMontage(DodgeMontage, AnimationPlayRate);

			// Adjust DodgeTime based on play rate
			DodgeTime = DodgeMontage->GetPlayLength() / AnimationPlayRate;
		}

		// Launch the character forward with a roll
		FVector DodgeDirection = GetActorForwardVector(); // Forward direction
		float DodgeDistance = 600.f * DodgeLenght; // Adjust this value as needed
		FVector DodgeVelocity = DodgeDirection * DodgeDistance / DodgeTime;

		LaunchCharacter(DodgeVelocity, true, true);

		// Optionally, reset bIsDodge after a delay to allow for another dodge
		GetWorldTimerManager().SetTimer(DodgeResetHandle, this, &AThirdPersonCharacter::ResetDodge, DodgeTime, false);
		GetWorldTimerManager().SetTimer(EnableMovementHandle, this, &AThirdPersonCharacter::EnableMovement, 2.0f, false);
	}
}

void AThirdPersonCharacter::ResetDodge()
{
	bIsDodge = false;

	float DodgeCooldown = 0.5f; 
	GetWorldTimerManager().SetTimer(DodgeCooldownHandle, this, &AThirdPersonCharacter::EnableDodge, DodgeCooldown, false);
}

void AThirdPersonCharacter::EnableDodge()
{
	bIsDodge = true;
}

void AThirdPersonCharacter::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void AThirdPersonCharacter::DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop)
{
	if (PlayerInventory->FindMatchingItem(ItemToDrop))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation{ GetActorLocation() + (GetActorForwardVector() * 50.0f) };
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
	
		const int32 RemoveQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);

		Pickup->InitializeDrop(ItemToDrop, RemoveQuantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item to drop was somehow null!"));
	}
}

void AThirdPersonCharacter::DropCurrentItem()
{
	if (bIsEquipGun && EquippedItem && EquippedItem->IsPickedUpItem())
	{
		bIsPrimaryWeapon = false;
		bIsEquipGun = false;

		EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Enable physics on the weapon so it falls to the ground
		if (UStaticMeshComponent* ItemtMesh = EquippedItem->FindComponentByClass<UStaticMeshComponent>())
		{
			ItemtMesh->SetSimulatePhysics(true);
		}
		else if (USkeletalMeshComponent* ItemSkeletaltMesh = EquippedItem->FindComponentByClass<USkeletalMeshComponent>())
		{
			ItemSkeletaltMesh->SetSimulatePhysics(true);
		}

		FVector ForwardVector = GetActorForwardVector();
		FVector DropLocation = GetActorLocation() + ForwardVector * 200.0f + FVector(0.0f, 0.0f, 50.0f);

		// Drop item tại vị trí đã xác định
		EquippedItem->SetActorLocation(DropLocation);

		// Hẹn giờ để bật lại va chạm sau khi thả
		FTimerHandle CollisionTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(CollisionTimerHandle, [this]()
			{
				if (EquippedItem)
				{
					EquippedItem->SetActorEnableCollision(true);
				}
			}, 0.5f, false);

		// Xóa tham chiếu đến vật phẩm đã thả
		EquippedItem = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No item to drop!"));
	}
}

void AThirdPersonCharacter::DropHeldFlashlight()
{
	//Check the player character get Flashlight? 
	if (bIsEquipFlashlight && EquippedFlashlight && EquippedFlashlight->IsPickedUp())
	{
		// Detach the flashlight from the character
		EquippedFlashlight->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Enable physics on the flashlight so it falls to the ground
		if (UStaticMeshComponent* FlashlightMesh = EquippedFlashlight->FindComponentByClass<UStaticMeshComponent>())
		{
			FlashlightMesh->SetSimulatePhysics(true);
		}

		// Get the forward vector of the character and add an impulse for realism
		FVector ForwardVector = GetActorForwardVector();
		FVector DropLocation = GetActorLocation() + ForwardVector * 200.0f + FVector(0.0f, 0.0f, 50.0f);

		// Set the location to drop the flashlight
		EquippedFlashlight->SetActorLocation(DropLocation);

		// Allow collision after a slight delay
		GetWorld()->GetTimerManager().SetTimer(ToggleTimerHandle, [this]()
			{
				if (EquippedFlashlight)
				{
					EquippedFlashlight->SetActorEnableCollision(true);
				}
			}, 0.5f, false);

		// Clear the flashlight reference after dropping
		EquippedFlashlight = nullptr;
		bIsEquipFlashlight = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No flashlight to drop!"));
	}
}

void AThirdPersonCharacter::DropPistol()
{
	//Check the player character get Flashlight? 
	if (bIsEquipPistol && EquippedPistol && EquippedPistol->IsPickedUp())
	{
		// Detach the flashlight from the character
		EquippedPistol->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Enable physics on the flashlight so it falls to the ground
		if (UStaticMeshComponent* PistolMesh = EquippedPistol->FindComponentByClass<UStaticMeshComponent>())
		{
			PistolMesh->SetSimulatePhysics(true);
		}
		else if (USkeletalMeshComponent* PistolSkeletalMesh = EquippedPistol->FindComponentByClass<USkeletalMeshComponent>())
		{
			PistolSkeletalMesh->SetSimulatePhysics(true);
		}

		// Get the forward vector of the character and add an impulse for realism
		FVector ForwardVector = GetActorForwardVector();
		FVector DropLocation = GetActorLocation() + ForwardVector * 200.0f + FVector(0.0f, 0.0f, 50.0f);

		// Set the location to drop the flashlight
		EquippedPistol->SetActorLocation(DropLocation);

		// Allow collision after a slight delay
		GetWorld()->GetTimerManager().SetTimer(ToggleTimerHandle, [this]()
			{
				if (EquippedPistol)
				{
					EquippedPistol->SetActorEnableCollision(true);
				}
			}, 0.5f, false);

		// Clear the flashlight reference after dropping
		EquippedPistol = nullptr;
		bIsEquipPistol = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No flashlight to drop!"));
	}
}

void AThirdPersonCharacter::DropSword()
{
	//Check the player character get Sword? 
	if (bIsEquipWeapon && EquippedSword && EquippedSword->IsPickedUp())
	{
		bIsItemSlot = false;
		bIsEquipWeapon = false;

		// Detach the flashlight from the character
		EquippedSword->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Enable physics on the Sword so it falls to the ground
		if (UStaticMeshComponent* SwordMesh = EquippedSword->FindComponentByClass<UStaticMeshComponent>())
		{
			SwordMesh->SetSimulatePhysics(true);
		}

		// Get the forward vector of the character and add an impulse for realism
		FVector ForwardVector = GetActorForwardVector();
		FVector DropLocation = GetActorLocation() + ForwardVector * 200.0f + FVector(0.0f, 0.0f, 50.0f);

		// Set the location to drop the Sword
		EquippedSword->SetActorLocation(DropLocation);

		// Allow collision after a slight delay
		GetWorld()->GetTimerManager().SetTimer(ToggleTimerHandle, [this]()
			{
				if (EquippedSword)
				{
					EquippedSword->SetActorEnableCollision(true);
				}
			}, 0.5f, false);

		// Clear the Sword reference after dropping
		EquippedSword = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Sword to drop!"));
	}
}

void AThirdPersonCharacter::ToggleMenu()
{
	HUD->ToggleMenu();

	if (HUD->bIsMenuVisible)
	{
		StopAiming();
	}
	GetWorldTimerManager().SetTimer(MenuVisibleTimerHandle, this, &AThirdPersonCharacter::DelayMenuVisible, 3.0f, false);
}

void AThirdPersonCharacter::DelayMenuVisible()
{
	HUD->bIsMenuVisible = false;
}

void AThirdPersonCharacter::DamagePlayerifnotinZone_Implementation()
{
	if (!bIsInZone && bIsAlive)
	{
		Health = FMath::Clamp(Health - 0.1f,0.f,100.f);
		if (Health <= 0.f)
		{
			bIsAlive = false;
			Mlt_PlayerDeath();
		}
		CltDamagePlayerLocally();
	}
}

void AThirdPersonCharacter::CltDamagePlayerLocally_Implementation()
{
	OnPlayerDamage();
}

void AThirdPersonCharacter::Mlt_SkydiveForPlayer_Implementation()
{
	GetCharacterMovement()->Velocity = UKismetMathLibrary::VLerp(FVector(), GetVelocity(), 0.1);
	GetCharacterMovement()->AirControl = 0.7;
	GetCharacterMovement()->GravityScale = 0.1;
}

void AThirdPersonCharacter::Mlt_PlayerLanded_Implementation()
{
	GetCharacterMovement()->AirControl = 0.35;
	GetCharacterMovement()->GravityScale = 1.75;
}

void AThirdPersonCharacter::ToggleCrouch()
{
	if (bIsCrouching)
	{
		StopCrouching();
	}
	else
	{
		StartCrouching();
	}
}

void AThirdPersonCharacter::StartCrouching()
{
	Crouch();
	bIsCrouching = true;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	bZoomingIn = true;
	CurrentCameraDistance = CrouchCameraDistance; // Set target distance when crouching
}

void AThirdPersonCharacter::StopCrouching()
{
	UnCrouch();
	bIsCrouching = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	bZoomingIn = false;
	CurrentCameraDistance = NormalCameraDistance; // Reset target distance when not crouching
}

void AThirdPersonCharacter::EnableStaminaGain()
{
	CanStaminaRecharge = true;
}

void AThirdPersonCharacter::DepletedAllStamina()
{
	EndRunning();	
}

void AThirdPersonCharacter::StartSwimming()
{
	bIsSwimming = true;
	GetCharacterMovement()->GetPhysicsVolume()->bWaterVolume = true;
	GetCharacterMovement()->GravityScale = 0.2f;
}

void AThirdPersonCharacter::StopSwimming()
{
	bIsSwimming = false;
	GetCharacterMovement()->GetPhysicsVolume()->bWaterVolume = false;
	GetCharacterMovement()->GravityScale = 1.0f;
}

void AThirdPersonCharacter::Clt_GunStoppedFire(bool& bCanShoots)
{
	// Check if there is ammo left
	if (GunAmmo > 0)
	{
		bCanShoots = true; // Allow shooting if there is ammo
	}
	else
	{
		// If no ammo, stop shooting and play out of ammo sound
		bCanShoots = false;
		UGameplayStatics::PlaySoundAtLocation(this, OutofAmmoSound, EquippedItem->GetSkeletalMesh()->GetBoneLocation("Muzzle"));
		UE_LOG(LogTemp, Warning, TEXT("Out of Ammo or Cannot Shoot!"));
	}
}

void AThirdPersonCharacter::ReloadGun()
{
	if (bIsEquipGun && EquippedItem && EquippedItem->ItemType == EItemtypes::Weapon)
	{
		// Call AmmoProcessPistol for rifle-type weapon
		AmmoProcess(GunAmmo, SumGunAmmo, AmmoAmount);
	}
	else if (bIsEquipPistol && EquippedPistol && EquippedPistol->PistolType == EPistolTypes::DesertEagle)
	{
		// Call AmmoProcessPistol for pistol-type weapon
		AmmoProcess(PistolAmmo, SumPistolAmmo, AmmoAmountPistol);
	}
}

void AThirdPersonCharacter::ScreamPlayerCharacter()
{
	FTimerHandle DelayScreamTimerHandle;
	FVector ScreamLocation = GetActorLocation();
	
	if (!bIsScream)
	{
		bIsScream = true;
		MakeNoise(1.f, this, ScreamLocation, 0);
		UGameplayStatics::PlaySoundAtLocation(this, ScreamSound, GetActorLocation());
		GetWorldTimerManager().SetTimer(DelayScreamTimerHandle, this, &AThirdPersonCharacter::ResetScreamPlayerCharacter, 3.f, false);
	}
}

void AThirdPersonCharacter::ResetScreamPlayerCharacter()
{
	bIsScream = false;
}

void AThirdPersonCharacter::TargetLockEnemy()
{
	if (ActorToTargetLock == nullptr)
	{
		// Get camera location and direction
		FVector CameraLocation = ThirdPersonCamera->GetComponentLocation();
		FRotator CameraRotation = ThirdPersonCamera->GetComponentRotation();
		FVector ForwardVector = CameraRotation.Vector();

		FVector Start = CameraLocation;
		FVector End = Start + (ForwardVector * 1000.0f); // Trace 1000 units forward

		// Debug: Draw a line for visualization
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 5.0f);

		// Collision query setup
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this); // Ignore self

		// Define the object types to trace (PhysicsBody equivalent in Blueprint)
		FCollisionObjectQueryParams ObjectQueryParams(ECollisionChannel::ECC_PhysicsBody);

		// Perform sphere trace
		FHitResult HitResult;
		bool bHit = GetWorld()->SweepSingleByObjectType(
			HitResult,
			Start,
			End,
			FQuat::Identity,
			ObjectQueryParams,
			FCollisionShape::MakeSphere(100.0f), // Sphere radius of 100 units
			TraceParams
		);

		if (bHit && HitResult.GetActor())
		{
			// Debug log the hit actor's name
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());

			// Check if the hit actor has the "Damageable" tag
			if (HitResult.GetActor()->ActorHasTag("Damageable"))
			{
				// Lock onto the actor
				ActorToTargetLock = HitResult.GetActor();
				UE_LOG(LogTemp, Warning, TEXT("Target Locked: %s"), *ActorToTargetLock->GetName());
			}
		}
	}
	else
	{
		// Unlock the target if already locked
		ActorToTargetLock = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Target Unlocked"));
	}
}

void AThirdPersonCharacter::Mlt_PlayerDeath_Implementation()
{
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	DisableInput(GetWorld()->GetFirstPlayerController());
}

bool AThirdPersonCharacter::IsEnemy_Implementation()
{
	return (Health > 0);
}

float AThirdPersonCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageCaused = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	AThirdPersonCharacter* KilledCharacter = Cast<AThirdPersonCharacter>(this);
	AThirdPersonCharacter* PlayerCharacter = Cast<AThirdPersonCharacter>(DamageCauser);

	if (HasAuthority())
	{
		// Xử lý sát thương của nhân vật có áo giáp hoặc không có
		if (EquippedItem && EquippedItem->ItemType == EItemtypes::Armor)
		{
			DamageCaused = HandleDamageWithArmor(DamageAmount);
		}
		else
		{
			DamageCaused = HandleDamageWithoutArmor(DamageAmount);
		}

		// Xử lý sát thương cho AI
		if (AICharacter)
		{
			DamageCaused = HandleAIDamage(DamageAmount);
		}

		// Kiểm tra nếu nhân vật bị giết
		if (GetHealth() <= 0.0f)
		{
			HandlePlayerDeath(PlayerCharacter, KilledCharacter);
		}

		// Cập nhật sát thương lên client
		CltDamagePlayerLocally();
	}

	// Xử lý hiệu ứng máu nếu có sát thương từ điểm
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		SpawnBloodEffect(DamageEvent);
	}

	return DamageCaused;
}

float AThirdPersonCharacter::HandleDamageWithArmor(float DamageAmount)
{
	float DamageCaused = DamageAmount;
	Armor = FMath::Clamp(Armor - DamageAmount, 0.f, 100.f);

	if (GetArmor() > 0)
	{
		DamageCaused = FMath::Min(DamageAmount, Armor);
		Armor -= DamageCaused;
	}
	else
	{
		float RemainingDamage = DamageAmount - Armor;
		Health = FMath::Clamp(Health - RemainingDamage, 0.f, 100.f);
		DamageCaused = RemainingDamage;
	}

	return DamageCaused;
}

float AThirdPersonCharacter::HandleDamageWithoutArmor(float DamageAmount)
{
	bEquipedArmor = false;
	float DamageCaused = FMath::Min(Health, DamageAmount);
	Health = FMath::Clamp(Health - DamageCaused, 0.f, 100.f);
	return DamageCaused;
}

float AThirdPersonCharacter::HandleAIDamage(float DamageAmount)
{
	float DamageCaused = FMath::Min(AICharacter->MaxHealth, DamageAmount);
	AICharacter->MaxHealth = FMath::Clamp(AICharacter->MaxHealth - DamageCaused, 0.f, AICharacter->MaxHealth);

	if (AICharacter->GetMaxHealth() <= 0.f)
	{
		HandleAIDeath();
	}

	return DamageCaused;
}

void AThirdPersonCharacter::HandleAIDeath()
{
	bIsAlive = false;
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UGameplayStatics::PlaySoundAtLocation(this, DieSound, GetMesh()->GetComponentLocation());
	UE_LOG(LogTemp, Warning, TEXT("AI Character has died!"));
}

void AThirdPersonCharacter::HandlePlayerDeath(AThirdPersonCharacter* PlayerCharacter, AThirdPersonCharacter* KilledCharacter)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->OnKill(KilledCharacter);
	}
	HandleDeath();
}

void AThirdPersonCharacter::SpawnBloodEffect(const FDamageEvent& DamageEvent)
{
	FVector HitLocation = ((FPointDamageEvent*)&DamageEvent)->HitInfo.Location;
	FRotator HitRotation = ((FPointDamageEvent*)&DamageEvent)->HitInfo.Normal.Rotation();
	UNiagaraComponent* BloodEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this, P_Blood, HitLocation, HitRotation, FVector(1, 1, 1), EAttachLocation::KeepRelativeOffset, true, ENCPoolMethod::AutoRelease);
	BloodEffect->Activate(true);
}

void AThirdPersonCharacter::HandleDeath()
{
	bIsAlive = false;
	DisableInput(GetWorld()->GetFirstPlayerController());
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UGameplayStatics::PlaySoundAtLocation(this, DieSound, GetMesh()->GetComponentLocation());

	UE_LOG(LogTemp, Warning, TEXT("You died!"));

	Mlt_PlayerDeath();  // Multiplayer death handling
}

void AThirdPersonCharacter::SrvJumpFromPlane_Implementation()
{
	if (bIsInPlane)
	{
		bIsInPlane = false;
		if (AAir_Plane* Plane = Cast<AAir_Plane>(UGameplayStatics::GetActorOfClass(GetWorld(),AAir_Plane::StaticClass())))
		{
			//Spawn the player a little bit lower than the plane
			SetActorLocation(Plane->GetActorLocation() - FVector(0, 0, 1000));
			CltSwitchPlayerViewToCharacter();
			Mlt_SkydiveForPlayer();
		}
	}
}

void AThirdPersonCharacter::CltSwitchPlayerViewToCharacter_Implementation()
{
	OnSwitchPlayerViewToCharacter();
}

void AThirdPersonCharacter::SrvSwitchPlayerViewToPlane_Implementation()
{
	if (!bIsInPlane)
	{
		bIsInPlane = true;
		CltSwitchPlayerViewToPlane();
	}
}

void AThirdPersonCharacter::CltSwitchPlayerViewToPlane_Implementation()
{
	OnSwitchPlayerViewToPlane();
}

void AThirdPersonCharacter::Mlt_ShootWeapon_Implementation(UParticleSystem* ParticleToSpawn)
{
	UGameplayStatics::SpawnEmitterAttached(ParticleToSpawn,
											EquippedItem->GetSkeletalMesh(),
											"Muzzle",
											FVector(),
											FRotator(),
											FVector(0.2,0.2,0.2),
											EAttachLocation::KeepRelativeOffset,
											true,
											EPSCPoolMethod::AutoRelease);
}

void AThirdPersonCharacter::SrvShootWeapon_Implementation()
{
	// Check if the character can fire and has ammo
	if (!bCanFire || GunAmmo <= 0)
	{
		HandleOutOfAmmo();
		return;
	}

	// Check if the equipped item is a gun
	if (bIsEquipGun && EquippedItem && EquippedItem->ItemType == EItemtypes::Weapon)
	{
		// Cast to weapon and verify
		WeaponRef = Cast<AWeapon>(EquippedItem);

		if (WeaponRef)
		{
			// Decrease ammo count and check if still has enough
			DecreaseAmmo();

			// Execute firing effects and recoil
			Mlt_ShootWeapon(WeaponRef->MuzzleParticle);
			PlayShootSound();
			ApplyRecoil();

			// Fire the ammo if AmmoClass is valid
			FireAmmo();

			// Process hit detection for collision on shot
			ProcessHit();

			// Start cooldown for firing rate control
			StartFireRateCooldown();
		}
	}
}

void AThirdPersonCharacter::HandleOutOfAmmo()
{
	bCanFire = false;
	Clt_GunStoppedFire(bCanShoot);
}

void AThirdPersonCharacter::DecreaseAmmo()
{
	GunAmmo = FMath::Clamp(GunAmmo - 1, 0, 30);

	if (GunAmmo <= 0)
	{
		bCanFire = false; // Dừng bắn nếu hết đạn
		UE_LOG(LogTemp, Warning, TEXT("Out of ammo!"));
		Clt_GunStoppedFire(bCanFire); // Cập nhật thông tin đạn và logic bắn
	}
}

void AThirdPersonCharacter::PlayShootSound()
{
	UGameplayStatics::PlaySoundAtLocation(this, ShootSound, EquippedItem->GetSkeletalMesh()->GetBoneLocation("Muzzle"));
}

void AThirdPersonCharacter::ApplyRecoil()
{
	FRotator RecoilRotation = FRotator(-RecoilPitch, FMath::RandRange(-RecoilYaw, RecoilYaw), 0);
	AddControllerPitchInput(RecoilRotation.Pitch);
	AddControllerYawInput(RecoilRotation.Yaw);
}

void AThirdPersonCharacter::FireAmmo()
{
	if (AmmoClass)
	{
		FVector MuzzleLocation = EquippedItem->GetSkeletalMesh()->GetSocketLocation("Muzzle");
		FRotator MuzzleRotation = EquippedItem->GetSkeletalMesh()->GetSocketRotation("Muzzle");

		// Apply pitch adjustment if needed
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the AGunAmmo actor at the Muzzle location and rotation
			AGunAmmo* AmmoActor = World->SpawnActor<AGunAmmo>(AmmoClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (AmmoActor)
			{
				// Launch ammo in the muzzle's forward direction
				FVector LaunchDirection = MuzzleRotation.Vector();
				AmmoActor->FireInDirection(LaunchDirection);
			}
		}
	}
}

void AThirdPersonCharacter::ProcessHit()
{
	if (!DamageClass)
	{
		DamageClass = UDamageType::StaticClass(); // Gán giá trị mặc định nếu không có DamageClass
	}

	// Vị trí bắt đầu và kết thúc của line trace
	FVector StartLocation = EquippedItem->GetSkeletalMesh()->GetSocketLocation("Muzzle");
	FVector EndLocation = (EquippedItem->GetSkeletalMesh()->GetSocketTransform("Muzzle", RTS_World).GetRotation().GetForwardVector() * 3000.f) + StartLocation;

	// Thực hiện line trace
	FHitResult LocalHitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(LocalHitResult, StartLocation, EndLocation, ECC_Visibility);

	// Debug: Vẽ đường ray và điểm va chạm (nếu có)
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor(100, 0, 0), 5);
	if (bHit)
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), LocalHitResult.Location, 10, 10, FLinearColor::Yellow, 5);
	}

	// Nếu va chạm thành công, xử lý các đối tượng va chạm
	if (bHit)
	{
		AActor* HitActor = LocalHitResult.GetActor();

		// Xử lý va chạm với các nhân vật người chơi
		if (AThirdPersonCharacter* Character = Cast<AThirdPersonCharacter>(HitActor))
		{
			AdjustDamageByHitLocation(LocalHitResult.BoneName, DamageValues); // Điều chỉnh sát thương theo vị trí va chạm
			UGameplayStatics::ApplyPointDamage(Character, DamageValues, StartLocation, LocalHitResult, GetController(), this, DamageClass);
		}
		// Xử lý va chạm với AI
		else if (AAICharacterBase* AICharacters = Cast<AAICharacterBase>(HitActor))
		{
			AdjustDamageByHitLocation(LocalHitResult.BoneName, DamageValues); // Điều chỉnh sát thương theo vị trí va chạm
			UGameplayStatics::ApplyPointDamage(AICharacters, DamageValues, StartLocation, LocalHitResult, GetController(), this, DamageClass);
		}
	}
	else
	{
		// Nếu không có va chạm, thông báo debug
		UE_LOG(LogTemp, Warning, TEXT("Line Trace Failed"));
	}
}

void AThirdPersonCharacter::StartFireRateCooldown()
{
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &AThirdPersonCharacter::ResetFire, FireRate, false);
}

void AThirdPersonCharacter::ToggleGun()
{
	if (bIsEquipWeapon || bIsEquipFlashlight || bIsEquipPistol) return;

	if (EquippedItem)
	{
		if (!bIsEquipGun)
		{
			DrawGun();
		}
		else
		{
			SheathGun();
		}
	}
}

void AThirdPersonCharacter::DrawGun()
{
	if (DrawGunMontage)
	{
		this->PlayAnimMontage(DrawGunMontage, 1.0f);
	}

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	EquippedItem->AttachToComponent(GetMesh(), AttachRules, FName("MainWeaponSocket"));

	bIsEquipGun = true;
	WeaponSlot = 2;
}

void AThirdPersonCharacter::SheathGun()
{
	if (SheathGunMontage)
	{
		this->PlayAnimMontage(SheathGunMontage, 1.0f);
	}

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	EquippedItem->AttachToComponent(GetMesh(), AttachRules, FName("EquipGun"));

	bIsEquipGun = false;
	WeaponSlot = 0;
}

void AThirdPersonCharacter::ToggleFlashlights()
{
	if (bIsEquipWeapon || bIsEquipGun || bIsEquipPistol) return;

	if (EquippedFlashlight)
	{
		if (!bIsEquipFlashlight)
		{
			DrawFlashlight();
		}
		else
		{
			SheathFlashlight();
		}
	}
}

void AThirdPersonCharacter::DrawFlashlight()
{
	if (EquippedFlashlight && DrawFlashlightMontage)
	{
		this->PlayAnimMontage(DrawFlashlightMontage, 1.0f);
		WeaponSlot = 3;
		bIsEquipFlashlight = true;

		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
		EquippedFlashlight->AttachToComponent(GetMesh(), AttachRules, FName("FlashLight"));
		EquippedFlashlight->FlashLightMesh->SetVisibility(true);
	}
}

void AThirdPersonCharacter::SheathFlashlight()
{
	if (EquippedFlashlight && SheathFlashlightMontage)
	{
		this->PlayAnimMontage(SheathFlashlightMontage, 1.0f);
		WeaponSlot = 0;
		bIsEquipFlashlight = false;

		EquippedFlashlight->FlashLightMesh->SetVisibility(false);
	}
}

void AThirdPersonCharacter::TogglePistol()
{
	if (bIsEquipWeapon || bIsEquipGun || bIsEquipFlashlight) return;

	if (EquippedPistol)
	{
		if (!bIsEquipPistol)
		{
			DrawPistol();
		}
		else
		{
			SheathPistol();
		}
	}
}

void AThirdPersonCharacter::DrawPistol()
{
	if (DrawPistolMontage) // Check if montage is valid
	{
		this->PlayAnimMontage(DrawPistolMontage, 1.0f); // Play equip montage
	}

	WeaponSlot = 5;
	bIsEquipPistol = true;

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	EquippedPistol->AttachToComponent(GetMesh(), AttachRules, FName("Pistol"));
}

void AThirdPersonCharacter::SheathPistol()
{
	if (SheathPistolMontage) // Check if montage is valid
	{
		this->PlayAnimMontage(SheathPistolMontage, 1.0f); // Play equip montage
	}

	WeaponSlot = 0;
	bIsEquipPistol = false;

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	EquippedPistol->AttachToComponent(GetMesh(), AttachRules, FName("EquipPistol"));
}

void AThirdPersonCharacter::PlayerEquipWeapon()
{
	
}

void AThirdPersonCharacter::FirePistol()
{
	// Only allow firing if there is ammo available
	if (!bCanShootPistol || PistolAmmo <= 0)
	{
		HandlePistolOutofAmmo();
		return;
	}

	// Ensure the pistol is equipped and the type is Desert Eagle
	if (bIsEquipPistol && EquippedPistol && EquippedPistol->PistolType == EPistolTypes::DesertEagle)
	{
		// Process ammo and handle firing effects
		ProcessHitAmmo();
		MuzzleParticalPistol();
		UGameplayStatics::PlaySoundAtLocation(this, DesertEagleSound, EquippedPistol->PistolMesh->GetSocketLocation(FName("Muzzle")));

		// Reduce ammo by 1 and check if we are out of ammo after firing
		PlayerPistolAmmo();
	}
}

void AThirdPersonCharacter::ProcessHitAmmo()
{
	if (!DamageClass)
	{
		DamageClass = UDamageType::StaticClass();
	}

	// Get the start location from the muzzle socket
	FVector StartLocation = EquippedPistol->PistolMesh->GetSocketLocation(FName("Muzzle"));

	// Get the forward vector of the muzzle socket's world transform
	FVector ForwardVector = EquippedPistol->PistolMesh->GetSocketTransform(FName("Muzzle"), RTS_World).GetRotation().GetForwardVector();

	// Calculate the end location by extending the forward vector from the start location
	FVector EndLocation = StartLocation + (ForwardVector * 3000.f);

	FHitResult LocalHitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(LocalHitResult, StartLocation, EndLocation, ECC_Visibility);

	// Draw debug line to visualize the shot path
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor(100, 0, 0), 5);
	if (bHit)
	{
		// Draw a debug sphere at the hit location
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), LocalHitResult.Location, 10, 10, FLinearColor::Yellow, 5);
	}

	if (bHit)
	{
		AActor* HitActor = LocalHitResult.GetActor();

		if (AThirdPersonCharacter* Character = Cast<AThirdPersonCharacter>(HitActor))
		{
			AdjustDamageByHitLocation(LocalHitResult.BoneName, DamageValues);
			UGameplayStatics::ApplyPointDamage(Character, DamageValues, ForwardVector, LocalHitResult, GetController(), this, DamageClass);
		}
		else if (AAICharacterBase* AICharacters = Cast<AAICharacterBase>(HitActor))
		{
			AdjustDamageByHitLocation(LocalHitResult.BoneName, DamageValues);
			UGameplayStatics::ApplyPointDamage(AICharacters, DamageValues, ForwardVector, LocalHitResult, GetController(), this, DamageClass);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Line Trace Failed"));
	}
}

void AThirdPersonCharacter::MuzzleParticalPistol()
{
	UGameplayStatics::SpawnEmitterAttached(EquippedPistol->MuzzleParticle,
		EquippedPistol->PistolMesh,
		FName("Muzzle"),
		FVector(),
		FRotator(),
		FVector(1.0f, 1.0f, 1.0f),
		EAttachLocation::KeepRelativeOffset,
		true,
		EPSCPoolMethod::AutoRelease);
}

void AThirdPersonCharacter::PlayerPistolAmmo()
{
	// Reduce ammo by 1, but ensure it doesn't go below 0
	PistolAmmo = FMath::Clamp(PistolAmmo - 1, 0, 12);

	// Check if ammo is depleted and handle out-of-ammo case
	if (PistolAmmo <= 0)
	{
		HandlePistolOutofAmmo();
	}
}

void AThirdPersonCharacter::DesertEagleAmmo()
{
	// Disable movement during pickup
	GetCharacterMovement()->DisableMovement();

	// Add picked-up ammo to total ammo
	AddAmmo(SumPistolAmmo,AmmoAmountPistol);

	// Pickup ammo and reset the nearby ammo pointer
	if (NearbyAmmo)
	{
		EquippedAmmo = NearbyAmmo;
		NearbyAmmo->PickupPistol();
		NearbyAmmo = nullptr;

		// Hide the ammo mesh
		if (EquippedAmmo && EquippedAmmo->AmmoMesh)
		{
			EquippedAmmo->AmmoMesh->SetVisibility(false);
		}
	}

	// Re-enable movement after a short delay
	GetWorld()->GetTimerManager().SetTimer(PickupTimerHandle, this, &AThirdPersonCharacter::EnableMovement, 4.5f, false);
}

void AThirdPersonCharacter::HandlePistolOutofAmmo()
{
	bCanShootPistol = false;
	PistolStopFire();
}

void AThirdPersonCharacter::PistolStopFire()
{
	// Check if there is ammo to determine firing status
	bIsStopFire = (PistolAmmo > 0);

	if (!bIsStopFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("Out of Ammo"));
	}
}

void AThirdPersonCharacter::PistolAmmoActor()
{
	if (AmmoClass)
	{
		FVector MuzzleLocation = EquippedPistol->PistolMesh->GetSocketLocation("Muzzle");
		FRotator MuzzleRotation = EquippedPistol->PistolMesh->GetSocketRotation("Muzzle");

		// Apply pitch adjustment if needed
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the AGunAmmo actor at the Muzzle location and rotation
			AGunAmmo* AmmoActor = World->SpawnActor<AGunAmmo>(AmmoClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (AmmoActor)
			{
				// Launch ammo in the muzzle's forward direction
				FVector LaunchDirection = MuzzleRotation.Vector();
				AmmoActor->FireInDirection(LaunchDirection);
			}
		}
	}
}

void AThirdPersonCharacter::ReloadPistolCoolDown()
{
	bIsReload = false;
}

void AThirdPersonCharacter::AmmoProcess(int32& AmountAmmo, int32& SumAmmo, int32 MaxAmmo)
{
	if (SumAmmo > 0)
	{
		// Calculate the amount of ammo that can be reloaded
		int32 MaxReloadAmount = FMath::Min(MaxAmmo - AmountAmmo, SumAmmo);
		AmountAmmo += MaxReloadAmount; // Increase ammo in the magazine
		SumAmmo -= MaxReloadAmount;    // Decrease the total ammo pool

		if (bIsEquipPistol && !bIsReload && EquippedPistol && EquippedPistol->PistolMesh)
		{
			bIsReload = true;

			// Play reload sound and animation
			UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, EquippedPistol->PistolMesh->GetSocketLocation("Reload"));
			PlayAnimMontage(ReloadPistolMontage, 1.0f);

			// Set a timer to reset the reload state
			GetWorld()->GetTimerManager().SetTimer(ResetReloadGunTimerHandle, this, &AThirdPersonCharacter::ReloadPistolCoolDown, 3.0f, false);
		}
		else if (bIsEquipGun && !bIsReload && EquippedItem && EquippedItem->GetSkeletalMesh())
		{
			bIsReload = true;

			// Play reload sound and animation
			UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, EquippedItem->GetSkeletalMesh()->GetBoneLocation("Reload"));
			PlayAnimMontage(ReloadMontage, 1.0f);

			// Set a timer to reset the reload state
			GetWorld()->GetTimerManager().SetTimer(ResetReloadGunTimerHandle, this, &AThirdPersonCharacter::ReloadPistolCoolDown, 3.0f, false);
		}
	}
	else
	{
		// If there's no ammo to reload
		bIsReload = false;
	}
}

void AThirdPersonCharacter::ResetRecoil(float DeltaTime)
{
	if (Controller)
	{
		FRotator CurrentRotation = Controller->GetControlRotation();
		FRotator TargetRotation = FRotator(0.0f, 0.0f, 0.0f); // Rotation to reset to (original)
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RecoilResetSpeed);
		Controller->SetControlRotation(NewRotation);
	}
}

void AThirdPersonCharacter::AttackSword()
{
	// Kiểm tra xem nhân vật có thể tấn công hay không
	if (EquippedSword && !bIsAttacking && bIsEquipWeapon)
	{
		bIsAttacking = true;

		// Phát AnimMontage dựa trên bước combo hiện tại
		switch (ComboCount)
		{
		case 0:
			if (ComboMontage1)
			{
				PlayAnimMontage(ComboMontage1, 1.0f);
				ComboCount = 1;
			}
			break;
		case 1:
			if (ComboMontage2)
			{
				PlayAnimMontage(ComboMontage2, 1.0f);
				ComboCount = 2;
			}
			break;
		case 2:
			if (ComboMontage3)
			{
				PlayAnimMontage(ComboMontage3, 1.0f);
				ComboCount = 0; // Reset combo sau khi tấn công lần 3
			}
			break;
		}

		// Đặt thời gian reset combo nếu không tấn công tiếp theo
		GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);
		GetWorldTimerManager().SetTimer(ComboResetTimerHandle, this, &AThirdPersonCharacter::ResetCombo, ComboResetTime, false);
	}
}

void AThirdPersonCharacter::ResetCombo()
{
	ComboCount = 0;
	bIsAttacking = false; // Cho phép tấn công lại
}

void AThirdPersonCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false; // Cho phép tấn công tiếp theo khi kết thúc AnimMontage
}

void AThirdPersonCharacter::ToggleWeaponEquip()
{
	if (bIsEquipGun || bIsEquipFlashlight || bIsEquipPistol) return;

	// Check if the sword has been picked up (i.e., EquippedSword is valid)
	if (EquippedSword)
	{
		// Check if the weapon is currently equipped
		if (!bIsEquipWeapon)
		{
			EquipWeapon();
		}
		else
		{
			SheatheWeapon();
		}
	}
	else
	{
		// Sword has not been picked up yet, so you cannot toggle
		UE_LOG(LogTemp, Warning, TEXT("You need to pick up the sword first!"));
	}
}

void AThirdPersonCharacter::EquipWeapon()
{
	// Play the Equip animation montage
	if (DrawMontage) // Check if montage is valid
	{
		this->PlayAnimMontage(DrawMontage, 1.0f); // Play equip montage
	}

	// Attach sword to the character's hand
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	EquippedSword->AttachToComponent(GetMesh(), AttachRules, FName("Sword")); // Attach to the "Sword"

	// Update the Equipped variable and Weapon Slot
	bIsEquipWeapon = true;
	WeaponSlot = 1; // Update weapon slot (assuming WeaponSlot is an int)
}

void AThirdPersonCharacter::SheatheWeapon()
{
	// Play the Sheathe animation montage
	if (UnarmedEquipMontage) // Check if montage is valid
	{
		this->PlayAnimMontage(UnarmedEquipMontage, 1.0f); // Play sheathe montage
	}

	// Attach sword to the character's back
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	EquippedSword->AttachToComponent(GetMesh(), AttachRules, FName("EquipSword")); // Attach to the "EquipSword"

	// Update the Equipped variable and Weapon Slot
	bIsEquipWeapon = false;
	WeaponSlot = 0; // Reset weapon slot
}

void AThirdPersonCharacter::SpawnSkill()
{
	// Check if the skill is on cooldown
	if (bIsSkillOnCooldown)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill is on cooldown! Please wait."));
		return; // Exit if the skill is on cooldown
	}

	// Check if the sword is equipped and if it has a skill available
	if (bIsEquipWeapon && EquippedSword && EquippedSword->Skill1)
	{
		// Play skill summon animation
		this->PlayAnimMontage(SummonSkillMontage, 1.0f);

		// Set spawn location and rotation
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() + 500.0f; // Adjusted to spawn in front of the character
		FRotator SpawnRotation = GetActorRotation();

		// Disable movement during skill casting
		GetCharacterMovement()->DisableMovement();

		// Spawn the skill
		EquippedSword->SpawnSkill1(SpawnLocation, SpawnRotation);

		// Set skill cooldown
		bIsSkillOnCooldown = true;
		CurrentCooldownTime = SkillCoolDown;

		// Play sound effect at character's location
		UGameplayStatics::PlaySoundAtLocation(this, ThunderSound, GetActorLocation());

		// Set timer to reset skill cooldown
		GetWorldTimerManager().SetTimer(SkillTimerHandle, this, &AThirdPersonCharacter::ResetSkillCooldown, SkillCoolDown, false);
		GetWorldTimerManager().SetTimer(PickupTimerHandle, this, &AThirdPersonCharacter::EnableMovement, 2.0f, false);
	}
	else
	{
		// Log if the sword is not equipped or skill is unavailable
		UE_LOG(LogTemp, Warning, TEXT("Sword is not equipped or no skill available!"));
	}
}

void AThirdPersonCharacter::ResetSkillCooldown()
{
	bIsSkillOnCooldown = false; // Reset the cooldown
	CurrentCooldownTime = 0.0f;
}

void AThirdPersonCharacter::UpdateCooldownTime(float DeltaTime)
{
	if (bIsSkillOnCooldown)
	{
		CurrentCooldownTime = FMath::Max(CurrentCooldownTime - DeltaTime, 0.0f);
	}
}

void AThirdPersonCharacter::LineTraceSword()
{
	// Ensure ActorSword and its mesh are valid
	if (!ActorSword || !ActorSword->SwordMesh) return;

	// Get Socket Location for the start and end points of the sword
	FVector StartLocation = ActorSword->SwordMesh->GetSocketLocation(FName("Start"));
	FVector EndLocation = ActorSword->SwordMesh->GetSocketLocation(FName("End"));

	// Setup LineTrace
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // Ignore self
	TraceParams.AddIgnoredActor(ActorSword); // Ignore sword itself

	// Debug line for visualization
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1, 0, 1);

	// Perform LineTrace to detect hit
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			// Play hit reaction animation if provided
			if (HitMontage)
			{
				PlayAnimMontage(HitMontage, 1.0f);
			}

			// Apply damage based on whether the hit actor is a player or AI character
			if (AThirdPersonCharacter* HitPlayerCharacter = Cast<AThirdPersonCharacter>(HitActor))
			{
				AdjustDamageByHitLocation(HitResult.BoneName, DamageValues);
				UGameplayStatics::ApplyPointDamage(HitPlayerCharacter, DamageValues, StartLocation, HitResult, GetController(), this, DamageClass);
			}
			else if (AAICharacterBase* HitAICharacter = Cast<AAICharacterBase>(HitActor))
			{
				AdjustDamageByHitLocation(HitResult.BoneName, DamageValues);
				UGameplayStatics::ApplyPointDamage(HitAICharacter, DamageValues, StartLocation, HitResult, GetController(), this, DamageClass);
			}
		}
	}
}

void AThirdPersonCharacter::AdjustDamageByHitLocation(FName BoneName, float& DamageValue)
{
	if (BoneName == "head")
	{
		DamageValue *= 2.0f; // Double damage for headshots
	}
	else if (BoneName == "rightfoot" || BoneName == "leftfoot")
	{
		DamageValue *= 0.5f; // Half damage for foot shots
	}
}

void AThirdPersonCharacter::ResetFire()
{
	bCanFire = true;
}

void AThirdPersonCharacter::Clt_PlayerBackpack_Implementation()
{
	//if (EquippedItem && EquippedItem->ItemType == EItemtypes::Backpack)
	//{
	//	bIsBackpack = true;
	//	int32 AdditionalSlots = GetInventory()->GetSlotsCapacity();
	//	float AdditionalWeightCapacity = GetInventory()->GetWeightCapacity();

	//	this->GetInventory()->SetSlotsCapacity(AdditionalSlots + 100);
	//	this->GetInventory()->SetWeightCapacity(AdditionalWeightCapacity + 400.f);
	//}
}

void AThirdPersonCharacter::SrvHealthPotion_Implementation()
{
	if (EquippedItem && EquippedItem->ItemType == EItemtypes::HealthPotion)
	{
		HealthRef = Cast<Ahealth>(EquippedItem);
		if (HealthRef)
		{
			bIsDrink = true;
			Mlt_ShootWeapon(HealthRef->HealthParticle);

			UGameplayStatics::PlaySoundAtLocation(this, HealthSound, EquippedItem->GetSkeletalMesh()->GetBoneLocation("HealthPotion"));

			float HealthPoint = 20.f;

			if (Health < 100.f)
			{
				Health += HealthPoint;
			}
			else
			{
				Health += 0;
			}

			// put TimerHandle before cancel object
			FTimerHandle DrinkHealthPotionTimerHandle;
			GetWorldTimerManager().SetTimer(DrinkHealthPotionTimerHandle, this, &AThirdPersonCharacter::DestroyHealthItem, 3.7f, false); // Wait for 3.7 seconds before cancel health potion
		}
	}
}

void AThirdPersonCharacter::DestroyHealthItem()
{
	if (EquippedItem)
	{
		// Hủy đối tượng thuốc hồi máu
		EquippedItem->Destroy();
		EquippedItem = nullptr; // Đặt lại tham chiếu
	}
}

void AThirdPersonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AThirdPersonCharacter, bIsInPlane);
	DOREPLIFETIME(AThirdPersonCharacter, bIsInZone);
	DOREPLIFETIME(AThirdPersonCharacter, bIsAlive);
	DOREPLIFETIME(AThirdPersonCharacter, CurrentEquippedItem);
	DOREPLIFETIME_CONDITION(AThirdPersonCharacter, ReplicatedControlRotation, COND_SkipOwner);
	DOREPLIFETIME(AThirdPersonCharacter, bIsAiming);
	DOREPLIFETIME(AThirdPersonCharacter, bIsDrink);
	DOREPLIFETIME(AThirdPersonCharacter, bIsReload);
}

void AThirdPersonCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	Mlt_PlayerLanded();
}

void AThirdPersonCharacter::ReplicateControlRotation()
{
	if (HasAuthority() || IsLocallyControlled())
	{
		ReplicatedControlRotation = GetController()->GetControlRotation();
	}
}

void AThirdPersonCharacter::SrvAimPlayer_Implementation(bool bShouldAim)
{
	if (bIsEquipGun && EquippedItem && EquippedItem->ItemType == Weapon)
	{
		StopAiming();
		bIsAiming = bShouldAim;
		Clt_AimPlayer(bShouldAim);
	}
	else if(bIsEquipPistol && EquippedPistol && EquippedPistol->PistolType == EPistolTypes::DesertEagle)
	{
		StopAiming();
		bIsAiming = bShouldAim;
		Clt_AimPlayer(bShouldAim);
	}
	else
	{
		bIsAiming = false;
		Clt_AimPlayer(false);
	}
}

void AThirdPersonCharacter::Clt_AimPlayer_Implementation(bool bShouldAim)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (bShouldAim)
	{
		StopAiming();
		if (bIsEquipGun && EquippedItem && EquippedItem->ItemType == EItemtypes::Weapon)
		{
			WeaponRef = Cast<AWeapon>(EquippedItem);
			if (WeaponRef)
			{
				if (PlayerController)
				{
					PlayerController->SetViewTarget(WeaponRef);
				}
			}
		}
		else if (bIsEquipPistol && EquippedPistol && EquippedPistol->PistolType == EPistolTypes::DesertEagle)
		{
			PistolRef = Cast<APistol>(EquippedPistol);
			if (PistolRef)
			{
				if(PlayerController)
				{
					PlayerController->SetViewTarget(PistolRef);
				}
			}
		}
	}
	else
	{
		if (PlayerController)
		{
			PlayerController->SetViewTarget(this);
		}
	}
}

void AThirdPersonCharacter::SetupStimulusSource()
{
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
		if (StimulusSource)
		{
			StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
			StimulusSource->RegisterWithPerceptionSystem();
		}
}

void AThirdPersonCharacter::PickupFlashlight()
{
	if (NearbyFlashlight && !NearbyFlashlight->IsPickedUp())
	{
		// Disable movement during pickup
		GetCharacterMovement()->DisableMovement();

		EquippedFlashlight = NearbyFlashlight;
		NearbyFlashlight->PickupFlashlight();
		NearbyFlashlight = nullptr;

		EquippedFlashlight->FlashLightMesh->SetVisibility(false);

		// Optionally, re-enable movement after a short delay
		GetWorld()->GetTimerManager().SetTimer(PickupTimerHandle, this, &AThirdPersonCharacter::EnableMovement, 4.5f, false);
	}
}

void AThirdPersonCharacter::PickupPistol()
{
	if (NearbyPistol && !NearbyPistol->IsPickedUp() && NearbyPistol->PistolType == EPistolTypes::DesertEagle)
	{
		// Disable movement during pickup
		GetCharacterMovement()->DisableMovement();

		EquippedPistol = NearbyPistol;
		NearbyPistol->PickupPistol();
		NearbyPistol = nullptr;

		EquippedPistol->PistolMesh->SetVisibility(true);
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget,true);
		EquippedPistol->AttachToComponent(GetMesh(), AttachRules, FName("EquipPistol"));

		// Optionally, re-enable movement after a short delay
		GetWorld()->GetTimerManager().SetTimer(PickupTimerHandle, this, &AThirdPersonCharacter::EnableMovement, 4.5f, false);
	}
}

void AThirdPersonCharacter::PickupAmmoType()
{
	DesertEagleAmmo();
	/*if (NearbyAmmo)
	{
		switch (NearbyAmmo->AmmoType)
		{
		case EAmmoTypes::DesertEagle:
			
			break;
		default:
			break;
		}
	}*/
}

void AThirdPersonCharacter::PickupItem()
{
	// Check if there's a valid nearby item that hasn't been picked up yet
	if (!NearbyItem || NearbyItem->IsPickedUpItem()) return;

	switch (NearbyItem->ItemType)
	{
	case EItemtypes::HealthPotion:
		PickupHealthPotion();
		break;

	case EItemtypes::Weapon:
		PickupWeapon();
		break;

	case EItemtypes::Armor:
		PickupArmor();
		break;

	case EItemtypes::Backpack:
		PickupBackpack();
		break;

	case EItemtypes::Ammo:
		PickupAmmo();
		break;

	default:
		break;
	}
}

void AThirdPersonCharacter::PickupHealthPotion()
{
	SetPickupItemLocation();
	EquippedItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HealthPotion"));
	WeaponSlot = 0;

}

void AThirdPersonCharacter::PickupWeapon()
{
	WeaponSlot = 0;
	bIsPrimaryWeapon = true;
	SetPickupItemLocation();
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	EquippedItem->AttachToComponent(GetMesh(), AttachRules, FName("EquipGun"));
}

void AThirdPersonCharacter::PickupArmor()
{
	SetPickupItemLocation();
	EquippedItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Armor"));
	WeaponSlot = 0;
}

void AThirdPersonCharacter::PickupBackpack()
{
	SetPickupItemLocation();
	EquippedItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Backpack"));
	WeaponSlot = 0;
}

void AThirdPersonCharacter::PickupAmmo()
{
	SetPickupItemLocation();
	AddAmmo(SumGunAmmo,AmmoAmount);
	if (NearbyItem)
	{
		NearbyItem->SetActorHiddenInGame(true);
		NearbyItem->SetActorEnableCollision(false);
	}
	WeaponSlot = 0;
}

void AThirdPersonCharacter::SetPickupItemLocation()
{
	// Disable movement during pickup
	GetCharacterMovement()->DisableMovement();
	EquippedItem = NearbyItem;
	NearbyItem->PickupItem();

	// Re-enable movement after a short delay
	GetWorld()->GetTimerManager().SetTimer(PickupTimerHandle, this, &AThirdPersonCharacter::EnableMovement, 4.5f, false);
}

bool AThirdPersonCharacter::PickupSword()
{
	if (NearbySword && !NearbySword->IsPickedUp() && NearbySword->SwordType == ESwordtypes::Sword)
	{
		if (!bIsItemSlot)
		{
			WeaponSlot = 0;

			// Disable movement during pickup
			GetCharacterMovement()->DisableMovement();

			bIsItemSlot = true;

			EquippedSword = NearbySword;
			NearbySword->PickupSword();

			// Attach Sword to the character's hand (socket "EquipSword")
			FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
			EquippedSword->AttachToComponent(GetMesh(), AttachRules, FName("EquipSword"));

			// Optionally, re-enable movement after a short delay
			GetWorld()->GetTimerManager().SetTimer(PickupTimerHandle, this, &AThirdPersonCharacter::EnableMovement, 4.5f, false);

			return true;
		}
	}
	return false;
}

void AThirdPersonCharacter::EnableMovement()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AThirdPersonCharacter::FlashlightActor()
{
	// Tìm đèn pin gần nhân vật để nhặt
	GetOverlappingActors(OverlappingActors, AFlashLight::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		NearbyFlashlight = Cast<AFlashLight>(OverlappingActors[0]);
	}
	else
	{
		NearbyFlashlight = nullptr;
	}
}

void AThirdPersonCharacter::PistolActor()
{
	GetOverlappingActors(OverlappingActors, APistol::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		NearbyPistol = Cast<APistol>(OverlappingActors[0]);
	}
	else
	{
		NearbyPistol = nullptr;
	}
}

void AThirdPersonCharacter::AmmoActor()
{
	GetOverlappingActors(OverlappingActors, AGunAmmoType::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		NearbyAmmo = Cast<AGunAmmoType>(OverlappingActors[0]);
	}
	else
	{
		NearbyAmmo = nullptr;
	}
}

void AThirdPersonCharacter::ItemActor()
{
	// Tìm vật phẩm gần nhân vật để nhặt
	GetOverlappingActors(OverlappingActors, AItem::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		NearbyItem = Cast<AItem>(OverlappingActors[0]);
	}
	else
	{
		NearbyItem = nullptr;
	}
}

void AThirdPersonCharacter::SwordActor()
{
	//tìm sword gần nhân vật để nhặt
	GetOverlappingActors(OverlappingActors, ASword::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		NearbySword = Cast<ASword>(OverlappingActors[0]);
	}
	else
	{
		NearbySword = nullptr;
	}
}

void AThirdPersonCharacter::ToggleFlashlight()
{
	if (bIsEquipFlashlight && EquippedFlashlight)
	{
		EquippedFlashlight->ToggleLight();
	}
}

int32 AThirdPersonCharacter::AddAmmo(int32& SumAmmo,int32 Amount)
{
	// Check ammo type and update corresponding ammo
	if (AmmoTypeRef && AmmoTypeRef->AmmoType == EAmmoTypes::DesertEagle)
	{
		SumAmmo += Amount;
		return SumAmmo; // Return updated pistol ammo count
	}
	else if (EquippedItem && EquippedItem->ItemType == EItemtypes::Ammo) // Example for gun ammo
	{
		SumAmmo += Amount;
		return SumAmmo; // Return updated gun ammo count
	}

	// Return 0 if no ammo was added
	return 0;
}

void AThirdPersonCharacter::OnKill(AThirdPersonCharacter* KilledCharacter)
{
	if (KilledCharacter)
	{
		// Get experience value from the killed character
		float ExperienceGained = KilledCharacter->GetExperienceValue();
		AddExperience(ExperienceGained); // Add experience to the player

		// Show the gained experience in the widget
		ShowExperience(ExperienceGained);
	}
}

void AThirdPersonCharacter::SwitchCameraPlayer()
{
	// Toggle between first-person and third-person cameras using a flip-flop logic
	if (!bIsFirstPerson)
	{
		// Activate first-person camera and deactivate third-person camera
		FirstPersonCamera->SetActive(true);
		ThirdPersonCamera->SetActive(false);

		// Enable pawn control rotation for first-person perspective
		FirstPersonCamera->bUsePawnControlRotation = true;
		ThirdPersonCamera->bUsePawnControlRotation = false;

		bIsFirstPerson = true;  // Update flag to indicate first-person view
		UE_LOG(LogTemp, Warning, TEXT("The player character is in first-person perspective"));
	}
	else
	{
		// Activate third-person camera and deactivate first-person camera
		ThirdPersonCamera->SetActive(true);
		FirstPersonCamera->SetActive(false);

		// Enable pawn control rotation for third-person perspective
		ThirdPersonCamera->bUsePawnControlRotation = true;
		FirstPersonCamera->bUsePawnControlRotation = false;

		bIsFirstPerson = false;  // Update flag to indicate third-person view
		UE_LOG(LogTemp, Warning, TEXT("The player character is in third-person perspective"));
	}
}

void AThirdPersonCharacter::AddExperience(float Value)
{
	this->Experience += Value;
	PlayerExperience();
}

float AThirdPersonCharacter::PlayerExperience()
{
	// Check if experience exceeds 100
	if (this->Experience >= 100.0f)
	{
		++Level;
		ResetExperience();

		// Prevent leveling up beyond the maximum level
		if (Level >= MaxLevel)
		{
			Level = MaxLevel; // Clamp the level
			return 0; // Return 0 to indicate no more leveling
		}
	}
	return Experience;
}

float AThirdPersonCharacter::ResetExperience()
{
	this->Experience = FMath::Max(0.0f, this->Experience - 100.0f); // Subtract 100 and ensure Experience doesn't go negative
	return Experience;
}

float AThirdPersonCharacter::GetExperienceValue() const
{
	return 20.0f; // Example value for how much experience the player gains
}

void AThirdPersonCharacter::ShowExperience(float Value)
{
	if (ExperienceWidgetClass)
	{
		// Create the widget if it hasn't been created yet
		if (!ExperienceWidgetInstance)
		{
			ExperienceWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ExperienceWidgetClass);
		}

		// Check if the widget is valid and add it to the viewport
		if (ExperienceWidgetInstance && !ExperienceWidgetInstance->IsInViewport())
		{
			ExperienceWidgetInstance->AddToViewport();
		}

		// Assuming ExperienceText is a text block in the widget to show the gained experience
		UTextBlock* ExperienceText = Cast<UTextBlock>(ExperienceWidgetInstance->GetWidgetFromName(TEXT("ExperienceText")));
		if (ExperienceText)
		{
			ExperienceText->SetText(FText::FromString(FString::Printf(TEXT("Gained %f Experience"), Experience)));
		}

		// Set a timer to hide the widget after 3 seconds
		GetWorldTimerManager().SetTimer(ExperienceTimerHandle, this, &AThirdPersonCharacter::HiddenExperience, 3.0f, false);
	}
}

void AThirdPersonCharacter::HiddenExperience()
{
	if (ExperienceWidgetInstance)
	{
		ExperienceWidgetInstance->RemoveFromViewport();
		ExperienceWidgetInstance = nullptr; // Clear the instance so it can be recreated when needed
	}
}

void AThirdPersonCharacter::InteractDialog()
{
	if (CurrentTradeCharacter)
	{
		CurrentTradeCharacter->StartDialog();
	}
}

void AThirdPersonCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	ATradeCharacter* TradeChar = Cast<ATradeCharacter>(OtherActor);
	if (TradeChar)
	{
		CurrentTradeCharacter = TradeChar;
	}
}

void AThirdPersonCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATradeCharacter* TradeChar = Cast<ATradeCharacter>(OtherActor);
	if (TradeChar && TradeChar == CurrentTradeCharacter)
	{
		CurrentTradeCharacter = nullptr;
		UGameplayStatics::GetPlayerController(this, 0)->SetShowMouseCursor(false);
	}
}
