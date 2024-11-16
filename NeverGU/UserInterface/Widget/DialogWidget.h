#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogWidget.generated.h"

class UTextBlock;
class UButton;
class UHorizontalBox;

UCLASS()
class NEVERGU_API UDialogWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Hàm để cập nhật nội dung hội thoại
    UFUNCTION(BlueprintCallable, Category = "Dialog")
    void UpdateDialog(const FString& DialogText);

    // Hàm để xử lý khi người chơi nhấn phím tiếp theo
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Dialog")
    void OnNextDialog();

    class FOnButtonClickedEvent* CancelClick;

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Dialog")
    void CancelDialog();

    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Store")
    void StoreDialog();

    bool bIsClicked;

    UPROPERTY(EditDefaultsOnly, Category = "Store")
    TSubclassOf<class UStoreItemWidget> StoreClass;

private:
    // Đây là nơi chứa TextBlock để hiển thị hội thoại
    UPROPERTY(meta = (BindWidget))
    UTextBlock* DialogTextBlock;

    UPROPERTY(meta = (BindWidget))
    UButton* NextButton;

    UPROPERTY(meta = (BindWidget))
    UButton* CancelButton;

    UPROPERTY(meta = (BindWidget))
    UButton* StoreButton;

    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* DialogBox;

    // Tham chiếu đến NPC để gọi AdvanceDialog
    UPROPERTY()
    class ATradeCharacter* OwningTradeCharacter;
};
