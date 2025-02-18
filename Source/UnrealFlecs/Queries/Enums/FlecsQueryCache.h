// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsQueryCache.generated.h"

UENUM(BlueprintType)
enum class EFlecsQueryCacheType : uint8
{
	Default = flecs::QueryCacheDefault, /**< Behavior determined by query creation context */
	Auto = flecs::QueryCacheAuto, /**< Cache query terms that are cacheable */
	All = flecs::QueryCacheAll, /**< Require that all query terms can be cached */
	None = flecs::QueryCacheNone /**< No caching */
}; // enum class EFlecsQueryCacheType

FORCEINLINE NO_DISCARD constexpr flecs::query_cache_kind_t ToFlecsQueryCache(EFlecsQueryCacheType CacheType) NOEXCEPT
{
	return static_cast<flecs::query_cache_kind_t>(CacheType);
}