/**
 * @file addons/cpp/mixins/metrics/builder.hpp
 * @brief Metric builder.
 */

#pragma once

#define ECS_EVENT_DESC_ID_COUNT_MAX (8)

namespace flecs {

/**
 * \ingroup cpp_addon_metrics
 * @{
 */

/** Event builder interface */
struct metric_builder {
    metric_builder(flecs::world_t *world, flecs::entity_t entity) 
        : m_world(world) 
    {
        m_desc.entity = entity;
    }

    ~metric_builder();

    metric_builder& member(flecs::entity_t e) {
        m_desc.member = e;
        return *this;
    }

    metric_builder& member(const char *name);

    template <typename T>
    metric_builder& member(const char *name);

    metric_builder& dotmember(const char *name);

    template <typename T>
    metric_builder& dotmember(const char *name);

    metric_builder& id(flecs::id_t the_id) {
        m_desc.id = the_id;
        return *this;
    }

    metric_builder& id(flecs::entity_t first, flecs::entity_t second) {
        m_desc.id = ecs_pair(first, second);
        return *this;
    }

    template <typename T>
    metric_builder& id() {
        return id(_::cpp_type<T>::id(m_world));
    }

    template <typename First>
    metric_builder& id(flecs::entity_t second) {
        return id(_::cpp_type<First>::id(m_world), second);
    }

    template <typename Second>
    metric_builder& id_second(flecs::entity_t first) {
        return id(first, _::cpp_type<Second>::id(m_world));
    }

    template <typename First, typename Second>
    metric_builder& id() {
        return id<First>(_::cpp_type<Second>::id(m_world));
    }

    metric_builder& targets(bool value = true) {
        m_desc.targets = value;
        return *this;
    }

    metric_builder& kind(flecs::entity_t the_kind) {
        m_desc.kind = the_kind;
        return *this;
    }

    template <typename Kind>
    metric_builder& kind() {
        return kind(_::cpp_type<Kind>::id(m_world));
    }

    metric_builder& brief(const char *b) {
        m_desc.brief = b;
        return *this;
    }

    operator flecs::entity();

protected:
    flecs::world_t *m_world;
    ecs_metric_desc_t m_desc = {};
    bool m_created = false;
};

/**
 * @}
 */

}
