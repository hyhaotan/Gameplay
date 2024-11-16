// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/AI/AICharacterBase.h"
#include "Components/WidgetComponent.h"
#include "NeverGU/Data/Const.h"
#include "Components/BoxComponent.h"
#include "HealthBarWidget.h"
#include "NeverGU/AI/NPC.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"

// Sets default values
AAICharacterBase::AAICharacterBase() :WidgetComponent{ CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Value")) }, Health{ MaxHealth }
, RightFistCollisionBox{ CreateDefaultSubobject<UBoxComponent>(TEXT("RightFirtCollisionBox")) }
{
	PrimaryActorTick.bCanEverTick = true;
	if (WidgetComponent)
	{
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComponent->SetRelativeLocation(defs::HealthBar);
		static ConstructorHelpers::FClassFinder<UUserWidget>WidgetClass{ TEXT("/Game/AI/WB_HealthBar") };
		if (WidgetClass.Succeeded())
		{
			WidgetComponent->SetWidgetClass(WidgetClass.Class);
		}
	}

	if (RightFistCollisionBox)
	{
		RightFistCollisionBox->SetBoxExtent(defs::RightFistBoxSize, false);
		FAttachmentTransformRules const Rules{
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false };
		RightFistCollisionBox->AttachToComponent(GetMesh(), Rules, defs::RightFistSocketName);
		RightFistCollisionBox->SetRelativeLocation(defs::CollisionBoxLocation);
	}
}

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
	RightFistCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AAICharacterBase::OnAttackOverlapBegin);
	RightFistCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AAICharacterBase::OnAttackOverlapEnd);
}

void AAICharacterBase::OnAttackOverlapBegin(
	UPrimitiveComponent* const OverlapComponent,
	AActor* const OtherActor,
	UPrimitiveComponent* const OtheComponent,
	int const OtherBodyIndex,
	bool const FromSweep,
	FHitResult const& SweepResult)
{
	if (OtherActor == this)
	{
		return;
	}
	// cast to npc and create new health for npc if npc affected
	if (auto const Enemy = Cast<ANPC>(OtherActor))
	{
		auto const NewHealth = Enemy->GetHealth() - Enemy->GetMaxHealth() * 0.1f;
		Enemy->SetHealth(NewHealth);
	}
	//cast to player and create new health for player if player affected
	else if (auto const Player = Cast <AThirdPersonCharacter>(OtherActor))
	{
		/*auto const NewHealth = Player->GetHealth() - Player->GetMaxHealth() * 0.07;
		Player->SetHealth(NewHealth);*/
	}
}

void AAICharacterBase::OnAttackOverlapEnd(
	UPrimitiveComponent* const OverlapComponent,
	AActor* const OtherActor,
	UPrimitiveComponent* const OtherComponent,
	int const OtherBodyIndex)
{
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (auto const Widget = Cast <UHealthBarWidget>(WidgetComponent->GetUserWidgetObject()))
	{
		Widget->SetBarValuePercent(Health / MaxHealth);
	}
}

void AAICharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAICharacterBase::SetHealth(float const NewHealth)
{
	Health = NewHealth;
	//cast to player if player health lower 0 then player will lose
	if (Cast<AThirdPersonCharacter>(this))
	{
		if (Health <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("You lose!"));
			GetWorld()->GetFirstLocalPlayerFromController()->ConsoleCommand("Quit");
		}
	}
	//cast to npc if the npc health lower 0 then player will win
	else if (Cast<ANPC>(this))
	{
		if (Health <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("You Win!"));
			GetWorld()->GetFirstLocalPlayerFromController()->ConsoleCommand("Quit");
		}
	}
}

void AAICharacterBase::AttackStart() const
{
	RightFistCollisionBox->SetCollisionProfileName("Fist");
	RightFistCollisionBox->SetNotifyRigidBodyCollision(true);
}

void AAICharacterBase::AttackEnd() const
{
	RightFistCollisionBox->SetCollisionProfileName("Fist");
	RightFistCollisionBox->SetNotifyRigidBodyCollision(false);
}