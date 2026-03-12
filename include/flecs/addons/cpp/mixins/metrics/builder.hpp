/**
 * @file addons/cpp/mixins/metrics/builder.hpp
 * @brief Metric builder.
 */

#pragma once

namespace flecs {

/**
 * @ingroup cpp_addons_metrics
 * @{
 */

/** Metric builder interface. */
struct metric_builder {
    /** Construct a metric builder. */
    metric_builder(flecs::world_t *world, flecs::entity_t entity)
        : world_(world)
    {
        desc_.entity = entity;
    }

    /** Destructor. Finalizes the metric. */
    ~metric_builder();

    /** Set the member to use for the metric by entity ID. */
    metric_builder& member(flecs::entity_t e) {
        desc_.member = e;
        return *this;
    }

    /** Set the member to use for the metric by name. */
    metric_builder& member(const char *name);

    /** Set the member to use for the metric by type and name. */
    template <typename T>
    metric_builder& member(const char *name);

    /** Set the member to use for the metric using dot notation. */
    metric_builder& dotmember(const char *name);

    /** Set the member to use for the metric by type using dot notation. */
    template <typename T>
    metric_builder& dotmember(const char *name);

    /** Set the ID for the metric. */
    metric_builder& id(flecs::id_t the_id) {
        desc_.id = the_id;
        return *this;
    }

    /** Set the ID for the metric as a pair. */
    metric_builder& id(flecs::entity_t first, flecs::entity_t second) {
        desc_.id = ecs_pair(first, second);
        return *this;
    }

    /** Set the ID for the metric by type. */
    template <typename T>
    metric_builder& id() {
        return id(_::type<T>::id(world_));
    }

    /** Set the ID for the metric as a pair with type First. */
    template <typename First>
    metric_builder& id(flecs::entity_t second) {
        return id(_::type<First>::id(world_), second);
    }

    /** Set the ID for the metric as a pair with type Second. */
    template <typename Second>
    metric_builder& id_second(flecs::entity_t first) {
        return id(first, _::type<Second>::id(world_));
    }

    /** Set the ID for the metric as a pair with types First and Second. */
    template <typename First, typename Second>
    metric_builder& id() {
        return id<First>(_::type<Second>::id(world_));
    }

    /** Set whether to create metrics for targets. */
    metric_builder& targets(bool value = true) {
        desc_.targets = value;
        return *this;
    }

    /** Set the metric kind (e.g., Counter, Gauge). */
    metric_builder& kind(flecs::entity_t the_kind) {
        desc_.kind = the_kind;
        return *this;
    }

    /** Set the metric kind by type. */
    template <typename Kind>
    metric_builder& kind() {
        return kind(_::type<Kind>::id(world_));
    }

    /** Set a brief description for the metric. */
    metric_builder& brief(const char *b) {
        desc_.brief = b;
        return *this;
    }

    /** Finalize the metric and return the entity. */
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
