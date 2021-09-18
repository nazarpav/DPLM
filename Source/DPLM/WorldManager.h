// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldManager.generated.h"

UCLASS()
class DPLM_API AWorldManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldManager();
	void CreateWorld(uint32 seed, uint32 worldSize, uint32 worldHeight);
protected:
	UFUNCTION()
	void CreateChunk(uint32 x, uint32 y, float sideSize, uint32 worldHeight, float worldSize);
	UFUNCTION()
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
