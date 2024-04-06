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

    world.component<Value>();
    world.component<Source>();

    world.entity<metrics::Instance>("::flecs::metrics::Instance");
    world.entity<metrics::Metric>("::flecs::metrics::Metric");
    world.entity<metrics::Counter>("::flecs::metrics::Metric::Counter");
    world.entity<metrics::CounterId>("::flecs::metrics::Metric::CounterId");
    world.entity<metrics::CounterIncrement>("::flecs::metrics::Metric::CounterIncrement");
    world.entity<metrics::Gauge>("::flecs::metrics::Metric::Gauge");
}

inline metric_builder::~metric_builder() {
    if (!created_) {
        ecs_metric_init(world_, &desc_);
    }
}

inline metric_builder& metric_builder::member(const char *name) {
    flecs::entity m;
    if (desc_.id) {
        flecs::entity_t type = ecs_get_typeid(world_, desc_.id);
        m = flecs::entity(world_, type).lookup(name);
    } else {
        m = flecs::world(world_).lookup(name);
    }
    if (!m) {
        flecs::log::err("member '%s' not found", name);
    }
    return member(m);
}

template <typename T>
inline metric_builder& metric_builder::member(const char *name) {
    flecs::entity e (world_, _::type<T>::id(world_));
    flecs::entity_t m = e.lookup(name);
    if (!m) {
        flecs::log::err("member '%s' not found in type '%s'", 
            name, e.path().c_str());
        return *this;
    }
    return member(m);
}

inline metric_builder& metric_builder::dotmember(const char *expr) {
    desc_.dotmember = expr;
    return *this;
}

template <typename T>
inline metric_builder& metric_builder::dotmember(const char *expr) {
    desc_.dotmember = expr;
    desc_.id = _::type<T>::id(world_);
    return *this;
}

inline metric_builder::operator flecs::entity() {
    if (!created_) {
        created_ = true;
        flecs::entity result(world_, ecs_metric_init(world_, &desc_));
        desc_.entity = result;
        return result;
    } else {
        return flecs::entity(world_, desc_.entity);
    }
}

template <typename... Args>
inline flecs::metric_builder world::metric(Args &&... args) const {
    flecs::entity result(world_, FLECS_FWD(args)...);
    return flecs::metric_builder(world_, result);
}

template <typename Kind>
inline untyped_component& untyped_component::metric(
    flecs::entity_t parent, 
    const char *brief, 
    const char *metric_name) 
{
    flecs::world w(world_);
    flecs::entity e(world_, id_);

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
