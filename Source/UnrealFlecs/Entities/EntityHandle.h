// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs/addons/cpp/flecs.hpp"
#include "SolidMacros/Macros.h"
#include "EntityHandle.generated.h"

USTRUCT(BlueprintType)
struct FFlecsEntityHandle
{
	GENERATED_BODY()
	
	OPTIONAL_FORCEINLINE NO_DISCARD friend uint32 GetTypeHash(const FFlecsEntityHandle& InHandle)
	{
		return GetTypeHash(InHandle.Entity.id());
	}
	
public:
	OPTIONAL_FORCEINLINE CONSTEXPR FFlecsEntityHandle() = default;
	OPTIONAL_FORCEINLINE CONSTEXPR explicit FFlecsEntityHandle(const flecs::entity& InEntity)
		: Entity(InEntity) {}

	OPTIONAL_FORCEINLINE NO_DISCARD flecs::entity GetEntity() const NOEXCEPT
	{
		return Entity;
	}

	OPTIONAL_FORCEINLINE NO_DISCARD bool IsValid() const NOEXCEPT
	{
		return Entity.is_valid();
	}

	OPTIONAL_FORCEINLINE NO_DISCARD bool IsAlive() const NOEXCEPT
	{
		return Entity.is_alive();
	}

	OPTIONAL_FORCEINLINE NO_DISCARD bool IsPair() const
	{
		return Entity.is_pair();
	}

	OPTIONAL_FORCEINLINE NO_DISCARD operator flecs::entity() const NOEXCEPT
	{
		return GetEntity();
	}

	OPTIONAL_FORCEINLINE NO_DISCARD operator bool() const NOEXCEPT
	{
		return IsValid();
	}
	
	OPTIONAL_FORCEINLINE NO_DISCARD bool operator==(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() == Other.GetEntity();
	}

	OPTIONAL_FORCEINLINE NO_DISCARD bool operator!=(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() != Other.GetEntity();
	}

	OPTIONAL_FORCEINLINE NO_DISCARD bool operator==(const flecs::entity& Other) const
	{
		return GetEntity() == Other;
	}

	OPTIONAL_FORCEINLINE NO_DISCARD bool operator!=(const flecs::entity& Other) const
	{
		return GetEntity() != Other;
	}

private:
	flecs::entity Entity;
}; // struct FFlecsEntityHandle