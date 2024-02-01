// Solstice Games © 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs/addons/cpp/flecs.hpp"
#include "SolidMacros/Macros.h"
#include "FlecsEntityHandle.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityHandle
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsEntityHandle() = default;
	FORCEINLINE FFlecsEntityHandle(const flecs::entity& InEntity) : Entity(InEntity) {}
	FORCEINLINE FFlecsEntityHandle(const FFlecsEntityHandle& Other) : Entity(Other.Entity) {}
	FORCEINLINE FFlecsEntityHandle(FFlecsEntityHandle&& Other) NOEXCEPT : Entity(Other.Entity) {}
	
	FORCEINLINE NO_DISCARD flecs::entity GetEntity() const { return Entity; }
	
	FORCEINLINE NO_DISCARD operator flecs::entity() const { return GetEntity(); }
	FORCEINLINE NO_DISCARD operator flecs::id_t() const { return GetEntity().id(); }
	
	FORCEINLINE NO_DISCARD bool IsValid() const { return GetEntity().is_valid(); }
	FORCEINLINE NO_DISCARD bool IsAlive() const { return GetEntity().is_alive(); }

	FORCEINLINE NO_DISCARD bool HasTag(const FFlecsEntityHandle& Tag) const { return GetEntity().has(Tag); }

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() == Other.GetEntity();
	}
	
	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() != Other.GetEntity();
	}

	FORCEINLINE NO_DISCARD bool operator==(const flecs::entity& Other) const
	{
		return GetEntity() == Other;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const flecs::entity& Other) const
	{
		return GetEntity() != Other;
	}
	
private:
	flecs::entity Entity;
};

template<>
struct TStructOpsTypeTraits<FFlecsEntityHandle> : public TStructOpsTypeTraitsBase2<FFlecsEntityHandle>
{
	enum
	{
		WithCopy = true,
		WithSerializer = true,
		WithStructuredSerializer = true,
		WithPostSerialize = true,
		WithIdenticalViaEquality = true
	};
};
