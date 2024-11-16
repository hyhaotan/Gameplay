#pragma once

#include "CoreMinimal.h"
#include "DialogData.generated.h"

USTRUCT(BlueprintType)
struct NEVERGU_API FDialogData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    FString DialogText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    FString SpeakerName;
};