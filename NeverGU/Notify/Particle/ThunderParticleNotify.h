#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify_PlayParticleEffect.h"
#include "ThunderParticleNotify.generated.h"

/**
 * Custom AnimNotify to play a particle effect
 */
UCLASS()
class NEVERGU_API UThunderParticleNotify : public UAnimNotify_PlayParticleEffect
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    UParticleSystem* ParticleEffect;

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
