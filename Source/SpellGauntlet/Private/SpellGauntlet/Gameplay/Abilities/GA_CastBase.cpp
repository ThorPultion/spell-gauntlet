// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellGauntlet/Gameplay/Abilities/GA_CastBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "SpellGauntlet/Gameplay/Notifies/AnimNotify_GameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

UGA_CastBase::UGA_CastBase()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_CastBase::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    // Always call the Super function first for essential setup
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // CommitAbility checks for cost and cooldowns
    // If it fails, it will call EndAbility automatically and return false
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        OnMontageEnded();
        return; // Failed to commit
    }

    if (!IsValid(MontageToPlay))
    {
        OnMontageEnded();
        return;
    }

    // We create an Ability Task to play the montage
    UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        FName("MontageTask"),
        MontageToPlay,
        MontagePlayRate// Montage BP class
    );

    // Listen for AnimNotify_GameplayEvent from montage
    UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MontageEventTag);

    EventTask->EventReceived.AddDynamic(this, &UGA_CastBase::OnMontageEventReceived);
    EventTask->ReadyForActivation();

    // Bind our callbacks to the tasks delegates
    if (MontageTask)
    {
        // Bind a function to be called when the montage ends, is interrupted, or is cancelled
        MontageTask->OnCompleted.AddDynamic(this, &UGA_CastBase::OnMontageEnded);
        MontageTask->OnInterrupted.AddDynamic(this, &UGA_CastBase::OnAbilityCancelled);
        MontageTask->OnCancelled.AddDynamic(this, &UGA_CastBase::OnAbilityCancelled);

        // Start the montage task
        MontageTask->ReadyForActivation();


        float CalculatedCastDuration = GetDurationUntilMontageEvent();

        // Sending cast start event to UI
        FGameplayEventData Payload;
        Payload.EventTag = CastBarUpdateTag;
        Payload.EventMagnitude = CalculatedCastDuration;

        // Local event send, does not replicate!
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), CastBarUpdateTag, Payload);
    }
}


void UGA_CastBase::OnMontageEnded()
{
    EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGA_CastBase::OnMontageEventReceived(FGameplayEventData Payload)
{

}

// Gets notify event time from montage
float UGA_CastBase::GetDurationUntilMontageEvent()
{
    float Duration = 0;

    for (const FAnimNotifyEvent& NotifyEvent : MontageToPlay->Notifies)
    {
        if (UAnimNotify_GameplayEvent* GN = Cast<UAnimNotify_GameplayEvent>(NotifyEvent.Notify))
        {
            if (GN->EventTag == MontageEventTag)
            {
                Duration = NotifyEvent.GetTriggerTime();
            }
        }
    }

    // Calculate duration until the notify for the ability gets triggered, getting our cast time
    return Duration / MontagePlayRate;
}

// This function is called if the montage is interrupted or cancelled
void UGA_CastBase::OnAbilityCancelled()
{
    EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UGA_CastBase::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled
)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}