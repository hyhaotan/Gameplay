#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestData.generated.h"

UENUM()
enum class EObjectiveType : uint8
{
    Location UMETA(DisplayName = "Location"),
    Kill UMETA(DisplayName = "Kill"),
    Interact UMETA(DisplayName = "Interact"),
    Collect UMETA(DisplayName = "Collect")
};

USTRUCT()
struct FObjectiveDetails
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    FText ObjectiveName;

    UPROPERTY(EditAnywhere)
    FText Description;

    UPROPERTY(EditAnywhere)
    EObjectiveType Type;

    UPROPERTY(EditAnywhere)
    FString ObjectiveID;

    UPROPERTY(EditAnywhere)
    int32 Quantity;

    UPROPERTY(EditAnywhere)
    bool IsOptional;

    UPROPERTY(EditAnywhere)
    int32 BonusXP;
};

USTRUCT()
struct FStageDetails
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    FText StageName;

    UPROPERTY(EditAnywhere)
    FText Description;

    UPROPERTY(EditAnywhere)
    TArray<FObjectiveDetails> Objectives;

    UPROPERTY(EditAnywhere)
    int32 XPReward;

    UPROPERTY(EditAnywhere)
    TMap<FName, int32> ItemReward;

    UPROPERTY(EditAnywhere)
    int32 CurrencyRewards;
};

USTRUCT()
struct FQuestDetails : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    FText QuestName;

    UPROPERTY(EditAnywhere)
    FText LogDescription;

    UPROPERTY(EditAnywhere)
    FText TrackingDescription;

    UPROPERTY(EditAnywhere)
    bool IsMainQuest;

    UPROPERTY(EditAnywhere)
    TArray<FStageDetails> Stage;
};
