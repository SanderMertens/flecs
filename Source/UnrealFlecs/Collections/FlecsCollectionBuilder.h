// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsCollectionBuilder.generated.h"

struct FFlecsComponentCollection;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionBuilder
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsCollectionBuilder() = default;
	
	FORCEINLINE explicit FFlecsCollectionBuilder(FFlecsComponentCollection* InCollection)
		: Collection(InCollection)
	{
	}

	NO_DISCARD FORCEINLINE bool IsValid() const
	{
		return Collection != nullptr;
	}
	
	FFlecsComponentCollection* Collection = nullptr;
	
}; // struct FFlecsCollectionBuilder