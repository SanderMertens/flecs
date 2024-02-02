// Solstice Games © 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "SolidMacros/Macros.h"
#include "FlecsEntityHandle.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityHandle
{
	GENERATED_BODY()

public:
	FFlecsEntityHandle() = default;
	FFlecsEntityHandle(const flecs::entity& InEntity) : Entity(InEntity) {}
	
	flecs::entity GetEntity() const { return Entity; }
	
	operator flecs::entity() const { return GetEntity(); }
	operator flecs::id_t() const { return GetEntity().id(); }
	
	bool IsValid() const { return GetEntity().is_valid(); }
	bool IsAlive() const { return GetEntity().is_alive(); }

	bool HasTag(const FFlecsEntityHandle& Tag) const { return GetEntity().has(Tag); }

	bool operator==(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() == Other.GetEntity();
	}
	
	bool operator!=(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() != Other.GetEntity();
	}

	bool operator==(const flecs::entity& Other) const
	{
		return GetEntity() == Other;
	}

	bool operator!=(const flecs::entity& Other) const
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
	};
	
}; // struct TStructOpsTypeTraits<FFlecsEntityHandle>
