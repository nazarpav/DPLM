// Fill out your copyright notice in the Description page of Project Settings.

#pragma optimize("", off)

#include "BaseEnemy.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	UStaticMeshComponent* _defMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/base/models/creatures/zombie/source/zombie.zombie'"));
	_defMesh->SetStaticMesh(MeshAsset.Object);
	_defMesh->SetSimulatePhysics(true);
	_defMesh->WakeRigidBody();
	_defMesh->SetMobility(EComponentMobility::Movable);
	_defMesh->BodyInstance.bLockXRotation = true;
	_defMesh->BodyInstance.bLockYRotation = true;
	RootComponent = _defMesh;
	/*auto&& capsulecomp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsulecomp"));
	capsulecomp->SetMobility(EComponentMobility::Movable);
	capsulecomp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);*/
	Tags.Add("enemy");
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
}
void ABaseEnemy::StartLife(AActor* player) {
	_player = player;
	SetActorRotation(FRotator(0.f, 0.f, 0.f));
}
// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (_player) {
		float dist = FVector::Dist(_player->GetTransform().GetLocation(), GetTransform().GetLocation());
		FVector playerLoc = _player->GetTransform().GetLocation();
		if (dist < 1500.f&& dist > 0.f) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("go"));
			FVector dir = UKismetMathLibrary::GetDirectionUnitVector(playerLoc, GetTransform().GetLocation());
			auto&& HitMovement = Cast<UProjectileMovementComponent>(GetComponentByClass(UProjectileMovementComponent::StaticClass()));
			HitMovement->Velocity = dir * 400;
			dir *= -1;
			FRotator();
			FRotator rot = dir.Rotation();
			rot.Yaw -= 90.f + FMath::RandRange(5.f, 15.f);
			rot.Pitch = 0.f;
		/*	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FVector(angle).ToString());
			SetActorRotation(FRotator(0.f, angle,0.f));*/
			SetActorRotation(rot);
			//SetActorLocation(GetActorLocation() + dir * FMath::RandRange(5.f,15.f));
		}
	}

}

