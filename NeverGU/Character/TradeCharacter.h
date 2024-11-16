#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NeverGU/Data/DialogData.h"
#include "TradeCharacter.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class UWidgetComponent;
class UDialogWidget;

UCLASS()
class NEVERGU_API ATradeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ATradeCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called when an actor begins overlapping with the sphere component
    UFUNCTION()
    void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Called when an actor ends overlapping with the sphere component
    UFUNCTION()
    void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION(BlueprintCallable, Category = "Dialog")
    void StartDialog();

    UFUNCTION(BlueprintCallable, Category = "Dialog")
    void AdvanceDialog();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    TArray<FDialogData> Dialogs;

private:
    int32 CurrentDialogIndex;

    // Class of the dialog widget to instantiate
    UPROPERTY(EditAnywhere, Category = "Dialog")
    TSubclassOf<UDialogWidget> DialogWidgetClass;

    // Instance of the dialog widget
    UPROPERTY()
    UDialogWidget* DialogWidget;

    UPROPERTY()
    class UStoreItemWidget* StoreItemWidget;

    UPROPERTY()
    class UProductWindowWidget* ProductWindowWidget;

    // The skeletal mesh of the character
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* SkeletalMesh;

    // The sphere component that triggers interactions
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USphereComponent* SphereInteract;

    // The widget component to display interaction information
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* InteractWidget;
};
