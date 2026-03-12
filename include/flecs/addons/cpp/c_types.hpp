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

using world_t = ecs_world_t;                         /**< World type. */
using world_info_t = ecs_world_info_t;               /**< World info type. */
using id_t = ecs_id_t;                               /**< Id type. */
using entity_t = ecs_entity_t;                       /**< Entity type. */
using type_t = ecs_type_t;                           /**< Type type. */
using table_t = ecs_table_t;                         /**< Table type. */
using term_t = ecs_term_t;                           /**< Term type. */
using query_t = ecs_query_t;                         /**< Query type. */
using query_group_info_t = ecs_query_group_info_t;   /**< Query group info type. */
using observer_t = ecs_observer_t;                   /**< Observer type. */
using iter_t = ecs_iter_t;                           /**< Iterator type. */
using ref_t = ecs_ref_t;                             /**< Ref type. */
using table_record_t = ecs_table_record_t;           /**< Table record type. */
using table_records_t = ecs_table_records_t;         /**< Table records type. */
using component_record_t = ecs_component_record_t;   /**< Component record type. */
using type_info_t = ecs_type_info_t;                 /**< Type info type. */
using type_hooks_t = ecs_type_hooks_t;               /**< Type hooks type. */
using flags32_t = ecs_flags32_t;                     /**< 32-bit flags type. */
using flags64_t = ecs_flags64_t;                     /**< 64-bit flags type. */

/** Inout kind. */
enum inout_kind_t {
    InOutDefault = EcsInOutDefault,  /**< InOutDefault. */
    InOutNone = EcsInOutNone,        /**< InOutNone. */
    InOutFilter = EcsInOutFilter,    /**< InOutFilter. */
    InOut = EcsInOut,                /**< InOut. */
    In = EcsIn,                      /**< In. */
    Out = EcsOut                     /**< Out. */
};

/** Operator kind. */
enum oper_kind_t {
    And = EcsAnd,                    /**< And operator. */
    Or = EcsOr,                      /**< Or operator. */
    Not = EcsNot,                    /**< Not operator. */
    Optional = EcsOptional,          /**< Optional operator. */
    AndFrom = EcsAndFrom,            /**< AndFrom operator. */
    OrFrom = EcsOrFrom,              /**< OrFrom operator. */
    NotFrom = EcsNotFrom             /**< NotFrom operator. */
};

/** Query cache kind. */
enum query_cache_kind_t {
    QueryCacheDefault = EcsQueryCacheDefault, /**< Default query cache. */
    QueryCacheAuto = EcsQueryCacheAuto,       /**< Auto query cache. */
    QueryCacheAll = EcsQueryCacheAll,         /**< Cache all. */
    QueryCacheNone = EcsQueryCacheNone        /**< No caching. */
};

/** Id bit flags. */
static const flecs::entity_t PAIR = ECS_PAIR;                   /**< Pair flag. */
static const flecs::entity_t AUTO_OVERRIDE = ECS_AUTO_OVERRIDE; /**< Auto override flag. */
static const flecs::entity_t TOGGLE = ECS_TOGGLE;               /**< Toggle flag. */

////////////////////////////////////////////////////////////////////////////////
//// Builtin components and tags
////////////////////////////////////////////////////////////////////////////////

/** Builtin EcsComponent type. */
using Component = EcsComponent;
/** Builtin EcsIdentifier type. */
using Identifier = EcsIdentifier;
/** Builtin EcsPoly type. */
using Poly = EcsPoly;
/** Builtin EcsDefaultChildComponent type. */
using DefaultChildComponent = EcsDefaultChildComponent;
/** Builtin EcsParent type. */
using Parent = EcsParent;

/** Builtin Query tag. */
static const flecs::entity_t Query = EcsQuery;
/** Builtin Observer tag. */
static const flecs::entity_t Observer = EcsObserver;
/** Builtin Module tag. */
static const flecs::entity_t Module = EcsModule;
/** Builtin Prefab tag. */
static const flecs::entity_t Prefab = EcsPrefab;
/** Builtin Disabled tag. */
static const flecs::entity_t Disabled = EcsDisabled;
/** Builtin Empty tag. */
static const flecs::entity_t Empty = EcsEmpty;
/** Builtin Monitor tag. */
static const flecs::entity_t Monitor = EcsMonitor;
/** Builtin System tag. */
static const flecs::entity_t System = EcsSystem;
/** Builtin Pipeline tag. */
static const flecs::entity_t Pipeline = ecs_id(EcsPipeline);
/** Builtin Phase tag. */
static const flecs::entity_t Phase = EcsPhase;
/** Builtin Constant tag. */
static const flecs::entity_t Constant = EcsConstant;
/** Builtin ParentDepth tag. */
static const flecs::entity_t ParentDepth = EcsParentDepth;

/** Builtin OnAdd event. */
static const flecs::entity_t OnAdd = EcsOnAdd;
/** Builtin OnRemove event. */
static const flecs::entity_t OnRemove = EcsOnRemove;
/** Builtin OnSet event. */
static const flecs::entity_t OnSet = EcsOnSet;
/** Builtin OnTableCreate event. */
static const flecs::entity_t OnTableCreate = EcsOnTableCreate;
/** Builtin OnTableDelete event. */
static const flecs::entity_t OnTableDelete = EcsOnTableDelete;

/** Self term flag. */
static const uint64_t Self = EcsSelf;
/** Up term flag. */
static const uint64_t Up = EcsUp;
/** Trav term flag. */
static const uint64_t Trav = EcsTrav;
/** Cascade term flag. */
static const uint64_t Cascade = EcsCascade;
/** Desc term flag. */
static const uint64_t Desc = EcsDesc;
/** IsVariable term flag. */
static const uint64_t IsVariable = EcsIsVariable;
/** IsEntity term flag. */
static const uint64_t IsEntity = EcsIsEntity;
/** IsName term flag. */
static const uint64_t IsName = EcsIsName;
/** TraverseFlags term flag mask. */
static const uint64_t TraverseFlags = EcsTraverseFlags;
/** TermRefFlags term flag mask. */
static const uint64_t TermRefFlags = EcsTermRefFlags;

/** Builtin Flecs entity. */
static const flecs::entity_t Flecs = EcsFlecs;
/** Builtin FlecsCore entity. */
static const flecs::entity_t FlecsCore = EcsFlecsCore;
/** Builtin World entity. */
static const flecs::entity_t World = EcsWorld;

/** Wildcard entity. */
static const flecs::entity_t Wildcard = EcsWildcard;
/** Any entity. */
static const flecs::entity_t Any = EcsAny;
/** This variable entity. */
static const flecs::entity_t This = EcsThis;
/** Transitive trait. */
static const flecs::entity_t Transitive = EcsTransitive;
/** Reflexive trait. */
static const flecs::entity_t Reflexive = EcsReflexive;
/** Final trait. */
static const flecs::entity_t Final = EcsFinal;
/** Inheritable trait. */
static const flecs::entity_t Inheritable = EcsInheritable;
/** PairIsTag trait. */
static const flecs::entity_t PairIsTag = EcsPairIsTag;
/** Exclusive trait. */
static const flecs::entity_t Exclusive = EcsExclusive;
/** Acyclic trait. */
static const flecs::entity_t Acyclic = EcsAcyclic;
/** Traversable trait. */
static const flecs::entity_t Traversable = EcsTraversable;
/** Symmetric trait. */
static const flecs::entity_t Symmetric = EcsSymmetric;
/** With trait. */
static const flecs::entity_t With = EcsWith;
/** OneOf trait. */
static const flecs::entity_t OneOf = EcsOneOf;
/** Trait tag. */
static const flecs::entity_t Trait = EcsTrait;
/** Relationship tag. */
static const flecs::entity_t Relationship = EcsRelationship;
/** Target tag. */
static const flecs::entity_t Target = EcsTarget;
/** CanToggle trait. */
static const flecs::entity_t CanToggle = EcsCanToggle;

/** OnInstantiate trait. */
static const flecs::entity_t OnInstantiate = EcsOnInstantiate;
/** Override trait. */
static const flecs::entity_t Override = EcsOverride;
/** Inherit trait. */
static const flecs::entity_t Inherit = EcsInherit;
/** DontInherit trait. */
static const flecs::entity_t DontInherit = EcsDontInherit;

/** OnDelete cleanup trait. */
static const flecs::entity_t OnDelete = EcsOnDelete;
/** OnDeleteTarget cleanup trait. */
static const flecs::entity_t OnDeleteTarget = EcsOnDeleteTarget;
/** Remove cleanup action. */
static const flecs::entity_t Remove = EcsRemove;
/** Delete cleanup action. */
static const flecs::entity_t Delete = EcsDelete;
/** Panic cleanup action. */
static const flecs::entity_t Panic = EcsPanic;

/** IsA relationship. */
static const flecs::entity_t IsA = EcsIsA;
/** ChildOf relationship. */
static const flecs::entity_t ChildOf = EcsChildOf;
/** DependsOn relationship. */
static const flecs::entity_t DependsOn = EcsDependsOn;
/** SlotOf relationship. */
static const flecs::entity_t SlotOf = EcsSlotOf;

/** OrderedChildren tag. */
static const flecs::entity_t OrderedChildren = EcsOrderedChildren;
/** Singleton tag. */
static const flecs::entity_t Singleton = EcsSingleton;

/** Name identifier. */
static const flecs::entity_t Name = EcsName;
/** Symbol identifier. */
static const flecs::entity_t Symbol = EcsSymbol;

/** Sparse storage tag. */
static const flecs::entity_t Sparse = EcsSparse;
/** DontFragment storage tag. */
static const flecs::entity_t DontFragment = EcsDontFragment;

/** PredEq query predicate. */
static const flecs::entity_t PredEq = EcsPredEq;
/** PredMatch query predicate. */
static const flecs::entity_t PredMatch = EcsPredMatch;
/** PredLookup query predicate. */
static const flecs::entity_t PredLookup = EcsPredLookup;

/** ScopeOpen query scope marker. */
static const flecs::entity_t ScopeOpen = EcsScopeOpen;
/** ScopeClose query scope marker. */
static const flecs::entity_t ScopeClose = EcsScopeClose;

/** @} */

}
