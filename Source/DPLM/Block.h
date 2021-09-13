// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBlock.h"
#include "Block.generated.h"

/**
 * 
 */
UCLASS()
class DPLM_API ABlock : public ABaseBlock
{
	GENERATED_BODY()

public:
		ABlock();
		void AddInstance(FTransform pos);
		void DeleteInstance(FVector pos);
		void DeleteInstance(int32 index);
		void SelectInstance(int32 index);
		void DeselectInstance(int32 index);
		FTransform GetInstanceTransform(int32 index);
		FVector GetInstanceLocation(int32 index);
};
