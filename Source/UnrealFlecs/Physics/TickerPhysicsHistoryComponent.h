// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "flecs.h"
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

REGISTER_USTRUCT_FLECS_COMPONENT(FTickerPhysicsHistoryItem, [](flecs::world World, flecs::untyped_component Component)
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

REGISTER_USTRUCT_FLECS_COMPONENT(FTickerPhysicsHistoryComponent, [](flecs::world World, flecs::untyped_component Component)
	{
		Component.add(flecs::Sparse);
	});
