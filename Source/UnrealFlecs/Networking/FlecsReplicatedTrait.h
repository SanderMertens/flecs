// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsReplicatedTrait.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsReplicatedTrait
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Replicated Trait")
	uint8 bPushModel : 1 = true;
	
}; // struct FFlecsReplicatedTrait
