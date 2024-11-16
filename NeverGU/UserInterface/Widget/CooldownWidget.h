// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CooldownWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEVERGU_API UCooldownWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // Gọi khi widget được khởi tạo
    virtual void NativeConstruct() override;

    // Hàm này sẽ được ràng buộc với Text Block trong Widget Blueprint
    UFUNCTION(BlueprintCallable, Category = "Cooldown")
    FText GetCooldownText() const;

protected:
    // Tên của Text Block trong widget
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* CooldownTextBlock;

    // Tham chiếu đến nhân vật hiện tại
    class AThirdPersonCharacter* PlayerCharacter;

    // Hàm này sẽ lấy tham chiếu đến nhân vật
    void InitializeCharacter();
};
