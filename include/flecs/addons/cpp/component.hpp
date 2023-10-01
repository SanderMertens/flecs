/**
 * @file addons/cpp/component.hpp
 * @brief Registering/obtaining info from components.
 */

#pragma once

#include <ctype.h>
#include <stdio.h>

/**
 * @defgroup cpp_components Components
 * @brief Registering and working with components.
 * 
 * \ingroup cpp_core
 * @{
 */

namespace flecs {

namespace _ {

// Trick to obtain typename from type, as described here
// https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
//
// The code from the link has been modified to work with more types, and across
// multiple compilers. The resulting string should be the same on all platforms
// for all compilers.
//

#if defined(__GNUC__) || defined(_WIN32)
template <typename T>
inline static const char* type_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN(const char*, type_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};
    static const size_t front_len = ECS_FUNC_NAME_FRONT(const char*, type_name);
    return ecs_cpp_get_type_name(result, ECS_FUNC_NAME, len, front_len);
} 
#else
#error "implicit component registration not supported"
#endif

// Translate a typename into a language-agnostic identifier. This allows for
// registration of components/modules across language boundaries.
template <typename T>
inline static const char* symbol_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN(const char*, symbol_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};  
    return ecs_cpp_get_symbol_name(result, type_name<T>(), len);
}

template <> inline const char* symbol_name<uint8_t>() {
    return "u8";
}
template <> inline const char* symbol_name<uint16_t>() {
    return "u16";
}
template <> inline const char* symbol_name<uint32_t>() {
    return "u32";
}
template <> inline const char* symbol_name<uint64_t>() {
    return "u64";
}
template <> inline const char* symbol_name<int8_t>() {
    return "i8";
}
template <> inline const char* symbol_name<int16_t>() {
    return "i16";
}
template <> inline const char* symbol_name<int32_t>() {
    return "i32";
}
template <> inline const char* symbol_name<int64_t>() {
    return "i64";
}
template <> inline const char* symbol_name<float>() {
    return "f32";
}
template <> inline const char* symbol_name<double>() {
    return "f64";
}

// If type is trivial, don't register lifecycle actions. While the functions
// that obtain the lifecycle callback do detect whether the callback is required
// adding a special case for trivial types eases the burden a bit on the
// compiler as it reduces the number of templates to evaluate.
template<typename T, enable_if_t<
    std::is_trivial<T>::value == true
        >* = nullptr>
void register_lifecycle_actions(ecs_world_t*, ecs_entity_t) { }

// If the component is non-trivial, register component lifecycle actions. 
// Depending on the type not all callbacks may be available.
template<typename T, enable_if_t<
    std::is_trivial<T>::value == false
        >* = nullptr>
void register_lifecycle_actions(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_type_hooks_t cl{};
    cl.ctor = ctor<T>();
    cl.dtor = dtor<T>();

    cl.copy = copy<T>();
    cl.copy_ctor = copy_ctor<T>();
    cl.move = move<T>();
    cl.move_ctor = move_ctor<T>();

    cl.ctor_move_dtor = ctor_move_dtor<T>();
    cl.move_dtor = move_dtor<T>();

    ecs_set_hooks_id( world, component, &cl);
}

// Instantiates a per-instance global component cache index
struct cpp_type_component_cache_index {
    cpp_type_component_cache_index()
        : index(ecs_cpp_component_id_storage_add()) {}

    ecs_component_cache_index_t const index;
};

// Class that manages component ids across worlds & binaries.
// The cpp_type class stores the component id for a C++ type in a static global
// variable that is shared between worlds. Whenever a component is used this
// class will check if it already has been registered (has the global id been
// set), and if not, register the component with the world.
//
// If the id has been set, the class will ensure it is known by the world. If it
// is not known the component has been registered by another world and will be
// registered with the world using the same id. If the id does exist, the class
// will register it as a component, and verify whether the input is consistent.
template <typename T>
struct cpp_type_impl {
    // Obtain a component identifier for explicit component registration.
    static entity_t id_explicit(world_t *world,
        const char *name = nullptr, bool allow_tag = true, flecs::id_t id = 0,
        bool is_component = true, bool *existing = nullptr, flecs::id_t s_id = 0)
    {
        ecs_assert(world != nullptr, ECS_INTERNAL_ERROR, name);

        if (const ecs_cached_component_info_t* info = ecs_lookup_cached_component_info(world, cached_component_index.index)) {
            return info->component;
        } else {
            // We assert below that the world is in a mutable state
            ecs_world_t* const base = const_cast<ecs_world_t*>(ecs_get_world(world));
            ecs_assert(!ecs_stage_is_readonly(base), ECS_INVALID_WHILE_READONLY, name);

            // If no id has been registered yet for the component (indicating the
            // component has not yet been registered), or if the id does not
            // exists in the world (indicating a multi-world application),
            // register it. */
            const char *symbol = nullptr;
            if (id) {
                symbol = ecs_get_symbol(base, id);
            }
            if (!symbol) {
                symbol = symbol_name<T>();
            }

            const bool is_tag = is_empty<T>::value && allow_tag;

            const size_t component_size = is_tag ? 0U : size();
            const size_t component_alignment = is_tag ? 0U : alignment();

            const entity_t entity = ecs_cpp_component_register_explicit(
                base, s_id, id, name, type_name<T>(), symbol,
                component_size, component_alignment, is_component, existing);

            // Component wasn't registered yet, set the values. Register component
            // name as the fully qualified flecs path.
            ecs_cached_component_info_t* inserted =
                ecs_get_or_create_cached_component_info(base, cached_component_index.index);

            ecs_assert(!!inserted, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(!ecs_is_cached_component_info_valid(inserted), ECS_INTERNAL_ERROR,
                NULL);

            inserted->component = entity;
            inserted->size = static_cast<ecs_size_t>(component_size);
            inserted->alignment = static_cast<ecs_size_t>(component_alignment);

            ecs_assert(ecs_is_cached_component_info_valid(inserted), ECS_INTERNAL_ERROR, NULL);

            // If component is enum type, register constants
            #if FLECS_CPP_ENUM_REFLECTION_SUPPORT            
            _::init_enum<T>(base, entity);
            #endif

            return entity;
        }
    }

    // Obtain a component identifier for implicit component registration. This
    // is almost the same as id_explicit, except that this operation 
    // automatically registers lifecycle callbacks.
    // Additionally, implicit registration temporarily resets the scope & with
    // state of the world, so that the component is not implicitly created with
    // the scope/with of the code it happens to be first used by.
    static id_t id(world_t *world, const char *name = nullptr,
        bool allow_tag = true)
    {
        ecs_assert(world != nullptr, ECS_INTERNAL_ERROR, name);

        if (const ecs_cached_component_info_t* info = ecs_lookup_cached_component_info(world, cached_component_index.index)) {
            return info->component;
        } else {
            // If no id has been registered yet, do it now.
            const ecs_entity_t prev_scope = ecs_set_scope(world, 0);
            const ecs_id_t prev_with = ecs_set_with(world, 0);

            // This will register a component id, but will not register 
            // lifecycle callbacks.
            bool existing;
            const entity_t id = id_explicit(world, name, allow_tag, 0, true, &existing);
            ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(ecs_lookup_cached_component_info(world, cached_component_index.index) != NULL,
                       ECS_INTERNAL_ERROR, NULL);

            // Register lifecycle callbacks, but only if the component has a
            // size. Components that don't have a size are tags, and tags don't
            // require construction/destruction/copy/move's. */
            if (size() && !existing) {
                register_lifecycle_actions<T>(world, id);
            }
            
            if (prev_with) {
                ecs_set_with(world, prev_with);
            }
            if (prev_scope) {
                ecs_set_scope(world, prev_scope);
            }

            return id;
        }
    }

    /// Looks the assigned component up in the provided world.
    /// It can happen that the component has not been initialized yet.
    static entity_t lookup(const world_t* world) {
        const ecs_cached_component_info_t* info = ecs_lookup_cached_component_info(world, cached_component_index.index);
        return info ? info->component : 0;
    }

    // Was the component already registered.
    static bool registered(const world_t* world) {
        return !!lookup(world);
    }

    // Return the size of this component.
    static size_t size() {
        return sizeof(T);
    }

    // Return the alignment of this component.
    static size_t alignment() {
        return alignof(T);
    }

    // Acquire a per instance incremental index for a world-local component index cache.
    static cpp_type_component_cache_index cached_component_index;
};

// Global templated variables that hold component identifier and other info
template <typename T>
cpp_type_component_cache_index cpp_type_impl<T>::cached_component_index;

// Front facing class for implicitly registering a component & obtaining 
// static component data

// Regular type
template <typename T>
struct cpp_type<T, if_not_t< is_pair<T>::value >> 
    : cpp_type_impl<base_type_t<T>> { };

// Pair type
template <typename T>
struct cpp_type<T, if_t< is_pair<T>::value >>
{
    // Override id method to return id of pair
    static id_t id(world_t *world = nullptr) {
        return ecs_pair(
            cpp_type< pair_first_t<T> >::id(world),
            cpp_type< pair_second_t<T> >::id(world));
    }
};

} // namespace _

/** Untyped component class.
 * Generic base class for flecs::component.
 * 
 * \ingroup cpp_components
 */
struct untyped_component : entity {
    using entity::entity;
    
#   ifdef FLECS_META
#   include "mixins/meta/untyped_component.inl"
#   endif
#   ifdef FLECS_METRICS
#   include "mixins/metrics/untyped_component.inl"
#   endif
};

/** Component class.
 * Class used to register components and component metadata.
 * 
 * \ingroup cpp_components
 */
template <typename T>
struct component : untyped_component {
    /** Register a component.
     * If the component was already registered, this operation will return a handle
     * to the existing component.
     * 
     * @param world The world for which to register the component.
     * @param name Optional name (overrides typename).
     * @param allow_tag If true, empty types will be registered with size 0.
     * @param id Optional id to register component with.
     */
    component(
        flecs::world_t *world, 
        const char *name = nullptr, 
        bool allow_tag = true, 
        flecs::id_t id = 0) 
    {
        const char *n = name;
        bool implicit_name = false;
        if (!n) {
            n = _::type_name<T>();

            /* Keep track of whether name was explicitly set. If not, and the 
            * component was already registered, just use the registered name.
            *
            * The registered name may differ from the typename as the registered
            * name includes the flecs scope. This can in theory be different from
            * the C++ namespace though it is good practice to keep them the same */
            implicit_name = true;
        }

        /* Obtain a registered component id. */
        if (const entity_t registered = _::cpp_type<T>::lookup(world)) {
            id = registered;

            ecs_cpp_component_validate(world, id, n, _::symbol_name<T>(),
                _::cpp_type<T>::size(),
                _::cpp_type<T>::alignment(),
                implicit_name);
        } else {
            /* If component is registered from an existing scope, ignore the
             * namespace in the name of the component. */
            if (implicit_name && (ecs_get_scope(world) != 0)) {
                /* If the type is a template type, make sure to ignore ':'
                 * inside the template parameter list. */
                const char *start = strchr(n, '<'), *last_elem = NULL;
                if (start) {
                    const char *ptr = start;
                    while (ptr[0] && (ptr[0] != ':') && (ptr > n)) {
                        ptr --;
                    }
                    if (ptr[0] == ':') {
                        last_elem = ptr;
                    }
                } else {
                    last_elem = strrchr(n, ':');
                }
                if (last_elem) {
                    name = last_elem + 1;
                }
            }

            /* Find or register component */
            bool existing;
            id = ecs_cpp_component_register(world, id, n, _::symbol_name<T>(),
                ECS_SIZEOF(T), ECS_ALIGNOF(T), implicit_name, &existing);

            if (!existing) {
                /* Initialize static component data */
                id = _::cpp_type<T>::id_explicit(world, name, allow_tag, id);
            } else {
                ecs_assert(ecs_is_valid(world, id), ECS_INTERNAL_ERROR, NULL);
            }

            /* Initialize lifecycle actions (ctor, dtor, copy, move) */
            if (_::cpp_type<T>::size() && !existing) {
                _::register_lifecycle_actions<T>(world, id);
            }
        }

        m_world = world;
        m_id = id;
    }

    /** Register on_add hook. */
    template <typename Func>
    component<T>& on_add(Func&& func) {
        using Invoker = typename _::each_invoker<
            typename std::decay<Func>::type, T>;
        flecs::type_hooks_t h = get_hooks();
        ecs_assert(h.on_add == nullptr, ECS_INVALID_OPERATION, 
            "on_add hook is already set");
        BindingCtx *ctx = get_binding_ctx(h);
        h.on_add = Invoker::run_add;
        ctx->on_add = FLECS_NEW(Invoker)(FLECS_FWD(func));
        ctx->free_on_add = reinterpret_cast<ecs_ctx_free_t>(
            _::free_obj<Invoker>);
        ecs_set_hooks_id(m_world, m_id, &h);
        return *this;
    }

    /** Register on_remove hook. */
    template <typename Func>
    component<T>& on_remove(Func&& func) {
        using Invoker = typename _::each_invoker<
            typename std::decay<Func>::type, T>;
        flecs::type_hooks_t h = get_hooks();
        ecs_assert(h.on_remove == nullptr, ECS_INVALID_OPERATION, 
            "on_remove hook is already set");
        BindingCtx *ctx = get_binding_ctx(h);
        h.on_remove = Invoker::run_remove;
        ctx->on_remove = FLECS_NEW(Invoker)(FLECS_FWD(func));
        ctx->free_on_remove = reinterpret_cast<ecs_ctx_free_t>(
            _::free_obj<Invoker>);
        ecs_set_hooks_id(m_world, m_id, &h);
        return *this;
    }

    /** Register on_set hook. */
    template <typename Func>
    component<T>& on_set(Func&& func) {
        using Invoker = typename _::each_invoker<
            typename std::decay<Func>::type, T>;
        flecs::type_hooks_t h = get_hooks();
        ecs_assert(h.on_set == nullptr, ECS_INVALID_OPERATION, 
            "on_set hook is already set");
        BindingCtx *ctx = get_binding_ctx(h);
        h.on_set = Invoker::run_set;
        ctx->on_set = FLECS_NEW(Invoker)(FLECS_FWD(func));
        ctx->free_on_set = reinterpret_cast<ecs_ctx_free_t>(
            _::free_obj<Invoker>);
        ecs_set_hooks_id(m_world, m_id, &h);
        return *this;
    }

#   ifdef FLECS_META
#   include "mixins/meta/component.inl"
#   endif

private:
    using BindingCtx = _::component_binding_ctx;

    BindingCtx* get_binding_ctx(flecs::type_hooks_t& h){        
        BindingCtx *result = static_cast<BindingCtx*>(h.binding_ctx);
        if (!result) {
            result = FLECS_NEW(BindingCtx);
            h.binding_ctx = result;
            h.binding_ctx_free = reinterpret_cast<ecs_ctx_free_t>(
                _::free_obj<BindingCtx>);
        }
        return result;
    }

    flecs::type_hooks_t get_hooks() {
        const flecs::type_hooks_t* h = ecs_get_hooks_id(m_world, m_id);
        if (h) {
            return *h;
        } else {
            return {};
        }
    }
};

/** Reset static component ids.
 * When components are registered their component ids are stored in a static
 * type specific variable. This stored id is passed into component registration
 * functions to ensure consistent ids across worlds.
 * 
 * In some cases this can be undesirable, like when a process repeatedly creates
 * worlds with different components. A typical example where this can happen is
 * when running multiple tests in a single process, where each test registers
 * its own set of components.
 * 
 * This operation can be used to prevent reusing of component ids and force 
 * generating a new ids upon registration.
 * 
 * Note that this operation should *never* be called while there are still
 * alive worlds in a process. Doing so results in undefined behavior.
 * 
 * Also note that this operation does not actually change the static component
 * variables. It only ensures that the next time a component id is requested, a
 * new id will be generated.
 * 
 * \ingroup cpp_components
 */
ECS_DEPRECATED("reset was deprecated, world-local component ids "
               "are supported by default now.")
inline void reset() {}

}

/** @} */
