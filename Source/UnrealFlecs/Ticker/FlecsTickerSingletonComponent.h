// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsTickerSingletonComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsTickerSingletonComponent final
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsTickerSingletonComponent() = default;
	FORCEINLINE FFlecsTickerSingletonComponent(const int32 InTickId) : TickId(InTickId) {}

	UPROPERTY()
	int32 TickId = 0;
	
}; // struct FFlecsTickerSingletonComponent

REGISTER_FLECS_COMPONENT(FFlecsTickerSingletonComponent,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Sparse)
			.Add(flecs::Singleton);
	});

