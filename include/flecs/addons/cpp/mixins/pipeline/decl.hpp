/**
 * @file addons/cpp/mixins/pipeline/decl.hpp
 * @brief Pipeline module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_pipelines Pipelines
 * @ingroup cpp_addons
 * Pipelines order and schedule systems for execution.
 *
 * @{
 */

/** Forward declaration for pipeline. */
template <typename ... Components>
struct pipeline;

/** Forward declaration for pipeline builder. */
template <typename ... Components>
struct pipeline_builder;

/** Built-in pipeline phase run at startup. */
static const flecs::entity_t OnStart = EcsOnStart;
/** Built-in pipeline phase run before each frame. */
static const flecs::entity_t PreFrame = EcsPreFrame;
/** Built-in pipeline phase for loading data. */
static const flecs::entity_t OnLoad = EcsOnLoad;
/** Built-in pipeline phase run after loading data. */
static const flecs::entity_t PostLoad = EcsPostLoad;
/** Built-in pipeline phase run before the update. */
static const flecs::entity_t PreUpdate = EcsPreUpdate;
/** Built-in pipeline phase for the main update. */
static const flecs::entity_t OnUpdate = EcsOnUpdate;
/** Built-in pipeline phase for validation after the update. */
static const flecs::entity_t OnValidate = EcsOnValidate;
/** Built-in pipeline phase run after the update. */
static const flecs::entity_t PostUpdate = EcsPostUpdate;
/** Built-in pipeline phase run before storing data. */
static const flecs::entity_t PreStore = EcsPreStore;
/** Built-in pipeline phase for storing data. */
static const flecs::entity_t OnStore = EcsOnStore;
/** Built-in pipeline phase run after each frame. */
static const flecs::entity_t PostFrame = EcsPostFrame;

/** @} */

}
