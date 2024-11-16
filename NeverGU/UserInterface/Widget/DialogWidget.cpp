#include "DialogWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "NeverGU/Character/TradeCharacter.h"
#include "NeverGU/Input/ThirdPersonCharacter.h"
#include "Components/HorizontalBox.h"
#include "NeverGU/UserInterface/Widget/StoreItemWidget.h"
#include "Kismet/GameplayStatics.h"

void UDialogWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Ensure CancelButton exists and has a valid event binding
    if (CancelButton)
    {
        CancelButton->SetCursor(EMouseCursor::Hand);
        CancelButton->OnClicked.AddDynamic(this, &UDialogWidget::CancelDialog);
    }

    // Ensure NextButton exists and has a valid event binding
    if (NextButton)
    {
        NextButton->SetCursor(EMouseCursor::Hand);
        NextButton->OnClicked.AddDynamic(this, &UDialogWidget::OnNextDialog);
    }

    // Ensure StoreButton exists and has a valid event binding
    if (StoreButton)
    {
        StoreButton->SetCursor(EMouseCursor::Hand);
        StoreButton->OnClicked.AddDynamic(this, &UDialogWidget::StoreDialog);
    }
}

void UDialogWidget::StoreDialog()
{
    if (!StoreClass) return; // Ensure the StoreClass is valid

    // Create the store widget if it doesn't already exist
    UStoreItemWidget* StoreWidget = CreateWidget<UStoreItemWidget>(GetOwningPlayer(), StoreClass);
    if (StoreWidget)
    {
        if (!bIsClicked) // First time clicking the Store button
        {
            // Show the store widget
            StoreWidget->AddToViewport();
            StoreWidget->SetVisibility(ESlateVisibility::Visible);

            // Remove the dialog from parent to hide it
            this->SetVisibility(ESlateVisibility::Collapsed);

            // Show the mouse cursor for interaction with the store
            UGameplayStatics::GetPlayerController(this, 0)->SetShowMouseCursor(true);

            // Update the clicked state
            bIsClicked = true;
        }
        else // If the store is already open, close it
        {
            // Remove the store widget from the parent (close the store)
            StoreWidget->SetVisibility(ESlateVisibility::Collapsed);

            // Hide the mouse cursor
            UGameplayStatics::GetPlayerController(this, 0)->SetShowMouseCursor(false);

            // Update the clicked state
            bIsClicked = false;
        }
    }
}

void UDialogWidget::UpdateDialog(const FString& DialogText)
{
    // Set dialog text to the TextBlock, if it's available
    if (DialogTextBlock)
    {
        DialogTextBlock->SetText(FText::FromString(DialogText));
    }
}

void UDialogWidget::OnNextDialog()
{
    // If owning character is set, advance the dialog
    if (OwningTradeCharacter)
    {
        OwningTradeCharacter->AdvanceDialog();
    }
}

void UDialogWidget::CancelDialog()
{
    // Check if the dialog box exists
    if (CancelButton)
    {
        if (!bIsClicked)
        {
            // Remove and hide the dialog
            this->SetVisibility(ESlateVisibility::Collapsed);
            UGameplayStatics::GetPlayerController(this, 0)->SetShowMouseCursor(false);
            bIsClicked = true;
        }
        else
        {
            bIsClicked = false;
            UE_LOG(LogTemp, Warning, TEXT("Can't Click CancelDialog!"));
        }
    }
}
