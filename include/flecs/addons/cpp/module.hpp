
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Define a module
////////////////////////////////////////////////////////////////////////////////

template <typename T>
flecs::entity module(const flecs::world& world) {
    flecs::entity result = world.id<T>().entity();
    ecs_set_scope(world, result);
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
    ecs_set_scope(world, 0);

    // Initialize module component type & don't allow it to be registered as a
    // tag, as this would prevent calling emplace()
    auto m_c = component<T>(world, nullptr, false);
    ecs_add_id(world, m_c, EcsModule);

    world.emplace<T>(world);

    ecs_set_scope(world, scope);

    // // It should now be possible to lookup the module
    ecs_entity_t m = ecs_lookup_symbol(world, symbol, true);
    ecs_assert(m != 0, ECS_MODULE_UNDEFINED, symbol);
    ecs_assert(m == m_c, ECS_INTERNAL_ERROR, NULL);

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
