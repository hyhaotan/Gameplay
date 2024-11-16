// Fill out your copyright notice in the Description page of Project Settings.


#include "NeverGU/AI/HealthBarWidget.h"

void UHealthBarWidget::NativeConstruct()
{
}

void UHealthBarWidget::SetBarValuePercent(float const value)
{
	//set health value percent
	HealthValue->SetPercent(value);
}