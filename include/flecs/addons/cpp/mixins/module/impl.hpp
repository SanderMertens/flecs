/**
 * @file addons/cpp/mixins/module/impl.hpp
 * @brief Module implementation.
 */

#pragma once

namespace flecs {

namespace _ {

template <typename T>
ecs_entity_t do_import(world& world, const char *symbol) {
    ecs_trace("#[magenta]import#[reset] %s", _::type_name<T>());
    ecs_log_push();

    ecs_entity_t scope = ecs_set_scope(world, 0);

    // Initialize module component type & don't allow it to be registered as a
    // tag, as this would prevent calling emplace()
    auto m_c = component<T>(world, nullptr, false);
    ecs_add_id(world, m_c, EcsModule);

    ecs_set_scope(world, m_c);
    world.emplace<T>(world);
    ecs_set_scope(world, scope);

    // It should now be possible to lookup the module
    ecs_entity_t m = ecs_lookup_symbol(world, symbol, true);
    ecs_assert(m != 0, ECS_MODULE_UNDEFINED, symbol);
    ecs_assert(m == m_c, ECS_INTERNAL_ERROR, NULL);

    ecs_log_pop();     

    return m;
}

template <typename T>
flecs::entity import(world& world) {
    const char *symbol = _::symbol_name<T>();

    ecs_entity_t m = ecs_lookup_symbol(world, symbol, true);
    
    if (!_::cpp_type<T>::registered(world)) {

        /* Module is registered with world, initialize static data */
        if (m) {
            _::cpp_type<T>::init(world, m, false);
        
        /* Module is not yet registered, register it now */
        } else {
            m = _::do_import<T>(world, symbol);
        }

    /* Module has been registered, but could have been for another world. Import
     * if module hasn't been registered for this world. */
    } else if (!m) {
        m = _::do_import<T>(world, symbol);
    }

    return flecs::entity(world, m);
}

}

/**
 * @defgroup cpp_addons_modules Modules
 * @brief Modules organize components, systems and more in reusable units of code.
 * 
 * \ingroup cpp_addons
 * @{
 */

template <typename Module>
inline flecs::entity world::module(const char *name) const {
    flecs::id_t result = _::cpp_type<Module>::id(m_world);
    if (name) {
        ecs_add_path_w_sep(m_world, result, 0, name, "::", "::");
    }
    ecs_set_scope(m_world, result);
    return flecs::entity(m_world, result);
}

template <typename Module>
inline flecs::entity world::import() {
    return flecs::_::import<Module>(*this);
}

/** @} */

}
