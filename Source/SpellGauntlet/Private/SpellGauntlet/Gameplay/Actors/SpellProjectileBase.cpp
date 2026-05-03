// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellGauntlet/Gameplay/Actors/SpellProjectileBase.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "SpellGauntlet/Player/SpellCharacterBase.h"
#include "AbilitySystemGlobals.h"

// Sets default values
ASpellProjectileBase::ASpellProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // Mesh as root.
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
}

// Called when the game starts or when spawned
void ASpellProjectileBase::BeginPlay()
{
    Super::BeginPlay();

    if (GetNetMode() == NM_Client)
    {
        APawn* InstigatorPawn = GetInstigator();
        if (InstigatorPawn && InstigatorPawn->IsLocallyControlled())
        {
            // We hide the mesh and stop collision locally so the shooter only interacts with their smooth LocalFake
            //SetActorHiddenInGame(true);
        }
    }

    // Making our mesh components collider trigger our OnHit on collision
    MeshComp->OnComponentHit.AddDynamic(this, &ASpellProjectileBase::OnHit);
}

void ASpellProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
    {
        if (HasAuthority())
        {
            Explosion(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
            SetActorHiddenInGame(true);
            SetActorEnableCollision(false);
            
            Destroy();
        }
    }
}

void ASpellProjectileBase::Explosion(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
    if (ASI)
    {
        UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();

        FGameplayCueParameters Parameters;
        Parameters.Location = Hit.ImpactPoint;
        Parameters.Normal = Hit.ImpactNormal;
        Parameters.EffectContext = ASC->MakeEffectContext();
        Parameters.EffectContext.AddHitResult(Hit);

        // Gameplay cue for hit effect
        ASC->ExecuteGameplayCue(HitCueTag, Parameters);
    }

    // For some reason this returns a flipped sign for z axis after telekinesis. Could have something to do with projectile component?
    FVector ProjectileDir = GetVelocity().GetSafeNormal();


    /* 
    When the projectiles mesh is simulating physics, the regular offset doesnt work.
    I presume this is because shortly after hitting the ground, the velocity of the projectile changes due to bounce.
    Reverse (+ ProjectileDir) is not enough to offset it.
    We avoid Hit.ImpactNormal normally because the player can spawn the projectile inside a wall.
    However, since the mesh will only be simulating physics after it has already been spawned (at the moment only due to Telekinesis)
    We are safe to use Hit.ImpactNormal.
    */
    FVector Location;
    if (!MeshComp->IsSimulatingPhysics())
    {
        Location = Hit.ImpactPoint - (ProjectileDir * 20.0f);
    }
    else
    {
        Location = Hit.ImpactPoint + (Hit.ImpactNormal * 20.0f);
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    // For example an explosion actor
    GetWorld()->SpawnActor<AActor>(ImpactBP, Location, FRotator::ZeroRotator, SpawnParams);
}

// Called every frame
void ASpellProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpellProjectileBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

