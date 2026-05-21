// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Actor.h"
#include "CoreSystems/BaseDamageType.h"
#include "Gameplay/Actors/Explosion.h"
#include "SpellProjectileBase.generated.h"

UCLASS()
class SPELLGAUNTLET_API ASpellProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpellProjectileBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectValues")
	float Damage = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectValues")
	float Radius = 500.0f;

	UFUNCTION()
	void Explosion(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* MoveComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEffects")
	FGameplayTag HitCueTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ImpactBP")
	TSubclassOf<AActor> ImpactBP;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; 

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
