// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellGauntlet/Gameplay/AbilitySystem/SpellGameplayAbility.h"
#include "GA_CastBase.generated.h"

/**
 * 
 */
UCLASS()
class SPELLGAUNTLET_API UGA_CastBase : public USpellGameplayAbility
{
	GENERATED_BODY()

public:
    UGA_CastBase();

protected:

    /** The Montage to play when this ability is activated */
    UPROPERTY(EditDefaultsOnly, Category = "Montage")
    TObjectPtr<UAnimMontage> MontageToPlay = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
    float MontagePlayRate = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Montage")
    FGameplayTag MontageEventTag;

    UPROPERTY(EditAnywhere, Category = "Event")
    FGameplayTag CastBarUpdateTag;

    /** Called when the ability is activated */
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

    /** Called when the ability is activated */
    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled
    ) override;

    /** Task delegate. Fired when the Montage_PlayAndWait task ends We use UFUNCTION so the delegate can find it. */
    UFUNCTION()
    void OnMontageEnded();

    /** Called when the ability must end */
    UFUNCTION()
    void OnAbilityCancelled();

    UFUNCTION()
    virtual void OnMontageEventReceived(FGameplayEventData Payload);

    float GetDurationUntilMontageEvent();
};
