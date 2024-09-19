/**
 * @file addons/cpp/c_types.hpp
 * @brief Aliases for types/constants from C API
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_globals API Types & Globals
 * @ingroup cpp_core
 * Types & constants bridged from C API.
 *
 * @{
 */

using world_t = ecs_world_t;
using world_info_t = ecs_world_info_t;
using id_t = ecs_id_t;
using entity_t = ecs_entity_t;
using type_t = ecs_type_t;
using table_t = ecs_table_t;
using term_t = ecs_term_t;
using query_t = ecs_query_t;
using query_group_info_t = ecs_query_group_info_t;
using observer_t = ecs_observer_t;
using iter_t = ecs_iter_t;
using ref_t = ecs_ref_t;
using type_info_t = ecs_type_info_t;
using type_hooks_t = ecs_type_hooks_t;
using flags32_t = ecs_flags32_t;

enum inout_kind_t {
    InOutDefault = EcsInOutDefault,
    InOutNone = EcsInOutNone,
    InOutFilter = EcsInOutFilter,
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

enum query_cache_kind_t {
    QueryCacheDefault = EcsQueryCacheDefault,
    QueryCacheAuto = EcsQueryCacheAuto,
    QueryCacheAll = EcsQueryCacheAll,
    QueryCacheNone = EcsQueryCacheNone
};

/** Id bit flags */
static const flecs::entity_t PAIR = ECS_PAIR;
static const flecs::entity_t AUTO_OVERRIDE = ECS_AUTO_OVERRIDE;
static const flecs::entity_t TOGGLE = ECS_TOGGLE;

////////////////////////////////////////////////////////////////////////////////
//// Builtin components and tags
////////////////////////////////////////////////////////////////////////////////

/* Builtin components */
using Component = EcsComponent;
using Identifier = EcsIdentifier;
using Poly = EcsPoly;
using DefaultChildComponent = EcsDefaultChildComponent;

/* Builtin tags */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Query = EcsQuery;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Observer = EcsObserver;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Private = EcsPrivate;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Module = EcsModule;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Prefab = EcsPrefab;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Disabled = EcsDisabled;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Empty = EcsEmpty;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Monitor = EcsMonitor;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t System = EcsSystem;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Pipeline = ecs_id(EcsPipeline);
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Phase = EcsPhase;

/* Builtin event tags */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t OnAdd = EcsOnAdd;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t OnRemove = EcsOnRemove;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t OnSet = EcsOnSet;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t OnTableCreate = EcsOnTableCreate;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t OnTableDelete = EcsOnTableDelete;

/* Builtin term flags */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const uint64_t Self = EcsSelf;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const uint64_t Up = EcsUp;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const uint64_t Trav = EcsTrav;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const uint64_t Cascade = EcsCascade;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const uint64_t Desc = EcsDesc;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const uint64_t IsVariable = EcsIsVariable;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const uint64_t IsEntity = EcsIsEntity;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const uint64_t IsName = EcsIsName;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const uint64_t TraverseFlags = EcsTraverseFlags;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const uint64_t TermRefFlags = EcsTermRefFlags;

/* Builtin entity ids */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Flecs = EcsFlecs;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t FlecsCore = EcsFlecsCore;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t World = EcsWorld;

/* Component traits */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Wildcard = EcsWildcard;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Any = EcsAny;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t This = EcsThis;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Transitive = EcsTransitive;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Reflexive = EcsReflexive;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Final = EcsFinal;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t PairIsTag = EcsPairIsTag;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Exclusive = EcsExclusive;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Acyclic = EcsAcyclic;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Traversable = EcsTraversable;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Symmetric = EcsSymmetric;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t With = EcsWith;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t OneOf = EcsOneOf;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Trait = EcsTrait;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Relationship = EcsRelationship;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Target = EcsTarget;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t CanToggle = EcsCanToggle;

/* OnInstantiate trait */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t OnInstantiate = EcsOnInstantiate;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Override = EcsOverride;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Inherit = EcsInherit;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t DontInherit = EcsDontInherit;

/* OnDelete/OnDeleteTarget traits */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t OnDelete = EcsOnDelete;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t OnDeleteTarget = EcsOnDeleteTarget;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Remove = EcsRemove;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Delete = EcsDelete;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Panic = EcsPanic;

/* Builtin relationships */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t IsA = EcsIsA;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t ChildOf = EcsChildOf;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t DependsOn = EcsDependsOn;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t SlotOf = EcsSlotOf;

/* Builtin identifiers */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Name = EcsName;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Symbol = EcsSymbol;

/* Storage */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Sparse = EcsSparse;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t Union = EcsUnion;

/* Builtin predicates for comparing entity ids in queries. */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t PredEq = EcsPredEq;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t PredMatch = EcsPredMatch;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t PredLookup = EcsPredLookup;

/* Builtin marker entities for query scopes */
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t ScopeOpen = EcsScopeOpen;
FLECS_API_GLOBAL FLECS_STATIC_IN_HEADER const flecs::entity_t ScopeClose = EcsScopeClose;

/** @} */

}
