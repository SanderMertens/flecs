// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsTickerComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsTickerComponent final
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsTickerComponent() = default;
	FORCEINLINE FFlecsTickerComponent(const int32 InTickId) : TickId(InTickId) {}

	UPROPERTY()
	int32 TickId = 0;
	
}; // struct FFlecsTickerComponent

REGISTER_FLECS_COMPONENT(FFlecsTickerComponent, [](flecs::world InWorld, flecs::untyped_component InComponent)
{
	InComponent.add(flecs::Sparse);
});

