// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Gameplay/AbilitySystem/SpellAbilitySystemComponent.h"
#include "SpellCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPhysicsConstraintComponent;
class USpellAttributeSet;
class USpellDefinition;

UCLASS()
class SPELLGAUNTLET_API ASpellCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpellCharacterBase();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<USpellAbilitySystemComponent> SpellASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<USpellAttributeSet> SpellAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraTrackingArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CameraTrackingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPhysicsConstraintComponent> TelekinesisConstraint;

	//** Called when GAS setup is ready on character */
	UFUNCTION(BlueprintImplementableEvent)
	void OnGasReady();

protected:

	/** Stomp ability trigger */
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	FGameplayTag StompEventTag;

	void OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	/** Handling death ragdoll in c++ */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Death")
	void HandleDeath();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual void Landed(const FHitResult& Hit) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Definitions for each ability which we grab the ability class from */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
	TArray<TObjectPtr<USpellDefinition>> StartingAbilities;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual USpellAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Replicated property set by knockback ability to carry over intended momentum */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FVector CorpseLaunchVelocity = FVector::ZeroVector;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Telekinesis")
	TWeakObjectPtr<AActor> TelekinesizedActor = nullptr;

private:

	void InitAbilityActorInfo();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
