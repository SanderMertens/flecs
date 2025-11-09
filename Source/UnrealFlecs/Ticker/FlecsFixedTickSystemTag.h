// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Properties/FlecsComponentProperties.h"
#include "Pipelines/FlecsOutsideMainLoopTag.h"

#include "FlecsFixedTickSystemTag.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsFixedTickSystemTag
{
	GENERATED_BODY()
}; // struct FFlecsFixedTickSystemTag

REGISTER_FLECS_COMPONENT(FFlecsFixedTickSystemTag,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.AddPairSecond<FFlecsOutsideMainLoopTag>(flecs::With);
	});

