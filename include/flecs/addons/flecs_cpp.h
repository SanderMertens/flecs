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
#define ECS_FUNC_NAME_FRONT(type, name) ((sizeof(#type) + sizeof(" flecs::_::() [T = ") + sizeof(#name)) - 3u)
#define ECS_FUNC_NAME_BACK (sizeof("]") - 1u)
#define ECS_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(__GNUC__)
#define ECS_FUNC_NAME_FRONT(type, name) ((sizeof(#type) + sizeof(" flecs::_::() [with T = ") + sizeof(#name)) - 3u)
#define ECS_FUNC_NAME_BACK (sizeof("]") - 1u)
#define ECS_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_WIN32)
#define ECS_FUNC_NAME_FRONT(type, name) ((sizeof(#type) + sizeof(" __cdecl flecs::_::<") + sizeof(#name)) - 3u)
#define ECS_FUNC_NAME_BACK (sizeof(">(void)") - 1u)
#define ECS_FUNC_NAME __FUNCSIG__
#else
#error "implicit component registration not supported"
#endif

#define ECS_FUNC_TYPE_LEN(type, name, str)\
    (flecs::string::length(str) - (ECS_FUNC_NAME_FRONT(type, name) + ECS_FUNC_NAME_BACK))

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
    ecs_size_t alignment,
    bool implicit_name);

FLECS_API
ecs_entity_t ecs_cpp_component_register_explicit(
    ecs_world_t *world,
    ecs_entity_t s_id,
    ecs_entity_t id,
    const char *name,
    const char *type_name,
    const char *symbol,
    size_t size,
    size_t alignment,
    bool is_component);

FLECS_API
ecs_entity_t ecs_cpp_enum_constant_register(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t id,
    const char *name,
    int value);

FLECS_API 
int32_t ecs_cpp_reset_count_get(void);

FLECS_API
int32_t ecs_cpp_reset_count_inc(void);

#ifdef __cplusplus
}
#endif

#endif // FLECS_CPP_H

#endif // FLECS_CPP
