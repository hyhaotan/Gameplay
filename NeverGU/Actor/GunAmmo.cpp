#include "NeverGU/Actor/GunAmmo.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

// Sets default values
AGunAmmo::AGunAmmo()
{
    // Set this actor to not call Tick() every frame, as it's unnecessary.
    PrimaryActorTick.bCanEverTick = false;

    // Create a collision component as the root
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->OnComponentHit.AddDynamic(this, &AGunAmmo::OnHit);
    CollisionComponent->InitSphereRadius(15.0f);
    RootComponent = CollisionComponent;

    // Create the Ammo mesh component and set its properties
    Ammo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunAmmo"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/Ammunition/SM_Shell_762x51.SM_Shell_762x51"));
    if (Mesh.Succeeded())
    {
        Ammo->SetStaticMesh(Mesh.Object);
    }
    Ammo->SetupAttachment(RootComponent); // Attach to the collision component
    Ammo->SetSimulatePhysics(true);
    Ammo->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Ammo->SetNotifyRigidBodyCollision(true); // Notify on collision
    Ammo->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f)); // Scale the ammo

    // Configure the projectile movement component
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->SetUpdatedComponent(CollisionComponent);
    ProjectileMovement->InitialSpeed = 3000.0f;
    ProjectileMovement->MaxSpeed = 3000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness = 0.3f;
    ProjectileMovement->ProjectileGravityScale = 1.0f;

    // Delete the projectile after 3 seconds.
    InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AGunAmmo::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame (not used here since Tick is disabled)
void AGunAmmo::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Function to launch the ammo in a specific direction
void AGunAmmo::FireInDirection(const FVector& ShootDirection)
{
    // Set the velocity in the given shoot direction
    ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
    ProjectileMovement->Activate();  // Activate movement
}

// Called when the projectile hits something.
void AGunAmmo::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor != nullptr && OtherActor != this && OtherComponent != nullptr && OtherComponent->IsSimulatingPhysics())
    {
        OtherComponent->AddImpulseAtLocation(ProjectileMovement->Velocity * 100.0f, Hit.ImpactPoint);
    }
    Destroy();
}
