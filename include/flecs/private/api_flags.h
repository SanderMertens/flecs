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

#define EcsIdExclusive                 (1u << 6)
#define EcsIdDontInherit               (1u << 7)
#define EcsIdTraversable               (1u << 8)
#define EcsIdTag                       (1u << 9)
#define EcsIdWith                      (1u << 10)
#define EcsIdUnion                     (1u << 11)
#define EcsIdAlwaysOverride            (1u << 12)

#define EcsIdHasOnAdd                  (1u << 16) /* Same values as table flags */
#define EcsIdHasOnRemove               (1u << 17) 
#define EcsIdHasOnSet                  (1u << 18)
#define EcsIdHasUnSet                  (1u << 19)
#define EcsIdHasOnTableFill            (1u << 20)
#define EcsIdHasOnTableEmpty           (1u << 21)
#define EcsIdHasOnTableCreate          (1u << 22)
#define EcsIdHasOnTableDelete          (1u << 23)
#define EcsIdEventMask\
    (EcsIdHasOnAdd|EcsIdHasOnRemove|EcsIdHasOnSet|EcsIdHasUnSet|\
        EcsIdHasOnTableFill|EcsIdHasOnTableEmpty|EcsIdHasOnTableCreate|\
            EcsIdHasOnTableDelete)

#define EcsIdMarkedForDelete           (1u << 30)

/* Utilities for converting from flags to delete policies and vice versa */
#define ECS_ID_ON_DELETE(flags) \
    ((ecs_entity_t[]){0, EcsRemove, EcsDelete, 0, EcsPanic}\
        [((flags) & EcsIdOnDeleteMask)])
#define ECS_ID_ON_DELETE_OBJECT(flags) ECS_ID_ON_DELETE(flags >> 3)
#define ECS_ID_ON_DELETE_FLAG(id) (1u << ((id) - EcsRemove))
#define ECS_ID_ON_DELETE_OBJECT_FLAG(id) (1u << (3 + ((id) - EcsRemove)))


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
#define EcsQueryHasOutColumns          (1u << 4u)  /* Does query have out columns */
#define EcsQueryHasMonitor             (1u << 5u)  /* Does query track changes */
#define EcsQueryTrivialIter            (1u << 6u)  /* Does the query require special features to iterate */


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
