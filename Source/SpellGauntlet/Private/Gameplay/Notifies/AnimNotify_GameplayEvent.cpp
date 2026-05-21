// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Notifies/AnimNotify_GameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    APawn* OwnerPawn = Cast<APawn>(Owner);
    
    // Capture the data now for accuracy and send it in event
    FVector PlayerLoc = Owner->GetActorLocation();
    FRotator ControlRot = OwnerPawn ? OwnerPawn->GetControlRotation() : Owner->GetActorRotation();

    FGameplayEventData Payload;
    Payload.EventTag = EventTag;
    Payload.Instigator = Owner;

    // We can use the simple ContextHandle HitResult hack or TargetData
    // Lets use the ContextHandle for simplicity since its just two vectors
    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner);
    if (ASI && ASI->GetAbilitySystemComponent())
    {
        FGameplayEffectContextHandle Context = ASI->GetAbilitySystemComponent()->MakeEffectContext();
        
        FHitResult SyncData;
        SyncData.Location = PlayerLoc;
        SyncData.ImpactNormal = ControlRot.Vector();
        
        Context.AddHitResult(SyncData);
        Payload.ContextHandle = Context;
    }

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, Payload);
}