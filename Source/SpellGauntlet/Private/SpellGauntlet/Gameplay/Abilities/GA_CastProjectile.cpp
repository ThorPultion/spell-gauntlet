// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellGauntlet/Gameplay/Abilities/GA_CastProjectile.h"
#include "SpellGauntlet/Gameplay/Actors/SpellProjectileBase.h"

// Utilizing base class hooked function for ability definition
// Spawning projectile when event is received from AnimNotify_GameplayEvent from the montage
void UGA_CastProjectile::OnMontageEventReceived(FGameplayEventData Payload)
{
    Super::OnMontageEventReceived(Payload);

    if (!ProjectileBP)
    {
        UGA_CastBase::OnMontageEnded();
        return;
    }
    AActor* CasterActor = GetAvatarActorFromActorInfo();

    APawn* CasterPawn = Cast<APawn>(CasterActor);

    FVector PlayerLocation;
    FVector SpawnLocation;
    FRotator SpawnRotation = FRotator::ZeroRotator;{}

    // We have stored data about player in AnimNotify_GameplayEvent for up to date information
    // We now grab it for use
    if (Payload.ContextHandle.IsValid())
    {
        const FHitResult* Hit = Payload.ContextHandle.GetHitResult();
        if (Hit)
        {
            PlayerLocation = Hit->Location;
            SpawnRotation = Hit->ImpactNormal.Rotation();

            // Could refactor to literally leave from hand
            // Would need to turn off collision to caster at least at start if not entirely
            SpawnLocation = PlayerLocation + (FVector::UpVector * UpwardsSpawnOffset) + (SpawnRotation.Vector() * ForwardsSpawnOffset);
        }
    }

    FHitResult SafetyHit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(CasterActor);

    bool bBlocked = GetWorld()->LineTraceSingleByChannel(
        SafetyHit,
        PlayerLocation,
        SpawnLocation,
        ECC_Visibility,
        Params
    );

    // If line trace hits, move spawn location slightly in front of the wall
    if (bBlocked)
    {
        SpawnLocation = SafetyHit.Location - (SpawnRotation.Vector() * 10.0f);
    }

    // Spawning a projectile on server and client to give an instant feedback to the clients
    // Could use a different BP for the client fake
    if (HasAuthority(&CurrentActivationInfo))
    {
        AActor* ServerProjectile = GetWorld()->SpawnActorDeferred<AActor>(ProjectileBP, FTransform(SpawnRotation, SpawnLocation), CasterActor, CasterPawn);

        if (ServerProjectile)
        {
            ServerProjectile->FinishSpawning(FTransform(SpawnRotation, SpawnLocation));
        }
    }
    // Client fake
    else if (IsLocallyControlled())
    {
        // TODO: Make local projectile get destroyed when server one does (desync at times causes local to live too long)
        // Steps: Add GA prediction key as variable to server projectile and add the same key to a map of fake actors that lives in playerstate or a subsystem
        // prediction key has to be carried over to GCN_Explosion, where the key will be used to search the map of fake actors for the one to destroy
        //AActor* LocalFake = GetWorld()->SpawnActorDeferred<AActor>(ProjectileBP, FTransform(SpawnRotation, SpawnLocation), nullptr, nullptr);

        //if (LocalFake)
        //{
        //    LocalFake->SetReplicates(false);
        //    LocalFake->FinishSpawning(FTransform(SpawnRotation, SpawnLocation));
        //}
    }
}