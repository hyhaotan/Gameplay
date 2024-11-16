// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NeverGU/Interfaces/CharacterInterface.h"
#include "NeverGU/Items/Item.h"
#include "NeverGU/AI/AICharacterBase.h"
#include "InputActionValue.h"
#include "NeverGU/Actor/GunAmmoType.h"
#include "NeverGU/Actor/FlashLight.h"
#include "NeverGU/Actor/Pistol.h"
#include "NeverGU/Data/InfoPlayer.h"
#include "NeverGU/Actor/Sword.h"
#include "ThirdPersonCharacter.generated.h"

class AThirdPersonPlayerController;
class UTimelineComponent;
class APlayerHUD;
class UItemBase;
class IInteractionInterface;
class UInventoryComponent;
class UNiagaraSystem;
class ATradeCharacter;
class USkeletalMeshComponent;
class UStaticMeshComponent;

USTRUCT()
struct FInteractionData
{
	GENERATED_USTRUCT_BODY()

	FInteractionData() : CurrentInteractable(nullptr),
	LastInteractionCheckTime(0.0f)
	{

	};

	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};

UCLASS()
class NEVERGU_API AThirdPersonCharacter : public ACharacter,public ICharacterInterface
{
	GENERATED_BODY()

protected:
	//------------------------------------PROPERTIES AND VARIABLES--------------------------------
	UPROPERTY()
	APlayerHUD* HUD;

	UPROPERTY()
	AThirdPersonPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ThirdPersonCamera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FirstPersonCamera" ,meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SpringArm", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	class UAIPerceptionStimuliSourceComponent* StimulusSource;

	UCameraComponent* GetCamera()
	{
		return ThirdPersonCamera;
	}

	USpringArmComponent* GetSpringArm()
	{
		return SpringArm;
	}

	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;

	UPROPERTY(VisibleAnywhere, Category = "Character | Inventory")
	TObjectPtr<UInventoryComponent> PlayerInventory;

	//interaction properties
	FTimerHandle TimerHandle_Interaction;
	FInteractionData InteractionData;

	TObjectPtr<UTimelineComponent> AimingCameraTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Aim Timeline")
	TObjectPtr<UCurveFloat> AimingCameraCurve;
public:
	//------------------------------------PROPERTIES AND VARIABLES--------------------------------


	//----------------------------------------------FUNCTION----------------------------------------
	AThirdPersonCharacter();

	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeighAdjust) override;

	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeighAdjust) override;

	void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); };

	FORCEINLINE UInventoryComponent* GetInventory() const { return PlayerInventory; };

	void UpdateInteractionWidget() const;

	void DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void DropCurrentItem();
protected:

	//------------------------------------------ENHANCED INPUT ACTION-------------------------------------------
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* RunAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* InteractItemAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* ToggleAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* DropItemAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* ScreamAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* TargetLockAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* PickupFlashlightAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* ToggleFlashlightAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* ToggleFlashlightsAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* SwitchCameraAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* ToggleSwordAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* TogglePistolAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* SkillAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInpt")
	class UInputAction* ToggleGunAction;
	//---------------------------------------------------------------------------------------------------

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	//----------------------------------------------FUNCTION----------------------------------------
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void StartRunning();
	void EndRunning();
	void Interact();

	void Aim();
	void StopAiming();
	UFUNCTION()
	void UpdateCameraTimeLine(const float TimelineValue) const;
	UFUNCTION()
	void CameraTimelineEnd();

	void ToggleMenu();
	void PerformInteractionCheck();
	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void BeginInteract();
	void EndInteract();
	void InteractItem();

	UFUNCTION()
	void DodgeForward();
	void ResetDodge();
	void EnableDodge();

	UFUNCTION()
	void DestroyHealthItem();

	UFUNCTION(Client,Reliable)
	void CltSwitchPlayerViewToPlane();

	UFUNCTION(Client, Reliable)
	void CltSwitchPlayerViewToCharacter();

	UFUNCTION(Server, Unreliable)
	void DamagePlayerifnotinZone();

	UFUNCTION(NetMulticast,Unreliable)
	void Mlt_PlayerDeath();

	UFUNCTION(Client, Unreliable)
	void CltDamagePlayerLocally();

	UFUNCTION(NetMulticast, Unreliable)
	void Mlt_SkydiveForPlayer();

	UFUNCTION(NetMulticast, Unreliable)
	void Mlt_PlayerLanded();

	//Animation Crouching
	void StartCrouching();
	void StopCrouching();
	void ToggleCrouch();

	UFUNCTION(BlueprintCallable)
	void EnableStaminaGain();

	UFUNCTION(BlueprintCallable)
	void DepletedAllStamina();

	UFUNCTION(BlueprintCallable)
	void StartSwimming();

	UFUNCTION(BlueprintCallable)
	void StopSwimming();

	UFUNCTION(Client, Unreliable, Category = "Backback")
	void Clt_PlayerBackpack();

	UFUNCTION(BlueprintCallable, Category = "Stopped Shoot")
	void Clt_GunStoppedFire(bool& bCanShoot);

	UFUNCTION(Category = "Reload Ammo")
	void ReloadGun();

	UFUNCTION(Category = "Scream Player")
	void ScreamPlayerCharacter();

	UFUNCTION(Category = "Scream Player")
	void ResetScreamPlayerCharacter();

	UFUNCTION(Category = "Target Lock Enemy")
	void TargetLockEnemy();


	//----------------------------------------PROPERTIES AND VARIABLES------------------------------------------
	//---------------------------------------------------OTHER-----------------------------------------------
	// Đèn pin đã được nhặt hay chưa
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlashLight")
	AFlashLight* EquippedFlashlight;

	// Kiểm tra có đèn pin gần để nhặt hay không
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlashLight")
	AFlashLight* NearbyFlashlight;

	// Reference to the flashlight the character is holding
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlashLight")
	AFlashLight* HeldFlashlight;

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	TSubclassOf<class AGunAmmo> AmmoClass;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	TSubclassOf<UDamageType> DamageClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Target Lock")
	AActor* ActorToTargetLock;

	AItem* NearbyItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pistol")
	APistol* EquippedPistol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pistol")
	APistol* NearbyPistol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	AGunAmmoType* EquippedAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	AGunAmmoType* NearbyAmmo;

	ASword* NearbySword;

	//All class child in AItem class parent
	class AWeapon* WeaponRef;
	class AAmmo* AmmoRef;
	class Ahealth* HealthRef;
	class ABackpack* BackpackRef;
	class AArmor* ArmorRef;
	class APistol* PistolRef;
	AGunAmmoType* AmmoTypeRef;

	FTimerHandle StaminaRechargeTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	UInfoPlayer* InfoPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	AAICharacterBase* AICharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStoreItemWidget* StoreItemWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UDialogWidget* DialogWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProductWindowWidget* ProductWindowWidget;

	//-------------------------------------------------------------------------------------------------------------

	//-----------------------------------------------------ANIMATION-----------------------------------------------
	// Montage to play when dodging
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reload | Gun", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reload | Pistol", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadPistolMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Draw", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DrawMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UnarmedEquip", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* UnarmedEquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKill", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SummonSkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* ComboMontage1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* ComboMontage2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* ComboMontage3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HoldWeapon")
	UAnimMontage* HoldWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HoldWeapon")
	UAnimMontage* FireGunMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draw | Gun")
	UAnimMontage* DrawGunMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sheath | Gun")
	UAnimMontage* SheathGunMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draw | Flashlight")
	UAnimMontage* DrawFlashlightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sheath | Flashlight")
	UAnimMontage* SheathFlashlightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draw | Pistol")
	UAnimMontage* DrawPistolMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sheath | Pistol")
	UAnimMontage* SheathPistolMontage;
	//--------------------------------------------------------------------------------------------------------------

	//--------------------------------------------------TIMER HANDLE------------------------------------------------
	// Timer handle for resetting the dodge state
	FTimerHandle DodgeResetHandle;

	FTimerHandle EnableMovementHandle;

	FTimerHandle DodgeCooldownHandle;

	FTimerHandle ReloadGunTimerHandle;

	//-------------------------------------------------------------------------------------------------------------

	//-------------------------------------INT--------------------------------------
	//-------------------------------------BOOLEAN----------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge", meta = (AllowPrivateAccess = "true"))
	bool bIsDodge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge", meta = (AllowPrivateAccess = "true"))
	bool bCanDodge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", Replicated)
	bool bIsReload;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Armor")
	bool bEquipedArmor;

	bool bIsCrouching;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Swim")
	bool bIsSwimming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run")
	bool bIsRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	bool CanStaminaRecharge;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Bullet")
	bool bHaveBullet;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Shoot")
	bool bCanShoot;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Fire")
	bool bCanFire;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Scream")
	bool bIsScream;

	bool bZoomingIn;

	// Boolean to check the camera mode
	bool bIsFirstPerson;

	//---------------------------------------------------------------------------------

	//-------------------------------------FLOAT------------------------------------
	
	UPROPERTY(EditAnywhere)
	float InteractLineTraceLength = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaSpringUsageRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaJumpUsageRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRechargeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float DelayForStaminaRecharge;

	float ZoomFactor;
	float NormalCameraDistance = 400.0f; // Default camera distance
	float CrouchCameraDistance = 500.0f; // Zoomed out camera distance when crouching
	float CurrentCameraDistance;         // Current camera distance for smooth interpolation

	// Initial and target arm lengths for the camera boom
	float CrouchArmLength;
	float StandingArmLength;

	//Interact length
	float InteractionCheckFrequency;
	float InteractionCheckDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player | Health", meta = (EditCondition = "Health >= 100"))
	float Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player | Armor", meta = (EditCondition = "Armor >= 100"))
	float Armor = 100;

	float GetArmor()
	{
		return Armor;
	}

	float GetHealth()
	{
		return Health;
	}


	//---------------------------------------------------------------------------------
	
	//---------------------------------------VECTOR------------------------------------

	//Timeline properties used for camera aiming trasition
	UPROPERTY(VisibleAnywhere, Category = "Character | Camera")
	FVector DefaultCameraLocation;

	UPROPERTY(VisibleAnywhere, Category = "Character | Camera")
	FVector AimingCameraLocation;

	//---------------------------------------------------------------------------------

public:
	//------------------------------------PROPERTIES AND VARIABLES--------------------------------
	//-------------------------------------------SOUND---------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShootSound")
	USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShootSound")
	USoundBase* ShootSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HealthSound")
	USoundBase* HealthSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorSound")
	USoundBase* DoorSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DieSound")
	USoundBase* DieSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OutofAmmo")
	USoundBase* OutofAmmoSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scream")
	USoundBase* ScreamSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scream")
	USoundBase* ThunderSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DesertEagle")
	USoundBase* DesertEagleSound;
	//----------------------------------------------------------------------------------------
	
	//----------------------------------------INVENTORY----------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", Replicated)
	TEnumAsByte<EItemtypes> CurrentEquippedItem = EItemtypes::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	AItem* EquippedItem;
	//---------------------------------------------------------------------------------------

	//------------------------------------------INTEGER--------------------------------------

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Ammo | AKM")
	int32 GunAmmo = 30;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ammo | AKM")
	int32 SumGunAmmo = 0;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Ammo | Pistol")
	int32 PistolAmmo = 12;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ammo | Pistol")
	int32 SumPistolAmmo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player | Level")
	int32 Level = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player | Level", meta = (UIMin = 0, UIMax = 100))
	int32 MaxLevel = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 ComboCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player | Age")
	int32 AgePlayer;

	int32 GetAge()
	{
		return AgePlayer;
	}

	//-----------------------------------------BOOLEAN---------------------------------------------
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Armor")
	bool bIsBackpack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", Replicated)
	bool bIsAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane", Replicated)
	bool bIsInPlane = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane", Replicated)
	bool bIsInZone = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	bool bIsDrink;

	bool bAiming;

	UPROPERTY(BlueprintReadWrite)
	bool bIsAttacking;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category ="Equip | Weapon")
	bool bIsEquipWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip | Gun")
	bool bIsEquipGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip | Gun")
	bool bIsEquipPistol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip | Flashlight")
	bool bIsEquipFlashlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EquipWeapon")
	bool bIsItemSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EquipWeapon")
	bool bIsPrimaryWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillCooldown")
	bool bIsSkillOnCooldown = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	bool bIsJumping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShootPistol")
	bool bCanShootPistol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StopShootPistol")
	bool bIsStopFire;
	//-------------------------------------FLOAT------------------------------------
	UFUNCTION(BlueprintCallable)
	float GetExperienceValue() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crouch)
	float CrouchSpeed;

	// Fire rate (time between shots)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireTimeRate;

	//Experience of the player
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player | Experience")
	float Experience;

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

	// Variables for recoil system
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Recoil")
	float RecoilPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Recoil")
	float RecoilYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Recoil")
	float RecoilResetSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Cooldown")
	float SkillCoolDown = 6.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Cooldown")
	float CurrentCooldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float ComboResetTime;

	//-------------------------------------------------STRING---------------------------------------------------
	//-------------------------------------------------VECTOR-----------------------------------------------

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crouch)
	FVector CrouchEyeOffset;

	//------------------------------------------------------------------------------------------------

	//---------------------------------------------OTHER---------------------------------------------------
	UPROPERTY(BlueprintReadWrite, EditAnywhere,Replicated)
	FRotator ReplicatedControlRotation;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* P_Blood;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ExperienceWidgetClass;

	UUserWidget* ExperienceWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player | Name")
	FText NamePlayer;

	FText GetName()
	{
		return NamePlayer;
	}

	//------------------------------------------------------------------------------------------------

	//----------------------------------------------FUNCTION----------------------------------------
	// Hàm để bật/tắt đèn pin
	void ToggleFlashlight();

	// Hàm để nhặt đèn pin
	void PickupFlashlight();

	void PickupPistol();

	void PickupItem();

	void PickupAmmoType();

	UFUNCTION(BlueprintCallable)
	bool PickupSword();

	void SetupStimulusSource();

	bool IsEnemy_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Plane")
	void OnSwitchPlayerViewToPlane();

	UFUNCTION(Server, Reliable)
	void SrvSwitchPlayerViewToPlane();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Plane")
	void SrvJumpFromPlane();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Character")
	void SrvAimPlayer(bool bShouldAim);

	UFUNCTION(Client, Reliable)
	void Clt_AimPlayer(bool bShouldAim);

	UFUNCTION(NetMulticast,Unreliable)
	void Mlt_ShootWeapon(UParticleSystem* ParticleToSpawn);

	UFUNCTION(Server, Unreliable,BlueprintCallable)
	void SrvShootWeapon();

	UFUNCTION(BlueprintImplementableEvent, Category = "Plane")
	void OnSwitchPlayerViewToCharacter();

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnPlayerDamage();

	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SrvHealthPotion();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Landed(const FHitResult& Hit) override;

	void ReplicateControlRotation();

	// Function to handle interaction
	void InteractDialog();

	// Reference to the current TradeCharacter the player can interact with
	ATradeCharacter* CurrentTradeCharacter;

	// Overlap functions to detect TradeCharacter
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	//------------------------------------PROPERTIES AND VARIABLES--------------------------------
	TArray<AActor*> OverlappingActors;

	ASword* ActorSword;

	// Lớp widget để hiển thị hồi chiêu
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UCooldownWidget> CooldownWidgetClass;

	// Tham chiếu đến widget hồi chiêu
	UCooldownWidget* CooldownWidget;
	
	//--------------------------------------------INT---------------------------------------------
	
	int32 AmmoAmount = 30;

	int32 AmmoAmountPistol = 12;

	int32 WeaponSlot = 0;
	
	//------------------------------------------BOOLEAN------------------------------------------------

	//--------------------------------------------------------------------------------------------
	
	//-------------------------------------------FLOAT-----------------------------------------------------
	
	float DamageValues = 20.0f;
	float Decreased = 100.0f;

	//-------------------------------------------STRING--------------------------------------------

	//-----------------------------------------TIMER HANDLE-----------------------------------------
	FTimerHandle ToggleTimerHandle;

	// Timer handle for controlling the fire rate
	FTimerHandle FireRateTimerHandle;

	FTimerHandle ExperienceTimerHandle;

	FTimerHandle MenuVisibleTimerHandle;

	FTimerHandle PickupTimerHandle;

	FTimerHandle SkillTimerHandle;

	FTimerHandle JumpTimerHandle;

	FTimerHandle ComboResetTimerHandle;

	FTimerHandle ResetFireGunTimerHandle;

	FTimerHandle ResetReloadGunTimerHandle;

	//----------------------------------------------------------------------------------------------

	ASword* EquippedSword;

	//----------------------------------------------FUNCTION--------------------------------------
	void DelayMenuVisible();

	void FlashlightActor();

	void PistolActor();

	void AmmoActor();

	void DropHeldFlashlight();

	void DropPistol();

	void ItemActor();

	void SwordActor();

	void DropSword();

	void ResetFire();

	int32 AddAmmo(int32& SumAmmo, int32 Amount);

	float PlayerExperience();

	float ResetExperience();

	void AddExperience(float Value);

	void OnKill(AThirdPersonCharacter* KilledCharacter);

	void SwitchCameraPlayer();

	// Function declarations
	void ShowExperience(float Value);
	void HiddenExperience();

	// Adjust damage based on hit location
	void AdjustDamageByHitLocation(FName BoneName, float& DamageValue);

	void AttackSword();

	UFUNCTION(BlueprintCallable)
	void LineTraceSword();

	void EnableMovement();

	void ResetRecoil(float DeltaTime);

	void SetPickupItemLocation();

	void HandleDeath();

	void ToggleWeaponEquip();

	void EquipWeapon();

	void SheatheWeapon();

	void SpawnSkill();

	void ResetSkillCooldown();

	void UpdateCooldownTime(float DeltaTime);

	virtual void Jump() override;
	virtual void StopJumping() override;
	void ResetJumping();

	UFUNCTION()
	void ResetCombo();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void PickupHealthPotion();
	void PickupWeapon();
	void PickupArmor();
	void PickupBackpack();
	void PickupAmmo();

	void HandleOutOfAmmo();
	void DecreaseAmmo();
	void PlayShootSound();
	void ApplyRecoil();
	void FireAmmo();
	void ProcessHit();
	void StartFireRateCooldown();

	float HandleDamageWithArmor(float DamageAmount);
	float HandleDamageWithoutArmor(float DamageAmount);
	float HandleAIDamage(float DamageAmount);
	void HandleAIDeath();
	void HandlePlayerDeath(AThirdPersonCharacter* PlayerCharacter, AThirdPersonCharacter* KilledCharacter);
	void SpawnBloodEffect(const FDamageEvent& DamageEvent);

	void ToggleGun();

	UFUNCTION(BlueprintCallable)
	void DrawGun();

	UFUNCTION(BlueprintCallable)
	void SheathGun();

	void ToggleFlashlights();
	void DrawFlashlight();
	void SheathFlashlight();

	void TogglePistol();
	void DrawPistol();
	void SheathPistol();

	void PlayerEquipWeapon();

	UFUNCTION(BlueprintCallable, Category = "FirePistol")
	void FirePistol();

	void ProcessHitAmmo();
	void MuzzleParticalPistol();
	void PlayerPistolAmmo();

	void DesertEagleAmmo();
	void HandlePistolOutofAmmo();
	void PistolStopFire();
	void PistolAmmoActor();
	void ReloadPistolCoolDown();

	void AmmoProcess(int32& AmountAmmo, int32& SumAmmo, int32 MaxAmmo);
};
