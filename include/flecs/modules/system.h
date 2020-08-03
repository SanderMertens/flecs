#ifdef FLECS_SYSTEM
#define FLECS_MODULE

#include "flecs/addons/module.h"

#ifndef FLECS_SYSTEMS_H
#define FLECS_SYSTEMS_H

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Components
////////////////////////////////////////////////////////////////////////////////

FLECS_EXPORT
extern ecs_type_t
    ecs_type(EcsTrigger),
    ecs_type(EcsSystem),
    ecs_type(EcsTickSource),
    ecs_type(EcsSignatureExpr),
    ecs_type(EcsSignature),
    ecs_type(EcsQuery),
    ecs_type(EcsIterAction),
    ecs_type(EcsContext);

/* Component used to provide a tick source to systems */
typedef struct EcsTickSource {
    bool tick;           /* True if providing tick */
    float time_elapsed;  /* Time elapsed since last tick */
} EcsTickSource;

/* Signature expression */
typedef struct EcsSignatureExpr {
    const char *expr;
} EcsSignatureExpr;

/* Parsed signature */
typedef struct EcsSignature {
    ecs_sig_t signature;
} EcsSignature;

/* Query component */
typedef struct EcsQuery {
    ecs_query_t *query;
} EcsQuery;

/* System action */
typedef struct EcsIterAction {
    ecs_iter_action_t action;
} EcsIterAction;

/* System context */
typedef struct EcsContext {
    const void *ctx;
} EcsContext;


////////////////////////////////////////////////////////////////////////////////
//// Systems API
////////////////////////////////////////////////////////////////////////////////

/** Declare a systen.
 * This macro declares a system with the specified function, kind and signature. 
 * Systems are matched with entities that match the system signature. The system
 * signature is specified as a comma-separated list of column expressions, where
 * a column expression can be any of the following: 
 *
 * - A simple component identifier ('Position')
 * - An OR expression ('Position | Velocity')
 * - An optional expression ('?Position')
 * - A NOT expression ('!Position')
 * - An OWNED expression ('OWNED:Position')
 * - A SHARED expression ('SHARED:Position')
 * - A PARENT expression ('PARENT:Position')
 * - A CASCADE expression ('CASCADE:Position')
 * - An entity expression ('MyEntity:Position')
 * - An empty expression (':Position')
 * 
 * The systen kind specifies the phase in which the system is ran.
 *
 * Examples:
 * ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity, !AngularVelocity);
 * ECS_SYSTEM(world, Transform, EcsPostUpdate, PARENT:Transform, Transform);
 *
 * In these examples, 'Move' and 'Transform' must be valid identifiers to a C
 * function of the following signature:
 *
 * void Move(ecs_iter_t *it) { ... }
 *
 * Inside this function the system can access the data from the signature with
 * the ECS_COLUMN macro:
 *
 * ECS_COLUMN(it, Position, p, 1);
 * ECS_COLUMN(it, Velocity, v, 2);
 *
 * For more details on system signatures and phases see the Flecs manual.
 */

#ifndef FLECS_LEGACY
#define ECS_SYSTEM(world, name, kind, ...) \
    ecs_iter_action_t ecs_iter_action(name) = name;\
    ecs_entity_t name = ecs_new_system(world, 0, #name, kind, #__VA_ARGS__, ecs_iter_action(name));\
    (void)ecs_iter_action(name);\
    (void)name;
#endif

#define ECS_TRIGGER(world, name, kind, component) \
    ecs_entity_t __F##name = ecs_new_trigger(world, 0, #name, kind, #component, name);\
    ecs_entity_t name = __F##name;\
    (void)__F##name;\
    (void)name;

/** Run a specific system manually.
 * This operation runs a single system manually. It is an efficient way to
 * invoke logic on a set of entities, as manual systems are only matched to
 * tables at creation time or after creation time, when a new table is created.
 *
 * Manual systems are useful to evaluate lists of prematched entities at
 * application defined times. Because none of the matching logic is evaluated
 * before the system is invoked, manual systems are much more efficient than
 * manually obtaining a list of entities and retrieving their components.
 *
 * An application may pass custom data to a system through the param parameter.
 * This data can be accessed by the system through the param member in the
 * ecs_iter_t value that is passed to the system callback.
 *
 * Any system may interrupt execution by setting the interrupted_by member in
 * the ecs_iter_t value. This is particularly useful for manual systems, where
 * the value of interrupted_by is returned by this operation. This, in
 * cominbation with the param argument lets applications use manual systems
 * to lookup entities: once the entity has been found its handle is passed to
 * interrupted_by, which is then subsequently returned.
 *
 * @param world The world.
 * @param system The system to run.
 * @param delta_time: The time passed since the last system invocation.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_EXPORT
ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    void *param);

/** Run system with offset/limit and type filter.
 * This operation is the same as ecs_run, but filters the entities that will be
 * iterated by the system.
 * 
 * Entities can be filtered in two ways. Offset and limit control the range of
 * entities that is iterated over. The range is applied to all entities matched
 * with the system, thus may cover multiple archetypes.
 * 
 * The type filter controls which entity types the system will evaluate. Only
 * types that contain all components in the type filter will be iterated over. A
 * type filter is only evaluated once per table, which makes filtering cheap if
 * the number of entities is large and the number of tables is small, but not as
 * cheap as filtering in the system signature.
 * 
 * @param world The world.
 * @param system The system to invoke.
 * @param delta_time: The time passed since the last system invocation.
 * @param filter A component or type to filter matched entities.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_EXPORT
ecs_entity_t ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param);

/** System status change callback */
typedef enum ecs_system_status_t {
    EcsSystemStatusNone = 0,
    EcsSystemEnabled,
    EcsSystemDisabled,
    EcsSystemActivated,
    EcsSystemDeactivated
} ecs_system_status_t;

typedef void (*ecs_system_status_action_t)(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_t status,
    void *ctx);

/** Set system status action.
 * The status action is invoked whenever a system is enabled or disabled. Note
 * that a system may be enabled but may not actually match any entities. In this
 * case the system is enabled but not _active_.
 *
 * In addition to communicating the enabled / disabled status, the action also
 * communicates changes in the activation status of the system. A system becomes
 * active when it has one or more matching entities, and becomes inactive when
 * it no longer matches any entities.
 * 
 * A system switches between enabled and disabled when an application invokes the
 * ecs_enable operation with a state different from the state of the system, for
 * example the system is disabled, and ecs_enable is invoked with enabled: true.
 *
 * Additionally a system may switch between enabled and disabled when it is an
 * EcsOnDemand system, and interest is generated or lost for one of its [out]
 * columns.
 *
 * @param world The world.
 * @param system The system for which to set the action.
 * @param action The action.
 * @param ctx Context that will be passed to the action when invoked.
 */
FLECS_EXPORT
void ecs_set_system_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_action_t action,
    const void *ctx);


////////////////////////////////////////////////////////////////////////////////
//// System debug API
////////////////////////////////////////////////////////////////////////////////

typedef struct ecs_dbg_system_t {
    ecs_entity_t system;   
    int32_t entities_matched_count;
    int32_t active_table_count;
    int32_t inactive_table_count;
    bool enabled;
    void *system_data;
} ecs_dbg_system_t;

FLECS_EXPORT
int ecs_dbg_system(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_dbg_system_t *dbg_out);

FLECS_EXPORT
ecs_table_t* ecs_dbg_get_active_table(
    ecs_world_t *world,
    ecs_dbg_system_t *dbg,
    int32_t index);

FLECS_EXPORT
ecs_table_t* ecs_dbg_get_inactive_table(
    ecs_world_t *world,
    ecs_dbg_system_t *dbg,
    int32_t index);

FLECS_EXPORT
ecs_type_t ecs_dbg_get_column_type(
    ecs_world_t *world,
    ecs_entity_t system,
    int32_t column_index);

FLECS_EXPORT
bool ecs_dbg_match_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t system,
    ecs_match_failure_t *failure_info_out);


////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

/* Pipeline component is empty: components and tags in module are static */
typedef struct FlecsSystem {
    int32_t dummy; 
} FlecsSystem;

FLECS_EXPORT
void FlecsSystemImport(
    ecs_world_t *world);

#define FlecsSystemImportHandles(handles)

#ifdef __cplusplus
}
#endif

#endif

#endif
