// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CastbarWidget.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UCastbarWidget::NativeConstruct()
{
	Super::NativeConstruct();


	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwningPawn))
		{
			// Listen for when the "Casting" tag is added or removed
			TagEventHandle = ASC->RegisterGameplayTagEvent(CastingStateTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UCastbarWidget::OnCastTagChanged);

			// Listen for when CastBarUpdate is called through SendGameplayEvent
			ASC->GenericGameplayEventCallbacks.FindOrAdd(CastBarUpdateTag).AddUObject(this, &UCastbarWidget::OnCastEventReceived);
		}
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UCastbarWidget::OnCastEventReceived(const FGameplayEventData* Payload)
{
	float Duration = Payload->EventMagnitude;
	StartCastBar(Duration);
}

void UCastbarWidget::NativeDestruct()
{
	// Clean up the delegates
	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwningPawn))
		{
			ASC->RegisterGameplayTagEvent(CastingStateTag, EGameplayTagEventType::NewOrRemoved).Remove(TagEventHandle);

			if (FGameplayEventMulticastDelegate* Delegate = ASC->GenericGameplayEventCallbacks.Find(CastBarUpdateTag))
			{
				Delegate->RemoveAll(this);
			}
		}
	}

	Super::NativeDestruct();
}

void UCastbarWidget::OnCastTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount == 0)
	{
		// Tag removed: (Interrupted, Finished, or Cancelled)
		bIsCasting = false;
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCastbarWidget::StartCastBar(float CastTime)
{
	MaxCastTime = CastTime;

	CurrentCastTime = 0.0f;
	bIsCasting = true;

	CastProgressBar->SetPercent(0.0f);
	SetVisibility(ESlateVisibility::Visible);
}

void UCastbarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


	// Actual progress bar calculation. Could be refactored to a timer instead for performance
	if (bIsCasting && MaxCastTime > 0.0f)
	{
		CurrentCastTime += InDeltaTime;
		float NewPercent = FMath::Clamp(CurrentCastTime / MaxCastTime, 0.0f, 1.0f);

		if (!IsValid(CastProgressBar))
		{
			return;
		}

		CastProgressBar->SetPercent(NewPercent);

		if (CastProgressBar->GetPercent() >= 1)
		{
			bIsCasting = false;
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}