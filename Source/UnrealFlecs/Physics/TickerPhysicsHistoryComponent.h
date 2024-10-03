// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TickerPhysicsHistoryComponent.generated.h"

USTRUCT()
struct UNREALFLECS_API FTickerPhysicsHistoryItem final
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TickId = 0;

	UPROPERTY()
	int32 PhysicsFrame = 0;
}; // struct FTickerPhysicsHistoryItem

USTRUCT(BlueprintType)
struct UNREALFLECS_API FTickerPhysicsHistoryComponent final
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FTickerPhysicsHistoryItem> HistoryItems;
}; // struct FTickerPhysicsHistoryComponent

REGISTER_FLECS_COMPONENT_PROPERTIES(FTickerPhysicsHistoryComponent,
	{ flecs::Sparse }, { } )
