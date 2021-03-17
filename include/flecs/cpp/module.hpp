
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Define a module
////////////////////////////////////////////////////////////////////////////////

template <typename T>
flecs::entity module(const flecs::world& world, const char *name = nullptr) {
    ecs_set_scope(world.c_ptr(), 0);
    flecs::entity result = pod_component<T>(world, name, false);
    ecs_set_scope(world.c_ptr(), result.id());
    return result;
}


////////////////////////////////////////////////////////////////////////////////
//// Import a module
////////////////////////////////////////////////////////////////////////////////

template <typename T>
ecs_entity_t do_import(world& world) {
    ecs_trace_1("import %s", _::name_helper<T>::name());
    ecs_log_push();

    ecs_entity_t scope = ecs_get_scope(world.c_ptr());

    // Allocate module, so the this ptr will remain stable
    // TODO: make sure memory is cleaned up with world
    T *module_data = FLECS_NEW(T)(world);

    ecs_set_scope(world.c_ptr(), scope);

    // It should now be possible to lookup the module
    char *symbol = _::symbol_helper<T>::symbol();
    ecs_entity_t m = ecs_lookup_symbol(world.c_ptr(), symbol);
    ecs_assert(m != 0, ECS_MODULE_UNDEFINED, symbol);
    ecs_os_free(symbol);

    _::cpp_type<T>::init(world.c_ptr(), m, false);

    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INTERNAL_ERROR, NULL);

    // Set module singleton component

    ecs_set_ptr_w_entity(
        world.c_ptr(), m,
        _::cpp_type<T>::id_no_lifecycle(world.c_ptr()), 
        _::cpp_type<T>::size(),
        module_data);

    ecs_log_pop();     

    return m;
}

template <typename T>
flecs::entity import(world& world) {
    const char *symbol = _::symbol_helper<T>::symbol();

    ecs_entity_t m = ecs_lookup_symbol(world.c_ptr(), symbol);
    
    if (!_::cpp_type<T>::registered()) {

        /* Module is registered with world, initialize static data */
        if (m) {
            _::cpp_type<T>::init(world.c_ptr(), m, false);
        
        /* Module is not yet registered, register it now */
        } else {
            m = do_import<T>(world);
        }

    /* Module has been registered, but could have been for another world. Import
     * if module hasn't been registered for this world. */
    } else if (!m) {
        m = do_import<T>(world);
    }

    return flecs::entity(world, m);
}

} // namespace flecs
