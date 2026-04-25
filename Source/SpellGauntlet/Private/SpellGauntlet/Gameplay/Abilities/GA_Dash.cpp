#include "SpellGauntlet/Gameplay/Abilities/GA_Dash.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_Dash::UGA_Dash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Dash::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!TriggerEventData)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Get the raw data pointer
    const FGameplayAbilityTargetData* RawData = TriggerEventData->TargetData.Get(0);

    // Safe check: Is this actually a LocationInfo struct?
    if (!RawData || !RawData->GetScriptStruct()->IsChildOf(FGameplayAbilityTargetData_LocationInfo::StaticStruct()))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Cast it to the LocationInfo struct
    const FGameplayAbilityTargetData_LocationInfo* LocInfo = static_cast<const FGameplayAbilityTargetData_LocationInfo*>(RawData);

    if (!LocInfo)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Get the input vector
    FVector DashDirection = LocInfo->TargetLocation.GetTargetingTransform().GetLocation();

    CommitAbility(Handle, ActorInfo, ActivationInfo);

    // Create the Root Motion Task
    UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask =
        UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
            this,                 // OwningAbility: 'this' pointer
            FName(TEXT("DashTask")), // TaskInstanceName
            DashDirection,        // WorldDirection
            DashStrength,         // Strength
            DashDuration,         // Duration
            false,                // bIsAdditive
            nullptr,              // StrengthOverTime (No curve for constant force)
            ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
            FVector::ZeroVector,  // Set velocity to 0 at the end
            0.0f,                 // ClampVelocityOnFinish
            true                  // bEnableGravity
        );

    // Handle Task Activation
    if (RootMotionTask)
    {
        // Bind the completion delegate
        RootMotionTask->OnFinish.AddDynamic(this, &UGA_Dash::OnDashFinished);

        // Start the task. This kicks off the constant force movement
        RootMotionTask->ReadyForActivation();
    }
    else
    {
        // If the task creation failed, immediately end the ability
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

void UGA_Dash::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled
)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Dash::OnDashFinished()
{
    // This is called when the Root Motion Task completes its duration

    // End the ability (bWasCancelled = false, since it finished successfully)
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}