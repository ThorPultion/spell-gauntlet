// Fill out your copyright notice in the Description page of Project Settings.

#include "SpellGauntlet/Player/SpellPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SpellGauntlet/Gameplay/AbilitySystem/SpellAttributeSet.h"
#include "AbilitySystemComponent.h"

void ASpellPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASpellPlayerState, TeamID);
    DOREPLIFETIME(ASpellPlayerState, KillCount);
    DOREPLIFETIME(ASpellPlayerState, DeathCount);
}

ASpellPlayerState::ASpellPlayerState()
{
    SpellAbilitySystemComponent = CreateDefaultSubobject<USpellAbilitySystemComponent>(TEXT("SpellAbilitySystemComponent"));
    SpellAbilitySystemComponent->SetIsReplicated(true);
    SpellAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    SpellAttributeSet = CreateDefaultSubobject<USpellAttributeSet>(TEXT("SpellAttributeSet"));

    // Set replication frequency
    SetNetUpdateFrequency(100);
}

UAbilitySystemComponent* ASpellPlayerState::GetAbilitySystemComponent() const
{
    return SpellAbilitySystemComponent;
}

void ASpellPlayerState::OnRep_Score()
{
    Super::OnRep_Score();

    // Tell anyone listening (like the UI) that the score changed
    OnScoreChanged.Broadcast(GetScore());
}

void ASpellPlayerState::UpdateScore(int32 NewScore)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        // Add to the built-in float Score
        SetScore(NewScore);

        // Manually call OnRep for the server/host player
        OnRep_Score();
    }
}
