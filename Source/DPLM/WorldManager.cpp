// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldManager.h"
#include "Block.h"

AWorldManager::AWorldManager()
{
	PrimaryActorTick.bCanEverTick = false;

}
void AWorldManager::CreateChunk(uint32 x, uint32 y, float sideSize, uint32 worldHeight, float worldSize) {
	float blockSize = 100.f;
	FVector startPos(x * sideSize * blockSize, y * sideSize * blockSize, 0.f);
	ABlock* block = GetWorld()->SpawnActor<ABlock>(startPos, FRotator(0.f));
	FVector pos(0.f);
	FTransform transform;
	transform.SetLocation(pos);
	float x_w_start = static_cast<float>(x) * sideSize / worldSize;
	float y_w_start = static_cast<float>(y) * sideSize / worldSize;
	for (uint32 xx = 0; xx < sideSize; xx++){
		pos.Y = 0.f;
		for (uint32 yy = 0; yy < sideSize; yy++) {
			pos.Z = 0.f;
			float res = (FMath::PerlinNoise2D(FVector2D(x_w_start + (xx / sideSize)*(sideSize / worldSize), y_w_start + (yy / sideSize) * (sideSize / worldSize)))+1.f)/2.f;
			uint32 height = FMath::Max(static_cast<uint32>(worldHeight * res), 1u);
			//height = worldHeight;
			for (uint32 zz = 0; zz < height; zz++) {
				transform.SetTranslation(pos);
				block->AddInstance(transform);
				pos.Z += blockSize;
				pos.Z = FMath::RoundHalfToZero(pos.Z / 100.f) * 100.f;
			}
			pos.Y += blockSize;
			pos.Y = FMath::RoundHalfToZero(pos.Y / 100.f) * 100.f;
		}
		pos.X += blockSize;
		pos.X = FMath::RoundHalfToZero(pos.X / 100.f) * 100.f;
	}
}
void AWorldManager::CreateWorld(uint32 seed, uint32 worldSize, uint32 worldHeight) {
	uint32 chunkSize = 32u;
	uint32 chunkCount = worldSize;
	FMath::RandInit(seed);
	for (size_t xx = 0; xx < chunkCount; xx++) {
		for (size_t yy = 0; yy < chunkCount; yy++) {
			CreateChunk(xx,yy,chunkSize,worldHeight, worldSize);
		}
	}
}
void AWorldManager::BeginPlay(){
	Super::BeginPlay();
}

void AWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

