/**
 * @file addons/flecs_cpp.h
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

// The functions in this file can be used from C or C++, but these macros are only relevant to C++.
#ifdef __cplusplus

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

#endif

FLECS_API
char* ecs_cpp_get_type_name(
    char *type_name, 
    const char *func_name,
    size_t len,
    size_t front_len);

FLECS_API
char* ecs_cpp_get_symbol_name(
    char *symbol_name,
    const char *type_name,
    size_t len);

FLECS_API
char* ecs_cpp_get_constant_name(
    char *constant_name,
    const char *func_name,
    size_t len,
    size_t back_len);

FLECS_API
const char* ecs_cpp_trim_module(
    ecs_world_t *world,
    const char *type_name);

FLECS_API
ecs_entity_t ecs_cpp_component_register(
    ecs_world_t *world,
    ecs_entity_t id,
    int32_t ids_index,
    const char *name,
    const char *cpp_name,
    const char *cpp_symbol,
    size_t size,
    size_t alignment,
    bool is_component,
    bool explicit_registration,
    bool *registered_out,
    bool *existing_out);

FLECS_API
void ecs_cpp_enum_init(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_entity_t underlying_type);

FLECS_API
ecs_entity_t ecs_cpp_enum_constant_register(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t id,
    const char *name,
    void *value,
    ecs_entity_t value_type,
    size_t value_size);

typedef struct ecs_cpp_get_mut_t {
    ecs_world_t *world;
    ecs_stage_t *stage;
    void *ptr;
    bool call_modified;
} ecs_cpp_get_mut_t;

FLECS_API
FLECS_ALWAYS_INLINE ecs_cpp_get_mut_t ecs_cpp_set(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    const void *new_ptr,
    size_t size);

FLECS_API
FLECS_ALWAYS_INLINE ecs_cpp_get_mut_t ecs_cpp_assign(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    const void *new_ptr,
    size_t size);

#ifdef FLECS_META
FLECS_API
const ecs_member_t* ecs_cpp_last_member(
    const ecs_world_t *world, 
    ecs_entity_t type);
#endif

#ifdef __cplusplus
}
#endif

#endif // FLECS_CPP_H

#endif // FLECS_CPP
