/**
 * @file api_flags.h
 * @brief Bitset flags used by internals.
 */

#ifndef FLECS_API_FLAGS_H
#define FLECS_API_FLAGS_H

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// World flags
////////////////////////////////////////////////////////////////////////////////

INLINE static CONSTEXPR uint32 EcsWorldQuitWorkers = 1u << 0;
INLINE static CONSTEXPR uint32 EcsWorldReadonly = 1u << 1;
INLINE static CONSTEXPR uint32 EcsWorldInit = 1u << 2;
INLINE static CONSTEXPR uint32 EcsWorldQuit = 1u << 3;
INLINE static CONSTEXPR uint32 EcsWorldFini = 1u << 4;
INLINE static CONSTEXPR uint32 EcsWorldMeasureFrameTime = 1u << 5;
INLINE static CONSTEXPR uint32 EcsWorldMeasureSystemTime = 1u << 6;
INLINE static CONSTEXPR uint32 EcsWorldMultiThreaded = 1u << 7;


////////////////////////////////////////////////////////////////////////////////
//// OS API flags
////////////////////////////////////////////////////////////////////////////////

INLINE static CONSTEXPR uint32 EcsOsApiHighResolutionTimer = 1u << 0;
INLINE static CONSTEXPR uint32 EcsOsApiLogWithColors = 1u << 1;
INLINE static CONSTEXPR uint32 EcsOsApiLogWithTimeStamp = 1u << 2;
INLINE static CONSTEXPR uint32 EcsOsApiLogWithTimeDelta = 1u << 3;


////////////////////////////////////////////////////////////////////////////////
//// Entity flags (set in upper bits of ecs_record_t::row)
////////////////////////////////////////////////////////////////////////////////

INLINE static CONSTEXPR uint32 EcsEntityIsId = 1u << 31;
INLINE static CONSTEXPR uint32 EcsEntityIsTarget = 1u << 30;
INLINE static CONSTEXPR uint32 EcsEntityIsTraversable = 1u << 29;

////////////////////////////////////////////////////////////////////////////////
//// Id flags (used by ecs_id_record_t::flags)
////////////////////////////////////////////////////////////////////////////////

INLINE static CONSTEXPR uint32 EcsIdOnDeleteRemove = 1u << 0;
INLINE static CONSTEXPR uint32 EcsIdOnDeleteDelete = 1u << 1;
INLINE static CONSTEXPR uint32 EcsIdOnDeletePanic = 1u << 2;

INLINE static CONSTEXPR uint32 EcsIdOnDeleteMask = EcsIdOnDeletePanic | EcsIdOnDeleteRemove | EcsIdOnDeleteDelete;

INLINE static CONSTEXPR uint32 EcsIdOnDeleteObjectRemove = 1u << 3;
INLINE static CONSTEXPR uint32 EcsIdOnDeleteObjectDelete = 1u << 4;
INLINE static CONSTEXPR uint32 EcsIdOnDeleteObjectPanic = 1u << 5;
INLINE static CONSTEXPR uint32 EcsIdOnDeleteObjectMask = EcsIdOnDeleteObjectPanic
        | EcsIdOnDeleteObjectRemove | EcsIdOnDeleteObjectDelete;

INLINE static CONSTEXPR uint32 EcsIdExclusive = 1u << 6;
INLINE static CONSTEXPR uint32 EcsIdDontInherit = 1u << 7;
INLINE static CONSTEXPR uint32 EcsIdTraversable = 1u << 8;
INLINE static CONSTEXPR uint32 EcsIdTag = 1u << 9;
INLINE static CONSTEXPR uint32 EcsIdWith = 1u << 10;
INLINE static CONSTEXPR uint32 EcsIdUnion = 1u << 11;
INLINE static CONSTEXPR uint32 EcsIdAlwaysOverride = 1u << 12;

INLINE static CONSTEXPR uint32 EcsIdHasOnAdd = 1u << 16;
INLINE static CONSTEXPR uint32 EcsIdHasOnRemove = 1u << 17;
INLINE static CONSTEXPR uint32 EcsIdHasOnSet = 1u << 18;
INLINE static CONSTEXPR uint32 EcsIdHasUnSet = 1u << 19;
INLINE static CONSTEXPR uint32 EcsIdHasOnTableFill = 1u << 20;
INLINE static CONSTEXPR uint32 EcsIdHasOnTableEmpty = 1u << 21;
INLINE static CONSTEXPR uint32 EcsIdHasOnTableCreate = 1u << 22;
INLINE static CONSTEXPR uint32 EcsIdHasOnTableDelete = 1u << 23;
INLINE static CONSTEXPR uint32 EcsIdEventMask
	= EcsIdHasOnAdd | EcsIdHasOnRemove | EcsIdHasOnSet | EcsIdHasUnSet
		| EcsIdHasOnTableFill | EcsIdHasOnTableEmpty | EcsIdHasOnTableCreate | EcsIdHasOnTableDelete;

INLINE static CONSTEXPR uint32 EcsIdMarkedForDelete = 1u << 30;


/* Utilities for converting from flags to delete policies and vice versa */
#define ECS_ID_ON_DELETE(flags) \
    ((ecs_entity_t[]){0, EcsRemove, EcsDelete, 0, EcsPanic}\
        [((flags) & EcsIdOnDeleteMask)])
#define ECS_ID_ON_DELETE_TARGET(flags) ECS_ID_ON_DELETE(flags >> 3)
#define ECS_ID_ON_DELETE_FLAG(id) (1u << ((id) - EcsRemove))
#define ECS_ID_ON_DELETE_TARGET_FLAG(id) (1u << (3 + ((id) - EcsRemove)))


////////////////////////////////////////////////////////////////////////////////
//// Iterator flags (used by ecs_iter_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsIterIsValid                 (1u << 0u)  /* Does iterator contain valid result */
#define EcsIterNoData                  (1u << 1u)  /* Does iterator provide (component) data */
#define EcsIterIsInstanced             (1u << 2u)  /* Is iterator instanced */
#define EcsIterHasShared               (1u << 3u)  /* Does result have shared terms */
#define EcsIterTableOnly               (1u << 4u)  /* Result only populates table */
#define EcsIterEntityOptional          (1u << 5u)  /* Treat terms with entity subject as optional */
#define EcsIterNoResults               (1u << 6u)  /* Iterator has no results */
#define EcsIterIgnoreThis              (1u << 7u)  /* Only evaluate non-this terms */
#define EcsIterMatchVar                (1u << 8u)  
#define EcsIterHasCondSet              (1u << 10u) /* Does iterator have conditionally set fields */
#define EcsIterProfile                 (1u << 11u) /* Profile iterator performance */
#define EcsIterTrivialSearch           (1u << 12u) /* Trivial iterator mode */
#define EcsIterTrivialSearchNoData     (1u << 13u) /* Trivial iterator w/no data */
#define EcsIterTrivialTest             (1u << 14u) /* Trivial test mode (constrained $this) */
#define EcsIterTrivialSearchWildcard   (1u << 15u) /* Trivial search with wildcard ids */

////////////////////////////////////////////////////////////////////////////////
//// Event flags (used by ecs_event_decs_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsEventTableOnly              (1u << 4u)   /* Table event (no data, same as iter flags) */
#define EcsEventNoOnSet                (1u << 16u)  /* Don't emit OnSet/UnSet for inherited ids */

////////////////////////////////////////////////////////////////////////////////
//// Filter flags (used by ecs_filter_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsFilterMatchThis             (1u << 1u)  /* Has terms that match This */
#define EcsFilterMatchOnlyThis         (1u << 2u)  /* Has only terms that match This */
#define EcsFilterMatchPrefab           (1u << 3u)  /* Does filter match prefabs */
#define EcsFilterMatchDisabled         (1u << 4u)  /* Does filter match disabled entities */
#define EcsFilterMatchEmptyTables      (1u << 5u)  /* Does filter return empty tables */
#define EcsFilterMatchAnything         (1u << 6u)  /* False if filter has no/only Not terms */
#define EcsFilterNoData                (1u << 7u)  /* When true, data fields won't be populated */
#define EcsFilterIsInstanced           (1u << 8u)  /* Is filter instanced (see ecs_filter_desc_t) */
#define EcsFilterPopulate              (1u << 9u)  /* Populate data, ignore non-matching fields */
#define EcsFilterHasCondSet            (1u << 10u) /* Does filter have conditionally set fields */
#define EcsFilterUnresolvedByName      (1u << 11u) /* Use by-name matching for unresolved entity identifiers */
#define EcsFilterHasPred               (1u << 12u) /* Filter has equality predicates */
#define EcsFilterHasScopes             (1u << 13u) /* Filter has query scopes */
#define EcsFilterIsTrivial             (1u << 14u) /* Trivial filter */
#define EcsFilterMatchOnlySelf         (1u << 15u) /* Filter has no up traversal */
#define EcsFilterHasWildcards          (1u << 16u) /* Filter has no up traversal */
#define EcsFilterOwnsStorage           (1u << 17u) /* Is ecs_filter_t object owned by filter */
#define EcsFilterOwnsTermsStorage      (1u << 18u) /* Is terms array owned by filter */

////////////////////////////////////////////////////////////////////////////////
//// Table flags (used by ecs_table_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsTableHasBuiltins            (1u << 1u)  /* Does table have builtin components */
#define EcsTableIsPrefab               (1u << 2u)  /* Does the table store prefabs */
#define EcsTableHasIsA                 (1u << 3u)  /* Does the table have IsA relationship */
#define EcsTableHasChildOf             (1u << 4u)  /* Does the table type ChildOf relationship */
#define EcsTableHasName                (1u << 5u)  /* Does the table type have (Identifier, Name) */
#define EcsTableHasPairs               (1u << 6u)  /* Does the table type have pairs */
#define EcsTableHasModule              (1u << 7u)  /* Does the table have module data */
#define EcsTableIsDisabled             (1u << 8u)  /* Does the table type has EcsDisabled */
#define EcsTableHasCtors               (1u << 9u)
#define EcsTableHasDtors               (1u << 10u)
#define EcsTableHasCopy                (1u << 11u)
#define EcsTableHasMove                (1u << 12u)
#define EcsTableHasUnion               (1u << 13u)
#define EcsTableHasToggle              (1u << 14u)
#define EcsTableHasOverrides           (1u << 15u)

#define EcsTableHasOnAdd               (1u << 16u) /* Same values as id flags */
#define EcsTableHasOnRemove            (1u << 17u)
#define EcsTableHasOnSet               (1u << 18u)
#define EcsTableHasUnSet               (1u << 19u)
#define EcsTableHasOnTableFill         (1u << 20u)
#define EcsTableHasOnTableEmpty        (1u << 21u)
#define EcsTableHasOnTableCreate       (1u << 22u)
#define EcsTableHasOnTableDelete       (1u << 23u)

#define EcsTableHasTraversable         (1u << 25u)
#define EcsTableHasTarget              (1u << 26u)

#define EcsTableMarkedForDelete        (1u << 30u)

/* Composite table flags */
#define EcsTableHasLifecycle        (EcsTableHasCtors | EcsTableHasDtors)
#define EcsTableIsComplex           (EcsTableHasLifecycle | EcsTableHasUnion | EcsTableHasToggle)
#define EcsTableHasAddActions       (EcsTableHasIsA | EcsTableHasUnion | EcsTableHasCtors | EcsTableHasOnAdd | EcsTableHasOnSet)
#define EcsTableHasRemoveActions    (EcsTableHasIsA | EcsTableHasDtors | EcsTableHasOnRemove | EcsTableHasUnSet)


////////////////////////////////////////////////////////////////////////////////
//// Query flags (used by ecs_query_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsQueryHasRefs                (1u << 1u)  /* Does query have references */
#define EcsQueryIsSubquery             (1u << 2u)  /* Is query a subquery */
#define EcsQueryIsOrphaned             (1u << 3u)  /* Is subquery orphaned */
#define EcsQueryHasOutTerms            (1u << 4u)  /* Does query have out terms */
#define EcsQueryHasNonThisOutTerms     (1u << 5u)  /* Does query have non-this out terms */
#define EcsQueryHasMonitor             (1u << 6u)  /* Does query track changes */
#define EcsQueryTrivialIter            (1u << 7u)  /* Does the query require special features to iterate */


////////////////////////////////////////////////////////////////////////////////
//// Aperiodic action flags (used by ecs_run_aperiodic)
////////////////////////////////////////////////////////////////////////////////

#define EcsAperiodicEmptyTables        (1u << 1u)  /* Process pending empty table events */
#define EcsAperiodicComponentMonitors  (1u << 2u)  /* Process component monitors */
#define EcsAperiodicEmptyQueries       (1u << 4u)  /* Process empty queries */

#ifdef __cplusplus
}
#endif

#endif
