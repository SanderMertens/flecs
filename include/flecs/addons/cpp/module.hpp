
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Define a module
////////////////////////////////////////////////////////////////////////////////

template <typename T>
flecs::entity module(const flecs::world& world, const char *name = nullptr) {
    ecs_set_scope(world.c_ptr(), 0);
    flecs::entity result = pod_component<T>(world, name, false);
    ecs_add_module_tag(world, result.id());
    ecs_set_scope(world.c_ptr(), result.id());

    // Only register copy/move/dtor, make sure to not instantiate ctor as the
    // default ctor doesn't work for modules. Additionally, the module ctor
    // should only be invoked once per import.
    EcsComponentLifecycle cl{};
    cl.copy = _::copy<T>();
    cl.move = _::move<T>();
    cl.dtor = _::dtor<T>();
    ecs_set_component_actions_w_id(world, result, &cl);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
//// Import a module
////////////////////////////////////////////////////////////////////////////////

template <typename T>
ecs_entity_t do_import(world& world, const char *symbol) {
    ecs_trace_1("import %s", _::name_helper<T>::name());
    ecs_log_push();

    ecs_entity_t scope = ecs_get_scope(world);

    // Create custom storage to prevent object destruction
    T* module_data = static_cast<T*>(ecs_os_malloc(sizeof(T)));
    FLECS_PLACEMENT_NEW(module_data, T(world));

    ecs_set_scope(world, scope);

    // It should now be possible to lookup the module
    ecs_entity_t m = ecs_lookup_symbol(world, symbol, true);
    ecs_assert(m != 0, ECS_MODULE_UNDEFINED, symbol);

    _::cpp_type<T>::init(world, m, false);

    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INTERNAL_ERROR, NULL);

    // Set module singleton component

    T* module_ptr = static_cast<T*>(
        ecs_get_mut_id(world, m, 
            _::cpp_type<T>::id_explicit(world, nullptr, false), NULL));

    *module_ptr = std::move(*module_data);

    // Don't dtor, as a module should only be destructed once when the module
    // component is removed.
    ecs_os_free(module_data);

    // Add module tag        
    ecs_add_id(world, m, flecs::Module);

    ecs_log_pop();     

    return m;
}

template <typename T>
flecs::entity import(world& world) {
    char *symbol = _::symbol_helper<T>::symbol();

    ecs_entity_t m = ecs_lookup_symbol(world.c_ptr(), symbol, true);
    
    if (!_::cpp_type<T>::registered()) {

        /* Module is registered with world, initialize static data */
        if (m) {
            _::cpp_type<T>::init(world.c_ptr(), m, false);
        
        /* Module is not yet registered, register it now */
        } else {
            m = do_import<T>(world, symbol);
        }

    /* Module has been registered, but could have been for another world. Import
     * if module hasn't been registered for this world. */
    } else if (!m) {
        m = do_import<T>(world, symbol);
    }

    ecs_os_free(symbol);

    return flecs::entity(world, m);
}

} // namespace flecs
