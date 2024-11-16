// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Sword.generated.h"

UENUM(BlueprintType)
enum ESwordtypes
{
	Other,
	Sword,
	Axe
};

UCLASS()
class NEVERGU_API ASword : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASword();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Hàm kiểm tra đèn pin có được nhặt hay không
	bool IsPickedUp() const { return bIsPickedUp; }

	// Hàm để nhặt đèn pin
	void PickupSword();

	// Biến lưu trạng thái đèn pin đã nhặt hay chưa
	bool bIsPickedUp;

	void DropSword();

	void SpawnSkill1(FVector SkillLocation, FRotator SkillRotation);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sword")
	TEnumAsByte<ESwordtypes> SwordType = Other;

	//Animation Pick up
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PickupAnimation")
	UAnimMontage* PickupMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword")
	UStaticMeshComponent* SwordMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword")
	UBoxComponent* SwordBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	UParticleSystemComponent* Skill1;
};
