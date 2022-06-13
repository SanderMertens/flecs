#pragma once

namespace flecs {

struct table {
    table() : m_world(nullptr), m_table(nullptr) { }

    table(world_t *world, table_t *t)
        : m_world(world)
        , m_table(t) { }

    flecs::string str() const {
        return flecs::string(ecs_table_str(m_world, m_table));
    }

    flecs::type type() const {
        return flecs::type(m_world, ecs_table_get_type(m_table));
    }

    bool has(flecs::id_t id) const {
        return ecs_search(m_world, m_table, id, 0) != -1;
    }

    template <typename T>
    bool has() const {
        return has(_::cpp_type<T>::id(m_world));
    }

    bool has(flecs::entity_t r, flecs::entity_t o) const {
        return has(ecs_pair(r, o));
    }

    template <typename R>
    bool has(flecs::entity_t o) const {
        return has(_::cpp_type<R>::id(m_world), o);
    }

    template <typename R, typename O>
    bool has() const {
        return has<R>(_::cpp_type<O>::id(m_world));
    }

    /* Implicit conversion to table_t */
    operator table_t*() const {
        return m_table;
    }
private:
    world_t *m_world;
    table_t *m_table;
};

}
