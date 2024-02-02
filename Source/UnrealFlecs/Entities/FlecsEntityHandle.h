// Solstice Games © 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsEntityHandle.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityHandle
{
	GENERATED_BODY()

public:
	FFlecsEntityHandle() = default;
	FFlecsEntityHandle(const flecs::entity& InEntity) : Entity(InEntity) {}
	
	FORCEINLINE flecs::entity GetEntity() const { return Entity; }
	
	FORCEINLINE operator flecs::entity() const { return GetEntity(); }
	FORCEINLINE operator flecs::id_t() const { return GetEntity().id(); }
	
	FORCEINLINE bool IsValid() const { return GetEntity().is_valid(); }
	FORCEINLINE bool IsAlive() const { return GetEntity().is_alive(); }

	FORCEINLINE bool Has(const FFlecsEntityHandle& InEntity) const { return GetEntity().has(InEntity); }

	FORCEINLINE void Add(const FFlecsEntityHandle& InEntity) const { GetEntity().add(InEntity); }
	FORCEINLINE void Remove(const FFlecsEntityHandle& InEntity) const { GetEntity().remove(InEntity); }

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

	FORCEINLINE flecs::entity* operator->() { return &Entity; }
	FORCEINLINE const flecs::entity* operator->() const { return &Entity; }
	
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
