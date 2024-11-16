// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/UserInterface/Widget/CooldownWidget.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"
#include "Components/TextBlock.h"

void UCooldownWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Khởi tạo tham chiếu đến nhân vật
    InitializeCharacter();
}

void UCooldownWidget::InitializeCharacter()
{
    // Lấy nhân vật hiện tại (pawn) để tham chiếu đến lớp nhân vật
    APlayerController* PlayerController = GetOwningPlayer();
    if (PlayerController)
    {
        PlayerCharacter = Cast<AThirdPersonCharacter>(PlayerController->GetPawn());
    }
}

FText UCooldownWidget::GetCooldownText() const
{
    // Nếu có tham chiếu đến nhân vật, trả về thời gian hồi chiêu dưới dạng văn bản
    if (PlayerCharacter && PlayerCharacter->bIsSkillOnCooldown)
    {
        // Chuyển thời gian còn lại sang chuỗi
        int32 TimeRemaining = FMath::CeilToInt(PlayerCharacter->CurrentCooldownTime);
        return FText::FromString(FString::Printf(TEXT("%ds"), TimeRemaining));
    }

    // Nếu không có hồi chiêu, hiển thị sẵn một chuỗi, ví dụ: "Ready!"
    return FText::FromString(TEXT("Ready!"));
}