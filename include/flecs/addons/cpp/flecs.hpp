/**
 * @file flecs.hpp
 * @brief Flecs C++ API.
 *
 * This is a C++11 wrapper around the Flecs C API.
 */

#pragma once

// The C++ API does not use STL, save for type_traits
#include <type_traits>

// Allows overriding flecs_static_assert, which is useful when testing
#ifndef flecs_static_assert
#define flecs_static_assert(cond, str) static_assert(cond, str)
#endif

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
using ComponentLifecycle = EcsComponentLifecycle;

enum inout_kind_t {
    InOutDefault = EcsInOutDefault,
    InOut = EcsInOut,
    In = EcsIn,
    Out = EcsOut
};

enum oper_kind_t {
    And = EcsAnd,
    Or = EcsOr,
    Not = EcsNot,
    Optional = EcsOptional,
    AndFrom = EcsAndFrom,
    OrFrom = EcsOrFrom,
    NotFrom = EcsNotFrom
};

enum var_kind_t {
    VarDefault = EcsVarDefault,
    VarIsEntity = EcsVarIsEntity,
    VarIsVariable = EcsVarIsVariable
};

class world;
class world_async_stage;
class snapshot;
class id;
class entity;
class entity_view;
class type;
class pipeline;
class iter;
class term;
class filter_iterator;
class child_iterator;
class world_filter;
class snapshot_filter;
class query_base;

template<typename ... Components>
class filter;

template<typename ... Components>
class query;

template<typename ... Components>
class system;

template<typename ... Components>
class observer;

template <typename ... Components>
class filter_builder;

template <typename ... Components>
class query_builder;

template <typename ... Components>
class system_builder;

template <typename ... Components>
class observer_builder;

namespace _
{
template <typename T, typename U = int>
class cpp_type;

template <typename Func, typename ... Components>
class each_invoker;
}

////////////////////////////////////////////////////////////////////////////////
//// Builtin components and tags 
////////////////////////////////////////////////////////////////////////////////

/* Builtin components */
using Component = EcsComponent;
using Type = EcsType;
using Identifier = EcsIdentifier;
using Timer = EcsTimer;
using RateFilter = EcsRateFilter;
using TickSource = EcsTickSource;
using Query = EcsQuery;
using Trigger = EcsTrigger;
using Observer = EcsObserver;

/* Builtin opaque components */
static const flecs::entity_t System = ecs_id(EcsSystem);

/* Builtin set constants */
static const uint8_t DefaultSet = EcsDefaultSet;
static const uint8_t Self = EcsSelf;
static const uint8_t SuperSet = EcsSuperSet;
static const uint8_t SubSet = EcsSubSet;
static const uint8_t Cascade = EcsCascade;
static const uint8_t All = EcsAll;
static const uint8_t Nothing = EcsNothing;

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
static const flecs::entity_t Tag = EcsTag;

/* Builtin relationships */
static const flecs::entity_t IsA = EcsIsA;
static const flecs::entity_t ChildOf = EcsChildOf;

/* Builtin identifiers */
static const flecs::entity_t Name = EcsName;
static const flecs::entity_t Symbol = EcsSymbol;

/* Cleanup rules */
static const flecs::entity_t OnDelete = EcsOnDelete;
static const flecs::entity_t OnDeleteObject = EcsOnDeleteObject;
static const flecs::entity_t Remove = EcsRemove;
static const flecs::entity_t Delete = EcsDelete;
static const flecs::entity_t Throw = EcsThrow;

}

#include "util.hpp"
#include "pair.hpp"
#include "function_traits.hpp"
#include "lifecycle_traits.hpp"
#include "iter.hpp"
#include "world.hpp"
#include "id.hpp"
#include "entity.hpp"
#include "component.hpp"
#include "invoker.hpp"
#include "builder.hpp"
#include "type.hpp"
#include "module.hpp"
#include "filter.hpp"
#include "snapshot.hpp"
#include "filter_iterator.hpp"
#include "query.hpp"
#include "system.hpp"
#include "observer.hpp"
#include "impl.hpp"
