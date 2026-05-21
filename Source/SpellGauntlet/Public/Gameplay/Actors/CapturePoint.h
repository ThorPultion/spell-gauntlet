// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/SpellPlayerState.h"
#include "CapturePoint.generated.h"

class USphereComponent;

UCLASS()
class SPELLGAUNTLET_API ACapturePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACapturePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	int32 ScorePerTimerTick = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	int32 TimerTickIntervalInSeconds = 1;

	/** Defines where the capture point can reposition to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Repositioning")
	TArray<TObjectPtr<AActor>> PossibleLocationActors;

	/** Ticks until capturepoint repositions to another possible location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Repositioning")
	int32 TicksUntilReposition = 5;

	/** Records amount of ticks so far for repositioning purposes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Repositioning")
	int32 PointsGivenTicks = 0;

	/** Handled purely by Overlap events */
	UPROPERTY()
	TArray<TObjectPtr<ASpellPlayerState>> OverlappingPlayers;

	int32 LocationIndex = 0;
	TArray<FVector> PossibleLocations;

	FTimerHandle ScoringTimerHandle;
	FTimerHandle RepositionTimerHandle;

	void GivePoints(ASpellPlayerState* PlayerState);

	void RepopulateRepositionLocations();

	void HandleScoringTimer(ASpellPlayerState* PlayerState);

	void Reposition();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
