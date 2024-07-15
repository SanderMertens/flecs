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

#define EcsWorldQuitWorkers           (1u << 0)
#define EcsWorldReadonly              (1u << 1)
#define EcsWorldInit                  (1u << 2)
#define EcsWorldQuit                  (1u << 3)
#define EcsWorldFini                  (1u << 4)
#define EcsWorldMeasureFrameTime      (1u << 5)
#define EcsWorldMeasureSystemTime     (1u << 6)
#define EcsWorldMultiThreaded         (1u << 7)


////////////////////////////////////////////////////////////////////////////////
//// OS API flags
////////////////////////////////////////////////////////////////////////////////

#define EcsOsApiHighResolutionTimer   (1u << 0)
#define EcsOsApiLogWithColors         (1u << 1)
#define EcsOsApiLogWithTimeStamp      (1u << 2)
#define EcsOsApiLogWithTimeDelta      (1u << 3)


////////////////////////////////////////////////////////////////////////////////
//// Entity flags (set in upper bits of ecs_record_t::row)
////////////////////////////////////////////////////////////////////////////////

#define EcsEntityIsId                 (1u << 31)
#define EcsEntityIsTarget             (1u << 30)
#define EcsEntityIsTraversable        (1u << 29)


////////////////////////////////////////////////////////////////////////////////
//// Id flags (used by ecs_id_record_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsIdOnDeleteRemove            (1u << 0)
#define EcsIdOnDeleteDelete            (1u << 1)
#define EcsIdOnDeletePanic             (1u << 2)
#define EcsIdOnDeleteMask\
    (EcsIdOnDeletePanic|EcsIdOnDeleteRemove|EcsIdOnDeleteDelete)

#define EcsIdOnDeleteObjectRemove      (1u << 3)
#define EcsIdOnDeleteObjectDelete      (1u << 4)
#define EcsIdOnDeleteObjectPanic       (1u << 5)
#define EcsIdOnDeleteObjectMask\
    (EcsIdOnDeleteObjectPanic|EcsIdOnDeleteObjectRemove|\
        EcsIdOnDeleteObjectDelete)

#define EcsIdOnInstantiateOverride     (1u << 6)
#define EcsIdOnInstantiateInherit      (1u << 7)
#define EcsIdOnInstantiateDontInherit  (1u << 8)
#define EcsIdOnInstantiateMask\
    (EcsIdOnInstantiateOverride|EcsIdOnInstantiateInherit|\
        EcsIdOnInstantiateDontInherit)

#define EcsIdExclusive                 (1u << 9)
#define EcsIdTraversable               (1u << 10)
#define EcsIdTag                       (1u << 11)
#define EcsIdWith                      (1u << 12)
#define EcsIdCanToggle                 (1u << 13)

#define EcsIdHasOnAdd                  (1u << 16) /* Same values as table flags */
#define EcsIdHasOnRemove               (1u << 17) 
#define EcsIdHasOnSet                  (1u << 18)
#define EcsIdHasOnTableFill            (1u << 20)
#define EcsIdHasOnTableEmpty           (1u << 21)
#define EcsIdHasOnTableCreate          (1u << 22)
#define EcsIdHasOnTableDelete          (1u << 23)
#define EcsIdIsSparse                  (1u << 24)
#define EcsIdIsUnion                   (1u << 25)
#define EcsIdEventMask\
    (EcsIdHasOnAdd|EcsIdHasOnRemove|EcsIdHasOnSet|\
        EcsIdHasOnTableFill|EcsIdHasOnTableEmpty|EcsIdHasOnTableCreate|\
            EcsIdHasOnTableDelete|EcsIdIsSparse|EcsIdIsUnion)

#define EcsIdMarkedForDelete           (1u << 30)

/* Utilities for converting from flags to delete policies and vice versa */
#define ECS_ID_ON_DELETE(flags) \
    ((ecs_entity_t[]){0, EcsRemove, EcsDelete, 0, EcsPanic}\
        [((flags) & EcsIdOnDeleteMask)])
#define ECS_ID_ON_DELETE_TARGET(flags) ECS_ID_ON_DELETE(flags >> 3)
#define ECS_ID_ON_DELETE_FLAG(id) (1u << ((id) - EcsRemove))
#define ECS_ID_ON_DELETE_TARGET_FLAG(id) (1u << (3 + ((id) - EcsRemove)))

/* Utilities for converting from flags to instantiate policies and vice versa */
#define ECS_ID_ON_INSTANTIATE(flags) \
    ((ecs_entity_t[]){EcsOverride, EcsOverride, EcsInherit, 0, EcsDontInherit}\
        [(((flags) & EcsIdOnInstantiateMask) >> 6)])
#define ECS_ID_ON_INSTANTIATE_FLAG(id) (1u << (6 + ((id) - EcsOverride)))


////////////////////////////////////////////////////////////////////////////////
//// Iterator flags (used by ecs_iter_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsIterIsValid                 (1u << 0u)  /* Does iterator contain valid result */
#define EcsIterNoData                  (1u << 1u)  /* Does iterator provide (component) data */
#define EcsIterIsInstanced             (1u << 2u)  /* Is iterator instanced */
#define EcsIterNoResults               (1u << 3u)  /* Iterator has no results */
#define EcsIterIgnoreThis              (1u << 4u)  /* Only evaluate non-this terms */
#define EcsIterHasCondSet              (1u << 6u)  /* Does iterator have conditionally set fields */
#define EcsIterProfile                 (1u << 7u)  /* Profile iterator performance */
#define EcsIterTrivialSearch           (1u << 8u)  /* Trivial iterator mode */
#define EcsIterTrivialSearchNoData     (1u << 9u)  /* Trivial iterator w/no data */
#define EcsIterTrivialTest             (1u << 10u) /* Trivial test mode (constrained $this) */
#define EcsIterTrivialTestWildcard     (1u << 11u) /* Trivial test w/wildcards */
#define EcsIterTrivialSearchWildcard   (1u << 12u) /* Trivial search with wildcard ids */
#define EcsIterCacheSearch             (1u << 13u) /* Cache search */
#define EcsIterFixedInChangeComputed   (1u << 14u) /* Change detection for fixed in terms is done */
#define EcsIterFixedInChanged          (1u << 15u) /* Fixed in terms changed */
#define EcsIterSkip                    (1u << 16u) /* Result was skipped for change detection */
#define EcsIterCppEach                 (1u << 17u) /* Uses C++ 'each' iterator */

/* Same as event flags */
#define EcsIterTableOnly               (1u << 18u)  /* Result only populates table */


////////////////////////////////////////////////////////////////////////////////
//// Event flags (used by ecs_event_decs_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsEventTableOnly              (1u << 18u) /* Table event (no data, same as iter flags) */
#define EcsEventNoOnSet                (1u << 16u) /* Don't emit OnSet for inherited ids */


////////////////////////////////////////////////////////////////////////////////
//// Query flags (used by ecs_query_t::flags)
////////////////////////////////////////////////////////////////////////////////

/* Flags that can only be set by the query implementation */
#define EcsQueryMatchThis             (1u << 11u) /* Query has terms with $this source */
#define EcsQueryMatchOnlyThis         (1u << 12u) /* Query only has terms with $this source */
#define EcsQueryMatchOnlySelf         (1u << 13u) /* Query has no terms with up traversal */
#define EcsQueryMatchWildcards        (1u << 14u) /* Query matches wildcards */
#define EcsQueryHasCondSet            (1u << 15u) /* Query has conditionally set fields */
#define EcsQueryHasPred               (1u << 16u) /* Query has equality predicates */
#define EcsQueryHasScopes             (1u << 17u) /* Query has query scopes */
#define EcsQueryHasRefs               (1u << 18u) /* Query has terms with static source */
#define EcsQueryHasOutTerms           (1u << 19u) /* Query has [out] terms */
#define EcsQueryHasNonThisOutTerms    (1u << 20u) /* Query has [out] terms with no $this source */
#define EcsQueryHasMonitor            (1u << 21u) /* Query has monitor for change detection */
#define EcsQueryIsTrivial             (1u << 22u) /* Query can use trivial evaluation function */
#define EcsQueryHasCacheable          (1u << 23u) /* Query has cacheable terms */
#define EcsQueryIsCacheable           (1u << 24u) /* All terms of query are cacheable */
#define EcsQueryHasTableThisVar       (1u << 25u) /* Does query have $this table var */
#define EcsQueryHasSparseThis         (1u << 26u) /* Does query have $this sparse fields */
#define EcsQueryCacheYieldEmptyTables      (1u << 27u) /* Does query cache empty tables */

////////////////////////////////////////////////////////////////////////////////
//// Term flags (used by ecs_term_t::flags_)
////////////////////////////////////////////////////////////////////////////////

#define EcsTermMatchAny               (1u << 0)
#define EcsTermMatchAnySrc            (1u << 1)
#define EcsTermTransitive             (1u << 2)
#define EcsTermReflexive              (1u << 3)
#define EcsTermIdInherited            (1u << 4)
#define EcsTermIsTrivial              (1u << 5)
#define EcsTermNoData                 (1u << 6)
#define EcsTermIsCacheable            (1u << 7)
#define EcsTermIsScope                (1u << 8)
#define EcsTermIsMember               (1u << 9)
#define EcsTermIsToggle               (1u << 10)
#define EcsTermKeepAlive              (1u << 11)
#define EcsTermIsSparse               (1u << 12)
#define EcsTermIsUnion                (1u << 13)
#define EcsTermIsOr                   (1u << 14)


////////////////////////////////////////////////////////////////////////////////
//// Observer flags (used by ecs_observer_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsObserverIsMulti             (1u << 1u)  /* Does observer have multiple terms */
#define EcsObserverIsMonitor           (1u << 2u)  /* Is observer a monitor */
#define EcsObserverIsDisabled          (1u << 3u)  /* Is observer entity disabled */
#define EcsObserverIsParentDisabled    (1u << 4u)  /* Is module parent of observer disabled  */
#define EcsObserverBypassQuery         (1u << 5u)

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
#define EcsTableNotQueryable           (1u << 9u)  /* Table should never be returned by queries */
#define EcsTableHasCtors               (1u << 10u)
#define EcsTableHasDtors               (1u << 11u)
#define EcsTableHasCopy                (1u << 12u)
#define EcsTableHasMove                (1u << 13u)
#define EcsTableHasToggle              (1u << 14u)
#define EcsTableHasOverrides           (1u << 15u)

#define EcsTableHasOnAdd               (1u << 16u) /* Same values as id flags */
#define EcsTableHasOnRemove            (1u << 17u)
#define EcsTableHasOnSet               (1u << 18u)
#define EcsTableHasOnTableFill         (1u << 20u)
#define EcsTableHasOnTableEmpty        (1u << 21u)
#define EcsTableHasOnTableCreate       (1u << 22u)
#define EcsTableHasOnTableDelete       (1u << 23u)
#define EcsTableHasSparse              (1u << 24u)
#define EcsTableHasUnion               (1u << 25u)

#define EcsTableHasTraversable         (1u << 26u)
#define EcsTableMarkedForDelete        (1u << 30u)

/* Composite table flags */
#define EcsTableHasLifecycle        (EcsTableHasCtors | EcsTableHasDtors)
#define EcsTableIsComplex           (EcsTableHasLifecycle | EcsTableHasToggle | EcsTableHasSparse)
#define EcsTableHasAddActions       (EcsTableHasIsA | EcsTableHasCtors | EcsTableHasOnAdd | EcsTableHasOnSet)
#define EcsTableHasRemoveActions    (EcsTableHasIsA | EcsTableHasDtors | EcsTableHasOnRemove)


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
