#pragma once

#include "CoreMinimal.h"
#include "SpellGauntlet/Gameplay/AbilitySystem/SpellGameplayAbility.h"
#include "GA_Dash.generated.h"

// Forward Declaration for the delegate
class UAbilityTask_ApplyRootMotionConstantForce;

UCLASS()
class SPELLGAUNTLET_API UGA_Dash : public USpellGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_Dash();

    /** The speed (force strength) of the dash */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
    float DashStrength = 1500.0f;

    /** The duration of the dash movement */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
    float DashDuration = 0.35f;

protected:
    // **Core UGameplayAbility Functions**
    // Called when the ability is activated (e.g., player presses the dash key)
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

    // Called when the ability is cancelled
    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled
    ) override;

    // Delegate function bound to the UAbilityTask_ApplyRootMotionConstantForce OnFinish event
    UFUNCTION()
    void OnDashFinished();
};