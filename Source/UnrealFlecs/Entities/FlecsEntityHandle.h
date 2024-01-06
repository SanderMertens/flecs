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
	SOLID_INLINE FFlecsEntityHandle() = default;
	SOLID_INLINE FFlecsEntityHandle(const flecs::entity& InEntity) : Entity(InEntity) {}
	SOLID_INLINE FFlecsEntityHandle(const FFlecsEntityHandle& Other) : Entity(Other.Entity) {}
	SOLID_INLINE FFlecsEntityHandle(FFlecsEntityHandle&& Other) NOEXCEPT : Entity(Other.Entity) {}
	
	SOLID_INLINE NO_DISCARD flecs::entity GetEntity() const { return Entity; }
	
	SOLID_INLINE NO_DISCARD operator flecs::entity() const { return GetEntity(); }
	SOLID_INLINE NO_DISCARD operator flecs::id_t() const { return GetEntity().id(); }
	
	SOLID_INLINE NO_DISCARD bool IsValid() const { return GetEntity().is_valid(); }
	SOLID_INLINE NO_DISCARD bool IsAlive() const { return GetEntity().is_alive(); }

	SOLID_INLINE NO_DISCARD bool HasTag(const FFlecsEntityHandle& Tag) const { return GetEntity().has(Tag); }

	SOLID_INLINE NO_DISCARD bool operator==(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() == Other.GetEntity();
	}
	
	SOLID_INLINE NO_DISCARD bool operator!=(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() != Other.GetEntity();
	}

	SOLID_INLINE NO_DISCARD bool operator==(const flecs::entity& Other) const
	{
		return GetEntity() == Other;
	}

	SOLID_INLINE NO_DISCARD bool operator!=(const flecs::entity& Other) const
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
