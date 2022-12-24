/**
 * @file addons/cpp/c_types.hpp
 * @brief Aliases for types/constants from C API
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_globals API Types & Globals
 * @brief Types & constants bridged from C API.
 * 
 * \ingroup cpp_core
 * @{
 */

using world_t = ecs_world_t;
using world_info_t = ecs_world_info_t;
using query_group_info_t = ecs_query_group_info_t;
using id_t = ecs_id_t;
using entity_t = ecs_entity_t;
using type_t = ecs_type_t;
using table_t = ecs_table_t;
using filter_t = ecs_filter_t;
using observer_t = ecs_observer_t;
using query_t = ecs_query_t;
using rule_t = ecs_rule_t;
using ref_t = ecs_ref_t;
using iter_t = ecs_iter_t;
using type_info_t = ecs_type_info_t;
using type_hooks_t = ecs_type_hooks_t;
using flags32_t = ecs_flags32_t;

enum inout_kind_t {
    InOutDefault = EcsInOutDefault,
    InOutNone = EcsInOutNone,
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

/** Id flags */
static const flecs::entity_t Pair = ECS_PAIR;
static const flecs::entity_t Override = ECS_OVERRIDE;
static const flecs::entity_t Toggle = ECS_TOGGLE;

////////////////////////////////////////////////////////////////////////////////
//// Builtin components and tags 
////////////////////////////////////////////////////////////////////////////////

/* Builtin components */
using Component = EcsComponent;
using Identifier = EcsIdentifier;
using Poly = EcsPoly;

/* Builtin tags */
static const flecs::entity_t Query = EcsQuery;
static const flecs::entity_t Observer = EcsObserver;
static const flecs::entity_t Private = EcsPrivate;
static const flecs::entity_t Module = EcsModule;
static const flecs::entity_t Prefab = EcsPrefab;
static const flecs::entity_t Disabled = EcsDisabled;
static const flecs::entity_t Empty = EcsEmpty;
static const flecs::entity_t Monitor = EcsMonitor;
static const flecs::entity_t System = EcsSystem;
static const flecs::entity_t Pipeline = ecs_id(EcsPipeline);
static const flecs::entity_t Phase = EcsPhase;

/* Builtin event tags */
static const flecs::entity_t OnAdd = EcsOnAdd;
static const flecs::entity_t OnRemove = EcsOnRemove;
static const flecs::entity_t OnSet = EcsOnSet;
static const flecs::entity_t UnSet = EcsUnSet;

/* Builtin term flags */
static const uint32_t Self = EcsSelf;
static const uint32_t Up = EcsUp;
static const uint32_t Down = EcsDown;
static const uint32_t Cascade = EcsCascade;
static const uint32_t Parent = EcsParent;
static const uint32_t IsVariable = EcsIsVariable;
static const uint32_t IsEntity = EcsIsEntity;
static const uint32_t Filter = EcsFilter;
static const uint32_t TraverseFlags = EcsTraverseFlags;

/* Builtin entity ids */
static const flecs::entity_t Flecs = EcsFlecs;
static const flecs::entity_t FlecsCore = EcsFlecsCore;
static const flecs::entity_t World = EcsWorld;

/* Relationship properties */
static const flecs::entity_t Wildcard = EcsWildcard;
static const flecs::entity_t Any = EcsAny;
static const flecs::entity_t This = EcsThis;
static const flecs::entity_t Transitive = EcsTransitive;
static const flecs::entity_t Reflexive = EcsReflexive;
static const flecs::entity_t Final = EcsFinal;
static const flecs::entity_t DontInherit = EcsDontInherit;
static const flecs::entity_t Tag = EcsTag;
static const flecs::entity_t Union = EcsUnion;
static const flecs::entity_t Exclusive = EcsExclusive;
static const flecs::entity_t Acyclic = EcsAcyclic;
static const flecs::entity_t Symmetric = EcsSymmetric;
static const flecs::entity_t With = EcsWith;
static const flecs::entity_t OneOf = EcsOneOf;

/* Builtin relationships */
static const flecs::entity_t IsA = EcsIsA;
static const flecs::entity_t ChildOf = EcsChildOf;
static const flecs::entity_t DependsOn = EcsDependsOn;
static const flecs::entity_t SlotOf = EcsSlotOf;

/* Builtin identifiers */
static const flecs::entity_t Name = EcsName;
static const flecs::entity_t Symbol = EcsSymbol;

/* Cleanup policies */
static const flecs::entity_t OnDelete = EcsOnDelete;
static const flecs::entity_t OnDeleteTarget = EcsOnDeleteTarget;
static const flecs::entity_t Remove = EcsRemove;
static const flecs::entity_t Delete = EcsDelete;
static const flecs::entity_t Panic = EcsPanic;

/** @} */

}
