////////////////////////////////////////////////////////////////////////////////
//// Aliases for types/constants from C API
////////////////////////////////////////////////////////////////////////////////

namespace flecs {

using world_t = ecs_world_t;
using id_t = ecs_id_t;
using entity_t = ecs_entity_t;
using type_t = ecs_type_t;
using table_t = ecs_table_t;
using snapshot_t = ecs_snapshot_t;
using filter_t = ecs_filter_t;
using query_t = ecs_query_t;
using ref_t = ecs_ref_t;
using iter_t = ecs_iter_t;
using ComponentLifecycle = EcsComponentLifecycle;

enum inout_kind_t {
    InOutDefault = EcsInOutDefault,
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

enum var_kind_t {
    VarDefault = EcsVarDefault,
    VarIsEntity = EcsVarIsEntity,
    VarIsVariable = EcsVarIsVariable
};

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

/* Doc components */
namespace doc {
    using Description = EcsDocDescription;
    static const flecs::entity_t Brief = EcsDocBrief;
    static const flecs::entity_t Detail = EcsDocDetail;
    static const flecs::entity_t Link = EcsDocLink;
}

/* REST components */
namespace rest {
    using Rest = EcsRest;
}

/* Builtin set constants */
static const uint8_t DefaultSet = EcsDefaultSet;
static const uint8_t Self = EcsSelf;
static const uint8_t SuperSet = EcsSuperSet;
static const uint8_t SubSet = EcsSubSet;
static const uint8_t Cascade = EcsCascade;
static const uint8_t All = EcsAll;
static const uint8_t Nothing = EcsNothing;
static const uint8_t Parent = EcsParent;

/* Builtin tag ids */
static const flecs::entity_t Module = EcsModule;
static const flecs::entity_t Prefab = EcsPrefab;
static const flecs::entity_t Disabled = EcsDisabled;
static const flecs::entity_t Inactive = EcsInactive;
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
static const flecs::entity_t Owned = ECS_OVERRIDE;

/* Builtin entity ids */
static const flecs::entity_t Flecs = EcsFlecs;
static const flecs::entity_t FlecsCore = EcsFlecsCore;
static const flecs::entity_t World = EcsWorld;

/* Relation properties */
static const flecs::entity_t Wildcard = EcsWildcard;
static const flecs::entity_t This = EcsThis;
static const flecs::entity_t Transitive = EcsTransitive;
static const flecs::entity_t TransitiveSelf = EcsTransitiveSelf;
static const flecs::entity_t Final = EcsFinal;
static const flecs::entity_t Tag = EcsTag;
static const flecs::entity_t Exclusive = EcsExclusive;

/* Builtin relationships */
static const flecs::entity_t IsA = EcsIsA;
static const flecs::entity_t ChildOf = EcsChildOf;

/* Builtin identifiers */
static const flecs::entity_t Name = EcsName;
static const flecs::entity_t Symbol = EcsSymbol;

/* Cleanup policies */
static const flecs::entity_t OnDelete = EcsOnDelete;
static const flecs::entity_t OnDeleteObject = EcsOnDeleteObject;
static const flecs::entity_t Remove = EcsRemove;
static const flecs::entity_t Delete = EcsDelete;
static const flecs::entity_t Throw = EcsThrow;

}
