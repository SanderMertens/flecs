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
#define EcsWorldQuit                  (1u << 2)
#define EcsWorldFini                  (1u << 3)
#define EcsWorldMeasureFrameTime      (1u << 4)
#define EcsWorldMeasureSystemTime     (1u << 5)


////////////////////////////////////////////////////////////////////////////////
//// OS API flags
////////////////////////////////////////////////////////////////////////////////

#define EcsOsApiHighResolutionTimer   (1u << 0)
#define EcsOsApiLogWithColors         (1u << 1)


////////////////////////////////////////////////////////////////////////////////
//// Entity flags (set in upper bits of ecs_record_t::row)
////////////////////////////////////////////////////////////////////////////////

#define EcsEntityObserved             (1u << 31)
#define EcsEntityObservedId           (1u << 30)
#define EcsEntityObservedObject       (1u << 29)
#define EcsEntityObservedAcyclic      (1u << 28)


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
#define EcsIdAcyclic                   (1u << 8)
#define EcsIdTag                       (1u << 9)
#define EcsIdWith                      (1u << 10)
#define EcsIdUnion                     (1u << 11)

#define EcsIdHasOnAdd                  (1u << 15) /* Same values as table flags */
#define EcsIdHasOnRemove               (1u << 16) 
#define EcsIdHasOnSet                  (1u << 17)
#define EcsIdHasUnSet                  (1u << 18)
#define EcsIdEventMask\
    (EcsIdHasOnAdd|EcsIdHasOnRemove|EcsIdHasOnSet|EcsIdHasUnSet)

#define EcsIdMarkedForDelete            (1u << 30)

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
#define EcsIterIsFilter                (1u << 1u)  /* Is iterator filter (metadata only) */
#define EcsIterIsInstanced             (1u << 2u)  /* Is iterator instanced */
#define EcsIterHasShared               (1u << 3u)  /* Does result have shared terms */
#define EcsIterTableOnly               (1u << 4u)  /* Result only populates table */
#define EcsIterEntityOptional          (1u << 5u)  /* Treat terms with entity subject as optional */
#define EcsIterNoResults               (1u << 6u)  /* Iterator has no results */
#define EcsIterIgnoreThis              (1u << 7u)  /* Only evaluate non-this terms */


////////////////////////////////////////////////////////////////////////////////
//// Filter flags (used by ecs_filter_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsFilterMatchThis             (1u << 1u)  /* Has terms that match This */
#define EcsFilterMatchOnlyThis         (1u << 2u)  /* Has only terms that match This */
#define EcsFilterMatchPrefab           (1u << 3u)  /* Does filter match prefabs */
#define EcsFilterMatchDisabled         (1u << 4u)  /* Does filter match disabled entities */
#define EcsFilterMatchEmptyTables      (1u << 5u)  /* Does filter return empty tables */
#define EcsFilterMatchAnything         (1u << 6u)  /* False if filter has no/only Not terms */
#define EcsFilterIsFilter              (1u << 7u)  /* When true, data fields won't be populated */
#define EcsFilterIsInstanced           (1u << 8u)  /* Is filter instanced (see ecs_filter_desc_t) */
#define EcsFilterPopulate              (1u << 9u)  /* Populate data, ignore non-matching fields */


////////////////////////////////////////////////////////////////////////////////
//// Table flags (used by ecs_table_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsTableHasBuiltins            (1u << 1u)  /* Does table have builtin components */
#define EcsTableIsPrefab               (1u << 2u)  /* Does the table store prefabs */
#define EcsTableHasIsA                 (1u << 3u)  /* Does the table have IsA relation */
#define EcsTableHasChildOf             (1u << 4u)  /* Does the table type ChildOf relation */
#define EcsTableHasPairs               (1u << 5u)  /* Does the table type have pairs */
#define EcsTableHasModule              (1u << 6u)  /* Does the table have module data */
#define EcsTableIsDisabled             (1u << 7u)  /* Does the table type has EcsDisabled */
#define EcsTableHasCtors               (1u << 8u)
#define EcsTableHasDtors               (1u << 9u)
#define EcsTableHasCopy                (1u << 10u)
#define EcsTableHasMove                (1u << 11u)
#define EcsTableHasUnion               (1u << 12u)
#define EcsTableHasDisabled            (1u << 13u)
#define EcsTableHasOverrides           (1u << 14u)

#define EcsTableHasOnAdd               (1u << 15u) /* Same values as id flags */
#define EcsTableHasOnRemove            (1u << 16u)
#define EcsTableHasOnSet               (1u << 17u)
#define EcsTableHasUnSet               (1u << 18u)

#define EcsTableMarkedForDelete        (1u << 30u)

/* Composite table flags */
#define EcsTableHasLifecycle        (EcsTableHasCtors | EcsTableHasDtors)
#define EcsTableIsComplex           (EcsTableHasLifecycle | EcsTableHasUnion | EcsTableHasDisabled)
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
