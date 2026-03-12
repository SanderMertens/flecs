/**
 * @file addons/flecs_cpp.h
 * @brief C++ utility functions.
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

/** Get type name from compiler-generated function name.
 *
 * @param type_name Buffer to write the type name to.
 * @param func_name The compiler-generated function name.
 * @param len The length of the type name.
 * @param front_len The number of characters to skip at the front.
 * @return The type name.
 */
FLECS_API
char* ecs_cpp_get_type_name(
    char *type_name,
    const char *func_name,
    size_t len,
    size_t front_len);

/** Get symbol name from type name.
 *
 * @param symbol_name Buffer to write the symbol name to.
 * @param type_name The type name.
 * @param len The length of the type name.
 * @return The symbol name.
 */
FLECS_API
char* ecs_cpp_get_symbol_name(
    /* If symbol_name is NULL, the function allocates a buffer with ecs_os_malloc().
     * Callers must free the returned pointer with ecs_os_free(). */
    char *symbol_name,
    const char *type_name,
    /* If len is 0, the function derives it from type_name. */
    size_t len);

/** Get constant name from compiler-generated function name.
 *
 * @param constant_name Buffer to write the constant name to.
 * @param func_name The compiler-generated function name.
 * @param len The length of the constant name.
 * @param back_len The number of characters to skip at the back.
 * @return The constant name.
 */
FLECS_API
char* ecs_cpp_get_constant_name(
    char *constant_name,
    const char *func_name,
    size_t len,
    size_t back_len);

/** Trim module prefix from type name.
 *
 * @param world The world.
 * @param type_name The type name to trim.
 * @return The trimmed type name.
 */
FLECS_API
const char* ecs_cpp_trim_module(
    ecs_world_t *world,
    const char *type_name);

typedef void (*ecs_cpp_type_action_t)(
    ecs_world_t *world,
    ecs_entity_t component);

typedef struct ecs_cpp_component_desc_t {
    ecs_entity_t id;
    int32_t ids_index;
    const char *name;
    const char *cpp_name;
    const char *cpp_symbol;
    size_t size;
    size_t alignment;
    ecs_cpp_type_action_t lifecycle_action;
    ecs_cpp_type_action_t enum_action;
    bool is_component;
    bool explicit_registration;
} ecs_cpp_component_desc_t;

/** Register a C++ component.
 *
 * @param world The world.
 * @param desc Component registration parameters.
 */
FLECS_API
ecs_entity_t ecs_cpp_component_register(
    ecs_world_t *world,
    const ecs_cpp_component_desc_t *desc);

/** Initialize a C++ enum type.
 *
 * @param world The world.
 * @param id The entity ID for the enum type.
 * @param underlying_type The underlying integer type of the enum.
 */
FLECS_API
void ecs_cpp_enum_init(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_entity_t underlying_type);

/** Register a C++ enum constant.
 *
 * @param world The world.
 * @param parent The parent enum type entity.
 * @param id The entity ID for the constant.
 * @param name The constant name.
 * @param value Pointer to the constant value.
 * @param value_type The type of the constant value.
 * @param value_size The size of the constant value.
 * @return The constant entity.
 */
FLECS_API
ecs_entity_t ecs_cpp_enum_constant_register(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t id,
    const char *name,
    void *value,
    ecs_entity_t value_type,
    size_t value_size);

/** Result type for C++ set/assign operations. */
typedef struct ecs_cpp_get_mut_t {
    ecs_world_t *world;       /**< The world. */
    ecs_stage_t *stage;       /**< The stage. */
    void *ptr;                /**< Pointer to the component data. */
    bool call_modified;       /**< Whether modified should be called. */
} ecs_cpp_get_mut_t;

/** Set a component value for a C++ entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component ID.
 * @param new_ptr Pointer to the new component value.
 * @param size The size of the component.
 * @return Result containing the component pointer and metadata.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_cpp_get_mut_t ecs_cpp_set(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    const void *new_ptr,
    size_t size);

/** Assign a component value for a C++ entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component ID.
 * @param new_ptr Pointer to the new component value.
 * @param size The size of the component.
 * @return Result containing the component pointer and metadata.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_cpp_get_mut_t ecs_cpp_assign(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t component,
    const void *new_ptr,
    size_t size);

/** Create a new entity from C++.
 *
 * @param world The world.
 * @param parent The parent entity.
 * @param name The entity name.
 * @param sep The path separator.
 * @param root_sep The root path separator.
 * @return The new entity.
 */
FLECS_API
FLECS_ALWAYS_INLINE ecs_entity_t ecs_cpp_new(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *name,
    const char *sep,
    const char *root_sep);

#ifdef FLECS_META
/** Get the last registered member for a type.
 *
 * @param world The world.
 * @param type The type entity.
 * @return Pointer to the last member, or NULL if none.
 */
FLECS_API
ecs_member_t* ecs_cpp_last_member(
    const ecs_world_t *world,
    ecs_entity_t type);
#endif

#ifdef __cplusplus
}
#endif

#endif // FLECS_CPP_H

#endif // FLECS_CPP
