#pragma once

#include <ctype.h>

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
    static const size_t len = ECS_FUNC_TYPE_LEN("type_name", ECS_FUNC_NAME);
    static char result[len + 1] = {};
    return ecs_cpp_get_type_name(result, ECS_FUNC_NAME, len);
} 
#else
#error "implicit component registration not supported"
#endif

// Translate a typename into a language-agnostic identifier. This allows for
// registration of components/modules across language boundaries.
template <typename T>
inline static const char* symbol_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN("symbol_name", ECS_FUNC_NAME);
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
    if (!ecs_component_has_actions(world, component)) {
        EcsComponentLifecycle cl{};
        cl.ctor = ctor<T>();
        cl.dtor = dtor<T>();

        cl.copy = copy<T>();
        cl.copy_ctor = copy_ctor<T>();
        cl.move = move<T>();
        cl.move_ctor = move_ctor<T>();

        cl.ctor_move_dtor = ctor_move_dtor<T>();
        cl.move_dtor = move_dtor<T>();

        ecs_set_component_actions_w_id( world, component, &cl);
    }
}

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
    // Initialize component identifier
    static void init(
        world_t* world, 
        entity_t entity, 
        bool allow_tag = true) 
    {
        // If an identifier was already set, check for consistency
        if (s_id) {
            ecs_assert(s_name.c_str() != nullptr, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(s_id == entity, ECS_INCONSISTENT_COMPONENT_ID, 
                type_name<T>());
            ecs_assert(allow_tag == s_allow_tag, ECS_INVALID_PARAMETER, NULL);

            // Component was already registered and data is consistent with new
            // identifier, so nothing else to be done.
            return;
        }

        // Component wasn't registered yet, set the values. Register component
        // name as the fully qualified flecs path.
        char *path = ecs_get_fullpath(world, entity);
        s_id = entity;
        s_name = flecs::string(path);
        s_allow_tag = allow_tag;
        s_size = sizeof(T);
        s_alignment = alignof(T);
        if (is_empty<T>::value && allow_tag) {
            s_size = 0;
            s_alignment = 0;
        }
    }

    // Obtain a component identifier for explicit component registration.
    static entity_t id_explicit(world_t *world = nullptr, 
        const char *name = nullptr, bool allow_tag = true, flecs::id_t id = 0)
    {
        if (!s_id) {
            // If no world was provided the component cannot be registered
            ecs_assert(world != nullptr, ECS_COMPONENT_NOT_REGISTERED, name);
        } else {
            ecs_assert(!id || s_id == id, ECS_INCONSISTENT_COMPONENT_ID, NULL);
            ecs_assert(s_allow_tag == allow_tag, ECS_INVALID_PARAMETER, NULL);
        }

        // If no id has been registered yet for the component (indicating the 
        // component has not yet been registered, or the component is used
        // across more than one binary), or if the id does not exists in the 
        // world (indicating a multi-world application), register it. */
        if (!s_id || (world && !ecs_exists(world, s_id))) {
            if (!s_id) {
                s_id = id;
            }

            ecs_assert(!id || s_id == id, ECS_INTERNAL_ERROR, NULL);

            init(world, s_id, allow_tag);

            entity_t entity = ecs_cpp_component_register_explicit(
                world, s_id, id, name, type_name<T>(), symbol_name<T>(), 
                    s_size, s_alignment);

            s_id = entity;

            // If component is enum type, register constants
            _::init_enum<T>(world, entity);
        }

        // By now the identifier must be valid and known with the world.
        ecs_assert(s_id != 0 && ecs_exists(world, s_id), 
            ECS_INTERNAL_ERROR, NULL);

        return s_id;
    }

    // Obtain a component identifier for implicit component registration. This
    // is almost the same as id_explicit, except that this operation 
    // automatically registers lifecycle callbacks.
    // Additionally, implicit registration temporarily resets the scope & with
    // state of the world, so that the component is not implicitly created with
    // the scope/with of the code it happens to be first used by.
    static id_t id(world_t *world = nullptr, const char *name = nullptr, 
        bool allow_tag = true) 
    {
        // If no id has been registered yet, do it now.
        if (!s_id || (world && !ecs_exists(world, s_id))) {
            ecs_entity_t prev_scope = 0;
            ecs_id_t prev_with = 0;

            if (world) {
                prev_scope = ecs_set_scope(world, 0);
                prev_with = ecs_set_with(world, 0);
            }
            
            // This will register a component id, but will not register 
            // lifecycle callbacks.
            id_explicit(world, name, allow_tag);

            // Register lifecycle callbacks, but only if the component has a
            // size. Components that don't have a size are tags, and tags don't
            // require construction/destruction/copy/move's. */
            if (size()) {
                register_lifecycle_actions<T>(world, s_id);
            }
            
            if (prev_with) {
                ecs_set_with(world, prev_with);
            }
            if (prev_scope) {
                ecs_set_scope(world, prev_scope);
            }
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        return s_id;
    }

    // Obtain a component name
    static const char* name(world_t *world = nullptr) {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            id(world);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        // If the id is set, the name should also have been set
        return s_name.c_str();
    }

    // Return the size of a component.
    static size_t size() {
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);
        return s_size;
    }

    // Return the alignment of a component.
    static size_t alignment() {
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);
        return s_alignment;
    }

    // Was the component already registered.
    static bool registered() {
        return s_id != 0;
    }

    // This function is only used to test cross-translation unit features. No
    // code other than test cases should invoke this function.
    static void reset() {
        s_id = 0;
        s_size = 0;
        s_alignment = 0;
        s_allow_tag = true;
        s_name.clear();
    }

private:
    static entity_t s_id;
    static flecs::string s_name;
    static size_t s_size;
    static size_t s_alignment;
    static bool s_allow_tag;
};

// Global templated variables that hold component identifier and other info
template <typename T> entity_t      cpp_type_impl<T>::s_id;
template <typename T> flecs::string cpp_type_impl<T>::s_name;
template <typename T> size_t        cpp_type_impl<T>::s_size;
template <typename T> size_t        cpp_type_impl<T>::s_alignment;
template <typename T> bool          cpp_type_impl<T>::s_allow_tag( true );

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
            cpp_type< pair_relation_t<T> >::id(world),
            cpp_type< pair_object_t<T> >::id(world));
    }
};

} // namespace _

struct untyped_component : entity {
    using entity::entity;
    
#   ifdef FLECS_META
#   include "mixins/meta/component.inl"
#   endif
};

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

        if (_::cpp_type<T>::registered()) {
            /* Obtain component id. Because the component is already registered,
             * this operation does nothing besides returning the existing id */
            id = _::cpp_type<T>::id_explicit(world, name, allow_tag, id);

            ecs_cpp_component_validate(world, id, n,
                _::cpp_type<T>::size(),
                _::cpp_type<T>::alignment(),
                implicit_name);
        } else {
            /* Find or register component */
            id = ecs_cpp_component_register(world, id, n, _::symbol_name<T>(),
                ECS_SIZEOF(T), ECS_ALIGNOF(T));

            /* Initialize static component data */
            id = _::cpp_type<T>::id_explicit(world, name, allow_tag, id);
        }

        /* Initialize lifecycle actions (ctor, dtor, copy, move) */
        if (_::cpp_type<T>::size()) {
            _::register_lifecycle_actions<T>(world, id);
        }

        m_world = world;
        m_id = id;
    }
};

template <typename T>
flecs::entity_t type_id() {
    return _::cpp_type<T>::id();
}

}
