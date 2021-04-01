/**
 * @file flecs.hpp
 * @brief Flecs C++ API.
 *
 * This is a C++11 wrapper around the Flecs C API.
 */

#pragma once

// The C++ API does not use STL, save for type_traits
#include <type_traits>

namespace flecs {

////////////////////////////////////////////////////////////////////////////////
//// Forward declarations and types
////////////////////////////////////////////////////////////////////////////////

using world_t = ecs_world_t;
using id_t = ecs_id_t;
using entity_t = ecs_entity_t;
using type_t = ecs_type_t;
using snapshot_t = ecs_snapshot_t;
using filter_t = ecs_filter_t;
using query_t = ecs_query_t;
using ref_t = ecs_ref_t;
using iter_t = ecs_iter_t;

class world;
class snapshot;
class entity;
class entity_view;
class type;
class iter;
class filter;
class filter_iterator;
class child_iterator;
class world_filter;
class snapshot_filter;

template<typename ... Components>
class query_iterator;

template<typename ... Components>
class query;

template<typename ... Components>
class system;

namespace _
{
template <typename T>
class cpp_type;
}

////////////////////////////////////////////////////////////////////////////////
//// Builtin components and tags 
////////////////////////////////////////////////////////////////////////////////

/* Builtin components */
using Component = EcsComponent;
using ComponentLifecycle = EcsComponentLifecycle;
using Trigger = EcsTrigger;
using Type = EcsType;
using Name = EcsName;
using Timer = EcsTimer;
using RateFilter = EcsRateFilter;
using TickSource = EcsTickSource;
using SignatureExpr = EcsSignatureExpr;
using Signature = EcsSignature;
using Query = EcsQuery;
using ViewAction = EcsIterAction;
using Context = EcsContext;

/* Builtin tag ids */
static const flecs::entity_t Module = EcsModule;
static const flecs::entity_t Prefab = EcsPrefab;
static const flecs::entity_t Hidden = EcsHidden;
static const flecs::entity_t Disabled = EcsDisabled;
static const flecs::entity_t DisabledIntern = EcsDisabledIntern;
static const flecs::entity_t Inactive = EcsInactive;
static const flecs::entity_t OnDemand = EcsOnDemand;
static const flecs::entity_t Monitor = EcsMonitor;
static const flecs::entity_t Pipeline = EcsPipeline;

/* Trigger tags */
static const flecs::entity_t OnAdd = EcsOnAdd;
static const flecs::entity_t OnRemove = EcsOnRemove;
static const flecs::entity_t OnSet = EcsOnSet;
static const flecs::entity_t UnSet = EcsUnSet;

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

/** Builtin roles */
static const flecs::entity_t Trait = ECS_PAIR;
static const flecs::entity_t Pair = ECS_PAIR;
static const flecs::entity_t Switch = ECS_SWITCH;
static const flecs::entity_t Case = ECS_CASE;
static const flecs::entity_t Owned = ECS_OWNED;

/* Builtin entity ids */
static const flecs::entity_t Flecs = EcsFlecs;
static const flecs::entity_t FlecsCore = EcsFlecsCore;
static const flecs::entity_t World = EcsWorld;

/* Ids used by rule solver */
static const flecs::entity_t Wildcard = EcsWildcard;
static const flecs::entity_t This = EcsThis;
static const flecs::entity_t Transitive = EcsTransitive;
static const flecs::entity_t Final = EcsFinal;

/* Builtin relationships */
static const flecs::entity_t IsA = EcsIsA;
static const flecs::entity_t ChildOf = EcsChildOf;

}

#include <flecs/cpp/util.hpp>
#include <flecs/cpp/column.hpp>
#include <flecs/cpp/world.hpp>
#include <flecs/cpp/entity.hpp>
#include <flecs/cpp/type.hpp>
#include <flecs/cpp/component.hpp>
#include <flecs/cpp/module.hpp>
#include <flecs/cpp/filter.hpp>
#include <flecs/cpp/snapshot.hpp>
#include <flecs/cpp/filter_iterator.hpp>
#include <flecs/cpp/iter.hpp>
#include <flecs/cpp/query.hpp>
#include <flecs/cpp/system.hpp>
#include <flecs/cpp/reader_writer.hpp>
#include <flecs/cpp/impl.hpp>

#ifdef FLECS_DEPRECATED
#include <flecs/addons/deprecated/flecs.hpp>
#endif
