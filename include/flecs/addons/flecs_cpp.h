/**
 * @file flecs_cpp.h
 * @brief C++ utility functions
 *
 * This header contains utility functions that are accessible from both C and
 * C++ code. These functions are not part of the public API and are not meant
 * to be used directly by applications.
 */

#ifdef FLECS_CPP

#ifndef FLECS_CPP_H
#define FLECS_CPP_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#define ECS_FUNC_NAME_FRONT(name) (sizeof("const char *flecs::_::"#name"() [T = ") - 3u)
#define ECS_FUNC_NAME_BACK (sizeof("]") - 1u)
#define ECS_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(__GNUC__)
#define ECS_FUNC_NAME_FRONT(name) (sizeof("const char *flecs::_::"#name"() [with T = ") - 3u)
#define ECS_FUNC_NAME_BACK (sizeof("]") - 1u)
#define ECS_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_WIN32)
#define ECS_FUNC_NAME_FRONT(name) (sizeof("const char *__cdecl flecs::_::"#name"<") - 3u)
#define ECS_FUNC_NAME_BACK (sizeof(">(void)") - 1u)
#define ECS_FUNC_NAME __FUNCSIG__
#else
#error "implicit component registration not supported"
#endif

#define ECS_FUNC_TYPE_LEN(name, str)\
    (flecs::string::length(str) - (ECS_FUNC_NAME_FRONT(name) + ECS_FUNC_NAME_BACK))

FLECS_API
char* ecs_cpp_get_type_name(
    char *type_name, 
    const char *func_name,
    size_t len);

FLECS_API
char* ecs_cpp_get_symbol_name(
    char *symbol_name,
    const char *type_name,
    size_t len);

FLECS_API
char* ecs_cpp_get_constant_name(
    char *constant_name,
    const char *func_name,
    size_t len);

FLECS_API
const char* ecs_cpp_trim_module(
    ecs_world_t *world,
    const char *type_name);

FLECS_API
void ecs_cpp_component_validate(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    size_t size,
    size_t alignment,
    bool implicit_name);

FLECS_API
ecs_entity_t ecs_cpp_component_register(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    const char *symbol,
    ecs_size_t size,
    ecs_size_t alignment);

FLECS_API
ecs_entity_t ecs_cpp_component_register_explicit(
    ecs_world_t *world,
    ecs_entity_t s_id,
    ecs_entity_t id,
    const char *name,
    const char *type_name,
    const char *symbol,
    size_t size,
    size_t alignment);

FLECS_API
ecs_entity_t ecs_cpp_enum_constant_register(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t id,
    const char *name,
    int value);

#ifdef __cplusplus
}
#endif

#endif // FLECS_CPP_H

#endif // FLECS_CPP
