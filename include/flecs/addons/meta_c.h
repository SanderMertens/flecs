/**
 * @file addons/meta_c.h
 * @brief Utility macros for populating reflection data in C.
 */

#ifdef FLECS_META_C

/**
 * @defgroup c_addons_meta_c Meta Utilities
 * @brief Macro utilities to automatically insert reflection data.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif

#ifndef FLECS_META_C_H
#define FLECS_META_C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Macro that controls behavior of API. Usually set in module header. When the
 * macro is not defined, it defaults to IMPL. */

/* Define variables used by reflection utilities. This should only be defined
 * by the module itself, not by the code importing the module */
/* #define ECS_META_IMPL IMPL */

/* Don't define variables used by reflection utilities but still declare the
 * variable for the component id. This enables the reflection utilities to be
 * used for global component variables, even if no reflection is used. */
/* #define ECS_META_IMPL DECLARE */

/* Don't define variables used by reflection utilities. This generates an extern
 * variable for the component identifier. */
/* #define ECS_META_IMPL EXTERN */

/** Declare component with descriptor */
#define ECS_META_COMPONENT(world, name)\
    ECS_COMPONENT_DEFINE(world, name);\
    ecs_meta_from_desc(world, ecs_id(name),\
        FLECS__##name##_kind, FLECS__##name##_desc)

/** ECS_STRUCT(name, body) */
#define ECS_STRUCT(name, ...)\
    ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, name, #__VA_ARGS__);\
    ECS_STRUCT_TYPE(name, __VA_ARGS__)

/** ECS_ENUM(name, body) */
#define ECS_ENUM(name, ...)\
    ECS_META_IMPL_CALL(ECS_ENUM_, ECS_META_IMPL, name, #__VA_ARGS__);\
    ECS_ENUM_TYPE(name, __VA_ARGS__)

/** ECS_BITMASK(name, body) */
#define ECS_BITMASK(name, ...)\
    ECS_META_IMPL_CALL(ECS_BITMASK_, ECS_META_IMPL, name, #__VA_ARGS__);\
    ECS_ENUM_TYPE(name, __VA_ARGS__)

/** Macro used to mark part of type for which no reflection data is created */
#define ECS_PRIVATE

/** Populate meta information from type descriptor. */
FLECS_API
int ecs_meta_from_desc(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_kind_t kind,
    const char *desc);


/* Private API */

/* Utilities to switch beteen IMPL, DECLARE and EXTERN variants */
#define ECS_META_IMPL_CALL_INNER(base, impl, name, type_desc)\
    base ## impl(name, type_desc)

#define ECS_META_IMPL_CALL(base, impl, name, type_desc)\
    ECS_META_IMPL_CALL_INNER(base, impl, name, type_desc)

/* ECS_STRUCT implementation */
#define ECS_STRUCT_TYPE(name, ...)\
    typedef struct __VA_ARGS__ name

#define ECS_STRUCT_ECS_META_IMPL ECS_STRUCT_IMPL

#define ECS_STRUCT_IMPL(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsStructType;\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_STRUCT_DECLARE(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_STRUCT_EXTERN(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name)


/* ECS_ENUM implementation */
#define ECS_ENUM_TYPE(name, ...)\
    typedef enum __VA_ARGS__ name

#define ECS_ENUM_ECS_META_IMPL ECS_ENUM_IMPL

#define ECS_ENUM_IMPL(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsEnumType;\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_ENUM_DECLARE(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_ENUM_EXTERN(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name)


/* ECS_BITMASK implementation */
#define ECS_BITMASK_TYPE(name, ...)\
    typedef enum __VA_ARGS__ name

#define ECS_BITMASK_ECS_META_IMPL ECS_BITMASK_IMPL

#define ECS_BITMASK_IMPL(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsBitmaskType;\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_BITMASK_DECLARE(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_BITMASK_EXTERN(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name)

#ifdef __cplusplus
}
#endif

#endif // FLECS_META_C_H

/** @} */

#endif // FLECS_META_C
