// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include "string"

ABlock::ABlock() {
	_mesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/base/models/blocks/ground/Grass_Block.Grass_Block'"));
	_mesh->SetMobility(EComponentMobility::Static);
	_mesh->SetAllUseCCD(true);
	_mesh->SetStaticMesh(MeshAsset.Object); 
	_mesh->SetCollisionProfileName("BlockAll");
	RootComponent = _mesh;
	Tags.Add("block");
}

void ABlock::AddInstance(FTransform pos)
{
	_mesh->AddInstance(pos);
	_mesh->bMultiBodyOverlap = true;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("LOG ->: %s"), std::to_string(_mesh->GetInstanceCount()).c_str()));
}

void ABlock::DeleteInstance(FVector pos)
{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("POS ->: %s"), *_mesh->InstanceBodies[0]->GetUnrealWorldTransform().GetLocation().ToString()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("POS ->: %s"), *_mesh->InstanceBodies[1]->GetUnrealWorldTransform().GetLocation().ToString()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("POS ->: %s"), *_mesh->InstanceBodies[2]->GetUnrealWorldTransform().GetLocation().ToString()));
	FVector cubeRadius = FVector(100.f);
	auto&& res = _mesh->GetInstancesOverlappingBox(FBox(pos - cubeRadius,pos + cubeRadius),false);
	for (size_t i = 0; i < res.Num(); i++) {
		_mesh->RemoveInstance(res[i]);
	}
	//for (size_t i = 0; i < _mesh->GetInstanceCount(); i++)
	//{
	//	if (_mesh->InstanceBodies.Num()>i&&_mesh->InstanceBodies[i]->GetUnrealWorldTransform().GetLocation().Equals(pos,1.f)) {
	//		_mesh->RemoveInstance(i);
	//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("DELETED"));
	//	}
	//}
}
