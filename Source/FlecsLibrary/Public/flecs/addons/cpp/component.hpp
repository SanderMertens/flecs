/**
 * @file addons/cpp/component.hpp
 * @brief Registering/obtaining info from components.
 */

#pragma once

#include <ctype.h>
#include <stdio.h>

#include "flecs/os_api.h"
#include "Concepts/SolidConcepts.h"
#include "flecs/Unreal/FlecsScriptStructComponent.h"
#include "flecs/Unreal/FlecsTypeMapComponent.h"

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
    static constexpr size_t len = ECS_FUNC_TYPE_LEN(const char*, type_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};
    static constexpr size_t front_len = ECS_FUNC_NAME_FRONT(const char*, type_name);
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
    static constexpr size_t len = ECS_FUNC_TYPE_LEN(const char*, symbol_name, ECS_FUNC_NAME);
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
template<typename T UE_REQUIRES(std::is_trivial<T>::value)>
void register_lifecycle_actions(ecs_world_t*, ecs_entity_t)
{
}

// If the component is non-trivial, register component lifecycle actions.
// Depending on the type not all callbacks may be available.
template<typename T UE_REQUIRES(!std::is_trivial<T>::value)>
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

struct FLECS_API type_impl_data {
    int32_t s_index;
    size_t s_size;
    size_t s_alignment;
    bool s_allow_tag;
};

FLECSLIBRARY_API extern robin_hood::unordered_map<std::string, type_impl_data> g_type_to_impl_data;

    template <typename T>
    inline type_impl_data& get_or_create_type_data(bool allow_tag) 
    {
        ecs_os_perf_trace_push("flecs.type_impl.init");
        
        std::string key = std::string(_::type_name<T>());  // => Solid::type_name<T>()
    
        auto it = g_type_to_impl_data.find(key);
        if (it != g_type_to_impl_data.end()) {
            return it->second;
        }

        // Create a new entry
        type_impl_data data = type_impl_data {
            .s_index = flecs_component_ids_index_get(),
            .s_size = sizeof(T),
            .s_alignment = alignof(T),
            .s_allow_tag = allow_tag
        };

        if (std::is_empty<T>::value && allow_tag) {
            data.s_size      = 0;
            data.s_alignment = 0;
        }
        
        data.s_index = flecs_component_ids_index_get();

        auto [ins_it, _] = g_type_to_impl_data.emplace(key, data);

        ecs_os_perf_trace_pop("flecs.type_impl.init");
        return ins_it->second;
    }

    template <typename T>
    inline type_impl_data* get_type_data_if_any()
    {
        std::string key = std::string(_::type_name<T>());
        auto it = g_type_to_impl_data.find(key);
        if (it != g_type_to_impl_data.end()) {
            return &it->second;
        }
        return nullptr;
    }

template <typename T>
struct type_impl {
    static_assert(!std::is_pointer<T>::value,
        "pointer types are not allowed for components");

    // init: create an entry in the global map if one does not exist
    static void init(bool allow_tag = true)
    {
        get_or_create_type_data<T>(allow_tag);
    }

    // init_builtin: set up the data, then set the world-local ID
    static void init_builtin(
        flecs::world_t *world,
        flecs::entity_t id,
        bool allow_tag = true)
    {
        auto &td = get_or_create_type_data<T>(allow_tag);
        flecs_component_ids_set(world, td.s_index, id);
    }

    // Register component id. This logic remains much the same, but
    // references the global map rather than static fields.
    static entity_t register_id(
        world_t *world,
        const char *name = nullptr,
        bool allow_tag = true,
        bool is_component = true,
        bool explicit_registration = false,
        flecs::id_t id = 0) 
    {
        ecs_os_perf_trace_push("flecs.type_impl.register_id");
        
        // Ensure there's a record for T
        auto& td = get_or_create_type_data<T>(allow_tag);
        
        bool registered = false, existing = false;

        flecs::entity_t c = ecs_cpp_component_register(
            world, id, td.s_index, name, type_name<T>(), 
            symbol_name<T>(), td.s_size, td.s_alignment,
            is_component, explicit_registration, &registered, &existing);
        
        ecs_assert(c != 0, ECS_INTERNAL_ERROR, NULL);

        if (registered) {
            // Register lifecycle callbacks, but only if the component has a
            // size. Components that don't have a size are tags, and tags don't
            // require construction/destruction/copy/move's.
            if (size() && !existing) {
                register_lifecycle_actions<T>(world, c);
            }

            if constexpr (Solid::IsScriptStruct<T>() && !std::is_same_v<T, FFlecsScriptStructComponent>)
            {
                flecs::world P_world(world);
                UScriptStruct* scriptStruct = TBaseStructure<T>::Get();
                ecs_assert(scriptStruct != nullptr, ECS_INTERNAL_ERROR, 
                           "script struct is null");

                flecs::entity entity_id = flecs::entity(world, c);
                
                static_cast<FFlecsTypeMapComponent*>(P_world.get_binding_ctx())
                    ->ScriptStructMap.emplace(scriptStruct, entity_id);
                
                entity_id.set<FFlecsScriptStructComponent>({ scriptStruct });
            }

            // If component is enum type, register constants. Make sure to do 
            // this after setting the component id, because the enum code will
            // be calling type<T>::id().
            #if FLECS_CPP_ENUM_REFLECTION_SUPPORT
            _::init_enum<T>(world, c);

            if constexpr (Solid::IsStaticEnum<T>())
            {
                flecs::world P_world(world);
                UEnum* scriptEnum = StaticEnum<T>();
                ecs_assert(scriptEnum != nullptr, ECS_INTERNAL_ERROR, 
                           "script enum is null");

                flecs::entity entity_id = flecs::entity(world, c);
                
                static_cast<FFlecsTypeMapComponent*>(P_world.get_binding_ctx())
                    ->ScriptEnumMap.emplace(scriptEnum, entity_id);
                
                entity_id.set<FFlecsScriptEnumComponent>({ scriptEnum });
            }
            
            #endif
        }

        return c;
    }
        
    static entity_t id(flecs::world_t *world)
    {
        ecs_os_perf_trace_push("flecs.type_impl.id");

        // Make sure there's a record
        auto *td = get_type_data_if_any<T>();
        ecs_assert(td != nullptr, ECS_INTERNAL_ERROR, 
            "type_impl<T>::id() called without prior init");

        // Check if there's a valid entity in this world for T
        flecs::entity_t c = flecs_component_ids_get_alive(world, td->s_index);
        
        #ifndef FLECS_CPP_NO_AUTO_REGISTRATION
        
        if (!c) {
            c = register_id(world);
            
        }
        #endif
        
        ecs_assert(c != 0, ECS_NOT_A_COMPONENT, "component not found, %s", 
            _::type_name<T>());
        
        ecs_os_perf_trace_pop("flecs.type_impl.id");
        return c;
    }

    // Return the size
    static size_t size() {
        auto* td = get_type_data_if_any<T>();
        ecs_assert(td != nullptr, ECS_INTERNAL_ERROR, 
            "type_impl<T>::size() called before init");
        return td->s_size;
    }

    // Return the alignment
    static size_t alignment() {
        auto* td = get_type_data_if_any<T>();
        ecs_assert(td != nullptr, ECS_INTERNAL_ERROR,
            "type_impl<T>::alignment() called before init");
        return td->s_alignment;
    }

    // Check if T is registered in the provided world
    static bool registered(flecs::world_t *world) {
        ecs_assert(world != nullptr, ECS_INVALID_PARAMETER, NULL);
        auto* td = get_type_data_if_any<T>();
        if (!td) {
            return false;
        }
        flecs::entity_t c = flecs_component_ids_get(world, td->s_index);
        return (c != 0);
    }
        
    static void reset()
    {
        std::string key = _::type_name<T>();
        g_type_to_impl_data.erase(key);
    }
};

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

    explicit untyped_component(flecs::world_t *world, const char *name = nullptr)
    {
        world_ = world;

        ecs_entity_desc_t desc = {};
        desc.name = name;
        desc.sep = "::";
        desc.root_sep = "::";
        desc.use_low_id = true;
        id_ = ecs_entity_init(world, &desc);
    }

    explicit untyped_component(world_t *world, const char *name, const char *sep, const char *root_sep)
    {
        world_ = world;

        ecs_entity_desc_t desc = {};
        desc.name = name;
        desc.sep = sep;
        desc.root_sep = root_sep;
        desc.use_low_id = true;
        id_ = ecs_entity_init(world, &desc);
    }

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
        world_ = world;
        id_ = _::type<T>::register_id(world, name, allow_tag, true, true, id);
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
