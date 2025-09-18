/**
 * @file addons/cpp/mixins/script/impl.hpp
 * @brief Script implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs 
{

inline flecs::entity script_builder::run() const {
    ecs_entity_t e = ecs_script_init(world_, &desc_);
    return flecs::entity(world_, e);
}

namespace _ {

    inline ecs_value_t get_const_var(const flecs::world_t *world, const char *name) {
        flecs::entity_t v = ecs_lookup_path_w_sep(
            world, 0, name, "::", "::", false);
        if (!v) {
            ecs_warn("unresolved const variable '%s', returning default", name);
            return {};
        }

        ecs_value_t value = ecs_const_var_get(world, v);
        if (value.ptr == nullptr) {
            ecs_warn("entity '%s' is not a const variable, returning default",
                name);
            return {};
        }

        return value;
    }

    template <typename T>
    inline T get_const_value(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const T& default_value) 
    {
        char *requested_type = ecs_get_path(world, type);
        char *var_type = ecs_get_path(world, value.type);
        ecs_warn("cannot cast const variable '%s' from type '%s' to '%s', "
                    "returning default", name, var_type, requested_type);
        ecs_os_free(requested_type);
        ecs_os_free(var_type);
        return default_value;
    }

    template <>
    inline char get_const_value<char>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const char& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return ecs_meta_get_char(&cur);
    }

    template <>
    inline int8_t get_const_value<int8_t>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const int8_t& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return static_cast<int8_t>(ecs_meta_get_int(&cur));
    }

    template <>
    inline int16_t get_const_value<int16_t>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const int16_t& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return static_cast<int16_t>(ecs_meta_get_int(&cur));
    }

    template <>
    inline int32_t get_const_value<int32_t>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const int32_t& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return static_cast<int32_t>(ecs_meta_get_int(&cur));
    }

    template <>
    inline int64_t get_const_value<int64_t>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const int64_t& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return ecs_meta_get_int(&cur);
    }

    template <>
    inline uint8_t get_const_value<uint8_t>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const uint8_t& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return static_cast<uint8_t>(ecs_meta_get_uint(&cur));
    }

    template <>
    inline uint16_t get_const_value<uint16_t>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const uint16_t& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return static_cast<uint16_t>(ecs_meta_get_uint(&cur));
    }

    template <>
    inline uint32_t get_const_value<uint32_t>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const uint32_t& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return static_cast<uint32_t>(ecs_meta_get_uint(&cur));
    }

    template <>
    inline uint64_t get_const_value<uint64_t>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const uint64_t& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return ecs_meta_get_uint(&cur);
    }

    template <>
    inline float get_const_value<float>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const float& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return static_cast<float>(ecs_meta_get_float(&cur));
    }

    template <>
    inline double get_const_value<double>(
        flecs::world_t *world, const char *name, ecs_value_t value, ecs_entity_t type, const double& default_value) 
    {
        (void)name; (void)type; (void)default_value;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
        return ecs_meta_get_float(&cur);
    }
}

template <typename T>
inline T world::get_const_var(
    const char *name, 
    const T& default_value) const 
{
    ecs_value_t value = flecs::_::get_const_var(world_, name);
    if (!value.ptr) {
        return default_value;
    }

    flecs::id_t type = flecs::_::type<T>::id(world_);
    if (type == value.type) {
        return *(static_cast<T*>(value.ptr));
    }

    return flecs::_::get_const_value<T>(
        world_, name, value, type, default_value);
}

template <typename T>
void world::get_const_var(
    const char *name, 
    T& out, 
    const T& default_value) const 
{
    ecs_value_t value = flecs::_::get_const_var(world_, name);
    if (!value.ptr) {
        out = default_value;
        return;
    }

    flecs::id_t type = flecs::_::type<T>::id(world_);
    if (type == value.type) {
        out = *(static_cast<T*>(value.ptr));
        return;
    }

    out = flecs::_::get_const_value<T>(
        world_, name, value, type, default_value);
}


namespace script {
namespace _ {

inline void init(flecs::world& world) {
    world.component<Script>("flecs::script::Script");
}

}
}

}
