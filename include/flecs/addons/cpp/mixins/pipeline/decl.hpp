/**
 * @file addons/cpp/mixins/pipeline/decl.hpp
 * @brief Pipeline module declarations.
 */

#pragma once

FLECS_API_NAMESPACE namespace flecs {

/**
 * @defgroup cpp_pipelines Pipelines
 * @ingroup cpp_addons
 * Pipelines order and schedule systems for execution.
 *
 * @{
 */

template <typename ... Components>
struct pipeline;

template <typename ... Components>
struct pipeline_builder;

/* Builtin pipeline tags */
FLECS_STATIC_IN_HEADER const flecs::entity_t OnStart = EcsOnStart;
FLECS_STATIC_IN_HEADER const flecs::entity_t PreFrame = EcsPreFrame;
FLECS_STATIC_IN_HEADER const flecs::entity_t OnLoad = EcsOnLoad;
FLECS_STATIC_IN_HEADER const flecs::entity_t PostLoad = EcsPostLoad;
FLECS_STATIC_IN_HEADER const flecs::entity_t PreUpdate = EcsPreUpdate;
FLECS_STATIC_IN_HEADER const flecs::entity_t OnUpdate = EcsOnUpdate;
FLECS_STATIC_IN_HEADER const flecs::entity_t OnValidate = EcsOnValidate;
FLECS_STATIC_IN_HEADER const flecs::entity_t PostUpdate = EcsPostUpdate;
FLECS_STATIC_IN_HEADER const flecs::entity_t PreStore = EcsPreStore;
FLECS_STATIC_IN_HEADER const flecs::entity_t OnStore = EcsOnStore;
FLECS_STATIC_IN_HEADER const flecs::entity_t PostFrame = EcsPostFrame;

/** @} */

}
