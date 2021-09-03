// Copyright Epic Games, Inc. All Rights Reserved.

#include "DPLMProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include <DPLM/Block.h>

ADPLMProjectile::ADPLMProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ADPLMProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 15.0f;
}

void ADPLMProjectile::SetIsRestored(bool flag)
{
	_isRestored = flag;
}

void ADPLMProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
		if (OtherActor->ActorHasTag("block")) {
			auto pos = Hit.Location;
			if (_isRestored) {
				FRotator rotate(0.f, 0.f, 0.f);
				FTransform transform;
				pos.X = ceilf(pos.X / 100.f) * 100.f;
				pos.Y = ceilf(pos.Y / 100.f) * 100.f;
				pos.Z = floorf(pos.Z / 100.f) * 100.f;
				transform.SetRotation(rotate.Quaternion());
				transform.SetTranslation(pos);
				static_cast<ABlock*>(OtherActor)->AddInstance(transform);
			}
			else {
				static_cast<ABlock*>(OtherActor)->DeleteInstance(pos);
			}
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("hit pos ->: %s"), *HitComp->GetComponentLocation().ToString()));
		}
	}
}