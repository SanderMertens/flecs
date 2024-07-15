/**
 * @file addons/cpp/mixins/metrics/builder.hpp
 * @brief Metric builder.
 */

#pragma once

#define ECS_EVENT_DESC_ID_COUNT_MAX (8)

namespace flecs {

/**
 * @ingroup cpp_addons_metrics
 * @{
 */

/** Event builder interface */
struct metric_builder {
    metric_builder(flecs::world_t *world, flecs::entity_t entity) 
        : world_(world) 
    {
        desc_.entity = entity;
    }

    ~metric_builder();

    metric_builder& member(flecs::entity_t e) {
        desc_.member = e;
        return *this;
    }

    metric_builder& member(const char *name);

    template <typename T>
    metric_builder& member(const char *name);

    metric_builder& dotmember(const char *name);

    template <typename T>
    metric_builder& dotmember(const char *name);

    metric_builder& id(flecs::id_t the_id) {
        desc_.id = the_id;
        return *this;
    }

    metric_builder& id(flecs::entity_t first, flecs::entity_t second) {
        desc_.id = ecs_pair(first, second);
        return *this;
    }

    template <typename T>
    metric_builder& id() {
        return id(_::type<T>::id(world_));
    }

    template <typename First>
    metric_builder& id(flecs::entity_t second) {
        return id(_::type<First>::id(world_), second);
    }

    template <typename Second>
    metric_builder& id_second(flecs::entity_t first) {
        return id(first, _::type<Second>::id(world_));
    }

    template <typename First, typename Second>
    metric_builder& id() {
        return id<First>(_::type<Second>::id(world_));
    }

    metric_builder& targets(bool value = true) {
        desc_.targets = value;
        return *this;
    }

    metric_builder& kind(flecs::entity_t the_kind) {
        desc_.kind = the_kind;
        return *this;
    }

    template <typename Kind>
    metric_builder& kind() {
        return kind(_::type<Kind>::id(world_));
    }

    metric_builder& brief(const char *b) {
        desc_.brief = b;
        return *this;
    }

    operator flecs::entity();

protected:
    flecs::world_t *world_;
    ecs_metric_desc_t desc_ = {};
    bool created_ = false;
};

/**
 * @}
 */

}
