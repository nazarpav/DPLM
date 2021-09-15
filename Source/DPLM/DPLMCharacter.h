// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <DPLM/BlockSelectMarker.h>
#include "Components/BoxComponent.h"
#include "Block.h"



#include "DPLMCharacter.generated.h"

//class BoxComponent;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ADPLMCharacter : public ACharacter
{
	GENERATED_BODY()
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
public:
	ADPLMCharacter();


protected:
	enum class BlockRayCastState {
		MISS,
		ENTER,
		EXIT
	};
	virtual void BeginPlay();
	void ConfigureBlockTest();
	//UBoxComponent* colisionComp;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;
protected:
	FVector lastBlockSelectedSide;
	FVector newBlockPos;
	UCameraComponent* mainCamera;
	FCollisionQueryParams collisionParam;
	FHitResult OutHit;
	ABlock* SelectedActor;
	int32 SelectedOtherBodyIndex;
	ABlockSelectMarker* blockMarker;
	/** Fires a projectile. */
	void OnFire();

	void Tick(float DeltaTime)override;

	BlockRayCastState BlockRayCast();

	void OnRestore();

	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

