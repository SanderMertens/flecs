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

USTRUCT(BlueprintType)
struct UNREALFLECS_API FTickerPhysicsHistoryComponent final
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FTickerPhysicsHistoryItem> HistoryItems;
}; // struct FTickerPhysicsHistoryComponent

REGISTER_COMPONENT_TAG_PROPERTIES(FTickerPhysicsHistoryComponent, flecs::Sparse);
