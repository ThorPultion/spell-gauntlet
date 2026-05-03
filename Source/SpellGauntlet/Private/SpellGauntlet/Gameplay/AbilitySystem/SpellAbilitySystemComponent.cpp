// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellGauntlet/Gameplay/AbilitySystem/SpellAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SpellGauntlet/Gameplay/AbilitySystem/SpellDefinition.h"

USpellAbilitySystemComponent::USpellAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpellAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USpellAbilitySystemComponent, AbilitySyncCounter);
}

// Replicating to clients that abilities have changed, so update UI and so on
void USpellAbilitySystemComponent::OnRep_AbilitySyncCounter()
{
    SendAbilitiesChangedEvent();
}

TArray<FGameplayAbilitySpecHandle> USpellAbilitySystemComponent::GrantAbilities(const TArray<USpellDefinition*>& AbilitiesToGrant)
{
    if (GetOwnerRole() != ROLE_Authority)
    {
        return TArray<FGameplayAbilitySpecHandle>();
    }

    TArray<FGameplayAbilitySpecHandle> AbilityHandles;

    for (const USpellDefinition* SpellDef : AbilitiesToGrant)
    {
        if (SpellDef)
        {
            TSubclassOf<UGameplayAbility> AbilityClass = SpellDef->SpellAbilityClass.Get();

            // Backup getter, if the defs class hasnt been loaded in say AssetManager yet
            if (!AbilityClass)
            {
                AbilityClass = SpellDef->SpellAbilityClass.LoadSynchronous();
            }

            FGameplayAbilitySpecHandle SpecHandle = GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, -1, GetAvatarActor()));
            AbilityHandles.Add(SpecHandle);
        }
    }

    // Informing that abilities have changed, this makes sure that all systems are updated
    AbilitySyncCounter++;

    SendAbilitiesChangedEvent();
    return AbilityHandles;
}

void USpellAbilitySystemComponent::Server_GrantAbilities_Implementation(const TArray<USpellDefinition*>& AbilitiesToGrant)
{
    GrantAbilities(AbilitiesToGrant);
}

void USpellAbilitySystemComponent::RemoveAbilities(const TArray<FGameplayAbilitySpecHandle>& AbilityHandlesToRemove)
{
    if (GetOwnerRole() != ROLE_Authority)
    {
        return;
    }

    for (int i = 0; i < AbilityHandlesToRemove.Num(); i++)
    {
        ClearAbility(AbilityHandlesToRemove[i]);
    }

    // Informing clients that abilities have changed with an onrep
    AbilitySyncCounter++;

    SendAbilitiesChangedEvent();
}

void USpellAbilitySystemComponent::Server_RemoveAbilities_Implementation(const TArray<FGameplayAbilitySpecHandle>& AbilityHandlesToRemove)
{
    RemoveAbilities(AbilityHandlesToRemove);
}

// Things like UI listen for this event to update.
void USpellAbilitySystemComponent::SendAbilitiesChangedEvent()
{
    FGameplayEventData EventData;
    EventData.EventTag = AbilitiesChangedTag;
    EventData.Instigator = GetAvatarActor();
    EventData.Target = GetAvatarActor();

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), EventData.EventTag, EventData);
}


