#pragma once

namespace flecs {

template <typename ... Components>
struct pipeline;

template <typename ... Components>
struct pipeline_builder;

/* Builtin pipeline tags */
static const flecs::entity_t PreFrame = EcsPreFrame;
static const flecs::entity_t OnLoad = EcsOnLoad;
static const flecs::entity_t PostLoad = EcsPostLoad;
static const flecs::entity_t PreUpdate = EcsPreUpdate;
static const flecs::entity_t OnUpdate = EcsOnUpdate;
static const flecs::entity_t OnValidate = EcsOnValidate;
static const flecs::entity_t PostUpdate = EcsPostUpdate;
static const flecs::entity_t PreStore = EcsPreStore;
static const flecs::entity_t OnStore = EcsOnStore;
static const flecs::entity_t PostFrame = EcsPostFrame;

}
