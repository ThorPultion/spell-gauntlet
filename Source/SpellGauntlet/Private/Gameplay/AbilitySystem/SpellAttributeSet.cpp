// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/AbilitySystem/SpellAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CoreSystems/SpellGameplayTags.h"
#include "Player/SpellCharacterBase.h"

USpellAttributeSet::USpellAttributeSet()
{
    // Setting attribute values to default max
    Mana.SetBaseValue(100.0f);
    Mana.SetCurrentValue(100.0f);
    MaxMana.SetBaseValue(100.0f);
    MaxMana.SetCurrentValue(100.0f);

    Health.SetBaseValue(100.0f);
    Health.SetCurrentValue(100.0f);
    MaxHealth.SetBaseValue(100.0f);
    MaxHealth.SetCurrentValue(100.0f);
}

void USpellAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // Setting up repnotifys for attributes
    DOREPLIFETIME_CONDITION_NOTIFY(USpellAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(USpellAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(USpellAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(USpellAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void USpellAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    // Prevent going above max
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
    }
}

void USpellAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(GetHealth());
    }
    else if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        SetMana(GetMana());
    }

    const bool bIsAlreadyDead = Data.Target.HasMatchingGameplayTag(SpellGameplayTags::State_Dead);
    if (GetHealth() <= 0.0f && !bIsAlreadyDead)
    {
        KillPlayer(Data);
    }
}

void USpellAttributeSet::KillPlayer(const FGameplayEffectModCallbackData& Data)
{
    FGameplayEventData Payload;
    Payload.Instigator = Data.EffectSpec.GetContext().GetEffectCauser();
    Payload.Target = Data.Target.GetAvatarActor();

    // Send the signal to start the Death Ability
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Data.Target.GetAvatarActor(), SpellGameplayTags::Event_Death, Payload);
}
