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

/** Builtin pipeline phase ran at startup. */
static const flecs::entity_t OnStart = EcsOnStart;
/** Builtin pipeline phase ran before each frame. */
static const flecs::entity_t PreFrame = EcsPreFrame;
/** Builtin pipeline phase for loading data. */
static const flecs::entity_t OnLoad = EcsOnLoad;
/** Builtin pipeline phase ran after loading data. */
static const flecs::entity_t PostLoad = EcsPostLoad;
/** Builtin pipeline phase ran before the update. */
static const flecs::entity_t PreUpdate = EcsPreUpdate;
/** Builtin pipeline phase for the main update. */
static const flecs::entity_t OnUpdate = EcsOnUpdate;
/** Builtin pipeline phase for validation after update. */
static const flecs::entity_t OnValidate = EcsOnValidate;
/** Builtin pipeline phase ran after the update. */
static const flecs::entity_t PostUpdate = EcsPostUpdate;
/** Builtin pipeline phase ran before storing data. */
static const flecs::entity_t PreStore = EcsPreStore;
/** Builtin pipeline phase for storing data. */
static const flecs::entity_t OnStore = EcsOnStore;
/** Builtin pipeline phase ran after each frame. */
static const flecs::entity_t PostFrame = EcsPostFrame;

/** @} */

}
