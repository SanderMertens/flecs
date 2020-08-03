
#ifndef FLECS_H
#define FLECS_H

/* FLECS_LEGACY should be defined when building for C89 */
// #define FLECS_LEGACY

/* FLECS_NO_CPP should be defined when building for C++ without the C++ API */
// #define FLECS_NO_CPP

/* FLECS_CUSTOM_BUILD should be defined when manually selecting features */
// #define FLECS_CUSTOM_BUILD

#ifndef FLECS_CUSTOM_BUILD
/* Modules */
#define FLECS_SYSTEM
#define FLECS_PIPELINE
#define FLECS_TIMER
#define FLECS_STATS

/* Addons */
#define FLECS_BULK
#define FLECS_DBG
#define FLECS_MODULE
#define FLECS_QUEUE
#define FLECS_READER_WRITER
#define FLECS_SNAPSHOT
#endif

#include "flecs/private/api_defines.h"
#include "flecs/private/vector.h"        /* Vector datatype */
#include "flecs/private/sparse.h"        /* Sparse set */
#include "flecs/private/map.h"           /* Hashmap */
#include "flecs/private/strbuf.h"        /* Efficient string builder */
#include "flecs/os_api.h"  /* Abstraction for operating system functions */

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Basic API types
////////////////////////////////////////////////////////////////////////////////

/** An entity identifier. */
typedef uint64_t ecs_entity_t;

/** A vector containing component identifiers used to describe a type. */
typedef const ecs_vector_t* ecs_type_t;

/** An ECS world is the container for all ECS data and supporting features. */
typedef struct ecs_world_t ecs_world_t;

/** A snapshot stores the state of a world in a particular point in time. */
typedef struct ecs_snapshot_t ecs_snapshot_t;

/** Queries are the primary mechanism for iterating (prematched) entities. */
typedef struct ecs_query_t ecs_query_t;

/* An iterator lets an application iterate entities across tables. */
typedef struct ecs_iter_t ecs_iter_t;

/** Refs cache data that lets them access components faster than ecs_get. */
typedef struct ecs_ref_t ecs_ref_t;

/** Describes how a filter should match components with a table. */
typedef enum ecs_match_kind_t {
    EcsMatchDefault = 0,
    EcsMatchAll,            /**< Match all components in a type */
    EcsMatchAny,            /**< Match one of the components in a type */
    EcsMatchExact           /**< Match the type exactly */
} ecs_match_kind_t;

/** Filters alllow for ad-hoc quick filtering of entity tables. */
typedef struct ecs_filter_t {
    ecs_type_t include;             /**< Components that should match */
    ecs_type_t exclude;             /**< Components that should not match */
    ecs_match_kind_t include_kind;  /**< Match kind for include components */
    ecs_match_kind_t exclude_kind;  /**< Match kind for exclude components */
} ecs_filter_t;

/** Type that contains various statistics of a world. */
typedef struct ecs_world_info_t {
    ecs_entity_t last_component_id;   /**< Last issued component entity id */
    ecs_entity_t last_id;             /**< Last issued entity id */
    ecs_entity_t min_id;              /**< First allowed entity id */
    ecs_entity_t max_id;              /**< Last allowed entity id */

    float delta_time_raw;       /**< Raw delta time (no time scaling) */
    float delta_time;           /**< Time passed to or computed by ecs_progress */
    float time_scale;           /**< Time scale applied to delta_time */
    float target_fps;           /**< Target fps */
    float frame_time_total;    /**< Total time spent processing a frame */
    float system_time_total;   /**< Total time spent in systems */
    float merge_time_total;    /**< Total time spent in merges */
    float world_time_total;    /**< Time elapsed in simulation */
    float world_time_total_raw; /**< Time elapsed in simulation (no scaling) */
    float sleep_err;           /**< Measured sleep error */
    
    int32_t frame_count_total;  /**< Total number of frames */
    int32_t merge_count_total;  /**< Total number of merges */
    int32_t pipeline_build_count_total; /**< Total number of pipeline builds */
    int32_t systems_ran_frame;  /**< Total number of systems ran in last frame */
} ecs_world_info_t;


////////////////////////////////////////////////////////////////////////////////
//// Function types
////////////////////////////////////////////////////////////////////////////////

/** Action callback for systems and triggers */
typedef void (*ecs_iter_action_t)(
    ecs_iter_t *it);

typedef bool (*ecs_iter_next_action_t)(
    ecs_iter_t *it);

/** Compare callback used for sorting */
typedef int (*ecs_compare_action_t)(
    ecs_entity_t e1,
    void *ptr1,
    ecs_entity_t e2,
    void *ptr2);    

/** Compare callback used for sorting */
typedef int32_t (*ecs_rank_type_action_t)(
    ecs_world_t *world,
    ecs_entity_t rank_component,
    ecs_type_t type);

/** Initialization action for modules */
typedef void (*ecs_module_action_t)(
    ecs_world_t *world);    

/** Action callback on world exit */
typedef void (*ecs_fini_action_t)(
    ecs_world_t *world,
    void *ctx);

#include "flecs/private/api_types.h"        /* Supporting API types */
#include "flecs/private/api_support.h"      /* Supporting API functions */
#include "flecs/private/log.h"              /* Logging API */
#include "flecs/type.h"                     /* Type API */


////////////////////////////////////////////////////////////////////////////////
//// Builtin components
////////////////////////////////////////////////////////////////////////////////

/** Entity name. */
typedef struct EcsName {
    const char *value;     /**< Entity name */
    const char *symbol;    /**< Optional symbol name, if it differs from name */
    char *alloc_value;     /**< If set, value will be freed on destruction */
} EcsName;

/** Component information. */
typedef struct EcsComponent {
    ecs_size_t size;           /**< Component size */
    ecs_size_t alignment;      /**< Component alignment */
} EcsComponent;

/** Component that stores an ecs_type_t. 
 * This component allows for the creation of entities that represent a type, and
 * therefore the creation of named types. This component is typically 
 * instantiated by ECS_TYPE. */
typedef struct EcsType {
    ecs_type_t type;        /**< Preserved nested types */
    ecs_type_t normalized;  /**< Union of type and nested AND types */
} EcsType;

/** Component that contains lifecycle callbacks for a component. */
typedef struct EcsComponentLifecycle {
    ecs_xtor_t ctor;        /**< Component constructor */
    ecs_xtor_t dtor;        /**< Component destructor */
    ecs_copy_t copy;        /**< Component copy */
    ecs_move_t move;        /**< Component move */
    void *ctx;              /**< User defined context */
} EcsComponentLifecycle;

/* Component used for registering component triggers */
typedef struct EcsTrigger {
    ecs_entity_t kind;
    ecs_iter_action_t action;
    ecs_entity_t component;
    ecs_entity_t self;
    void *ctx;
} EcsTrigger;


////////////////////////////////////////////////////////////////////////////////
//// Type roles
////////////////////////////////////////////////////////////////////////////////

/* Type roles are used to indicate the role of an entity in a type. If no flag
 * is specified, the entity is interpreted as a regular component or tag. Flags
 * are added to an entity by using a bitwise OR (|). An example:
 *
 * ecs_entity_t parent = ecs_new(world, 0);
 * ecs_entity_t child = ecs_add_entity(world, e, ECS_CHILDOF | parent);
 *
 * Type flags can also be used in type expressions, without the ECS prefix:
 *
 * ECS_ENTITY(world, Base, Position);
 * ECS_TYPE(world, InstanceOfBase, INSTANCEOF | Base);
 */

/** The INSTANCEOF role indicates that the components from the entity should be
 * shared with the entity that instantiates the type. */
#define ECS_INSTANCEOF ((ecs_entity_t)1 << 63)

/** The CHILDOF role indicates that the entity should be treated as a parent of
 * the entity that instantiates the type. */
#define ECS_CHILDOF ((ecs_entity_t)1 << 62)

/** The TRAIT role indicates that the entity is a trait identifier. */
#define ECS_TRAIT ((ecs_entity_t)1 << 61)

/** Enforce that all entities of a type are present in the type.
 * This flag can only be used in combination with an entity that has EcsType. */
#define ECS_AND ((ecs_entity_t)1 << 60)

/** Enforce that at least one entity of a type must be present in the type.
 * This flag can only be used in combination with an entity that has EcsType. */
#define ECS_OR ((ecs_entity_t)1 << 59)

/** Enforce that exactly one entity of a type must be present in the type.
 * This flag can only be used in combination with an entity that has EcsType. 
 * When another entity of the XOR'd type is added to an entity of this type, the
 * previous entity is removed from the entity. This makes XOR useful for
 * implementing state machines, as it allows for traversing states while 
 * ensuring that only one state is ever active at the same time. */
#define ECS_XOR ((ecs_entity_t)1 << 58)

/** None of the entities in a type may be added to the type.
 * This flag can only be used in combination with an entity that has EcsType. */
#define ECS_NOT ((ecs_entity_t)1 << 57)


////////////////////////////////////////////////////////////////////////////////
//// Builtin tags
////////////////////////////////////////////////////////////////////////////////

/* Builtin tag ids */
#define EcsModule (ECS_HI_COMPONENT_ID + 0)
#define EcsPrefab (ECS_HI_COMPONENT_ID + 1)
#define EcsHidden (ECS_HI_COMPONENT_ID + 2)
#define EcsDisabled (ECS_HI_COMPONENT_ID + 3)
#define EcsDisabledIntern (ECS_HI_COMPONENT_ID + 4)
#define EcsInactive (ECS_HI_COMPONENT_ID + 5)
#define EcsOnDemand (ECS_HI_COMPONENT_ID + 6)
#define EcsMonitor (ECS_HI_COMPONENT_ID + 7)
#define EcsPipeline (ECS_HI_COMPONENT_ID + 8)

/* Trigger tags */
#define EcsOnAdd (ECS_HI_COMPONENT_ID + 9)
#define EcsOnRemove (ECS_HI_COMPONENT_ID + 10)

/* Set system tags */
#define EcsOnSet (ECS_HI_COMPONENT_ID + 11)
#define EcsUnSet (ECS_HI_COMPONENT_ID + 12)

/* Builtin pipeline tags */
#define EcsPreFrame (ECS_HI_COMPONENT_ID + 13)
#define EcsOnLoad (ECS_HI_COMPONENT_ID + 14)
#define EcsPostLoad (ECS_HI_COMPONENT_ID + 15)
#define EcsPreUpdate (ECS_HI_COMPONENT_ID + 16)
#define EcsOnUpdate (ECS_HI_COMPONENT_ID + 17)
#define EcsOnValidate (ECS_HI_COMPONENT_ID + 18)
#define EcsPostUpdate (ECS_HI_COMPONENT_ID + 19)
#define EcsPreStore (ECS_HI_COMPONENT_ID + 20)
#define EcsOnStore (ECS_HI_COMPONENT_ID + 21)
#define EcsPostFrame (ECS_HI_COMPONENT_ID + 22)

/** Builtin entity ids */
#define EcsFlecs (ECS_HI_COMPONENT_ID + 23)
#define EcsFlecsCore (ECS_HI_COMPONENT_ID + 24)
#define EcsWorld (ECS_HI_COMPONENT_ID + 25)
#define EcsSingleton ((ecs_entity_t)(ECS_ENTITY_MASK) - 1)

/* Value used to quickly check if component is builtin. This is used to quickly
 * filter out tables with builtin components (for example for ecs_delete) */
#define EcsLastInternalComponentId (ecs_entity(EcsSystem))

/* The first user-defined component starts from this id. Ids up to this number
 * are reserved for builtin components */
#define EcsFirstUserComponentId (32)

/* The first user-defined entity starts from this id. Ids up to this number
 * are reserved for builtin components */
#define EcsFirstUserEntityId (ECS_HI_COMPONENT_ID + 32)


////////////////////////////////////////////////////////////////////////////////
//// Convenience macro's
////////////////////////////////////////////////////////////////////////////////

/* Macro's rely on variadic arguments which are C99 and above */
#ifndef FLECS_LEGACY

/** Declare a named entity with a type expression. 
 * Example:
 *   ECS_ENTITY(world, MyEntity, Position, Velocity);
 */ 
#define ECS_ENTITY(world, id, ...)\
    ecs_entity_t id = ecs_new_entity(world, 0, #id, #__VA_ARGS__);\
    (void)id;

/** Declare a prefab with a type expression. 
 * Example:
 *   ECS_PREFAB(world, MyEntity, Position, Velocity);
 */
#define ECS_PREFAB(world, id, ...) \
    ecs_entity_t id = ecs_new_prefab(world, 0, #id, #__VA_ARGS__);\
    (void)id;

/** Declare a component.
 * Example:
 *   ECS_COMPONENT(world, Position);
 */
#define ECS_COMPONENT(world, id) \
    ECS_ENTITY_VAR(id) = ecs_new_component(world, 0, #id, sizeof(id), ECS_ALIGNOF(id));\
    ECS_VECTOR_STACK(FLECS__T##id, ecs_entity_t, &FLECS__E##id, 1);\
    (void)ecs_entity(id);\
    (void)ecs_type(id);\

/** Declare a tag.
 * Example:
 *   ECS_TAG(world, MyTag);
 */
#define ECS_TAG(world, id) \
    ECS_ENTITY(world, id, 0);\
    ECS_VECTOR_STACK(FLECS__T##id, ecs_entity_t, &id, 1);\
    (void)ecs_type(id);\

/** Declare a type.
 * Example:
 *   ECS_TYPE(world, MyType, Position, Velocity);
 */
#define ECS_TYPE(world, id, ...) \
    ecs_entity_t id = ecs_new_type(world, 0, #id, #__VA_ARGS__);\
    ECS_TYPE_VAR(id) = ecs_type_from_entity(world, id);\
    (void)id;\
    (void)ecs_type(id);\

/** Declare a constructor.
 * Example:
 *   ECS_CTOR(MyType, ptr, { ptr->value = NULL; });
 */
#define ECS_CTOR(type, var, ...)\
    ECS_XTOR_IMPL(type, ctor, var, __VA_ARGS__)

/** Declare a destructor.
 * Example:
 *   ECS_DTOR(MyType, ptr, { free(ptr->value); });
 */
#define ECS_DTOR(type, var, ...)\
    ECS_XTOR_IMPL(type, dtor, var, __VA_ARGS__)

/** Declare a copy action.
 * Example:
 *   ECS_COPY(MyType, dst, src, { dst->value = strdup(src->value); });
 */
#define ECS_COPY(type, dst_var, src_var, ...)\
    ECS_COPY_IMPL(type, dst_var, src_var, __VA_ARGS__)

/** Declare a move action.
 * Example:
 *   ECS_MOVE(MyType, dst, src, { dst->value = src->value; src->value = 0; });
 */
#define ECS_MOVE(type, dst_var, src_var, ...)\
    ECS_MOVE_IMPL(type, dst_var, src_var, __VA_ARGS__)

/* Map from typename to function name of component lifecycle action */
#define ecs_ctor(type) type##_ctor
#define ecs_dtor(type) type##_dtor
#define ecs_copy(type) type##_copy
#define ecs_move(type) type##_move

#endif /* FLECS_LEGACY */


////////////////////////////////////////////////////////////////////////////////
//// World API
////////////////////////////////////////////////////////////////////////////////

/** Create a new world.
 * A world manages all the ECS data and supporting infrastructure. Applications 
 * must have at least one world. Entities, component and system handles are 
 * local to a world and should not be shared between worlds.
 *
 * @return A new world object
 */
FLECS_EXPORT
ecs_world_t* ecs_init(void);

/** Same as ecs_init, but with minimal set of modules loaded.
 *
 * @return A new world object
 */
FLECS_EXPORT
ecs_world_t* ecs_mini(void);

/** Create a new world with arguments.
 * Same as ecs_init, but allows passing in command line arguments. These can be
 * used to dynamically enable flecs features to an application, like performance
 * monitoring or the web dashboard (if it is installed) without having to modify
 * the code of an application.
 * 
 * The following options are available:
 * --threads [n]   Use n worker threads
 * --fps [hz]      Run at hz FPS
 * --admin [port]  Enable admin dashboard (requires flecs-systems-admin & flecs-systems-civetweb)
 * --console       Enables console (requires flecs-systems-console)
 * --debug         Enables debug tracing
 *
 * @return A new world object
 */
FLECS_EXPORT
ecs_world_t* ecs_init_w_args(
    int argc,
    char *argv[]);

/** Delete a world.
 * This operation deletes the world, and everything it contains.
 *
 * @param world The world to delete.
 * @return Zero if successful, non-zero if failed.
 */
FLECS_EXPORT
int ecs_fini(
    ecs_world_t *world);

/** Register action to be executed when world is destroyed.
 * Fini actions are typically used when a module needs to clean up before a
 * world shuts down.
 * 
 * @param world The world.
 * @param action The function to execute.
 * @param ctx Userdata to pass to the function */
FLECS_EXPORT
void ecs_atfini(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx);

/** Register action to be executed once after frame.
 * Post frame actions are typically used for calling operations that cannot be
 * invoked during iteration, such as changing the number of threads.
 * 
 * @param world The world.
 * @param action The function to execute.
 * @param ctx Userdata to pass to the function */
FLECS_EXPORT
void ecs_run_post_frame(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx);    

/** Register ctor, dtor, copy & move actions for component.
 *
 * @param world The world.
 * @param component The component id for which to register the actions
 * @param actions Type that contains the component actions.
 */
FLECS_EXPORT
void ecs_set_component_actions_w_entity(
    ecs_world_t *world,
    ecs_entity_t component,
    EcsComponentLifecycle *actions);

#ifndef FLECS_LEGACY
#define ecs_set_component_actions(world, component, ...)\
    ecs_set_component_actions_w_entity(world, ecs_entity(component), &(EcsComponentLifecycle)__VA_ARGS__)

#endif
/** Set a world context.
 * This operation allows an application to register custom data with a world
 * that can be accessed anywhere where the application has the world object.
 *
 * @param world The world.
 * @param ctx A pointer to a user defined structure.
 */
FLECS_EXPORT
void ecs_set_context(
    ecs_world_t *world,
    void *ctx);

/** Get the world context.
 * This operation retrieves a previously set world context.
 *
 * @param world The world.
 * @return The context set with ecs_set_context. If no context was set, the
 *         function returns NULL.
 */
FLECS_EXPORT
void* ecs_get_context(
    ecs_world_t *world);

/** Get world info.
 *
 * @param world The world.
 * @return Pointer to the world info. This pointer will remain valid for as long
 *         as the world is valid.
 */
FLECS_EXPORT
const ecs_world_info_t* ecs_get_world_info(
    ecs_world_t *world);

/** Dimension the world for a specified number of entities.
 * This operation will preallocate memory in the world for the specified number
 * of entities. Specifying a number lower than the current number of entities in
 * the world will have no effect. Note that this function does not allocate
 * memory for components (use ecs_dim_type for that).
 *
 * @param world The world.
 * @param entity_count The number of entities to preallocate.
 */
FLECS_EXPORT
void ecs_dim(
    ecs_world_t *world,
    int32_t entity_count);

/** Dimension a type for a specified number of entities.
 * This operation will preallocate memory for a type (table) for the
 * specified number of entites. Specifying a number lower than the current
 * number of entities in the table will have no effect.
 *
 * @param world The world.
 * @param type Handle to the type, as obtained by ecs_type_get.
 * @param entity_count The number of entities to preallocate.
 */
FLECS_EXPORT
void ecs_dim_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t entity_count);

/** Set a range for issueing new entity ids.
 * This function constrains the entity identifiers returned by ecs_new to the 
 * specified range. This operation can be used to ensure that multiple processes
 * can run in the same simulation without requiring a central service that
 * coordinates issueing identifiers.
 * 
 * If id_end is set to 0, the range is infinite. If id_end is set to a non-zero
 * value, it has to be larger than id_start. If id_end is set and ecs_new is
 * invoked after an id is issued that is equal to id_end, the application will
 * abort.
 * 
 * @param world The world.
 * @param id_start The start of the range.
 * @param id_end The end of the range.
 */
FLECS_EXPORT
void ecs_set_entity_range(
    ecs_world_t *world,
    ecs_entity_t id_start,
    ecs_entity_t id_end);

/** Enable/disable range limits.
 * When an application is both a receiver of range-limited entities and a
 * producer of range-limited entities, range checking needs to be temporarily
 * disabled when inserting received entities. Range checking is disabled on a 
 * stage, so setting this value is thread safe.
 *
 * @param world The world.
 * @param enable True if range checking should be enabled, false to disable.
 * @return The previous value.
 */
FLECS_EXPORT
bool ecs_enable_range_check(
    ecs_world_t *world,
    bool enable);

/** Enable world locking while in progress.
 * When locking is enabled, Flecs will lock the world while in progress. This
 * allows applications to interact with the world from other threads without
 * running into race conditions.
 *
 * This is a better alternative to applications putting a lock around calls to
 * ecs_progress, since ecs_progress can sleep when FPS control is enabled,
 * which is time during which other threads could perform work.
 *
 * Locking must be enabled before applications can use the ecs_lock and
 * ecs_unlock functions. Locking is turned off by default.
 *
 * @param world The world.
 * @param enable True if locking is to be enabled.
 * @result The previous value of the setting.
 */
FLECS_EXPORT
bool ecs_enable_locking(
    ecs_world_t *world,
    bool enable);

/** Locks the world.
 * See ecs_enable_locking for details.
 *
 * @param world The world.
 */
FLECS_EXPORT
void ecs_lock(
    ecs_world_t *world);

/** Unlocks the world.
 * See ecs_enable_locking for details.
 * 
 * @param world The world.
 */
FLECS_EXPORT
void ecs_unlock(
    ecs_world_t *world);

/** Wait until world becomes available.
 * When a non-flecs thread needs to interact with the world, it should invoke
 * this function to wait until the world becomes available (as in, it is not
 * progressing the frame). Invoking this function guarantees that the thread
 * will not starve. (as opposed to simply taking the world lock).
 *
 * An application will have to invoke ecs_end_wait after this function returns.
 * 
 * @param world The world.
 */
void ecs_begin_wait(
    ecs_world_t *world);

/** Release world after calling ecs_begin_wait.
 * This operation should be invoked after invoking ecs_begin_wait, and will
 * release the world back to the thread running the main loop.
 *
 * @param world The world.
 */
void ecs_end_wait(
    ecs_world_t *world);

/** Enable or disable tracing.
 * This will enable builtin tracing. For tracing to work, it will have to be
 * compiled in which requires defining one of the following macro's:
 *
 * ECS_TRACE_0 - All tracing is disabled
 * ECS_TRACE_1 - Enable tracing level 1
 * ECS_TRACE_2 - Enable tracing level 2 and below
 * ECS_TRACE_3 - Enable tracing level 3 and below
 *
 * If no tracing level is defined and this is a debug build, ECS_TRACE_3 will
 * have been automatically defined.
 *
 * The provided level corresponds with the tracing level. If -1 is provided as
 * value, warnings are disabled. If -2 is provided, errors are disabled as well.
 *
 * @param level Desired tracing level.
 */
FLECS_EXPORT
void ecs_tracing_enable(
    int level);


////////////////////////////////////////////////////////////////////////////////
//// Entity Creation
////////////////////////////////////////////////////////////////////////////////

/** Create new entity id.
 * This operation returns an unused entity id.
 *
 * @param world The world.
 * @return The new entity id.
 */
FLECS_EXPORT
ecs_entity_t ecs_new_id(
    ecs_world_t *world);

/** Create new component id.
 * This operation returns a new component id. Component ids are the same as
 * entity ids, but can make use of the [1 .. ECS_HI_COMPONENT_ID] range.
 * 
 * This operation does not recycle ids.
 *
 * @param world The world.
 * @return The new component id.
 */
FLECS_EXPORT
ecs_entity_t ecs_new_component_id(
    ecs_world_t *world);

/** Create new entity.
 * This operation creates a new entity with a single entity in its type. The
 * entity may contain type roles. This operation recycles ids.
 *
 * @param world The world.
 * @param entity The entity to initialize the new entity with.
 * @return The new entity.
 */
FLECS_EXPORT
ecs_entity_t ecs_new_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Create new entity.
 * This operation creates a new entity initialized with a type. This operation 
 * recycles ids.
 *
 * @param world The world.
 * @param type The type to initialize the new entity with.
 * @return The new entity.
 */
FLECS_EXPORT
ecs_entity_t ecs_new_w_type(
    ecs_world_t *world,
    ecs_type_t type);

/** Create a new entity.
 * This operation creates a new entity with a single component in its type. This
 * operation accepts variables created with ECS_COMPONENT, ECS_TYPE and ECS_TAG.
 * This operation recycles ids.
 * 
 * @param world The world.
 * @param type The component type.
 * @return The new entity.
 */
#define ecs_new(world, type)\
    ecs_new_w_type(world, ecs_type(type))

/** Create N new entities.
 * This operation is the same as ecs_new_w_entity, but creates N entities
 * instead of one and does not recycle ids. Ids of created entities are
 * guaranteed to be consecutive.
 * 
 * @param world The world.
 * @param entity The entity.
 * @param count The number of entities to create.
 * @return The first entity id of the newly created entities.
 */
FLECS_EXPORT
ecs_entity_t ecs_bulk_new_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    int32_t count);

/** Create N new entities.
 * This operation is the same as ecs_new_w_type, but creates N entities
 * instead of one and does not recycle ids. Ids of created entities are
 * guaranteed to be consecutive.
 * 
 * @param world The world.
 * @param type The type.
 * @param count The number of entities to create.
 * @return The first entity id of the newly created entities.
 */
FLECS_EXPORT
ecs_entity_t ecs_bulk_new_w_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t count);

/** Create N new entities and initialize components.
 * This operation is the same as ecs_bulk_new_w_type, but initializes components
 * with the provided component array. Instead of a type the operation accepts an
 * array of component identifiers (entities). The component arrays need to be
 * provided in the same order as the component identifiers.
 * 
 * @param world The world.
 * @param components Array with component identifiers.
 * @param count The number of entities to create.
 * @param data The data arrays to initialize the components with.
 * @return The first entity id of the newly created entities.
 */
FLECS_EXPORT
ecs_entity_t ecs_bulk_new_w_data(
    ecs_world_t *world,
    int32_t count,
    ecs_entities_t *component_ids,
    void *data);

/** Create N new entities.
 * This operation is the same as ecs_new, but creates N entities
 * instead of one and does not recycle ids. Ids of created entities are
 * guaranteed to be consecutive.
 * 
 * @param world The world.
 * @param component The component type.
 * @param count The number of entities to create.
 * @return The first entity id of the newly created entities.
 */
#define ecs_bulk_new(world, component, count)\
    ecs_bulk_new_w_type(world, ecs_type(component), count)


////////////////////////////////////////////////////////////////////////////////
//// Adding components
////////////////////////////////////////////////////////////////////////////////

/** Add an entity to an entity.
 * This operation adds a single entity to the type of an entity. Type roles may
 * be used in combination with the added entity. If the entity already has the
 * entity, this operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param entity_add The entity to add.
 */
FLECS_EXPORT
void ecs_add_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t entity_add);

/** Add a type to an entity.
 * This operation adds a type to an entity. The resulting type of the entity
 * will be the union of the previous type and the provided type. If the added
 * type did not have new components, this operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param type The type to add.
 */
FLECS_EXPORT
void ecs_add_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type);

/** Add a component, type or tag to an entity.
 * This operation adds a type to an entity. The resulting type of the entity
 * will be the union of the previous type and the provided type. If the added
 * type did not have new components, this operation will have no side effects.
 *
 * This operation accepts variables declared by ECS_COMPONENT, ECS_TYPE and
 * ECS_TAG.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component, type or tag to add.
 */
#define ecs_add(world, entity, component)\
    ecs_add_type(world, entity, ecs_type(component))


////////////////////////////////////////////////////////////////////////////////
//// Removing components
////////////////////////////////////////////////////////////////////////////////

/** Remove an entity from an entity.
 * This operation removes a single entity from the type of an entity. Type roles
 * may be used in combination with the added entity. If the entity does not have
 * the entity, this operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param entity_remove The entity to remove.
 */
FLECS_EXPORT
void ecs_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t entity_remove);

/** Remove a type from an entity.
 * This operation removes a type to an entity. The resulting type of the entity
 * will be the difference of the previous type and the provided type. If the 
 * type did not overlap with the entity type, this operation has no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param type The type to remove.
 */
FLECS_EXPORT
void ecs_remove_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type);

/** Remove a component, type or tag from an entity.
 * This operation removes a type to an entity. The resulting type of the entity
 * will be the difference of the previous type and the provided type. If the 
 * type did not overlap with the entity type, this operation has no side effects.
 *
 * This operation accepts variables declared by ECS_COMPONENT, ECS_TYPE and
 * ECS_TAG.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component, type or tag to remove.
 */
#define ecs_remove(world, entity, type)\
    ecs_remove_type(world, entity, ecs_type(type))


////////////////////////////////////////////////////////////////////////////////
//// Adding + removing components
////////////////////////////////////////////////////////////////////////////////

/** Add / remove entity from entities matching a filter.
 * Combination of ecs_add_entity and ecs_remove_entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @param to_add The entity to add.
 * @param to_remove The entity to remove.
 */
FLECS_EXPORT
void ecs_add_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_add,
    ecs_entity_t to_remove);

/** Add / remove type from entities matching a filter.
 * Combination of ecs_add_type and ecs_remove_type.
 *
 * @param world The world.
 * @param entity The entity.
 * @param to_add The type to add.
 * @param to_remove The type to remove.
 */
FLECS_EXPORT
void ecs_add_remove_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t to_add,
    ecs_type_t to_remove);

/** Add / remove component, type or tag from entity.
 * Combination of ecs_add and ecs_remove.
 *
 * @param world The world.
 * @param entity The entity.
 * @param to_add The component, type or tag to add.
 * @param to_remove The component, type or tag to remove.
 */
#define ecs_add_remove(world, entity, to_add, to_remove)\
    ecs_add_remove_type(world, entity, ecs_type(to_add), ecs_type(to_remove))


////////////////////////////////////////////////////////////////////////////////
//// Traits
////////////////////////////////////////////////////////////////////////////////

/** Add a trait
 * This operation adds a trait from an entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity for which to remove the trait.
 * @param trait The trait to remove.
 */
#define ecs_add_trait(world, entity, component, trait)\
    ecs_add_entity(world, entity, ecs_trait(ecs_entity(component), ecs_entity(trait)))

/** Remove a trait
 * This operation removes a trait from an entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity for which to remove the trait.
 * @param trait The trait to remove.
 */
#define ecs_remove_trait(world, entity, component, trait)\
    ecs_remove_entity(world, entity, ecs_trait(ecs_entity(component), ecs_entity(trait)))

/** Test if an entity has a trait.
 * This operation returns true if the entity has the provided trait for the
 * specified component in its type.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity.
 * @param trait The entity.
 * @return True if the entity has the trait, false if not.
 */
#define ecs_has_trait(world, entity, component, trait)\
    ecs_has_entity(world, entity, ecs_trait(ecs_entity(component), ecs_entity(trait)))


#ifndef FLECS_LEGACY
/** Set trait for component. 
 * This operation adds a trait for an entity and component. Traits can be added
 * multiple times to the same entity, as long as it is for different components.
 *
 * Traits can be matched with systems by providing the TRAIT role to the 
 * trait component in the system signature. A system will match multiple times
 * with the same entity if the trait is added for multiple components.
 *
 * * This operation can only be used with traits that are components.
 *
 * @param world The world.
 * @param e The entity.
 * @param component The component for which to add the trait.
 * @param trait The trait to add.
 */
#define ecs_set_trait(world, entity, component, trait, ...)\
    ecs_set_ptr_w_entity(world, entity, ecs_trait(ecs_entity(component), ecs_entity(trait)), sizeof(trait), &(trait)__VA_ARGS__)


/** Set tag trait for component. 
 * This operation is similar to ecs_set_trait, but is used for trait tags. When
 * a trait tag is set on an entity, the trait type is not used (tags have no
 * type) and instead the component type is used.
 *
 * This operation can only be used with traits that are not components.
 *
 * @param world The world.
 * @param e The entity.
 * @param component The component for which to add the trait.
 * @param trait The trait to add.
 */
#define ecs_set_trait_tag(world, entity, trait, component, ...)\
    ecs_set_ptr_w_entity(world, entity, ecs_trait(ecs_entity(component), trait), sizeof(component), &(component)__VA_ARGS__)

#endif
/** Get trait for component. 
 * This operation obtains the value of a trait for a componetn that has been 
 * added by ecs_set_trait.
 *
 * @param world The world.
 * @param e The entity.
 * @param component The component to which the trait was added.
 * @param trait The trait that was added.
 */
#define ecs_get_trait(world, entity, component, trait)\
    ((trait*)ecs_get_w_entity(world, entity, ecs_trait(ecs_entity(component), ecs_entity(trait))))


////////////////////////////////////////////////////////////////////////////////
//// Deleting entities
////////////////////////////////////////////////////////////////////////////////

/** Delete an entity.
 * This operation will delete an entity and all of its components. The entity id
 * will be recycled. Repeatedly calling ecs_delete without ecs_new, 
 * ecs_new_w_entity or ecs_new_w_type will cause a memory leak as it will cause
 * the list with ids that can be recycled to grow unbounded.
 *
 * @param world The world.
 * @param entity The entity.
 */
FLECS_EXPORT
void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity);


////////////////////////////////////////////////////////////////////////////////
//// Cloning entities
////////////////////////////////////////////////////////////////////////////////

/** Clone an entity
 * This operation clones the components of one entity into another entity. If
 * no destination entity is provided, a new entity will be created. Component
 * values are not copied unless copy_value is true.
 *
 * @param world The world.
 * @param dst The entity to copy the components to.
 * @param src The entity to copy the components from.
 * @param copy_value If true, the value of components will be copied to dst.
 * @return The destination entity.
 */
FLECS_EXPORT
ecs_entity_t ecs_clone(
    ecs_world_t *world,
    ecs_entity_t dst,
    ecs_entity_t src,
    bool copy_value);


////////////////////////////////////////////////////////////////////////////////
//// Getting components
////////////////////////////////////////////////////////////////////////////////

/** Get an immutable pointer to a component.
 * This operation obtains a const pointer to the requested component. The
 * operation accepts the component entity id.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity id of the component to obtain.
 * @return The component pointer, NULL if the entity does not have the component.
 */
FLECS_EXPORT
const void* ecs_get_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component);

/** Get an immutable pointer to a component.
 * Same as ecs_get_w_entity, but accepts the typename of a component.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to obtain.
 * @return The component pointer, NULL if the entity does not have the component.
 */
#define ecs_get(world, entity, component)\
    ((const component*)ecs_get_w_entity(world, entity, ecs_entity(component)))

/* -- Get cached pointer -- */

/** Get an immutable reference to a component.
 * This operation is similar to ecs_get_w_entity but it stores temporary
 * information in a `ecs_ref_t` value which allows subsequent lookups to be
 * faster.
 *
 * @param world The world.
 * @param ref Pointer to a ecs_ref_t value. Must be initialized.
 * @param entity The entity.
 * @param component The entity id of the component to obtain.
 * @return The component pointer, NULL if the entity does not have the component.
 */
FLECS_EXPORT
const void* ecs_get_ref_w_entity(
    ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_entity_t entity,
    ecs_entity_t component);

/** Get an immutable reference to a component.
 * Same as ecs_get_ref_w_entity, but accepts the typename of a component.
 *
 * @param world The world.
 * @param ref Pointer to a ecs_ref_t value. Must be initialized.
 * @param entity The entity.
 * @param component The component to obtain.
 * @return The component pointer, NULL if the entity does not have the component.
 */
#define ecs_get_ref(world, ref, entity, component)\
    ((const component*)ecs_get_ref_w_entity(world, ref, entity, ecs_entity(component)))

/** Get a mutable pointer to a component.
 * This operation is similar to ecs_get_w_entity but it returns a mutable 
 * pointer. If this operation is invoked from inside a system, the entity will
 * be staged and a pointer to the staged component will be returned.
 *
 * If the entity did not yet have the component, the component will be added by
 * this operation. In this case the is_added out parameter will be set to true.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity id of the component to obtain.
 * @param is_added Out parameter that returns true if the component was added.
 * @return The component pointer.
 */
FLECS_EXPORT
void* ecs_get_mut_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool *is_added);

/** Get a mutable pointer to a component.
 * Same as ecs_get_mut_w_entity but accepts a component typename.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to obtain.
 * @param is_added Out parameter that returns true if the component was added.
 * @return The component pointer.
 */
#define ecs_get_mut(world, entity, component, is_added)\
    ((component*)ecs_get_mut_w_entity(world, entity, ecs_entity(component), is_added))

/** Signal that a component has been modified.
 * This operation allows an application to signal to Flecs that a component has
 * been modified. As a result, OnSet systems will be invoked.
 *
 * This operation is commonly used together with ecs_get_mut.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity id of the component that was modified.
 */
FLECS_EXPORT 
void ecs_modified_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component);

/** Signal that a component has been modified.
 * Same as ecs_modified_w_entity but accepts a component typename.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component that was modified.
 */
#define ecs_modified(world, entity, component)\
    ecs_modified_w_entity(world, entity, ecs_entity(component))


////////////////////////////////////////////////////////////////////////////////
//// Setting components
////////////////////////////////////////////////////////////////////////////////

/** Set the value of a component.
 * This operation allows an application to set the value of a component. The
 * operation is equivalent to calling ecs_get_mut and ecs_modified.
 *
 * If the provided entity is 0, a new entity will be created.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity id of the component to set.
 * @param size The size of the pointer to the value.
 * @param ptr The pointer to the value.
 * @return The entity. A new entity if no entity was provided.
 */
FLECS_EXPORT
ecs_entity_t ecs_set_ptr_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    size_t size,
    const void *ptr);

/** Set the value of a component.
 * Same as ecs_set_ptr_w_entity, but accepts a component typename and 
 * automatically determines the type size.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to set.
 * @param size The size of the pointer to the value.
 * @return The entity. A new entity if no entity was provided.
 */
#define ecs_set_ptr(world, entity, component, ptr)\
    ecs_set_ptr_w_entity(world, entity, ecs_entity(component), sizeof(component), ptr)

/* Conditionally skip macro's as compound literals and variadic arguments are 
 * not supported in C89 */
#ifndef FLECS_LEGACY

/** Set the value of a component.
 * Same as ecs_set_ptr, but accepts a value instead of a pointer to a value.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to set.
 * @param size The size of the pointer to the value.
 * @return The entity. A new entity if no entity was provided.
 */
#define ecs_set(world, entity, component, ...)\
    ecs_set_ptr_w_entity(world, entity, ecs_entity(component), sizeof(component), &(component)__VA_ARGS__)
#endif


////////////////////////////////////////////////////////////////////////////////
//// Testing for components
////////////////////////////////////////////////////////////////////////////////

/** Test if an entity has an entity.
 * This operation returns true if the entity has the provided entity in its 
 * type.
 *
 * @param world The world.
 * @param entity The entity.
 * @param to_check The entity to test for.
 * @return True if the entity has the entity, false if not.
 */
FLECS_EXPORT
bool ecs_has_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_check);

/** Test if an entity has a type.
 * This operation returns true if the entity has the provided type in its 
 * type.
 *
 * @param world The world.
 * @param entity The entity.
 * @param type The type to test for.
 * @return True if the entity has the type, false if not.
 */
FLECS_EXPORT
bool ecs_has_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type);

/** Test if an entity has a component, type or tag.
 * This operation returns true if the entity has the provided component, type or
 * tag in its type.
 *
 * @param world The world.
 * @param entity The entity.
 * @param type The component, type or tag to test for.
 * @return True if the entity has the type, false if not.
 */
#define ecs_has(world, entity, type)\
    ecs_has_type(world, entity, ecs_type(type))

/** Test if an entity owns component, type or tag.
 * This operation is similar to ecs_has, but will return false if the entity
 * does not own the component, which is the case if the component is defined on
 * a base entity with an INSTANCEOF role.
 *
 * @param world The world.
 * @param entity The entity.
 * @param type The component, type or tag to test for.
 * @return True if the entity owns the component, type or tag, false if not.
 */
#define ecs_owns(world, entity, type, owned)\
    ecs_type_owns_type(world, ecs_get_type(world, entity), ecs_type(type), owned)

/** Test if an entity owns an entity.
 * This operation is similar to ecs_has, but will return false if the entity
 * does not own the entity, which is the case if the entity is defined on
 * a base entity with an INSTANCEOF role.
 *
 * @param world The world.
 * @param entity The entity.
 * @param type The entity to test for.
 * @return True if the entity owns the entity, false if not.
 */
#define ecs_owns_entity(world, entity, has, owned)\
    ecs_type_owns_entity(world, ecs_get_type(world, entity), has, owned)


////////////////////////////////////////////////////////////////////////////////
//// Entity information
////////////////////////////////////////////////////////////////////////////////

/** Get the type of an entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The type of the entity, NULL if the entity has no components.
 */
FLECS_EXPORT
ecs_type_t ecs_get_type(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Get the name of an entity.
 * This will return the name as specified in the EcsName component.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The type of the entity, NULL if the entity has no name.
 */
FLECS_EXPORT
const char* ecs_get_name(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Get the parent of an entity.
 * This will return a parent of the entity that has the specified component. If
 * the component is 0, the operation will return the first parent that it finds
 * in the entity type (an entity with a CHILDOF role).
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity id of a component that the parent must have.
 * @return The parent of the entity, 0 if no parent was found.
 */
FLECS_EXPORT
ecs_entity_t ecs_get_parent_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component);

/** Get the parent of an entity.
 * Same as ecs_get_parent_w_entity but accepts a component typename.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component A component that the parent must have.
 * @return The parent of the entity, 0 if no parent was found.
 */
#define ecs_get_parent(world, entity, component)\
    ecs_get_parent_w_entity(world, entity, ecs_entity(component))


////////////////////////////////////////////////////////////////////////////////
//// Enabling / disabling entities
////////////////////////////////////////////////////////////////////////////////

/** Enable or disable an entity.
 * This operation enables or disables an entity by adding or removing the
 * EcsDisabled tag. A disabled entity will not be matched with any systems,
 * unless the system explicitly specifies the EcsDisabled tag.
 *
 * @param world The world.
 * @param entity The entity to enable or disable.
 * @param enabled true to enable the entity, false to disable.
 */
FLECS_EXPORT
void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool enabled);


////////////////////////////////////////////////////////////////////////////////
//// Filter API
////////////////////////////////////////////////////////////////////////////////

/** Return a filter iterator.
 * A filter iterator lets an application iterate over entities that match the
 * specified filter. If NULL is provided for the filter, the iterator will
 * iterate all tables in the world.
 * 
 * @param world The world.
 * @param filter The filter.
 * @return An iterator that can be used with ecs_filter_next.
 */
FLECS_EXPORT
ecs_iter_t ecs_filter_iter(
    ecs_world_t *world,
    const ecs_filter_t *filter);  

/** Iterate tables matched by filter.
 * This operation progresses the filter iterator to the next table. The 
 * iterator must have been initialized with `ecs_filter_iter`. This operation 
 * must be invoked at least once before interpreting the contents of the 
 * iterator.
 *
 * @param it The iterator
 * @return True if more data is available, false if not.
 */
FLECS_EXPORT
bool ecs_filter_next(
    ecs_iter_t *iter);


////////////////////////////////////////////////////////////////////////////////
//// Query API
////////////////////////////////////////////////////////////////////////////////

/** Create a query.
 * This operation creates a query. Queries are used to iterate over entities
 * that match a signature expression.
 * 
 * Queries are 'persistent' meaning they are registered with
 * the world and continuously matched with new entities (tables). Queries
 * are the fastest way to iterate over entities, as a lot of processing is
 * done when entities are matched, outside of the main loop.
 *
 * Queries are the mechanism used by systems, and as such both accept the
 * same signature expressions, and have similar performance. 
 *
 * Queries, like systems, iterate over component data from the main stage.
 * This means that when an application is iterating a query outside of a system,
 * care must be taken when adding/removing components or creating/deleting
 * entities, as this may corrupt the iteration.
 *
 * When a query is iterated over inside a system normal staging applies, and an
 * application can safely update entities.
 *
 * @param world The world.
 * @param sig The query signature expression.
 * @return The new query.
 */
FLECS_EXPORT
ecs_query_t* ecs_query_new(
    ecs_world_t *world,
    const char *sig);

/** Cleanup a query.
 * This operation frees a query.
 *
 * @param query The query.
 */
FLECS_EXPORT
void ecs_query_free(
    ecs_query_t *query);

/** Return a query iterator.
 * A query iterator lets an application iterate over entities that match the
 * specified query. If a sorting function is specified, the query will check
 * whether a resort is required upon creating the iterator.
 *
 * @param query The query to iterate.
 * @return The query iterator.
 */
FLECS_EXPORT
ecs_iter_t ecs_query_iter(
    ecs_query_t *query);  

/** Iterate over a query.
 * This operation is similar to ecs_query_iter, but starts iterating from a
 * specified offset, and will not iterate more than limit entities.
 *
 * @param query The query to iterate.
 * @param offset The number of entities to skip.
 * @param limit The maximum number of entities to iterate.
 * @return The query iterator.
 */
FLECS_EXPORT
ecs_iter_t ecs_query_iter_page(
    ecs_query_t *query,
    int32_t offset,
    int32_t limit);  

/** Progress the query iterator.
 * This operation progresses the query iterator to the next table. The 
 * iterator must have been initialized with `ecs_query_iter`. This operation 
 * must be invoked at least once before interpreting the contents of the 
 * iterator.
 *
 * @param iter The iterator.
 * @returns True if more data is available, false if not.
 */
FLECS_EXPORT
bool ecs_query_next(
    ecs_iter_t *iter);      

/** Progress the query iterator for a worker thread.
 * This operation is similar to ecs_query_next, but provides the ability to 
 * divide entities up across multiple worker threads. The operation accepts a
 * current thread id and a total thread id, which is used to determine which
 * subset of entities should be assigned to the current thread.
 *
 * Current should be less than total, and there should be as many as total
 * threads. If there are less entities in a table than there are threads, only
 * as many threads as there are entities will iterate that table.
 *
 * @param it The iterator.
 * @param current Thread id of current thread.
 * @param total Total number of threads.
 * @returns True if more data is available, false if not.
 */
bool ecs_query_next_worker(
    ecs_iter_t *it,
    int32_t current,
    int32_t total);

/** Sort the output of a query.
 * This enables sorting of entities across matched tables. As a result of this
 * operation, the order of entities in the matched tables may be changed. 
 * Resorting happens when a query iterator is obtained, and only if the table
 * data has changed.
 *
 * If multiple queries that match the same (sub)set of tables specify different 
 * sorting functions, resorting is likely to happen every time an iterator is
 * obtained, which can significantly slow down iterations.
 *
 * The sorting function will be applied to the specified component. Resorting
 * only happens if that component has changed, or when the entity order in the
 * table has changed. If no component is provided, resorting only happens when
 * the entity order changes.
 *
 * @param world The world.
 * @param query The query.
 * @param component The component used to sort.
 * @param compare The compare function used to sort the components.
 */
FLECS_EXPORT
void ecs_query_order_by(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t component,
    ecs_compare_action_t compare);

/** Group and sort matched tables.
 * Similar yo ecs_query_order_by, but instead of sorting individual entities, this
 * operation only sorts matched tables. This can be useful of a query needs to
 * enforce a certain iteration order upon the tables it is iterating, for 
 * example by giving a certain component or tag a higher priority.
 *
 * The sorting function assigns a "rank" to each type, which is then used to
 * sort the tables. Tables with higher ranks will appear later in the iteration.
 * 
 * Resorting happens when a query iterator is obtained, and only if the set of
 * matched tables for a query has changed. If table sorting is enabled together
 * with entity sorting, table sorting takes precedence, and entities will be
 * sorted within each set of tables that are assigned the same rank.
 *
 * @param world The world.
 * @param query The query.
 * @param component The component used to determine the group rank.
 * @param rank_action The rank action.
 */
FLECS_EXPORT
void ecs_query_group_by(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t component,
    ecs_rank_type_action_t rank_action);

/** Returns whether the query data changed since the last iteration.
 * This operation must be invoked before obtaining the iterator, as this will
 * reset the changed state. The operation will return true after:
 * - new entities have been matched with
 * - matched entities were deleted
 * - matched components were changed
 * 
 * @param query The query.
 * @return true if entities changed, otherwise false.
 */
FLECS_EXPORT
bool ecs_query_changed(
    ecs_query_t *query);

////////////////////////////////////////////////////////////////////////////////
//// Counting entities
////////////////////////////////////////////////////////////////////////////////

/** Count entities that have an entity.
 * Returns the number of entities that have the specified entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The number of entities that have the entity.
 */
FLECS_EXPORT
int32_t ecs_count_entity(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Count entities that have a type.
 * Returns the number of entities that have the specified type.
 *
 * @param world The world.
 * @param type The type.
 * @return The number of entities that have the type.
 */
FLECS_EXPORT
int32_t ecs_count_type(
    ecs_world_t *world,
    ecs_type_t type);

/** Count entities that have a component, type or tag.
 * Returns the number of entities that have the specified component, type or tag.
 *
 * @param world The world.
 * @param type The component, type or tag.
 * @return The number of entities that have the component, type or tag.
 */
#define ecs_count(world, type)\
    ecs_count_type(world, ecs_type(type))

/** Count entities that match a filter.
 * Returns the number of entities that match the specified filter.
 *
 * @param world The world.
 * @param type The type.
 * @return The number of entities that match the specified filter.
 */
FLECS_EXPORT
int32_t ecs_count_w_filter(
    ecs_world_t *world,
    const ecs_filter_t *filter);


////////////////////////////////////////////////////////////////////////////////
//// Looking up entities
////////////////////////////////////////////////////////////////////////////////

/** Lookup an entity by name.
 * Returns an entity that matches the specified name. Only looks for entities in
 * the current scope (root if no scope is provided).
 *
 * @param world The world.
 * @param name The entity name.
 * @return The entity with the specified name, or 0 if no entity was found.
 */
FLECS_EXPORT
ecs_entity_t ecs_lookup(
    ecs_world_t *world,
    const char *name);

/** Lookup a child entity by name.
 * Returns an entity that matches the specified name. Only looks for entities in
 * the provided parent. If no parent is provided, look in the current scope (
 * root if no scope is provided).
 *
 * @param world The world.
 * @param name The entity name.
 * @return The entity with the specified name, or 0 if no entity was found.
 */
FLECS_EXPORT
ecs_entity_t ecs_lookup_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *name);

/** Lookup an entity from a path.
 * Lookup an entity from a provided path, relative to the provided parent. The
 * operation will use the provided separator to tokenize the path expression. If
 * the provided path contains the prefix, the search will start from the root.
 *
 * If the entity is not found in the provided parent, the operation will 
 * continue to search in the parent of the parent, until the root is reached. If
 * the entity is still not found, the lookup will search in the flecs.core
 * scope. If the entity is not found there either, the function returns 0.
 *
 * @param world The world.
 * @param parent The entity from which to resolve the path.
 * @param path The path to resolve.
 * @param sep The path separator.
 * @param prefix The path prefix.
 * @return The entity if found, else 0.
 */
FLECS_EXPORT
ecs_entity_t ecs_lookup_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix);

/** Lookup an entity from a path.
 * Same as ecs_lookup_path_w_sep, but with defaults for the separator and
 * prefix. These defaults are used when looking up identifiers in a type or
 * signature expression.
 *
 * @param world The world.
 * @param parent The entity from which to resolve the path.
 * @param path The path to resolve.
 * @return The entity if found, else 0. 
 */
#define ecs_lookup_path(world, parent, path)\
    ecs_lookup_path_w_sep(world, parent, path, ".", NULL)

/** Lookup an entity from a full path.
 * Same as ecs_lookup_pat, but  searches from the current scope, or root scope
 * if no scope is set.
 *
 * @param world The world.
 * @param path The path to resolve.
 * @return The entity if found, else 0. 
 */
#define ecs_lookup_fullpath(world, path)\
    ecs_lookup_path_w_sep(world, 0, path, ".", NULL)

/** Lookup an entity by its symbol name.
 * This looks up an entity by the symbol name that was provided in EcsName. The
 * operation does not take into account scoping, which means it will search all
 * entities that have an EcsName.
 *
 * This operation can be useful to resolve, for example, a type by its C 
 * identifier, which does not include the Flecs namespacing.
 */
FLECS_EXPORT
ecs_entity_t ecs_lookup_symbol(
    ecs_world_t *world,
    const char *name);


////////////////////////////////////////////////////////////////////////////////
//// Path utilities
////////////////////////////////////////////////////////////////////////////////

/** Get a path identifier for an entity.
 * This operation creates a path that contains the names of the entities from
 * the specified parent to the provided entity, separated by the provided 
 * separator. If no parent is provided the path will be relative to the root. If
 * a prefix is provided, the path will be prefixed by the prefix.
 *
 * If the parent is equal to the provided child, the operation will return an
 * empty string. If a nonzero component is provided, the path will be created by 
 * looking for parents with that component.
 *
 * The returned path should be freed by the application.
 *
 * @param world The world.
 * @param parent The entity from which to create the path.
 * @param child The entity to which to create the path.
 * @param component The component of the parent.
 * @return The relative entity path.
 */
FLECS_EXPORT
char* ecs_get_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    ecs_entity_t component,
    const char *sep,
    const char *prefix);

/** Get a path identifier for an entity.
 * Same as ecs_get_path_w_sep, but with default values for the separator and
 * prefix. These defaults are used throughout Flecs whenever identifiers are
 * used in type or signature expressions.
 *
 * @param world The world.
 * @param parent The entity from which to create the path.
 * @param child The entity to which to create the path.
 * @return The relative entity path.
 */
#define ecs_get_path(world, parent, child)\
    ecs_get_path_w_sep(world, parent, child, 0, ".", NULL)

/** Get a full path for an entity.
 * Same as ecs_get_path, but with default values for the separator and
 * prefix, and the path is created from the current scope, or root if no scope
 * is provided.
 *
 * @param world The world.
 * @param child The entity to which to create the path.
 * @return The entity path.
 */
#define ecs_get_fullpath(world, child)\
    ecs_get_path_w_sep(world, 0, child, 0, ".", NULL)

/** Find or create entity from path.
 * This operation will find or create an entity from a path, and will create any
 * intermediate entities if required. If the entity already exists, no entities
 * will be created.
 *
 * If the path starts with the prefix, then the entity will be created from the
 * root scope.
 *
 * @param world The world.
 * @param parent The entity relative to which the entity should be created.
 * @param path The path to create the entity for.
 * @param sep The separator used in the path.
 * @param prefix The prefix used in the path.
 * @return The entity.
 */
FLECS_EXPORT
ecs_entity_t ecs_new_from_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix);

/** Find or create entity from path.
 * Same as ecs_new_from_path_w_sep, but with defaults for sep and prefix.
 *
 * @param world The world.
 * @param parent The entity relative to which the entity should be created.
 * @param path The path to create the entity for.
 * @return The entity.
 */
#define ecs_new_from_path(world, parent, path)\
    ecs_new_from_path_w_sep(world, parent, path, ".", NULL)

/** Find or create entity from full path.
 * Same as ecs_new_from_path, but entity will be created from the current scope,
 * or root scope if no scope is set.
 *
 * @param world The world.
 * @param path The path to create the entity for.
 * @return The entity.
 */
#define ecs_new_from_fullpath(world, path)\
    ecs_new_from_path_w_sep(world, 0, path, ".", NULL)

/** Add specified path to entity.
 * This operation is similar to ecs_new_from_path, but will instead add the path
 * to an existing entity.
 *
 * If an entity already exists for the path, it will be returned instead.
 *
 * @param world The world.
 * @param entity The entity to which to add the path.
 * @param parent The entity relative to which the entity should be created.
 * @param path The path to create the entity for.
 * @param sep The separator used in the path.
 * @param prefix The prefix used in the path.
 * @return The entity.
 */ 
FLECS_EXPORT
ecs_entity_t ecs_add_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix);

/** Add specified path to entity.
 * Same as ecs_add_from_path_w_sep, but with defaults for sep and prefix.
 *
 * @param world The world.
 * @param entity The entity to which to add the path. 
 * @param parent The entity relative to which the entity should be created.
 * @param path The path to create the entity for.
 * @return The entity.
 */
#define ecs_add_path(world, entity, parent, path)\
    ecs_add_path_w_sep(world, entity, parent, path, ".", NULL)

/** Add specified path to entity.
 * Same as ecs_add_from_path, but entity will be created from the current scope,
 * or root scope if no scope is set.
 *
 * @param world The world.
 * @param entity The entity to which to add the path.
 * @param path The path to create the entity for.
 * @return The entity.
 */
#define ecs_add_fullpath(world, entity, path)\
    ecs_add_path_w_sep(world, entity, 0, path, ".", NULL)


////////////////////////////////////////////////////////////////////////////////
//// Scope API
////////////////////////////////////////////////////////////////////////////////

/** Does entity have children.
 *
 * @param world The world
 * @param entity The entity
 * @return True if the entity has children, false if not.
 */
FLECS_EXPORT
int32_t ecs_get_child_count(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Return a scope iterator.
 * A scope iterator iterates over all the child entities of the specified 
 * parent.
 *
 * @param world The world.
 * @param parent The parent entity for which to iterate the children.
 * @return The iterator.
 */
FLECS_EXPORT
ecs_iter_t ecs_scope_iter(
    ecs_world_t *world,
    ecs_entity_t parent);

/** Return a filtered scope iterator.
 * Same as ecs_scope_iter, but results will be filtered.
 *
 * @param world The world.
 * @param parent The parent entity for which to iterate the children.
 * @return The iterator.
 */
FLECS_EXPORT
ecs_iter_t ecs_scope_iter_w_filter(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_filter_t *filter);

/** Progress the scope iterator.
 * This operation progresses the scope iterator to the next table. The iterator
 * must have been initialized with `ecs_scope_iter`. This operation must be
 * invoked at least once before interpreting the contents of the iterator.
 *
 * @param it The iterator
 * @return True if more data is available, false if not.
 */
FLECS_EXPORT
bool ecs_scope_next(
    ecs_iter_t *it);

/** Set the current scope.
 * This operation sets the scope of the current stage to the provided entity.
 * As a result new entities will be created in this scope, and lookups will be
 * relative to the provided scope.
 *
 * It is considered good practice to restore the scope to the old value.
 *
 * @param world The world.
 * @param scope The entity to use as scope.
 * @return The previous scope.
 */
FLECS_EXPORT
ecs_entity_t ecs_set_scope(
    ecs_world_t *world,
    ecs_entity_t scope);

/** Get the current scope.
 * Get the scope set by ecs_set_scope. If no scope is set, this operation will
 * return 0.
 *
 * @param world The world.
 * @return The current scope.
 */
FLECS_EXPORT
ecs_entity_t ecs_get_scope(
    ecs_world_t *world);

/** Set a name prefix for newly created entities.
 * This is a utility that lets C modules use prefixed names for C types and
 * C functions, while using names for the entity names that do not have the 
 * prefix. The name prefix is currently only used by ECS_COMPONENT.
 *
 * @param world The world.
 * @param prefix The name prefix to use.
 * @return The previous prefix.
 */
FLECS_EXPORT
const char* ecs_set_name_prefix(
    ecs_world_t *world,
    const char *prefix);    

////////////////////////////////////////////////////////////////////////////////
//// Iter API
////////////////////////////////////////////////////////////////////////////////

/** Obtain column data. 
 * This operation is to be used to obtain a component array for a specific 
 * column in the system or query signature. The column is identified by the 
 * provided index. For example, if this is the provided signature:
 * 
 * Position, Velocity
 * 
 * Position is at index 1, and Velocity is at index 2.
 *
 * This operation may return NULL if the column is optional, and the current
 * table does not have the data. Additionally, if the column points to a shared
 * component or a reference, the returned value should be interpreted as a 
 * pointer instead of an array.
 *
 * The provided size must match the size of the component, otherwise the 
 * function may fail.
 * 
 * @param it The iterator.
 * @param size The size of the component.
 * @param column The index identifying the column in a signature.
 * @return A pointer to the column data.
 */
FLECS_EXPORT
void* ecs_column_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t column);

/** Obtain column data. 
 * This operation is similar to ecs_column_w_size, except that it accepts the
 * component typename.
 * 
 * @param it The iterator.
 * @param type The typename of the component for which to obtain the data.
 * @param column The index identifying the column in a signature.
 * @return A pointer to the column data.
 */
#define ecs_column(it, type, column)\
    ((type*)ecs_column_w_size(it, sizeof(type), column))

/** Test if column is owned or not.
 * The following signature shows an example of one owned components and two
 * components that are not owned by the current entity:
 * 
 * Position, PARENT:Velocity, MyEntity:Mass
 * 
 * Position is an owned component. Velocity and Mass both belong to a different
 * entity. This operation will return false for Position, and true for Velocity
 * and Mass. If a component is matched from a prefab, this operation will also
 * return false.
 * 
 * @param it The it parameter passed into the system.
 * @param index The index identifying the column in a system signature.
 * @return True if column is owned, false if column is not.
 */
FLECS_EXPORT
bool ecs_is_owned(
    const ecs_iter_t *it,
    int32_t column);

/** Obtain a single element. 
 * This operation is similar to ecs_column, but instead of an array it obtains
 * a single element from a component array. The advantage of using ecs_element
 * is that a system can be agnostic towards whether a component is owned or not,
 * at the cost of some additional performance overhead.
 *
 * @param it The iterator.
 * @param size The component size.
 * @param column The index identifying the column in a signature.
 * @param row The current row in the table.
 * @return A pointer to the current element.
 */
FLECS_EXPORT
void *ecs_element_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t column,
    int32_t row);

/** Obtain a single element. 
 * Same as ecs_element_w_size, but allows specifying a typename instead of a
 * size.
 *
 * @param it The iterator.
 * @param type The column type.
 * @param column The index identifying the column in a signature.
 * @param row The current row in the table.
 * @return A pointer to the current element.
 */
#define ecs_element(it, type, column, row)\
    ((type*)ecs_element_w_size(it, sizeof(type), column, row))

/** Obtain the source of a signature column.
 * This operation returns the source of a signature column. By default this will
 * return 0 for regular columns, but for columns where the components are
 * provided by entities other than the entity being iterated over, this will
 * return the source of the component.
 * 
 * @param it Pointer to the it object passed into the system callback.
 * @param column The index identifying the column in a signature.
 * @return The source entity for the column. 
 */
FLECS_EXPORT
ecs_entity_t ecs_column_source(
    const ecs_iter_t *it,
    int32_t column);

/** Obtain the entity id of the signature column.
 * This operation returns the entity id of the component or tag used in the
 * system signature. For example, when provided this signature:
 *
 * Position, Velocity
 *
 * ecs_column_entity(world, 1) will return the component handle for Position and
 * ecs_column_entity(world, 2) will return the componnet handle for Velocity.
 * 
 * @param it The iterator.
 * @param column The index identifying the column in a signature.
 * @return The entity id of the signature column.
 */
FLECS_EXPORT
ecs_entity_t ecs_column_entity(
    const ecs_iter_t *it,
    int32_t column);

/** Obtain the type of a column from inside a system. 
 * This operation is equivalent to ecs_column_entity, except that it returns
 * a type, instead of an entity handle. Invoking this function is the same as
 * doing:
 * 
 * ecs_type_from_entity( ecs_column_entity(it, index));
 * 
 * @param it The iterator.
 * @param column The index identifying the column in a signature.
 * @return The type for the specified column, or NULL if failed.
 */ 
FLECS_EXPORT
ecs_type_t ecs_column_type(
    const ecs_iter_t *it,
    int32_t column);

/** Is the column readonly.
 * This operation returns if the column is a readonly column. Readonly columns
 * are marked in the system signature with the [in] modifier. 
 * 
 * @param it Pointer to the it object passed into the system callback.
 * @param column An index identifying the column.
 * @return True if the column is readonly, false otherwise. */
FLECS_EXPORT
bool ecs_is_readonly(
    const ecs_iter_t *it,
    int32_t column);

/** Get type of table that system is currently iterating over. 
 * This will return the type for all entities that are currently being iterated
 * over, until ecs_iter_next is invoked.
 *
 * @param it The iterator.
 * @return The type of the current table.
 */
FLECS_EXPORT
ecs_type_t ecs_iter_type(
    const ecs_iter_t *it);

/** Get component array from table.
 * In some cases an application may require access to the table component arrays
 * directly instead of going through the signature to table mapping. A typical
 * scenario where this would be used is when using a filter iterator, where
 * there is no signature, and thus ecs_column cannot be used.
 *
 * @param it The iterator.
 * @param column The index identifying the column in a table.
 * @return The component array corresponding to the column index.
 */
FLECS_EXPORT
void* ecs_table_column(
    const ecs_iter_t *it,
    int32_t column);

/** Get the size of a table column.
 *
 * @param it The iterator.
 * @param component The component for which to obtain the index. 
 */
FLECS_EXPORT
size_t ecs_table_column_size(
    const ecs_iter_t *it,
    int32_t column_index);

/** Get the index of the table column for a component.
 * 
 * @param it The iterator.
 * @param component The component for which to obtain the index.
 */
int32_t ecs_table_component_index(
    const ecs_iter_t *it,
    ecs_entity_t component);

/** Get a strongly typed pointer to a column (owned or shared). */
#define ECS_COLUMN(it, type, id, column)\
    ECS_ENTITY_VAR(type) = ecs_column_entity(it, column);\
    ECS_TYPE_VAR(type) = ecs_column_type(it, column);\
    type *id = ecs_column(it, type, column);\
    (void)ecs_entity(type);\
    (void)ecs_type(type);\
    (void)id

/** Get a strongly typed pointer to a column (owned or shared). */
#define ECS_CONST_COLUMN(it, type, id, column)\
    const type *id = ecs_const_column(it, type, column)

/** Obtain a handle to the component of a column */
#define ECS_COLUMN_COMPONENT(it, id, column)\
    ECS_ENTITY_VAR(id) = ecs_column_entity(it, column);\
    ECS_TYPE_VAR(id) = ecs_column_type(it, column);\
    (void)ecs_entity(id);\
    (void)ecs_type(id)

/** Obtain a handle to the entity of a column */
#define ECS_COLUMN_ENTITY(it, id, column)\
    ecs_entity_t id = ecs_column_entity(it, column);\
    ECS_TYPE_VAR(id) = ecs_column_type(it, column);\
    (void)id;\
    (void)ecs_type(id)

/** Utility macro for importing all handles for a module from a system column */
#define ECS_IMPORT_COLUMN(it, module, column) \
    module *ecs_module_ptr(module) = ecs_column(it, module, column);\
    ecs_assert(ecs_module_ptr(module) != NULL, ECS_MODULE_UNDEFINED, #module);\
    ecs_assert(!ecs_is_owned(it, column), ECS_COLUMN_IS_NOT_SHARED, NULL);\
    module ecs_module(module) = *ecs_module_ptr(module);\
    module##ImportHandles(ecs_module(module))


////////////////////////////////////////////////////////////////////////////////
//// Staging API
////////////////////////////////////////////////////////////////////////////////

/** Begin staging.
 * When staging is enabled, modifications to entities are stored to a stage.
 * This ensures that arrays are not modified while iterating. Modifications are
 * merged back to the "main stage" when ecs_staging_end is invoked.
 *
 * @param world The world
 * @return Whether world is currently staged.
 */
FLECS_EXPORT
bool ecs_staging_begin(
    ecs_world_t *world);

/** End staging.
 * If any data was staged, this operation will merge that data back to the main
 * stage.
 *
 * @param world The world
 * @return Whether world was staged.
 */
FLECS_EXPORT
bool ecs_staging_end(
    ecs_world_t *world,
    bool is_staged);

/** Manually merge.
 * When automerging is set to false, an application can invoke this operation to
 * force merging all stages.
 *
 * @param world The world.
 */
FLECS_EXPORT
void ecs_merge(
    ecs_world_t *world);

/** Enable / disable automerging.
 * When automerging is enabled, running a pipeline will automatically merge when
 * necessary. With automerging disabled, merging will not happen unless the
 * application manually invokes ecs_merge.
 *
 * @param world The world.
 */
FLECS_EXPORT
void ecs_set_automerge(
    ecs_world_t *world,
    bool auto_merge);

/* Optional modules */
#ifdef FLECS_SYSTEM
#include "flecs/modules/system.h"
#endif
#ifdef FLECS_PIPELINE
#include "flecs/modules/pipeline.h"
#endif
#ifdef FLECS_TIMER
#include "flecs/modules/timer.h"
#endif
#ifdef FLECS_STATS
#include "flecs/modules/stats.h"
#endif

/* Optional addons */
#ifdef FLECS_BULK
#include "flecs/addons/bulk.h"
#endif
#ifdef FLECS_DBG
#include "flecs/addons/dbg.h"
#endif
#ifdef FLECS_MODULE
#include "flecs/addons/module.h"
#endif
#ifdef FLECS_QUEUE
#include "flecs/addons/queue.h"
#endif
#ifdef FLECS_READER_WRITER
#include "flecs/addons/reader_writer.h"
#endif
#ifdef FLECS_SNAPSHOT
#include "flecs/addons/snapshot.h"
#endif

#ifdef __cplusplus
}

#ifndef FLECS_NO_CPP
#ifndef FLECS_LEGACY
#include "flecs/flecs.hpp"
#endif
#endif

#endif

#endif
