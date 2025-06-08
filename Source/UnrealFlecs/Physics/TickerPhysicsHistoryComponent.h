// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
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

REGISTER_FLECS_COMPONENT(FTickerPhysicsHistoryItem,
	[](flecs::world World, const FFlecsComponentHandle& Component)
	{
	});

USTRUCT(BlueprintType)
struct UNREALFLECS_API FTickerPhysicsHistoryComponent final
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FTickerPhysicsHistoryItem> HistoryItems;
}; // struct FTickerPhysicsHistoryComponent

REGISTER_FLECS_COMPONENT(FTickerPhysicsHistoryComponent,
	[](flecs::world World, const FFlecsComponentHandle& Component)
	{
		Component.Add(flecs::Sparse);
	});
