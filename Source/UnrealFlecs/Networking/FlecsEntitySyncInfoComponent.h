// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsEntitySyncInfoComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntitySyncInfoComponent final
{
	GENERATED_BODY()

public:
	UPROPERTY()
	uint32 bInitialized : 1 = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Entity Sync Info Component")
	uint32 bAlwaysRelevant : 1 = false;
	
}; // struct FFlecsEntitySyncInfoComponent
