// Copyright dannydev. All Rights Reserved

#include "Tile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATile::PlaceActors(TSubclassOf<AActor> objectToSpawn,
	const int minAmountToSpawn,
	const int maxAmountToSpawn,
	float spawnRadiusRange) {
	// Make sure there is an actor to spawn
	if (objectToSpawn) {
		// Get random number of objects to spawn between min and max amount
		const int32 RANDOM_AMOUNT_TO_SPAWN =
			FMath::RandRange(minAmountToSpawn, maxAmountToSpawn);

		FVector randomPointOnTile;
		// Spawn a random amount of objects
		for (size_t i = 0; i < RANDOM_AMOUNT_TO_SPAWN; i++) {
			// Place an actor only if there was an empty location
			if (FindEmptyLocation(randomPointOnTile, spawnRadiusRange))
				PlaceActor(objectToSpawn, randomPointOnTile);
		}
	}
}

bool ATile::IsPossibleToSpawnObject(const FVector& castLocation, float radius) const {
	// Store what was hit in here
	FHitResult hitResult;
	
	// Convert the location of the cast from local to world
	// because that's how it's treated in function below
	FVector globalCastLocation = ActorToWorld().TransformPosition(castLocation);

	// Cast the sphere of given radius at the given location
	bool hasHitAnything = GetWorld()->SweepSingleByChannel(hitResult,
		globalCastLocation,
		globalCastLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(radius));

	// Draw a debug sphere for visualization
	FColor sphereColor = hasHitAnything ? FColor::Red : FColor::Green;
	DrawDebugCapsule(GetWorld(), globalCastLocation, 0.0f, radius, FQuat::Identity, sphereColor, false, 60.0f);

	// TRUE if there isn't any blocking hit entry
	return !hasHitAnything;
}

void ATile::PlaceActor(TSubclassOf<AActor> toSpawn, const FVector& spawnLocation) {
	// Generate a random rotation
	FRotator randomObjectRotation = FRotator(0.0f, FMath::RandRange(0, 360), 0.0f);
	AActor* spawnedObject = GetWorld()->SpawnActor<AActor>(toSpawn,
		spawnLocation,
		randomObjectRotation);
	// Attach to its parent tile
	spawnedObject->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
}

bool ATile::FindEmptyLocation(FVector& outSpawnPoint, float spawnRadius) const {
	// Max number of possible loops
	const int MAX_AMOUNT_OF_LOOPS = 100;
	// Counter used to choose how many times this should loop
	int counter = 0;

	do {
		// Generate a random point on the tile
		outSpawnPoint = FMath::RandPointInBox(FBox(MinPoint, MaxPoint));

		// Increase counter if the spot wasn't empty
		// TRUE if the location was empty
		if (IsPossibleToSpawnObject(outSpawnPoint, spawnRadius))
			return true;
		else
			counter++;
	} while (counter < MAX_AMOUNT_OF_LOOPS);

	return false;
}