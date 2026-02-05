/**
 * @file addons/cpp/component.hpp
 * @brief Registering/obtaining info from components.
 */

#pragma once

/**
 * @defgroup cpp_components Components
 * @ingroup cpp_core
 * Registering and working with components.
 *
 * @{
 */

namespace flecs {

namespace _ {

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
template<typename T>
void register_lifecycle_actions(
    ecs_world_t *world,
    ecs_entity_t component)
{
    (void)world; (void)component;
    if constexpr (!std::is_trivial<T>::value) {
        // If the component is non-trivial, register component lifecycle actions.
        // Depending on the type not all callbacks may be available.
        ecs_type_hooks_t cl{};
        cl.ctor = ctor<T>(cl.flags);
        cl.dtor = dtor<T>(cl.flags);

        cl.copy = copy<T>(cl.flags);
        cl.copy_ctor = copy_ctor<T>(cl.flags);
        cl.move = move<T>(cl.flags);
        cl.move_ctor = move_ctor<T>(cl.flags);

        cl.ctor_move_dtor = ctor_move_dtor<T>(cl.flags);
        cl.move_dtor = move_dtor<T>(cl.flags);

        cl.flags &= ECS_TYPE_HOOKS_ILLEGAL;
        ecs_set_hooks_id(world, component, &cl);

        if (cl.flags & (ECS_TYPE_HOOK_MOVE_ILLEGAL|ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL))
        {
            ecs_add_id(world, component, flecs::Sparse);
        }
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
        index(); // Make sure global component index is initialized

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
        flecs_component_ids_set(world, index(), id);
    }

    // Register component id.
    static entity_t register_id(
        world_t *world,                     // The world
        const char *name = nullptr,         // User provided name (overrides typename)
        bool allow_tag = true,              // Register empty types as zero-sized components
        bool is_component = true,           // Add flecs::Component to result
        bool explicit_registration = false, // Entered from world.component<T>()?
        flecs::id_t id = 0)                 // User provided component id
    {
        init(allow_tag);
        ecs_assert(index() != 0, ECS_INTERNAL_ERROR, NULL);

        bool registered = false, existing = false;

        flecs::entity_t c = ecs_cpp_component_register(
            world, id, index(), name, type_name<T>(), 
            symbol_name<T>(), size(), alignment(),
            is_component, explicit_registration, &registered, &existing);

        ecs_assert(c != 0, ECS_INTERNAL_ERROR, NULL);

        if (registered) {
            // Register lifecycle callbacks, but only if the component has a
            // size. Components that don't have a size are tags, and tags don't
            // require construction/destruction/copy/move's.
            if (size() && !existing) {
                register_lifecycle_actions<T>(world, c);
            }

            // If component is enum type, register constants. Make sure to do 
            // this after setting the component id, because the enum code will
            // be calling type<T>::id().
            #if FLECS_CPP_ENUM_REFLECTION_SUPPORT
            _::init_enum<T>(world, c);
            #endif
        }

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

        flecs::entity_t c = flecs_component_ids_get(world, index());
        ecs_assert(c != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_is_alive(world, c), ECS_INVALID_OPERATION,
            "component '%s' was deleted, reregister before using",
            type_name<T>());
#else
        flecs::entity_t c = flecs_component_ids_get_alive(world, index());
        if (!c) {
            c = register_id(world);
        }
#endif
        return c;
    }

    // Return the size of a component.
    static size_t size() {
        return s_size;
    }

    // Return the alignment of a component.
    static size_t alignment() {
        return s_alignment;
    }

    // Was the component already registered.
    static bool registered(flecs::world_t *world) {
        ecs_assert(world != nullptr, ECS_INVALID_PARAMETER, NULL);

        if (!flecs_component_ids_get(world, index())) {
            return false;
        }

        return true;
    }

    // This function is only used to test cross-translation unit features. No
    // code other than test cases should invoke this function.
    static void reset() {
        s_size = 0;
        s_alignment = 0;
        s_allow_tag = true;
    }

    static int32_t index() {
        static int32_t index_ = flecs_component_ids_index_get();
        return index_;
    }

    static size_t s_size;
    static size_t s_alignment;
    static bool s_allow_tag;
};

// Global templated variables that hold component identifier and other info
template <typename T> inline size_t   type_impl<T>::s_size;
template <typename T> inline size_t   type_impl<T>::s_alignment;
template <typename T> inline bool     type_impl<T>::s_allow_tag( true );

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
  
    untyped_component() : entity() { }
    explicit untyped_component(flecs::world_t *world, flecs::entity_t id) : entity(world, id) { }
    explicit untyped_component(flecs::entity_t id) : entity(id) { }

    explicit untyped_component(flecs::world_t *world, const char *name)
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

protected:

flecs::type_hooks_t get_hooks() const {
    const flecs::type_hooks_t* h = ecs_get_hooks_id(world_, id_);
    if (h) {
        return *h;
    } else {
        return {};
    }
}

void set_hooks(flecs::type_hooks_t &h) {
    h.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world_, id_, &h);
}

public:

untyped_component& on_compare(
    ecs_cmp_t compare_callback) 
{
    ecs_assert(compare_callback, ECS_INVALID_PARAMETER, NULL);
    flecs::type_hooks_t h = get_hooks();
    h.cmp = compare_callback;
    h.flags &= ~ECS_TYPE_HOOK_CMP_ILLEGAL;
    if(h.flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL) {
        h.flags &= ~ECS_TYPE_HOOK_EQUALS_ILLEGAL;
        h.equals = NULL;
    }
    set_hooks(h);
    return *this;
}

untyped_component& on_equals(
    ecs_equals_t equals_callback) 
{
    ecs_assert(equals_callback, ECS_INVALID_PARAMETER, NULL);
    flecs::type_hooks_t h = get_hooks();
    h.equals = equals_callback;
    h.flags &= ~ECS_TYPE_HOOK_EQUALS_ILLEGAL;
    set_hooks(h);
    return *this;
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
        set_hooks(h);
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
        set_hooks(h);
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
        set_hooks(h);
        return *this;
    }

    /** Register on_replace hook. */
    template <typename Func>
    component<T>& on_replace(Func&& func) {
        using Delegate = typename _::each_delegate<
            typename std::decay<Func>::type, T, T>;
        flecs::type_hooks_t h = get_hooks();
        ecs_assert(h.on_set == nullptr, ECS_INVALID_OPERATION,
            "on_set hook is already set");
        BindingCtx *ctx = get_binding_ctx(h);
        h.on_replace = Delegate::run_replace;
        ctx->on_replace = FLECS_NEW(Delegate)(FLECS_FWD(func));
        ctx->free_on_replace = _::free_obj<Delegate>;
        set_hooks(h);
        return *this;
    }

    /** Register operator compare hook. */
    using untyped_component::on_compare;
    component<T>& on_compare() {
        ecs_cmp_t handler = _::compare<T>();
        ecs_assert(handler != NULL, ECS_INVALID_OPERATION, 
            "Type does not have operator> or operator< const or is inaccessible");
        on_compare(handler);
        return *this;
    }

    /** Type safe variant of compare op function */
    using cmp_hook = int(*)(const T* a, const T* b, const ecs_type_info_t *ti);
    component<T>& on_compare(cmp_hook callback) {
        on_compare(reinterpret_cast<ecs_cmp_t>(callback));
        return *this;
    }

    /** Register operator equals hook. */
    using untyped_component::on_equals;
    component<T>& on_equals() {
        ecs_equals_t handler = _::equals<T>();
        ecs_assert(handler != NULL, ECS_INVALID_OPERATION, 
            "Type does not have operator== const or is inaccessible");
        on_equals(handler);
        return *this;
    }

    /** Type safe variant of equals op function */
    using equals_hook = bool(*)(const T* a, const T* b, const ecs_type_info_t *ti);
    component<T>& on_equals(equals_hook callback) {
        on_equals(reinterpret_cast<ecs_equals_t>(callback));
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
};

}

/** @} */
