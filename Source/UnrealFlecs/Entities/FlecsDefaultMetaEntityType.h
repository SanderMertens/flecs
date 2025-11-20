// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FlecsId.h"

#include "FlecsDefaultMetaEntityType.generated.h"

namespace Unreal::Flecs
{
	// Not using FFlecsEntityHandle as we can't guarantee the entity is in the context of a UFlecsWorld
	using EntityFunctionPtr = std::function<void(flecs::entity)>;
} // namespace Unreal::Flecs

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsDefaultMetaEntity
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Default Entities")
	FString EntityName;

	UPROPERTY(EditAnywhere, Category = "Default Entities", meta = (ClampMin = "7000"))
	uint32 EntityId = 7001;

	UPROPERTY()
	FFlecsId SetId = 0;

	NO_DISCARD FORCEINLINE bool operator==(const FFlecsDefaultMetaEntity& Other) const
	{
		return EntityName == Other.EntityName && EntityId == Other.EntityId;
	}

	NO_DISCARD FORCEINLINE bool operator!=(const FFlecsDefaultMetaEntity& Other) const
	{
		return !(*this == Other);
	}

	FFlecsDefaultMetaEntity() = default;
	
}; // struct FFlecsEntityRecord

