// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FlecsGameplayTagEntityComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsGameplayTagEntityComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Gameplay Tag Entity Component")
	FGameplayTag Tag;
	
}; // struct FFlecsGameplayTagEntityComponent
