#include "SpellGauntlet/Player/SpellCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SpellGauntlet/Gameplay/AbilitySystem/SpellAbilitySystemComponent.h"
#include "SpellGauntlet/Gameplay/AbilitySystem/SpellAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "SpellGauntlet/Player/SpellPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SpellGauntlet/CoreSystems/SpellGameplayTags.h"
#include "SpellGauntlet/Gameplay/AbilitySystem/SpellDefinition.h"
#include "SpellGauntlet/Player/SpellPlayerSubsystem.h"

// Sets default values
ASpellCharacterBase::ASpellCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(GetMesh());

    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
    FirstPersonMesh->SetupAttachment(SpringArmComponent);
}

void ASpellCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASpellCharacterBase, CorpseLaunchVelocity);
}

void ASpellCharacterBase::HandleDeath_Implementation()
{
    GetCharacterMovement()->SetMovementMode(MOVE_None);
    SetReplicateMovement(false);

    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);

    // Might cause issues when player is already moving?
    // LaunchVelocity is a replicated variable assigned by knockback ability.
    if (!CorpseLaunchVelocity.IsZero())
    {
        GetMesh()->AddImpulse(CorpseLaunchVelocity, NAME_None, true);
    }

    // Making third person mesh visible
    GetMesh()->bOwnerNoSee = false;
    GetMesh()->MarkRenderStateDirty();
    FirstPersonMesh->SetVisibility(false);

    FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
    SpringArmComponent->DetachFromComponent(Rules);
}

void ASpellCharacterBase::OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount > 0)
    {
        HandleDeath();
    }
}

void ASpellCharacterBase::BeginPlay()
{
    Super::BeginPlay();
}

void ASpellCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // We check if weve set owner to see the mesh (when not dead, owner only sees first person mesh, not this one)
    // Essentially just an isDead check
    // TODO!: Refactor this to a timer and get camera location from controller!
    if (!GetMesh()->bOwnerNoSee)
    {
        UCameraComponent* MyCamera = FindComponentByClass<UCameraComponent>();
        APlayerController* PC = Cast<APlayerController>(GetController());

        if (MyCamera && PC)
        {
            FVector CameraLocation = MyCamera->GetComponentLocation();
            FVector TargetLocation = GetMesh()->GetComponentLocation();

            FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLocation);

            // Dissuade camera from going crazy
            TargetRot.Pitch = FMath::Clamp(TargetRot.Pitch, -85.0f, 85.0f);
            TargetRot.Roll = 0.f;

            // Smooth rotation
            FRotator CurrentRot = PC->GetControlRotation();
            FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 2.0f);

            PC->SetControlRotation(NewRot);
        }
    }
}

void ASpellCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASpellCharacterBase::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    if (HasAuthority())
    {
        OnRep_PlayerState();
    }

    SpellASC->ClearAllAbilities();

    for (const TObjectPtr<USpellDefinition>& SpellDef : StartingAbilities)
    {
        if (SpellDef)
        {
            TSubclassOf<UGameplayAbility> AbilityClass = SpellDef->SpellAbilityClass.Get();

            // Backup getter, if the defs class hasnt been loaded in say AssetManager yet
            if (!AbilityClass)
            {
                AbilityClass = SpellDef->SpellAbilityClass.LoadSynchronous();
            }

            SavedStartingAbilities.Add(AbilityClass);
        }
    }

    SpellASC->Server_GrantAbilities(SavedStartingAbilities);
}

void ASpellCharacterBase::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    InitAbilityActorInfo();
}

void ASpellCharacterBase::InitAbilityActorInfo()
{
    ASpellPlayerState* PS = GetPlayerState<ASpellPlayerState>();
    if (PS)
    {
        // Playerstate contains our GAS component
        // Owner = PlayerState (Permanent), Avatar = This Character (Temporary)
        SpellASC = Cast<USpellAbilitySystemComponent>(PS->GetAbilitySystemComponent());

        SpellASC->InitAbilityActorInfo(PS, this);
        SpellASC->RegisterGameplayTagEvent(SpellGameplayTags::State_Dead).AddUObject(this, &ASpellCharacterBase::OnDeadTagChanged);

        // Playerstate also contains the attributeset
        SpellAttributeSet = PS->GetAttributeSet();

        // A function for letting know that our ASC is ready for use
        OnGasReady();
    }
}

USpellAbilitySystemComponent* ASpellCharacterBase::GetAbilitySystemComponent() const
{
    return SpellASC;
}

// We could send the event from elsewhere like ASC or something. Not sure what would be best for this
// Keeping here for now
void ASpellCharacterBase::Landed(const FHitResult& Hit) {
    Super::Landed(Hit);

    if (!HasAuthority())
    {
        return;
    }
    FGameplayEventData EventData;
    EventData.EventTag = StompEventTag;
    EventData.ContextHandle = SpellASC->MakeEffectContext();
    EventData.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Hit);

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

void ASpellCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Delegate cleanup
    GetWorld()->GameStateSetEvent.RemoveAll(this);

    Super::EndPlay(EndPlayReason);
}