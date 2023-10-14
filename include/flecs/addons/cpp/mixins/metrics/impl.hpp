/**
 * @file addons/cpp/mixins/metrics/impl.hpp
 * @brief Metrics module implementation.
 */

#pragma once

namespace flecs {

inline metrics::metrics(flecs::world& world) {
    world.import<flecs::units>();

    /* Import C module  */
    FlecsMetricsImport(world);

    world.entity<metrics::Instance>("::flecs::metrics::Instance");
    world.entity<metrics::Metric>("::flecs::metrics::Metric");
    world.entity<metrics::Counter>("::flecs::metrics::Metric::Counter");
    world.entity<metrics::CounterId>("::flecs::metrics::Metric::CounterId");
    world.entity<metrics::CounterIncrement>("::flecs::metrics::Metric::CounterIncrement");
    world.entity<metrics::Gauge>("::flecs::metrics::Metric::Gauge");
}

inline metric_builder::~metric_builder() {
    if (!m_created) {
        ecs_metric_init(m_world, &m_desc);
    }
}

inline metric_builder& metric_builder::member(const char *name) {
    flecs::entity m;
    if (m_desc.id) {
        flecs::entity_t type = ecs_get_typeid(m_world, m_desc.id);
        m = flecs::entity(m_world, type).lookup(name);
    } else {
        m = flecs::world(m_world).lookup(name);
    }
    if (!m) {
        flecs::log::err("member '%s' not found", name);
    }
    return member(m);
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

inline metric_builder& metric_builder::dotmember(const char *expr) {
    m_desc.dotmember = expr;
    return *this;
}

template <typename T>
inline metric_builder& metric_builder::dotmember(const char *expr) {
    m_desc.dotmember = expr;
    m_desc.id = _::cpp_type<T>::id(m_world);
    return *this;
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
inline untyped_component& untyped_component::metric(
    flecs::entity_t parent, 
    const char *brief, 
    const char *metric_name) 
{
    flecs::world w(m_world);
    flecs::entity e(m_world, m_id);

    const flecs::member_t *m = ecs_cpp_last_member(w, e);
    if (!m) {
        return *this;
    }

    flecs::entity me = w.entity(m->member);
    flecs::entity metric_entity = me;
    if (parent) {
        const char *component_name = e.name();
        if (!metric_name) {
            if (ecs_os_strcmp(m->name, "value") || !component_name) {
                metric_entity = w.scope(parent).entity(m->name);
            } else {
                // If name of member is "value", use name of type.
                char *snake_name = flecs_to_snake_case(component_name);
                metric_entity = w.scope(parent).entity(snake_name);
                ecs_os_free(snake_name);
            }
        } else {
            metric_entity = w.scope(parent).entity(metric_name);
        }
    }

    w.metric(metric_entity).member(me).kind<Kind>().brief(brief);

    return *this;
}

}
