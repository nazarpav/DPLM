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
	blockMarker = GetWorld()->SpawnActor<ABlockSelectMarker>(FVector(), FRotator());
	blockMarker->SetActorHiddenInGame(true);
	collisionParam.AddIgnoredActor(GetOwner());
	collisionParam.AddIgnoredActor(blockMarker);
	ConfigureBlockTest();
	FVector pos(0.f, 0.f, 0.f);
	FRotator rotate(0.f, 0.f, 0.f);
	FTransform transform;
	transform.SetRotation(rotate.Quaternion());
	FActorSpawnParameters spawnInfo;
	ABlock* block = GetWorld()->SpawnActor<ABlock>(pos, rotate, spawnInfo);
	FVector w_size(100., 100., 30.);
	FMath::RandInit(65u);
	for (size_t j = 0; j < w_size.Y; j++)
	{
		pos.X = 0.f;
		pos.Y += 100.f;
		for (size_t i = 0; i < w_size.X; i++)
		{
			pos.Z = 0.f;
			pos.X += 100.f;
			float res = (FMath::PerlinNoise2D(FVector2D(w_size.X / (i + 1.f), w_size.Y / (j + 1.f))) + 1.f) / 4.f;
			size_t height = static_cast<size_t>(w_size.Z * res);
			height = FMath::Max(static_cast<unsigned int>(height), 1u);
			for (size_t h = 0; h < height; h++)
			{
				transform.SetTranslation(pos);
				block->AddInstance(transform);
				pos.Z += 100.f;
			}
		}
	}
	pos = FVector(0.f, 0.f, 0.f);
	pos.X = (w_size.X)*100.f;

	block = GetWorld()->SpawnActor<ABlock>(pos, rotate, spawnInfo);
	FMath::RandInit(66u);
	for (size_t j = 0; j < w_size.Y; j++)
	{
		pos.X = 0.f;
		pos.Y += 100.f;
		for (size_t i = 0; i < w_size.X; i++)
		{
			pos.Z = 0.f;
			pos.X += 100.f;
			float res = (FMath::PerlinNoise2D(FVector2D(w_size.X / (i + 1.f), w_size.Y / (j + 1.f))) + 1.f) / 2.f;
			size_t height = static_cast<size_t>(w_size.Z * res);
			height = FMath::Max(static_cast<unsigned int>(height), 1u);
			for (size_t h = 0; h < height; h++)
			{
				transform.SetTranslation(pos);
				block->AddInstance(transform);
				pos.Z += 100.f;
			}
		}
	}
	auto p_spawPos = w_size;
	p_spawPos.X /= 2.f;
	p_spawPos.Y /= 2.f;
	p_spawPos.Z += 2.f;
	p_spawPos *= 100.f;
	SetActorLocation(p_spawPos);
	SelectedOtherBodyIndex = -1;
}

//////////////////////////////////////////////////////////////////////////
// Input

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
	switch (BlockRayCast()) {
	case BlockRayCastState::MISS:
		if (blockMarker) {
			blockMarker->SetActorHiddenInGame(true);
		}
		SelectedOtherBodyIndex = -1;
		SelectedActor = nullptr;
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
				lastBlockSelectedSide = pos+50.f- OutHit.Location;
				lastBlockSelectedSide.X = FMath::Clamp(lastBlockSelectedSide.X, -1.f, 1.f);
				lastBlockSelectedSide.Y = FMath::Clamp(lastBlockSelectedSide.Y, -1.f, 1.f);
				lastBlockSelectedSide.Z = FMath::Clamp(lastBlockSelectedSide.Z, -1.f, 1.f);
				newBlockPos = pos + (lastBlockSelectedSide * 100.f);
			//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, newBlockPos.ToString());
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
