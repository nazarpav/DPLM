// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockSelectMarker.h"

ABlockSelectMarker::ABlockSelectMarker() {
	UStaticMeshComponent* _defMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/base/models/blocks/marker/emptyCube.emptyCube'"));
	_defMesh->SetStaticMesh(MeshAsset.Object);
	_defMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_defMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	RootComponent = _defMesh;
}