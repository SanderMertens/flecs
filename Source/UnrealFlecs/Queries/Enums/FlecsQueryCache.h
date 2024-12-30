// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsQueryCache.generated.h"

UENUM(BlueprintType)
enum class EFlecsQueryCacheType : uint8
{
	Default = flecs::QueryCacheDefault,
	Auto = flecs::QueryCacheAuto,
	All = flecs::QueryCacheAll,
	None = flecs::QueryCacheNone
}; // enum class EFlecsQueryCacheType

FORCEINLINE NO_DISCARD constexpr flecs::query_cache_kind_t ToFlecsQueryCache(EFlecsQueryCacheType CacheType) NOEXCEPT
{
	return static_cast<flecs::query_cache_kind_t>(CacheType);
}