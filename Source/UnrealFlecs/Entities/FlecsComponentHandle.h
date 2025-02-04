// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsEntityHandle.h"
#include "FlecsComponentHandle.generated.h"

/**
 * @TODO: Equivalent to flecs::untyped_component and flecs::component<T>
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct alignas(8) UNREALFLECS_API FFlecsComponentHandle : public FFlecsEntityHandle
{
	GENERATED_BODY()

public:

}; // struct FFlecsComponentHandle
