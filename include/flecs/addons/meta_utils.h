/**
 * @file meta.h
 * @brief Meta utilities addon.
 */

#ifdef FLECS_META_UTILS

#ifndef FLECS_META
#define FLECS_META
#endif

#include "meta.h"

#ifndef FLECS_META_UTILS_H
#define FLECS_META_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif


/* Public macro's */

/* Control import behavior by setting to IMPORT or EXPORT. Not setting the macro
 * is equivalent to IMPORT */
// #define ECS_META_IMPL IMPORT

/* Declare component with descriptor */
#define ECS_META_COMPONENT(world, name)\
    ECS_COMPONENT_DEFINE(world, name)

/* ECS_STRUCT(name, body) */
#define ECS_STRUCT(name, ...)\
    ECS_STRUCT_TYPE(name, __VA_ARGS__);\
    ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, name, #__VA_ARGS__)


/* Private macro's */

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
    ECS_COMPONENT_DECLARE(name)

#define ECS_STRUCT_EXPORT(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name)

#ifdef __cplusplus
}
#endif

#endif // FLECS_META_UTILS_H

#endif // FLECS_META_UTILS
