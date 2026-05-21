// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "CastbarWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;
struct FGameplayEventData;

UCLASS()
class SPELLGAUNTLET_API UCastbarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** The Tag that represents the "Casting" state */
	UPROPERTY(EditAnywhere, Category = "GAS | UI")
	FGameplayTag CastingStateTag;

	/** Event tag for when cast begins */
	UPROPERTY(EditAnywhere, Category = "GAS | UI")
	FGameplayTag CastBarUpdateTag;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* CastProgressBar;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Callback for when the Casting tag is added or removed
	void OnCastTagChanged(const FGameplayTag Tag, int32 NewCount);

	// Event for starting our cast bar with duration in payload
	void OnCastEventReceived(const FGameplayEventData* Payload);

	void StartCastBar(float CastTime);

private:
	bool bIsCasting = false;
	float CurrentCastTime = 0.0f;
	float MaxCastTime = 0.0f;

	// Keep track of the delegate handle so we can unbind it safely
	FDelegateHandle TagEventHandle;
};