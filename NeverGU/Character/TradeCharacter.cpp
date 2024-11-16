#include "TradeCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "NeverGU/UserInterface/Widget/DialogWidget.h"
#include "NeverGU/Data/DialogData.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "NeverGU/UserInterface/Widget/ProductWindowWidget.h"
#include "NeverGU/UserInterface/Widget/StoreItemWidget.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"

// Sets default values
ATradeCharacter::ATradeCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create the skeletal mesh and set it as the root component
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    RootComponent = SkeletalMesh;

    // Create the interaction sphere and attach it to the skeletal mesh
    SphereInteract = CreateDefaultSubobject<USphereComponent>(TEXT("SphereInteract"));
    SphereInteract->SetupAttachment(SkeletalMesh);
    SphereInteract->SetSphereRadius(60.0f);
    SphereInteract->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereInteract->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereInteract->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Create the interaction widget and attach it to the skeletal mesh
    InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
    InteractWidget->SetupAttachment(SkeletalMesh);
    InteractWidget->SetVisibility(false);  // Start as hidden

    CurrentDialogIndex = 0;
}

// Called when the game starts or when spawned
void ATradeCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Bind overlap events
    SphereInteract->OnComponentBeginOverlap.AddDynamic(this, &ATradeCharacter::OnComponentBeginOverlap);
    SphereInteract->OnComponentEndOverlap.AddDynamic(this, &ATradeCharacter::OnComponentEndOverlap);

    // Create the dialog widget instance
    if (DialogWidgetClass)
    {
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            DialogWidget = CreateWidget<UDialogWidget>(PlayerController, DialogWidgetClass);
            if (DialogWidget)
            {
                DialogWidget->AddToViewport();
                DialogWidget->SetVisibility(ESlateVisibility::Hidden); // Start hidden

                // Set OwningTradeCharacter in the widget so it can call AdvanceDialog
                this->DialogWidget;
            }
        }
    }
}

void ATradeCharacter::Tick(float DeltaTime)
{
}

void ATradeCharacter::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Check if the overlapping actor is a player character
    AThirdPersonCharacter* PlayerCharacter = Cast<AThirdPersonCharacter>(OtherActor);
    if (PlayerCharacter)
    {
        // Show the interaction widget when the player character overlaps
        InteractWidget->SetVisibility(true);
    }
}

void ATradeCharacter::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AThirdPersonCharacter* PlayerCharacter = Cast<AThirdPersonCharacter>(OtherActor);
    if (PlayerCharacter)
    {
        // Hide the interaction widget when the player character leaves
        InteractWidget->SetVisibility(false);
        UGameplayStatics::GetPlayerController(this, 0)->SetShowMouseCursor(false);

        if (DialogWidget && DialogWidget->IsVisible())
        {
            DialogWidget->SetVisibility(ESlateVisibility::Collapsed);
        }

        if (StoreItemWidget && StoreItemWidget->IsVisible())
        {
            StoreItemWidget->SetVisibility(ESlateVisibility::Collapsed);
        }

        if (ProductWindowWidget && ProductWindowWidget->IsVisible())
        {
            ProductWindowWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void ATradeCharacter::StartDialog()
{
    if (Dialogs.Num() > 0 && DialogWidget)
    {
        CurrentDialogIndex = 0;  // Reset dialog index

        // Display the first dialog line
        DialogWidget->UpdateDialog(Dialogs[CurrentDialogIndex].DialogText);

        // Show the dialog widget
        DialogWidget->SetVisibility(ESlateVisibility::Visible);
        UGameplayStatics::GetPlayerController(this, 0)->SetShowMouseCursor(true);
    }
}

void ATradeCharacter::AdvanceDialog()
{
    if (CurrentDialogIndex + 1 < Dialogs.Num())
    {
        CurrentDialogIndex++;
        // Show the next dialog
        DialogWidget->UpdateDialog(Dialogs[CurrentDialogIndex].DialogText);
    }
    else
    {
        // End the dialog when finished
        DialogWidget->SetVisibility(ESlateVisibility::Collapsed);
        UGameplayStatics::GetPlayerController(this, 0)->SetShowMouseCursor(false);
    }
}
