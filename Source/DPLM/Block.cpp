// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include "string"

ABlock::ABlock() {
	_mesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/base/models/blocks/ground/Grass_Block.Grass_Block'"));
	_mesh->SetMobility(EComponentMobility::Static);
	_mesh->SetAllUseCCD(true);
	_mesh->bMultiBodyOverlap = true;
	_mesh->SetStaticMesh(MeshAsset.Object); 
	_mesh->SetCollisionProfileName("BlockAll");
	RootComponent = _mesh;
	Tags.Add("block");
}
void ABlock::AddInstance(FTransform pos){
	_mesh->AddInstance(pos);
}
void ABlock::DeleteInstance(FVector pos)
{
}
void ABlock::DeleteInstance(int32 index){
	_mesh->RemoveInstance(index);
}
void ABlock::SelectInstance(int32 index){
	/*FTransform transform;	_mesh->GetInstanceTransform(index, transform);
	transform.SetScale3D(FVector(0.1f, 0.1f, 0.1f));
	_mesh->MarkRenderTransformDirty();
	_mesh->MarkRenderStateDirty();
	_mesh->UpdateInstanceTransform(index, transform);
	_mesh->MarkRenderStateDirty();
	_mesh->MarkRenderTransformDirty();*/
}
void ABlock::DeselectInstance(int32 index){
	/*FTransform transform;
	_mesh->GetInstanceTransform(index, transform);
	transform.SetScale3D(FVector(0.1f, 0.1f, 0.1f));
	_mesh->MarkRenderTransformDirty();
	_mesh->MarkRenderStateDirty();
	_mesh->UpdateInstanceTransform(index, transform);
	_mesh->MarkRenderStateDirty();
	_mesh->MarkRenderTransformDirty();*/
}
FTransform ABlock::GetInstanceTransform(int32 index){
	FTransform transform;
	_mesh->GetInstanceTransform(index, transform,true);
	return transform;
}
FVector ABlock::GetInstanceLocation(int32 index){
	FTransform transform;
	_mesh->GetInstanceTransform(index, transform, true);
	return transform.GetLocation();
}
