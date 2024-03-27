// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsEntitySyncInfoComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntitySyncInfoComponent final
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Entity Sync Info Component")
	uint8 bSyncAllComponents : 1 = true;
	
}; // struct FFlecsEntitySyncInfoComponent
