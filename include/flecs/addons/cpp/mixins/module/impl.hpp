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
    auto c_ = component<T>(world, nullptr, false);

    // Make module component sparse so that it'll never move in memory. This
    // guarantees that a module destructor can be reliably used to cleanup
    // module resources.
    c_.add(flecs::Sparse);

    ecs_set_scope(world, c_);
    world.emplace<T>(world);
    ecs_set_scope(world, scope);

    ecs_add_id(world, c_, EcsModule);

    // It should now be possible to lookup the module
    ecs_entity_t m = ecs_lookup_symbol(world, symbol, false, false);
    ecs_assert(m != 0, ECS_MODULE_UNDEFINED, symbol);
    ecs_assert(m == c_, ECS_INTERNAL_ERROR, NULL);

    ecs_log_pop();

    return m;
}

template <typename T>
flecs::entity import(world& world) {
    const char *symbol = _::symbol_name<T>();

    ecs_entity_t m = ecs_lookup_symbol(world, symbol, true, false);

    if (!_::type<T>::registered(world)) {

        /* Module is registered with world, initialize static data */
        if (m) {
            _::type<T>::init(m, false);

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
 * @ingroup cpp_addons
 * Modules organize components, systems and more in reusable units of code.
 *
 * @{
 */

template <typename Module>
inline flecs::entity world::module(const char *name) const {
    flecs::entity result = this->entity(_::type<Module>::id(
        world_, nullptr, false));

    if (name) {
        flecs::entity prev_parent = result.parent();
        ecs_add_path_w_sep(world_, result, 0, name, "::", "::");
        flecs::entity parent = result.parent();
        if (prev_parent != parent) {
            // Module was reparented, cleanup old parent(s)
            flecs::entity cur = prev_parent, next;
            do {
                next = cur.parent();

                ecs_iter_t it = ecs_each_id(world_, ecs_pair(EcsChildOf, cur));
                if (!ecs_iter_is_true(&it)) {
                    cur.destruct();
                }

                cur = next;
            } while (cur);
        }
    }

    return result;
}

template <typename Module>
inline flecs::entity world::import() {
    return flecs::_::import<Module>(*this);
}

/** @} */

}
