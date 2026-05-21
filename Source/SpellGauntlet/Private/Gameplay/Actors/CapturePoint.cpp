// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/CapturePoint.h"
#include "Components/SphereComponent.h"
#include "CoreSystems/SpellGameMode.h"

// Sets default values
ACapturePoint::ACapturePoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CaptureTrigger"));
    
    SetRootComponent(CollisionSphere);

    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACapturePoint::OnOverlapBegin);
    CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ACapturePoint::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ACapturePoint::BeginPlay()
{
	Super::BeginPlay();

    if (!HasAuthority())
    {
        return;
    }

    if (PossibleLocationActors.IsEmpty())
    {
        return;
    }

    // Grab possible locations
    RepopulateRepositionLocations();

    // Reposition at start to randomize starting position
    Reposition();
}

void ACapturePoint::RepopulateRepositionLocations()
{
    if (!HasAuthority())
    {
        return;
    }

    if (PossibleLocationActors.IsEmpty())
    {
        return;
    }

    PossibleLocations.Empty();

    // The actors that we use for repopulation are chosen from the scene in BP

    for (int32 i = 0; i < PossibleLocationActors.Num(); i++)
    {
        PossibleLocations.Add(PossibleLocationActors[i]->GetActorLocation());
    }
}

void ACapturePoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

    APawn* OverlappedPawn = Cast<APawn>(OtherActor);
    if (!OverlappedPawn) return;

    ASpellPlayerState* PlayerState = Cast<ASpellPlayerState>(OverlappedPawn->GetPlayerState());
    if (!PlayerState)
    {
        return;
    }

    // Keep track of players inside the cap point
    OverlappingPlayers.AddUnique(PlayerState);

    // Figure out if we should start scoring timer and do so if applicable
    HandleScoringTimer(PlayerState);
}

void ACapturePoint::HandleScoringTimer(ASpellPlayerState* PlayerState)
{
    if (!HasAuthority())
    {
        return;
    }

    // We only need to know that there is 1 differing TeamID in the point
    int32 FirstTeamID = OverlappingPlayers[0]->TeamID;
    bool bIsContested = false;

    for (const TObjectPtr<ASpellPlayerState>& Player : OverlappingPlayers)
    {
        if (Player->TeamID != FirstTeamID)
        {
            bIsContested = true;
			break;
        }
    }

    // If any differing TeamIDs, stop adding points
    // If only 1 TeamID, start giving points on timer
    if (bIsContested)
    {
        GetWorldTimerManager().ClearTimer(ScoringTimerHandle);
    }
    else if (!GetWorldTimerManager().IsTimerActive(ScoringTimerHandle))
    {
        // Delegate to be able to call function with parameter
        FTimerDelegate TimerDel;
        TimerDel.BindUObject(this, &ACapturePoint::GivePoints, PlayerState);

        GetWorldTimerManager().SetTimer(
            ScoringTimerHandle,
            TimerDel,
            TimerTickIntervalInSeconds,
            true
        );
    }
}

void ACapturePoint::GivePoints(ASpellPlayerState* PlayerState)
{
    if (!HasAuthority())
    {
        return;
    }

    // Update player score according to variables
    if (ASpellGameMode* GameMode = GetWorld()->GetAuthGameMode<ASpellGameMode>())
    {
        GameMode->UpdatePlayerScore(PlayerState, ScorePerTimerTick);
        PointsGivenTicks++;
    }

    // Every x ticks given, reposition the capture point
    if (PointsGivenTicks % TicksUntilReposition == 0)
    {
        Reposition();
    }
}

void ACapturePoint::Reposition()
{
    if (!HasAuthority())
    {
        return;
    }

    // If we already used each point, repopulate
    if (PossibleLocations.Num() == 0)
    {
        RepopulateRepositionLocations();
    }

    // random location
    LocationIndex = FMath::RandRange(0, PossibleLocations.Num() - 1);
    FVector randomLocation = PossibleLocations[LocationIndex];

    // The actual reposition
    SetActorLocation(randomLocation);

    // Remove possible location to avoid repeats
    PossibleLocations.RemoveAtSwap(LocationIndex);
}

void ACapturePoint::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!HasAuthority())
    {
        return;
    }

    APawn* Pawn = Cast<APawn>(OtherActor);
    if (!Pawn) return;

    ASpellPlayerState* PlayerState = Cast<ASpellPlayerState>(Pawn->GetPlayerState());
    if (!PlayerState)
    {
        return;
    }

    // Remove because overlap is ending
    OverlappingPlayers.Remove(PlayerState);

    // Stop timer if no overlaps
    // If there are still overlaps, figure out if we should start the timer
    if (OverlappingPlayers.Num() == 0)
    {
        GetWorldTimerManager().ClearTimer(ScoringTimerHandle);
    }
    else 
    {
        HandleScoringTimer(PlayerState);
    }
}