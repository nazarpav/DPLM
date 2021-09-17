// Copyright Epic Games, Inc. All Rights Reserved.

#include "DPLMCharacter.h"
#include "DPLMProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "DrawDebugHelpers.h"

#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ADPLMCharacter

ADPLMCharacter::ADPLMCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(40.f, 96.0f);
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	IsInGame = false;
}
void ADPLMCharacter::ConfigureBlockTest() {
	auto&& res = GetComponentsByTag(UCameraComponent::StaticClass(), FName("mainCamera"));
	if (res.Num() > 0) {
		mainCamera = static_cast<UCameraComponent*>(res[0]);
	}
}

void ADPLMCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
}
void ADPLMCharacter::EnterPlayerGame() {
	blockMarker = GetWorld()->SpawnActor<ABlockSelectMarker>(FVector(), FRotator());
	blockMarker->SetActorHiddenInGame(true);
	collisionParam.AddIgnoredActor(GetOwner());
	collisionParam.AddIgnoredActor(blockMarker);
	ConfigureBlockTest();
	SetActorLocation(FVector(500.f, 500.f, 7000.f));
	SelectedOtherBodyIndex = -1;
	IsInGame = true;
}
void ADPLMCharacter::ExitPlayerGame() {
	IsInGame = false;
}
void ADPLMCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ADPLMCharacter::OnFire);
	PlayerInputComponent->BindAction("Restore", IE_Pressed, this, &ADPLMCharacter::OnRestore);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADPLMCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADPLMCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADPLMCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADPLMCharacter::LookUpAtRate);
}
void ADPLMCharacter::OnFire()
{
	BlockRayCast();
	UWorld* const World = GetWorld();
	if (World != nullptr) {
		if (SelectedActor && SelectedOtherBodyIndex >= 0) {
			SelectedActor->DeleteInstance(SelectedOtherBodyIndex);
			blockMarker->SetActorHiddenInGame(true);
			SelectedOtherBodyIndex = -1;
			SelectedActor = nullptr;
		}
	}
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

void ADPLMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsInGame == false)return;
	switch (BlockRayCast()) {
	case BlockRayCastState::MISS:
		/*if (blockMarker) {
			blockMarker->SetActorHiddenInGame(true);
		}
		SelectedOtherBodyIndex = -1;
		SelectedActor = nullptr;*/
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("MISS"));
		break;
	case BlockRayCastState::ENTER:
		if (SelectedActor) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("ENTER"));
			blockMarker->SetActorLocation(SelectedActor->GetInstanceLocation(SelectedOtherBodyIndex));
			if (blockMarker) {
				blockMarker->SetActorHiddenInGame(false);
			}
		}
		break;
	}
}

ADPLMCharacter::BlockRayCastState ADPLMCharacter::BlockRayCast() {
	BlockRayCastState state;
	FVector start = mainCamera->GetComponentLocation()/*+FVector(1.f,-10.f,1.f)*/;
	FVector forward = mainCamera->GetForwardVector();
	FVector end = start + (forward * 500.f);
	if (GetWorld()->LineTraceSingleByChannel(OutHit, start, end, ECC_Visibility, collisionParam)) {
		if (OutHit.Actor.IsValid() && OutHit.Actor->ActorHasTag("block")) {
			if (SelectedOtherBodyIndex != OutHit.Item) {
				SelectedActor = static_cast<ABlock*>(OutHit.Actor.Get());
				SelectedOtherBodyIndex = OutHit.Item;
				state = BlockRayCastState::ENTER;
				FVector pos = SelectedActor->GetInstanceLocation(SelectedOtherBodyIndex);
				newBlockPos = pos + (OutHit.Normal * 100.f);
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("HIT"));
	}
	else {
		state = BlockRayCastState::MISS;
	}
	//DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 0.2, 0, 1);
	return state;
}

void ADPLMCharacter::OnRestore()
{
	UWorld* const World = GetWorld();
	if (World != nullptr) {
		if (SelectedActor && SelectedOtherBodyIndex >= 0) {
			FTransform transform;
			transform.SetRotation(FRotator(0.f, 0.f, 0.f).Quaternion());
			transform.SetTranslation(newBlockPos);
			SelectedActor->AddInstance(transform);
			blockMarker->SetHidden(true);
			SelectedOtherBodyIndex = -1;
			SelectedActor = nullptr;
		}
	}
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}
void ADPLMCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADPLMCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADPLMCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
}

void ADPLMCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
}

bool ADPLMCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	
	return false;
}
