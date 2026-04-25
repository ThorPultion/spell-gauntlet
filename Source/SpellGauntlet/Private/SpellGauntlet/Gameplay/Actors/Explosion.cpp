// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellGauntlet/Gameplay/Actors/Explosion.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
AExplosion::AExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AExplosion::BeginPlay()
{
	Super::BeginPlay();

    Explosion();	
}

// Called every frame
void AExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosion::Explosion()
{
    if (!HasAuthority())
    {
        return;
    }

    TArray<AActor*> IgnoredActors = { this };

    // Instigator is the controller that caused the explosion
    if (bIgnoreInstigator && GetInstigator())
    {
        IgnoredActors.Add(GetInstigator());
    }

    // Owner in the case of for example Fireball is the projectile, not the shooter
    if (GetOwner())
    {
        IgnoredActors.Add(GetOwner());
    }

    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

    // Find everyone in the blast zone
    UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), Radius, ObjectTypes, nullptr, IgnoredActors, OverlappedActors);

    for (AActor* Victim : OverlappedActors)
    {
        FHitResult HitResult;
        FVector Start = GetActorLocation();
        FVector End = Victim->GetActorLocation();

        // Trace to find walls
        FCollisionQueryParams TraceParams;
        TraceParams.AddIgnoredActors(IgnoredActors);
        TraceParams.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_ExplosionBlocker, TraceParams);

        // If we hit something and it wasnt the victim, it means a wall is in the way
        if (bHit && HitResult.GetActor() != Victim)
        {
            continue; // Skip this victim
        }

        // Calculating knockback
        FVector KnockbackDir = Victim->GetActorLocation() - GetActorLocation();
        KnockbackDir.Z += KnockHeight;
        KnockbackDir.Normalize();

        FGameplayEventData Payload;
        Payload.Instigator = GetInstigator();
        Payload.Target = Victim;
        // Adding desired damage to payload
        Payload.EventMagnitude = Damage;

        FTransform DataTransform;
        DataTransform.SetTranslation(KnockbackDir);
        DataTransform.SetScale3D(FVector(KnockForce, 0, 0));

        // Adding the direction as a literal transform
        FGameplayAbilityTargetData_LocationInfo* LocInfo = new FGameplayAbilityTargetData_LocationInfo();
        LocInfo->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
        LocInfo->TargetLocation.LiteralTransform = DataTransform;

        // Packing the direction to the payload that gets passed to the GAS ability
        FGameplayAbilityTargetDataHandle TargetHandle;
        TargetHandle.Add(LocInfo);
        Payload.TargetData = TargetHandle;

        // Sending info to gameplayability knock
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            Victim,
            KnockbackEventTag,
            Payload
        );
    }

    Destroy();
}

