/**
 * @file api_support.h
 * @brief Support functions and constants.
 *
 * Supporting types and functions that need to be exposed either in support of 
 * the public API or for unit tests, but that may change between minor / patch 
 * releases. 
 */

#ifndef FLECS_API_SUPPORT_H
#define FLECS_API_SUPPORT_H

#include "api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** This is the largest possible component id. Components for the most part
 * occupy the same id range as entities, however they are not allowed to overlap
 * with (8) bits reserved for id flags. */
static CONSTEXPR uint32_t ECS_MAX_COMPONENT_ID = ~static_cast<uint32_t>((ECS_ID_FLAGS_MASK >> 32));

/** The maximum number of nested function calls before the core will throw a
 * cycle detected error */
static CONSTEXPR uint32_t ECS_MAX_RECURSION = 512;

/** Maximum length of a parser token (used by parser-related addons) */
static CONSTEXPR uint32_t ECS_MAX_TOKEN_SIZE = 256;

////////////////////////////////////////////////////////////////////////////////
//// Global type handles
////////////////////////////////////////////////////////////////////////////////

/** This allows passing 0 as type to functions that accept ids */
#define FLECS_ID0ID_ 0

FLECS_API
char* ecs_module_path_from_c(
    const char *c_name);

bool ecs_identifier_is_0(
    const char *id);

/* Constructor that zeromem's a component value */
FLECS_API
void ecs_default_ctor(
    void *ptr, 
    int32_t count, 
    const ecs_type_info_t *ctx);

/* Create allocated string from format */
FLECS_DBG_API
char* ecs_vasprintf(
    const char *fmt,
    va_list args);

/* Create allocated string from format */
FLECS_API
char* ecs_asprintf(
    const char *fmt,
    ...);

/* Convert identifier to snake case */
FLECS_API
char* flecs_to_snake_case(
    const char *str);

FLECS_DBG_API
int32_t flecs_table_observed_count(
    const ecs_table_t *table);

FLECS_DBG_API
void flecs_dump_backtrace(
    void *stream);

/** Calculate offset from address */
#ifdef __cplusplus
#define ECS_OFFSET(o, offset) reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(o)) + (static_cast<uintptr_t>(offset)))
#else
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))
#endif
#define ECS_OFFSET_T(o, T) ECS_OFFSET(o, ECS_SIZEOF(T))

#define ECS_ELEM(ptr, size, index) ECS_OFFSET(ptr, (size) * (index))
    
#define ECS_ELEM_T(o, T, index) ECS_ELEM(o, ECS_SIZEOF(T), index)

OPTIONAL_FORCEINLINE CONSTEXPR void ECS_BIT_SET(uint32_t& flags, const uint32_t bit) NOEXCEPT
{
    flags |= bit;
}

OPTIONAL_FORCEINLINE CONSTEXPR void ECS_BIT_CLEAR(uint32_t& flags, const uint32_t bit) NOEXCEPT
{
    flags &= ~bit;
}

template <typename CondType>
OPTIONAL_FORCEINLINE CONSTEXPR void ECS_BIT_COND(uint32_t& flags, const uint32_t bit, CondType cond) NOEXCEPT
{
    return cond ? ECS_BIT_SET(flags, bit) : ECS_BIT_CLEAR(flags, bit);
}

OPTIONAL_FORCEINLINE CONSTEXPR NO_DISCARD bool ECS_BIT_IS_SET(const uint32_t flags, const uint32_t bit) NOEXCEPT
{
    return (flags & bit) != 0;
}

#ifdef __cplusplus
}
#endif

#endif
