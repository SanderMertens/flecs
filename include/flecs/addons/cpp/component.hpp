/**
 * @file addons/cpp/component.hpp
 * @brief Registering/obtaining info from components.
 */

#pragma once

#include <ctype.h>
#include <stdio.h>

/**
 * @defgroup cpp_components Components
 * @ingroup cpp_core
 * Registering and working with components.
 *
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
inline const char* type_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN(const char*, type_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};
    static const size_t front_len = ECS_FUNC_NAME_FRONT(const char*, type_name);
    static const char* cppTypeName = ecs_cpp_get_type_name(result, ECS_FUNC_NAME, len, front_len);
    return cppTypeName;
}
#else
#error "implicit component registration not supported"
#endif

// Translate a typename into a language-agnostic identifier. This allows for
// registration of components/modules across language boundaries.
template <typename T>
inline const char* symbol_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN(const char*, symbol_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};
    static const char* cppSymbolName = ecs_cpp_get_symbol_name(result, type_name<T>(), len);
    return cppSymbolName;
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
    cl.ctor = ctor<T>(cl.flags);
    cl.dtor = dtor<T>(cl.flags);

    cl.copy = copy<T>(cl.flags);
    cl.copy_ctor = copy_ctor<T>(cl.flags);
    cl.move = move<T>(cl.flags);
    cl.move_ctor = move_ctor<T>(cl.flags);

    cl.ctor_move_dtor = ctor_move_dtor<T>(cl.flags);
    cl.move_dtor = move_dtor<T>(cl.flags);

    ecs_set_hooks_id(world, component, &cl);

    if (cl.flags & (ECS_TYPE_HOOK_MOVE_ILLEGAL|ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL))
    {
        ecs_add_id(world, component, flecs::Sparse);
    }
}

template <typename T>
struct type_impl {
    static_assert(is_pointer<T>::value == false,
        "pointer types are not allowed for components");

    // Initialize component identifier
    static void init(
        bool allow_tag = true)
    {
        s_index = flecs_component_ids_index_get();
        s_allow_tag = allow_tag;
        s_size = sizeof(T);
        s_alignment = alignof(T);
        if (is_empty<T>::value && allow_tag) {
            s_size = 0;
            s_alignment = 0;
        }
    }

    static void init_builtin(
        flecs::world_t *world,
        flecs::entity_t id,
        bool allow_tag = true)
    {
        init(allow_tag);
        flecs_component_ids_set(world, s_index, id);
    }

    // Register component id.
    static entity_t register_id(world_t *world,
        const char *name = nullptr, bool allow_tag = true, flecs::id_t id = 0,
        bool is_component = false, bool implicit_name = true, const char *n = nullptr, 
        flecs::entity_t module = 0)
    {
        if (!s_index) {
            // This is the first time (in this binary image) that this type is
            // being used. Generate a static index that will identify the type
            // across worlds.
            init(allow_tag);
            ecs_assert(s_index != 0, ECS_INTERNAL_ERROR, NULL);
        }

        flecs::entity_t c = flecs_component_ids_get(world, s_index);

        if (!c || !ecs_is_alive(world, c)) {
            // When a component is implicitly registered, ensure that it's not
            // registered in the current scope of the application/that "with"
            // components get added to the component entity.
            ecs_entity_t prev_scope = ecs_set_scope(world, module);
            ecs_entity_t prev_with = ecs_set_with(world, 0);

            // At this point it is possible that the type was already registered
            // with the world, just not for this binary. The registration code
            // uses the type symbol to check if it was already registered. Note
            // that the symbol is separate from the typename, as an application
            // can override a component name when registering a type.
            bool existing = false;
            c = ecs_cpp_component_find(
                world, id, n, symbol_name<T>(), size(), alignment(), 
                implicit_name, &existing);

            const char *symbol = nullptr;
            if (c) {
                symbol = ecs_get_symbol(world, c);
            }
            if (!symbol) {
                symbol = symbol_name<T>();
            }

            c = ecs_cpp_component_register(world, c, c, name, type_name<T>(), 
                symbol, size(), alignment(), is_component, &existing);

            ecs_set_with(world, prev_with);
            ecs_set_scope(world, prev_scope);

            // Register lifecycle callbacks, but only if the component has a
            // size. Components that don't have a size are tags, and tags don't
            // require construction/destruction/copy/move's.
            if (size() && !existing) {
                register_lifecycle_actions<T>(world, c);
            }

            // Set world local component id
            flecs_component_ids_set(world, s_index, c);

            // If component is enum type, register constants. Make sure to do 
            // this after setting the component id, because the enum code will
            // be calling type<T>::id().
            #if FLECS_CPP_ENUM_REFLECTION_SUPPORT
            _::init_enum<T>(world, c);
            #endif
        }

        ecs_assert(c != 0, ECS_INTERNAL_ERROR, NULL);

        return c;
    }

    // Get type (component) id.
    // If type was not yet registered and automatic registration is allowed,
    // this function will also register the type.
    static entity_t id(world_t *world)
    {
#ifdef FLECS_CPP_NO_AUTO_REGISTRATION
        ecs_assert(registered(world), ECS_INVALID_OPERATION, 
            "component '%s' must be registered before use",
            type_name<T>());

        flecs::entity_t c = flecs_component_ids_get(world, s_index);
        ecs_assert(c != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_is_alive(world, c), ECS_INVALID_OPERATION,
            "component '%s' was deleted, reregister before using",
            type_name<T>());
#else
        flecs::entity_t c = flecs_component_ids_get_alive(world, s_index);
        if (!c) {
            c = register_id(world);
        }
#endif
        return c;
    }

    // Return the size of a component.
    static size_t size() {
        ecs_assert(s_index != 0, ECS_INTERNAL_ERROR, NULL);
        return s_size;
    }

    // Return the alignment of a component.
    static size_t alignment() {
        ecs_assert(s_index != 0, ECS_INTERNAL_ERROR, NULL);
        return s_alignment;
    }

    // Was the component already registered.
    static bool registered(flecs::world_t *world) {
        ecs_assert(world != nullptr, ECS_INVALID_PARAMETER, NULL);

        if (s_index == 0) {
            return false;
        }

        if (!flecs_component_ids_get(world, s_index)) {
            return false;
        }

        return true;
    }

    // This function is only used to test cross-translation unit features. No
    // code other than test cases should invoke this function.
    static void reset() {
        s_index = 0;
        s_size = 0;
        s_alignment = 0;
        s_allow_tag = true;
    }

    static int32_t s_index;
    static size_t s_size;
    static size_t s_alignment;
    static bool s_allow_tag;
};

// Global templated variables that hold component identifier and other info
template <typename T> int32_t  type_impl<T>::s_index;
template <typename T> size_t   type_impl<T>::s_size;
template <typename T> size_t   type_impl<T>::s_alignment;
template <typename T> bool     type_impl<T>::s_allow_tag( true );

// Front facing class for implicitly registering a component & obtaining
// static component data

// Regular type
template <typename T>
struct type<T, if_not_t< is_pair<T>::value >>
    : type_impl<base_type_t<T>> { };

// Pair type
template <typename T>
struct type<T, if_t< is_pair<T>::value >>
{
    // Override id method to return id of pair
    static id_t id(world_t *world = nullptr) {
        return ecs_pair(
            type< pair_first_t<T> >::id(world),
            type< pair_second_t<T> >::id(world));
    }
};

} // namespace _

/** Untyped component class.
 * Generic base class for flecs::component.
 *
 * @ingroup cpp_components
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
 * @ingroup cpp_components
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

            // Keep track of whether name was explicitly set. If not, and the
            // component was already registered, just use the registered name.
            // The registered name may differ from the typename as the registered
            // name includes the flecs scope. This can in theory be different from
            // the C++ namespace though it is good practice to keep them the same */
            implicit_name = true;
        }

        // If component is registered by module, ensure it's registered in the
        // scope of the module.
        flecs::entity_t module = ecs_get_scope(world);

        // Strip off the namespace part of the component name, unless a name was
        // explicitly provided by the application.
        if (module && implicit_name) {
            // If the type is a template type, make sure to ignore
            // inside the template parameter list.
            const char *start = strchr(n, '<'), *last_elem = NULL;
            if (start) {
                const char *ptr = start;
                while (ptr[0] && (ptr[0] != ':') && (ptr > n)) {
                    ptr --;
                }
                if (ptr[0] == ':') {
                    last_elem = ptr;
                }
            }

            if (last_elem) {
                name = last_elem + 1;
            }
        }

        world_ = world;
        id_ = _::type<T>::register_id(
            world, name, allow_tag, id, true, implicit_name, n, module);
    }

    /** Register on_add hook. */
    template <typename Func>
    component<T>& on_add(Func&& func) {
        using Delegate = typename _::each_delegate<typename std::decay<Func>::type, T>;
        flecs::type_hooks_t h = get_hooks();
        ecs_assert(h.on_add == nullptr, ECS_INVALID_OPERATION,
            "on_add hook is already set");
        BindingCtx *ctx = get_binding_ctx(h);
        h.on_add = Delegate::run_add;
        ctx->on_add = FLECS_NEW(Delegate)(FLECS_FWD(func));
        ctx->free_on_add = _::free_obj<Delegate>;
        ecs_set_hooks_id(world_, id_, &h);
        return *this;
    }

    /** Register on_remove hook. */
    template <typename Func>
    component<T>& on_remove(Func&& func) {
        using Delegate = typename _::each_delegate<
            typename std::decay<Func>::type, T>;
        flecs::type_hooks_t h = get_hooks();
        ecs_assert(h.on_remove == nullptr, ECS_INVALID_OPERATION,
            "on_remove hook is already set");
        BindingCtx *ctx = get_binding_ctx(h);
        h.on_remove = Delegate::run_remove;
        ctx->on_remove = FLECS_NEW(Delegate)(FLECS_FWD(func));
        ctx->free_on_remove = _::free_obj<Delegate>;
        ecs_set_hooks_id(world_, id_, &h);
        return *this;
    }

    /** Register on_set hook. */
    template <typename Func>
    component<T>& on_set(Func&& func) {
        using Delegate = typename _::each_delegate<
            typename std::decay<Func>::type, T>;
        flecs::type_hooks_t h = get_hooks();
        ecs_assert(h.on_set == nullptr, ECS_INVALID_OPERATION,
            "on_set hook is already set");
        BindingCtx *ctx = get_binding_ctx(h);
        h.on_set = Delegate::run_set;
        ctx->on_set = FLECS_NEW(Delegate)(FLECS_FWD(func));
        ctx->free_on_set = _::free_obj<Delegate>;
        ecs_set_hooks_id(world_, id_, &h);
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
            h.binding_ctx_free = _::free_obj<BindingCtx>;
        }
        return result;
    }

    flecs::type_hooks_t get_hooks() {
        const flecs::type_hooks_t* h = ecs_get_hooks_id(world_, id_);
        if (h) {
            return *h;
        } else {
            return {};
        }
    }
};

}

/** @} */
