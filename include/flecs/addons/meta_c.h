/**
 * @file meta_c.h
 * @brief Utility macro's for populating reflection data in C.
 */

#ifdef FLECS_META_C

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif

#include "meta.h"
#include "parser.h"

#ifndef FLECS_META_C_H
#define FLECS_META_C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Public API */

/* Control import behavior by setting to IMPORT or EXPORT. Not setting the macro
 * is equivalent to IMPORT */
/* #define ECS_META_IMPL IMPORT */

/** Declare component with descriptor */
#define ECS_META_COMPONENT(world, name)\
    ECS_COMPONENT_DEFINE(world, name);\
    ecs_meta_from_desc(world, ecs_id(name),\
        FLECS__##name##_kind, FLECS__##name##_desc)

/** ECS_STRUCT(name, body) */
#define ECS_STRUCT(name, ...)\
    ECS_STRUCT_TYPE(name, __VA_ARGS__);\
    ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, name, #__VA_ARGS__)

/** ECS_ENUM(name, body) */
#define ECS_ENUM(name, ...)\
    ECS_ENUM_TYPE(name, __VA_ARGS__);\
    ECS_META_IMPL_CALL(ECS_ENUM_, ECS_META_IMPL, name, #__VA_ARGS__)

/** ECS_BITMASK(name, body) */
#define ECS_BITMASK(name, ...)\
    ECS_ENUM_TYPE(name, __VA_ARGS__);\
    ECS_META_IMPL_CALL(ECS_BITMASK_, ECS_META_IMPL, name, #__VA_ARGS__)

/** Populate meta information from type descriptor. */
FLECS_API
int ecs_meta_from_desc(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_kind_t kind,
    const char *desc);


/* Private API */

/* Utilities to switch beteen IMPORT and EXPORT variants */
#define ECS_META_IMPL_CALL_INNER(base, impl, name, type_desc)\
    base ## impl(name, type_desc)

#define ECS_META_IMPL_CALL(base, impl, name, type_desc)\
    ECS_META_IMPL_CALL_INNER(base, impl, name, type_desc)

/* ECS_STRUCT implementation */
#define ECS_STRUCT_TYPE(name, ...)\
    typedef struct __VA_ARGS__ name

#define ECS_STRUCT_ECS_META_IMPL ECS_STRUCT_IMPORT

#define ECS_STRUCT_IMPORT(name, type_desc)\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsStructType;\
    ECS_COMPONENT_DECLARE(name);\

#define ECS_STRUCT_EXPORT(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name)

/* ECS_ENUM implementation */
#define ECS_ENUM_TYPE(name, ...)\
    typedef enum __VA_ARGS__ name

#define ECS_ENUM_ECS_META_IMPL ECS_ENUM_IMPORT

#define ECS_ENUM_IMPORT(name, type_desc)\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsEnumType;\
    ECS_COMPONENT_DECLARE(name);\

#define ECS_ENUM_EXPORT(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name)

/* ECS_BITMASK implementation */
#define ECS_BITMASK_TYPE(name, ...)\
    typedef enum __VA_ARGS__ name

#define ECS_BITMASK_ECS_META_IMPL ECS_BITMASK_IMPORT

#define ECS_BITMASK_IMPORT(name, type_desc)\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsBitmaskType;\
    ECS_COMPONENT_DECLARE(name);\

#define ECS_BITMASK_EXPORT(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name)

#ifdef __cplusplus
}
#endif

#endif // FLECS_META_C_H

#endif // FLECS_META_C
