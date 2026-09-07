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

    template <bool Mutable, typename T>
    inline std::conditional_t<std::is_arithmetic_v<T>, T, const T&>
    get_script_var(flecs::world_t *world, const char *name, const T& default_value) {
        flecs::entity_t id = ecs_lookup_path_w_sep(
            world, 0, name, "::", "::", false);
        const char *kind = Mutable ? "mut" : "const";
        if (!id) {
            ecs_warn("unresolved %s variable '%s', returning default", kind, name);
            return default_value;
        }

        ecs_value_t value;
        if constexpr (Mutable) {
            value = ecs_mut_var_get(world, id);
        } else {
            value = ecs_const_var_get(world, id);
        }
        if (!value.ptr) {
            ecs_warn("entity '%s' is not a %s variable, returning default", name, kind);
            return default_value;
        }

        flecs::id_t type = _::type<T>::id(world);
        if (type == value.type) {
            return *static_cast<const T*>(value.ptr);
        }

        if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
            ecs_meta_cursor_t cur = ecs_meta_cursor(world, value.type, value.ptr);
            if constexpr (std::is_same_v<T, char>) {
                return ecs_meta_get_char(&cur);
            } else if constexpr (std::is_floating_point_v<T>) {
                return static_cast<T>(ecs_meta_get_float(&cur));
            } else if constexpr (std::is_signed_v<T>) {
                return static_cast<T>(ecs_meta_get_int(&cur));
            } else {
                return static_cast<T>(ecs_meta_get_uint(&cur));
            }
        } else {
            char *requested_type = ecs_get_path(world, type);
            char *var_type = ecs_get_path(world, value.type);
            ecs_warn("cannot cast %s variable '%s' from type '%s' to '%s', "
                "returning default", kind, name, var_type, requested_type);
            ecs_os_free(requested_type);
            ecs_os_free(var_type);
            return default_value;
        }
    }

    inline const char* get_const_string(
        const flecs::world_t *world, const char *name, ecs_value_t value, const char *default_value)
    {
        if (value.type == flecs::String) {
            return *(static_cast<const char**>(value.ptr));
        }

        ecs_meta_cursor_t cur = ecs_meta_cursor(
            ECS_CONST_CAST(flecs::world_t*, world), value.type, value.ptr);
        const char *result = ecs_meta_get_string(&cur);
        if (!result) {
            char *var_type = ecs_get_path(world, value.type);
            ecs_warn("cannot cast const variable '%s' from type '%s' to "
                "'flecs::string', returning default", name, var_type);
            ecs_os_free(var_type);
            return default_value;
        }

        return result;
    }
}

template <typename T>
inline T world::get_const_var(const char *name, const T& default_value) const {
    return _::get_script_var<false>(world_, name, default_value);
}

template <typename T>
void world::get_const_var(const char *name, T& out, const T& default_value) const {
    out = _::get_script_var<false>(world_, name, default_value);
}

template <typename T>
inline T world::get_mut_var(const char *name, const T& default_value) const {
    return _::get_script_var<true>(world_, name, default_value);
}

template <typename T>
void world::get_mut_var(const char *name, T& out, const T& default_value) const {
    out = _::get_script_var<true>(world_, name, default_value);
}


template <>
inline const char* world::get_const_var<const char*>(
    const char *name,
    const char* const& default_value) const
{
    ecs_value_t value = flecs::_::get_const_var(world_, name);
    if (!value.ptr) {
        return default_value;
    }

    return flecs::_::get_const_string(world_, name, value, default_value);
}

template <>
inline void world::get_const_var<const char*>(
    const char *name,
    const char*& out,
    const char* const& default_value) const
{
    ecs_value_t value = flecs::_::get_const_var(world_, name);
    if (!value.ptr) {
        out = default_value;
        return;
    }

    out = flecs::_::get_const_string(world_, name, value, default_value);
}


namespace script {
namespace _ {

inline void init(flecs::world& world) {
    world.component<Script>("flecs::script::Script");
}

}
}

}
