// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoreSystems/BaseDamageType.h"
#include "Explosion.generated.h"

// Defining trace channel as blocker for explosion (regular collision, walls etc)
#define ECC_ExplosionBlocker ECC_GameTraceChannel1
UCLASS()
class SPELLGAUNTLET_API AExplosion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Explosion();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectValues")
	float Damage = -40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectValues")
	float Radius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectValues")
	float KnockHeight = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectValues")
	float KnockForce = 1300;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Knockback")
	FGameplayTag KnockbackEventTag;

	/** Ignore caster of explosion */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Settings")
	bool bIgnoreInstigator = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
