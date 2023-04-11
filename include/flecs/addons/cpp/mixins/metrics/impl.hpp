/**
 * @file addons/cpp/mixins/metrics/impl.hpp
 * @brief Metrics module implementation.
 */

#pragma once

namespace flecs {

inline metrics::metrics(flecs::world& world) {
    /* Import C module  */
    FlecsMetricsImport(world);

    world.entity<metrics::Counter>("::flecs::metrics::Kind::Counter");
    world.entity<metrics::CounterIncrement>("::flecs::metrics::Kind::CounterIncrement");
    world.entity<metrics::Gauge>("::flecs::metrics::Kind::Gauge");
}

inline metric_builder::~metric_builder() {
    if (!m_created) {
        ecs_metric_init(m_world, &m_desc);
    }
}

inline metric_builder& metric_builder::member(const char *name) {
    return member(flecs::world(m_world).lookup(name));
}

template <typename T>
inline metric_builder& metric_builder::member(const char *name) {
    flecs::entity e (m_world, _::cpp_type<T>::id(m_world));
    flecs::entity_t m = e.lookup(name);
    if (!m) {
        flecs::log::err("member '%s' not found in type '%s'", 
            name, e.path().c_str());
        return *this;
    }
    return member(m);
}

inline metric_builder::operator flecs::entity() {
    if (!m_created) {
        m_created = true;
        flecs::entity result(m_world, ecs_metric_init(m_world, &m_desc));
        m_desc.entity = result;
        return result;
    } else {
        return flecs::entity(m_world, m_desc.entity);
    }
}

template <typename... Args>
inline flecs::metric_builder world::metric(Args &&... args) const {
    flecs::entity result(m_world, FLECS_FWD(args)...);
    return flecs::metric_builder(m_world, result);
}

template <typename Kind>
inline untyped_component& untyped_component::metric(flecs::entity_t parent) {
    flecs::world w(m_world);
    flecs::entity e = w.entity(m_id);
    const flecs::Struct *s = e.get<flecs::Struct>();
    if (!s) {
        flecs::log::err("can't register metric, component '%s' is not a struct",    
            e.path().c_str());
        return *this;
    }

    ecs_member_t *m = ecs_vec_get_t(&s->members, ecs_member_t, 
        ecs_vec_count(&s->members) - 1);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs::entity me = e.lookup(m->name);
    if (!me) {
        flecs::log::err("can't find member '%s' in component '%s' for metric",    
            m->name, e.path().c_str());
        return *this;
    }

    flecs::entity metric_entity = w.scope(parent).entity(m->name);
    w.metric(metric_entity).member(me).kind<Kind>();

    return *this;
}

template <typename Kind>
inline untyped_component& untyped_component::metric(const char *parent) {
    flecs::world w(m_world);
    flecs::entity p = w.entity(parent);
    return metric<Kind>(p);
}

}
