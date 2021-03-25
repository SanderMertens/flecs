/**
 * @file flecs.h
 * @brief Flecs public API.
 *
 * This file contains the public API for Flecs.
 */

#ifndef FLECS_H
#define FLECS_H

/* FLECS_LEGACY should be defined when building for C89 */
// #define FLECS_LEGACY

/* FLECS_NO_DEPRECATED_WARNINGS disables deprecated warnings */
#define FLECS_NO_DEPRECATED_WARNINGS

/* FLECS_NO_CPP should be defined when building for C++ without the C++ API */
// #define FLECS_NO_CPP

/* FLECS_CUSTOM_BUILD should be defined when manually selecting features */
// #define FLECS_CUSTOM_BUILD

/* If this is a regular, non-custom build, build all modules and addons. */
#ifndef FLECS_CUSTOM_BUILD
/* Modules */
#define FLECS_SYSTEM
#define FLECS_PIPELINE
#define FLECS_TIMER

/* Addons */
#define FLECS_BULK
#define FLECS_DBG
#define FLECS_MODULE
#define FLECS_QUEUE
#define FLECS_READER_WRITER
#define FLECS_SNAPSHOT
#define FLECS_DIRECT_ACCESS
#define FLECS_STATS
#endif // ifndef FLECS_CUSTOM_BUILD

/* Unconditionally include deprecated definitions until the rest of the codebase
 * has caught up */
#define FLECS_DEPRECATED

/* Set to double or int to increase accuracy of time keeping. Note that when
 * using an integer type, an application has to provide the delta_time values
 * to the progress() function, as the code that measures time requires a
 * floating point type. */
#ifndef FLECS_FLOAT
#define FLECS_FLOAT float
#endif // FLECS_FLOAT

#include "flecs/private/api_defines.h"
#include "flecs/private/vector.h"        /* Vector datatype */
#include "flecs/private/sparse.h"        /* Sparse set */
#include "flecs/private/bitset.h"        /* Bitset */
#include "flecs/private/map.h"           /* Hashmap */
#include "flecs/private/switch_list.h"   /* Switch list */
#include "flecs/private/strbuf.h"        /* Efficient string builder */
#include "flecs/os_api.h"  /* Abstraction for operating system functions */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup api_types Basic API types
 * @{
 */

/** An entity identifier. */
typedef uint64_t ecs_entity_t;

/** An id. Ids are the things that can be added to an entity. An id can be an
 * entity or pair, and can have an optional role. */
typedef uint64_t ecs_id_t;

/** A vector containing component identifiers used to describe a type. */
typedef const ecs_vector_t* ecs_type_t;

/** An ECS world is the container for all ECS data and supporting features. */
typedef struct ecs_world_t ecs_world_t;

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

/** Type that contains information about the world. */
typedef struct ecs_world_info_t {
    ecs_entity_t last_component_id;   /**< Last issued component entity id */
    ecs_entity_t last_id;             /**< Last issued entity id */
    ecs_entity_t min_id;              /**< First allowed entity id */
    ecs_entity_t max_id;              /**< Last allowed entity id */

    FLECS_FLOAT delta_time_raw;      /**< Raw delta time (no time scaling) */
    FLECS_FLOAT delta_time;          /**< Time passed to or computed by ecs_progress */
    FLECS_FLOAT time_scale;          /**< Time scale applied to delta_time */
    FLECS_FLOAT target_fps;          /**< Target fps */
    FLECS_FLOAT frame_time_total;    /**< Total time spent processing a frame */
    FLECS_FLOAT system_time_total;   /**< Total time spent in systems */
    FLECS_FLOAT merge_time_total;    /**< Total time spent in merges */
    FLECS_FLOAT world_time_total;    /**< Time elapsed in simulation */
    FLECS_FLOAT world_time_total_raw; /**< Time elapsed in simulation (no scaling) */
    
    int32_t frame_count_total;  /**< Total number of frames */
    int32_t merge_count_total;  /**< Total number of merges */
    int32_t pipeline_build_count_total; /**< Total number of pipeline builds */
    int32_t systems_ran_frame;  /**< Total number of systems ran in last frame */
} ecs_world_info_t;

/** @} */

/**
 * @defgroup function_types Function Types
 * @{
 */

/** Action callback for systems and triggers */
typedef void (*ecs_iter_action_t)(
    ecs_iter_t *it);

typedef bool (*ecs_iter_next_action_t)(
    ecs_iter_t *it);

/** Compare callback used for sorting */
typedef int (*ecs_compare_action_t)(
    ecs_entity_t e1,
    const void *ptr1,
    ecs_entity_t e2,
    const void *ptr2);    

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

/** @} */


/**
 * @defgroup builtin_types Builtin Types
 * @{
 */

/** Entity name. */
typedef struct EcsName {
    const char *value;     /**< Entity name */
    char *symbol;          /**< Optional symbol name, if it differs from name */
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

/** Component used for registering component triggers */
typedef struct EcsTrigger {
    ecs_entity_t kind;
    ecs_iter_action_t action;
    ecs_entity_t component;
    ecs_entity_t self;
    void *ctx;
} EcsTrigger;

/** @} */

/* Only include deprecated definitions if deprecated addon is required */
#ifdef FLECS_DEPRECATED
#include "flecs/addons/deprecated.h"
#endif

/**
 * @defgroup type_roles Type Roles
 * @{
 */

/* Type roles are used to indicate the role of an entity in a type. If no flag
 * is specified, the entity is interpreted as a regular component or tag. Flags
 * are added to an entity by using a bitwise OR (|). An example:
 *
 * ecs_entity_t parent = ecs_new(world, 0);
 * ecs_entity_t child = ecs_add_pair(world, e, EcsChildOf, parent);
 *
 * Type flags can also be used in type expressions, without the ECS prefix:
 *
 * ECS_ENTITY(world, Base, Position);
 * ECS_TYPE(world, InstanceOfBase, INSTANCEOF | Base);
 */

/** Role bit added to roles to differentiate between roles and generations */
#define ECS_ROLE (1ull << 63)

/** Cases are used to switch between mutually exclusive components */
#define ECS_CASE (ECS_ROLE | (0x7Cull << 56))

/** Switches allow for fast switching between mutually exclusive components */
#define ECS_SWITCH (ECS_ROLE | (0x7Bull << 56))

/** The PAIR role indicates that the entity is a pair identifier. */
#define ECS_PAIR (ECS_ROLE | (0x7Aull << 56))

/** Enforce ownership of a component */
#define ECS_OWNED (ECS_ROLE | (0x75ull << 56))

/** Track whether component is enabled or not */
#define ECS_DISABLED (ECS_ROLE | (0x74ull << 56))

/** @} */

/**
 * @defgroup builtin_tags Builtin Tags
 * @{
 */

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

/* Ids used by rule solver */
#define EcsWildcard (ECS_HI_COMPONENT_ID + 27)
#define EcsThis (ECS_HI_COMPONENT_ID + 28)
#define EcsTransitive (ECS_HI_COMPONENT_ID + 29)
#define EcsFinal (ECS_HI_COMPONENT_ID + 30)

/* Builtin relationships */
#define EcsChildOf (ECS_HI_COMPONENT_ID + 31)
#define EcsIsA (ECS_HI_COMPONENT_ID + 32)

/* Value used to quickly check if component is builtin. This is used to quickly
 * filter out tables with builtin components (for example for ecs_delete) */
#define EcsLastInternalComponentId (ecs_id(EcsSystem))

/* The first user-defined component starts from this id. Ids up to this number
 * are reserved for builtin components */
#define EcsFirstUserComponentId (32)

/* The first user-defined entity starts from this id. Ids up to this number
 * are reserved for builtin components */
#define EcsFirstUserEntityId (ECS_HI_COMPONENT_ID + 32)

/** @} */

/**
 * @defgroup convenience_macros Convenience Macro's
 * @{
 */

/* Macro's rely on variadic arguments which are C99 and above */
#ifndef FLECS_LEGACY

/** Declare a component.
 * Example:
 *   ECS_COMPONENT(world, Position);
 */
#ifndef ECS_COMPONENT
#define ECS_COMPONENT(world, id) \
    ecs_id_t ecs_id(id) = ecs_new_component(world, 0, #id, sizeof(id), ECS_ALIGNOF(id));\
    (void)ecs_id(id);
#endif

/** Declare an extern component variable.
 * Use this macro in a header when defining a component identifier globally.
 * Must be used together with ECS_COMPONENT_DECLARE.
 *
 * Example:
 *   ECS_COMPONENT_EXTERN(Position);
 */
#ifndef ECS_COMPONENT_EXTERN
#define ECS_COMPONENT_EXTERN(id)\
    extern ecs_id_t ecs_id(id);
#endif

/** Declare a component variable outside the scope of a function.
 * Use this macro in a header when defining a component identifier globally.
 * Must be used together with ECS_COMPONENT_DEFINE.
 *
 * Example:
 *   ECS_COMPONENT_IMPL(Position);
 */
#ifndef ECS_COMPONENT_DECLARE 
#define ECS_COMPONENT_DECLARE(id)\
    ecs_id_t ecs_id(id);
#endif

/** Define a component, store in variable outside of the current scope.
 * Use this macro in a header when defining a component identifier globally.
 * Must be used together with ECS_COMPONENT_DECLARE.
 *
 * Example:
 *   ECS_COMPONENT_DEFINE(world, Position);
 */
#ifndef ECS_COMPONENT_DEFINE 
#define ECS_COMPONENT_DEFINE(world, id)\
    ecs_id(id) = ecs_new_component(world, ecs_id(id), #id, sizeof(id), ECS_ALIGNOF(id));
#endif

/** Declare a tag.
 * Example:
 *   ECS_TAG(world, MyTag);
 */
#ifndef ECS_TAG
#define ECS_TAG(world, id)\
    ECS_ENTITY(world, id, 0);
#endif

/** Declare an extern tag variable.
 * Use this macro in a header when defining a tag identifier globally.
 * Must be used together with ECS_TAG_DECLARE.
 *
 * Example:
 *   ECS_TAG_EXTERN(Enemy);
 */
#ifndef ECS_TAG_EXTERN
#define ECS_TAG_EXTERN(id)\
    extern ecs_entity_t id;
#endif

/** Declare a tag variable outside the scope of a function.
 * Use this macro in a header when defining a tag identifier globally.
 * Must be used together with ECS_TAG_DEFINE.
 *
 * Example:
 *   ECS_TAG_DECLARE(Enemy);
 */
#ifndef ECS_TAG_DECLARE 
#define ECS_TAG_DECLARE(id)\
    ecs_entity_t id;
#endif

/** Define a tag, store in variable outside of the current scope.
 * Use this macro in a header when defining a tag identifier globally.
 * Must be used together with ECS_TAG_DECLARE.
 *
 * Example:
 *   ECS_TAG_DEFINE(world, Enemy);
 */
#ifndef ECS_TAG_DEFINE  
#define ECS_TAG_DEFINE(world, id)\
    id = ecs_new_entity(world, id, #id, 0);
#endif

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

/** @} */

/**
 * @defgroup world_api World API
 * @{
 */

/** Create a new world.
 * A world manages all the ECS data and supporting infrastructure. Applications 
 * must have at least one world. Entities, component and system handles are 
 * local to a world and should not be shared between worlds.
 * 
 * This operation creates a world with all builtin modules loaded. 
 *
 * @return A new world object
 */
FLECS_API
ecs_world_t* ecs_init(void);

/** Same as ecs_init, but with minimal set of modules loaded.
 *
 * @return A new world object
 */
FLECS_API
ecs_world_t* ecs_mini(void);

/** Create a new world with arguments.
 * Same as ecs_init, but allows passing in command line arguments. These can be
 * used to dynamically enable flecs features to an application. Currently these
 * arguments are not used.
 *
 * @return A new world object
 */
FLECS_API
ecs_world_t* ecs_init_w_args(
    int argc,
    char *argv[]);

/** Delete a world.
 * This operation deletes the world, and everything it contains.
 *
 * @param world The world to delete.
 * @return Zero if successful, non-zero if failed.
 */
FLECS_API
int ecs_fini(
    ecs_world_t *world);

/** Register action to be executed when world is destroyed.
 * Fini actions are typically used when a module needs to clean up before a
 * world shuts down.
 * 
 * @param world The world.
 * @param action The function to execute.
 * @param ctx Userdata to pass to the function */
FLECS_API
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
FLECS_API
void ecs_run_post_frame(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx);    

/** Signal exit
 * This operation signals that the application should quit. It will cause
 * ecs_progress to return false.
 *
 * @param world The world to quit.
 */
FLECS_API
void ecs_quit(
    ecs_world_t *world);

/** Return whether a quit has been signaled.
 *
 * @param world The world.
 */
FLECS_API 
bool ecs_should_quit(
    const ecs_world_t *world);

/** Register ctor, dtor, copy & move actions for component.
 *
 * @param world The world.
 * @param component The component id for which to register the actions
 * @param actions Type that contains the component actions.
 */
FLECS_API
void ecs_set_component_actions_w_id(
    ecs_world_t *world,
    ecs_id_t id,
    EcsComponentLifecycle *actions);

#ifndef FLECS_LEGACY
#define ecs_set_component_actions(world, component, ...)\
    ecs_set_component_actions_w_id(world, ecs_id(component), &(EcsComponentLifecycle)__VA_ARGS__)
#endif

/** Set a world context.
 * This operation allows an application to register custom data with a world
 * that can be accessed anywhere where the application has the world object.
 *
 * @param world The world.
 * @param ctx A pointer to a user defined structure.
 */
FLECS_API
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
FLECS_API
void* ecs_get_context(
    const ecs_world_t *world);

/** Get world info.
 *
 * @param world The world.
 * @return Pointer to the world info. This pointer will remain valid for as long
 *         as the world is valid.
 */
FLECS_API
const ecs_world_info_t* ecs_get_world_info(
    const ecs_world_t *world);

/** Dimension the world for a specified number of entities.
 * This operation will preallocate memory in the world for the specified number
 * of entities. Specifying a number lower than the current number of entities in
 * the world will have no effect. Note that this function does not allocate
 * memory for components (use ecs_dim_type for that).
 *
 * @param world The world.
 * @param entity_count The number of entities to preallocate.
 */
FLECS_API
void ecs_dim(
    ecs_world_t *world,
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
FLECS_API
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
FLECS_API
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
FLECS_API
bool ecs_enable_locking(
    ecs_world_t *world,
    bool enable);

/** Locks the world.
 * See ecs_enable_locking for details.
 *
 * @param world The world.
 */
FLECS_API
void ecs_lock(
    ecs_world_t *world);

/** Unlocks the world.
 * See ecs_enable_locking for details.
 * 
 * @param world The world.
 */
FLECS_API
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
FLECS_API 
void ecs_begin_wait(
    ecs_world_t *world);

/** Release world after calling ecs_begin_wait.
 * This operation should be invoked after invoking ecs_begin_wait, and will
 * release the world back to the thread running the main loop.
 *
 * @param world The world.
 */
FLECS_API 
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
FLECS_API
void ecs_tracing_enable(
    int level);

/** Measure frame time. 
 * Frame time measurements measure the total time passed in a single frame, and 
 * how much of that time was spent on systems and on merging.
 *
 * Frame time measurements add a small constant-time overhead to an application.
 * When an application sets a target FPS, frame time measurements are enabled by
 * default.
 *
 * @param world The world.
 * @param enable Whether to enable or disable frame time measuring.
 */
FLECS_API void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable);

/** Measure system time. 
 * System time measurements measure the time spent in each system.
 *
 * System time measurements add overhead to every system invocation and 
 * therefore have a small but measurable impact on application performance.
 * System time measurements must be enabled before obtaining system statistics.
 *
 * @param world The world.
 * @param enable Whether to enable or disable system time measuring.
 */
FLECS_API void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable);   

/** Set target frames per second (FPS) for application.
 * Setting the target FPS ensures that ecs_progress is not invoked faster than
 * the specified FPS. When enabled, ecs_progress tracks the time passed since
 * the last invocation, and sleeps the remaining time of the frame (if any).
 *
 * This feature ensures systems are ran at a consistent interval, as well as
 * conserving CPU time by not running systems more often than required.
 *
 * Note that ecs_progress only sleeps if there is time left in the frame. Both
 * time spent in flecs as time spent outside of flecs are taken into
 * account.
 *
 * @param world The world.
 * @param fps The target FPS.
 */
FLECS_API
void ecs_set_target_fps(
    ecs_world_t *world,
    FLECS_FLOAT fps);     

/** Get current number of threads. */
FLECS_API
int32_t ecs_get_threads(
    ecs_world_t *world);

/** @} */

/**
 * @defgroup creating_entities Creating Entities
 * @{
 */

/** Create new entity id.
 * This operation returns an unused entity id.
 *
 * @param world The world.
 * @return The new entity id.
 */
FLECS_API
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
FLECS_API
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
FLECS_API
ecs_entity_t ecs_new_w_id(
    ecs_world_t *world,
    ecs_id_t id);

/** Create a new entity.
 * This operation creates a new entity with a single component in its type. This
 * operation accepts variables created with ECS_COMPONENT, ECS_TYPE and ECS_TAG.
 * This operation recycles ids.
 * 
 * @param world The world.
 * @param component The component.
 * @return The new entity.
 */
#ifndef ecs_new
#define ecs_new(world, type) ecs_new_w_id(world, ecs_id(type))
#endif

/** Create N new entities.
 * This operation is the same as ecs_new_w_id, but creates N entities
 * instead of one and does not recycle ids.
 * 
 * @param world The world.
 * @param entity The entity.
 * @param count The number of entities to create.
 * @return The first entity id of the newly created entities.
 */
FLECS_API
const ecs_entity_t* ecs_bulk_new_w_id(
    ecs_world_t *world,
    ecs_id_t id,
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
FLECS_API
const ecs_entity_t* ecs_bulk_new_w_data(
    ecs_world_t *world,
    int32_t count,
    const ecs_entities_t *component_ids,
    void *data);

/** Create N new entities.
 * This operation is the same as ecs_new, but creates N entities
 * instead of one and does not recycle ids.
 * 
 * @param world The world.
 * @param component The component type.
 * @param count The number of entities to create.
 * @return The first entity id of the newly created entities.
 */
#ifndef ecs_bulk_new
#define ecs_bulk_new(world, component, count)\
    ecs_bulk_new_w_id(world, ecs_id(component), count)
#endif

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
FLECS_API
ecs_entity_t ecs_clone(
    ecs_world_t *world,
    ecs_entity_t dst,
    ecs_entity_t src,
    bool copy_value);

/** @} */

/**
 * @defgroup adding_removing Adding & Removing
 * @{
 */

/** Add an entity to an entity.
 * This operation adds a single entity to the type of an entity. Type roles may
 * be used in combination with the added entity. If the entity already has the
 * entity, this operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id to add.
 */
FLECS_API
void ecs_add_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

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
#ifndef ecs_add 
#define ecs_add(world, entity, component)\
    ecs_add_id(world, entity, ecs_id(component))
#endif

/** Remove an entity from an entity.
 * This operation removes a single entity from the type of an entity. Type roles
 * may be used in combination with the added entity. If the entity does not have
 * the entity, this operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id to remove.
 */
FLECS_API
void ecs_remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

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
#ifndef ecs_remove
#define ecs_remove(world, entity, type)\
    ecs_remove_id(world, entity, ecs_id(type))
#endif

/** @} */


/**
 * @defgroup enabling_disabling Enabling & Disabling components.
 * @{
 */

/** Enable or disable component.
 * Enabling or disabling a component does not add or remove a component from an
 * entity, but prevents it from being matched with queries. This operation can
 * be useful when a component must be temporarily disabled without destroying
 * its value. It is also a more performant operation for when an application
 * needs to add/remove components at high frequency, as enabling/disabling is
 * cheaper than a regular add or remove.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The component.
 * @param enable True to enable the component, false to disable.
 */
FLECS_API 
void ecs_enable_component_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable);

#define ecs_enable_component(world, entity, T, enable)\
    ecs_enable_component_w_id(world, entity, ecs_id(T), enable)

/** Test if component is enabled.
 * Test whether a component is currently enabled or disabled. This operation
 * will return true when the entity has the component and if it has not been
 * disabled by ecs_enable_component.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The component.
 * @return True if the component is enabled, otherwise false.
 */
FLECS_API 
bool ecs_is_component_enabled_w_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

#define ecs_is_component_enabled(world, entity, T)\
    ecs_is_component_enabled_w_id(world, entity, ecs_id(T))

/** @} */


/**
 * @defgroup pairs Pairs
 * @{
 */

/** Create entity with pair.
 * This operation creates a new entity with a pair. A pair is a combination of a 
 * relation and an object, can can be used to store relationships between
 * entities. Example:
 *
 * subject = Alice, relation = Likes, object = Bob
 *
 * This operation accepts regular entities. For passing in component identifiers
 * use ecs_typeid, like this:
 *
 * ecs_new_w_pair(world, ecs_id(relation), object) 
 *
 * @param world The world.
 * @param relation The relation part of the pair to add.
 * @param object The object part of the pair to add.
 * @return The new entity.
 */
#define ecs_new_w_pair(world, relation, object)\
    ecs_new_w_id(world, ecs_pair(relation, object))

/** Add a pair.
 * This operation adds a pair to an entity. A pair is a combination of a 
 * relation and an object, can can be used to store relationships between
 * entities. Example:
 *
 * subject = Alice, relation = Likes, object = Bob
 *
 * This operation accepts regular entities. For passing in component identifiers
 * use ecs_typeid, like this:
 *
 * ecs_add_pair(world, subject, ecs_id(relation), object) 
 *
 * @param world The world.
 * @param subject The entity to which to add the pair.
 * @param relation The relation part of the pair to add.
 * @param object The object part of the pair to add.
 */
#define ecs_add_pair(world, subject, relation, object)\
    ecs_add_id(world, subject, ecs_pair(relation, object))

/** Remove a pair.
 * This operation removes a pair from an entity. A pair is a combination of a 
 * relation and an object, can can be used to store relationships between
 * entities. Example:
 *
 * subject = Alice, relation = Likes, object = Bob
 *
 * This operation accepts regular entities. For passing in component identifiers
 * use ecs_typeid, like this:
 *
 * ecs_remove_pair(world, subject, ecs_id(relation), object)
 *
 * @param world The world.
 * @param subject The entity from which to remove the pair.
 * @param relation The relation part of the pair to remove.
 * @param object The object part of the pair to remove.
 */
#define ecs_remove_pair(world, subject, relation, object)\
    ecs_remove_id(world, subject, ecs_pair(relation, object))

/** Test for a pair.
 * This operation tests if an entity has a pair. This operation accepts regular 
 * entities. For passing in component identifiers use ecs_typeid, like this:
 *
 * ecs_has_pair(world, subject, ecs_id(relation), object)
 *
 * @param world The world.
 * @param subject The entity from which to remove the pair.
 * @param relation The relation part of the pair to remove.
 * @param object The object part of the pair to remove.
 */
#define ecs_has_pair(world, subject, relation, object)\
    ecs_has_id(world, subject, ecs_pair(relation, object))


#ifndef FLECS_LEGACY

/** Set relation of pair.
 * This operation sets data for a pair, where the relation determines the type.
 * A pair is a combination of a relation and an object, can can be used to store 
 * relationships between entities.
 *
 * Pairs can contain data if either the relation or object of the pair are a
 * component. If both are a component, the relation takes precedence.
 *
 * If this operation is used with a pair where the relation is not a component,
 * it will fail. The object part of the pair expects a regular entity. To pass
 * a component as object, use ecs_typeid like this:
 *
 * ecs_set_pair(world, subject, relation, ecs_id(object))
 *
 * @param world The world.
 * @param subject The entity on which to set the pair.
 * @param relation The relation part of the pair. This must be a component.
 * @param object The object part of the pair.
 */
#define ecs_set_pair(world, subject, relation, object, ...)\
    ecs_set_ptr_w_id(world, subject,\
        ecs_pair(ecs_id(relation), object),\
        sizeof(relation), &(relation)__VA_ARGS__)


/** Set object of pair.
 * This operation sets data for a pair, where the object determines the type.
 * A pair is a combination of a relation and an object, can can be used to store 
 * relationships between entities.
 *
 * Pairs can contain data if either the relation or object of the pair are a
 * component. If both are a component, the relation takes precedence.
 *
 * If this operation is used with a pair where the object is not a component,
 * it will fail. The relation part of the pair expects a regular entity. To pass
 * a component as relation, use ecs_typeid like this:
 *
 * ecs_set_pair_object(world, subject, ecs_id(relation), object)
 *
 * @param world The world.
 * @param subject The entity.
 * @param relation The relation part of the pair.
 * @param object The object part of the pair. This must be a component.
 */
#define ecs_set_pair_object(world, subject, relation, object, ...)\
    ecs_set_ptr_w_id(world, subject,\
        ecs_pair(relation, ecs_id(object)),\
        sizeof(object), &(object)__VA_ARGS__)

#define ecs_get_mut_pair(world, subject, relation, object, is_added)\
    ((relation*)ecs_get_mut_w_id(world, subject,\
        ecs_pair(ecs_id(relation), object), is_added))

#define ecs_get_mut_pair_object(world, subject, relation, object, is_added)\
    ((object*)ecs_get_mut_w_id(world, subject,\
        ecs_pair(relation, ecs_id(object)), is_added))

#define ecs_modified_pair(world, subject, relation, object)\
    ecs_modified_w_id(world, subject, ecs_pair(relation, object))

#endif

/** Get relation of pair. 
 * This operation obtains the value of a pair, where the relation determines the
 * type. A pair is a combination of a relation and an object, can can be used to 
 * store relationships between entities.
 *
 * Pairs can contain data if either the relation or object of the pair are a
 * component. If both are a component, the relation takes precedence.  
 *
 * If this operation is used with a pair where the relation is not a component,
 * it will fail. The object part of the pair expects a regular entity. To pass
 * a component as relation, use ecs_typeid like this: 
 *
 * ecs_get_pair(world, subject, relation, ecs_id(object)) 
 *
 * @param world The world.
 * @param subject The entity.
 * @param relation The relation part of the pair. Must be a component.
 * @param object The object part of the pair.
 */
#define ecs_get_pair(world, subject, relation, object)\
    ((relation*)ecs_get_w_id(world, subject,\
        ecs_pair(ecs_id(relation), object)))

/** Get object of pair. 
 * This operation obtains the value of a pair, where the object determines the
 * type. A pair is a combination of a relation and an object, can can be used to 
 * store relationships between entities.
 *
 * Pairs can contain data if either the relation or object of the pair are a
 * component. If both are a component, the relation takes precedence.  
 *
 * If this operation is used with a pair where the object is not a component,
 * it will fail. The relation part of the pair expects a regular entity. To pass
 * a component as relation, use ecs_typeid like this: 
 *
 * ecs_get_pair_object(world, subject, ecs_id(relation), object)
 *
 * @param world The world.
 * @param subject The entity.
 * @param relation The relation part of the pair. Must be a component.
 * @param object The object part of the pair.
 */
#define ecs_get_pair_object(world, subject, relation, object)\
    ((object*)ecs_get_w_id(world, subject,\
        ecs_pair(relation, ecs_id(object))))

/** @} */


/**
 * @defgroup deleting Deleting Entities and components
 * @{
 */

/** Clear all components.
 * This operation will clear all components from an entity but will not delete
 * the entity itself. This effectively prevents the entity id from being 
 * recycled.
 *
 * @param world The world.
 * @param entity The entity.
 */
FLECS_API
void ecs_clear(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Delete an entity.
 * This operation will delete an entity and all of its components. The entity id
 * will be recycled. Repeatedly calling ecs_delete without ecs_new, 
 * ecs_new_w_id or ecs_new_w_type will cause a memory leak as it will cause
 * the list with ids that can be recycled to grow unbounded.
 *
 * @param world The world.
 * @param entity The entity.
 */
FLECS_API
void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity);


/** Delete children of an entity.
 * This operation deletes all children of a parent entity. If a parent has no
 * children this operation has no effect.
 *
 * @param world The world.
 * @param parent The parent entity.
 */
FLECS_API
void ecs_delete_children(
    ecs_world_t *world,
    ecs_entity_t parent);

/** @} */


/**
 * @defgroup getting Getting Components
 * @{
 */

/** Get an immutable pointer to a component.
 * This operation obtains a const pointer to the requested component. The
 * operation accepts the component entity id.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The entity id of the component to obtain.
 * @return The component pointer, NULL if the entity does not have the component.
 */
FLECS_API
const void* ecs_get_w_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** Get an immutable pointer to a component.
 * Same as ecs_get_w_id, but accepts the typename of a component.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The component to obtain.
 * @return The component pointer, NULL if the entity does not have the component.
 */
#define ecs_get(world, entity, component)\
    ((const component*)ecs_get_w_id(world, entity, ecs_id(component)))

/* -- Get cached pointer -- */

/** Get an immutable reference to a component.
 * This operation is similar to ecs_get_w_id but it stores temporary
 * information in a `ecs_ref_t` value which allows subsequent lookups to be
 * faster.
 *
 * @param world The world.
 * @param ref Pointer to a ecs_ref_t value. Must be initialized.
 * @param entity The entity.
 * @param component The entity id of the component to obtain.
 * @return The component pointer, NULL if the entity does not have the component.
 */
FLECS_API
const void* ecs_get_ref_w_id(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_entity_t entity,
    ecs_id_t id);

/** Get an immutable reference to a component.
 * Same as ecs_get_ref_w_id, but accepts the typename of a component.
 *
 * @param world The world.
 * @param ref Pointer to a ecs_ref_t value. Must be initialized.
 * @param entity The entity.
 * @param id The component to obtain.
 * @return The component pointer, NULL if the entity does not have the component.
 */
#define ecs_get_ref(world, ref, entity, component)\
    ((const component*)ecs_get_ref_w_id(world, ref, entity, ecs_id(component)))

/** Get case for switch.
 * This operation gets the current case for the specified switch. If the current
 * switch is not set for the entity, the operation will return 0.
 *
 * @param world The world.
 * @param e The entity.
 * @param sw The switch for which to obtain the case.
 * @return The current case for the specified switch. 
 */
FLECS_API
ecs_entity_t ecs_get_case(
    const ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t sw);

/** @} */


/**
 * @defgroup setting Setting Components
 * @{
 */

/** Get a mutable pointer to a component.
 * This operation is similar to ecs_get_w_id but it returns a mutable 
 * pointer. If this operation is invoked from inside a system, the entity will
 * be staged and a pointer to the staged component will be returned.
 *
 * If the entity did not yet have the component, the component will be added by
 * this operation. In this case the is_added out parameter will be set to true.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The entity id of the component to obtain.
 * @param is_added Out parameter that returns true if the component was added.
 * @return The component pointer.
 */
FLECS_API
void* ecs_get_mut_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool *is_added); 

/** Get a mutable pointer to a component.
 * Same as ecs_get_mut_w_id but accepts a component typename.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The component to obtain.
 * @param is_added Out parameter that returns true if the component was added.
 * @return The component pointer.
 */
#define ecs_get_mut(world, entity, component, is_added)\
    ((component*)ecs_get_mut_w_id(world, entity, ecs_id(component), is_added))

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
FLECS_API 
void ecs_modified_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** Signal that a component has been modified.
 * Same as ecs_modified_w_id but accepts a component typename.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The component that was modified.
 */
#define ecs_modified(world, entity, component)\
    ecs_modified_w_id(world, entity, ecs_id(component))

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
FLECS_API
ecs_entity_t ecs_set_ptr_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    const void *ptr);

/** Set the value of a component.
 * Same as ecs_set_ptr_w_id, but accepts a component typename and 
 * automatically determines the type size.
 *
 * @param world The world.
 * @param entity The entity.
 * @param component The component to set.
 * @param size The size of the pointer to the value.
 * @return The entity. A new entity if no entity was provided.
 */
#define ecs_set_ptr(world, entity, component, ptr)\
    ecs_set_ptr_w_id(world, entity, ecs_id(component), sizeof(component), ptr)

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
    ecs_set_ptr_w_id(world, entity, ecs_id(component), sizeof(component), &(component)__VA_ARGS__)

#endif

/** @} */


/**
 * @defgroup singleton Singleton components
 * @{
 */

#define ecs_singleton_get(world, comp)\
    ecs_get(world, ecs_id(comp), comp)

#ifndef FLECS_LEGACY
#define ecs_singleton_set(world, comp, ...)\
    ecs_set(world, ecs_id(comp), comp, __VA_ARGS__)
#endif

#define ecs_singleton_get_mut(world, comp)\
    ecs_get_mut(world, ecs_id(comp), comp, NULL)

#define ecs_singleton_modified(world, comp)\
    ecs_modified(world, ecs_id(comp), comp)

/**
 * @defgroup testing Testing Components
 * @{
 */

/** Test if an entity has an entity.
 * This operation returns true if the entity has the provided entity in its 
 * type.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id to test for.
 * @return True if the entity has the entity, false if not.
 */
FLECS_API
bool ecs_has_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** Test if an entity has a component, type or tag.
 * This operation returns true if the entity has the provided component, type or
 * tag in its type.
 *
 * @param world The world.
 * @param entity The entity.
 * @param type The component, type or tag to test for.
 * @return True if the entity has the type, false if not.
 */
#ifndef ecs_has
#define ecs_has(world, entity, type)\
    ecs_has_id(world, entity, ecs_id(type))
#endif

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
#ifndef ecs_owns
#define ecs_owns(world, entity, has, owned)\
    ecs_type_owns_id(world, ecs_get_type(world, entity), has, owned)
#endif

/** @} */

/**
 * @defgroup metadata Entity Metadata
 * @{
 */

/** Test whether an entity is valid.
 * An entity is valid if it is not 0 and if it is alive. If the provided id has
 * a role or a pair, the contents of the role or the pair will be checked for
 * validity.
 *
 * @param world The world.
 * @param e The entity.
 * @return True if the entity is valid, false if the entity is not valid.
 */
FLECS_API
bool ecs_is_valid(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Test whether an entity is alive.
 *
 * @param world The world.
 * @param e The entity.
 * @return True if the entity is alive, false if the entity is not alive.
 */
FLECS_API
bool ecs_is_alive(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Get alive identifier.
 * In some cases an application may need to work with identifiers from which
 * the generation has been stripped. A typical scenario in which this happens is
 * when iterating relationships in an entity type.
 *
 * For example, when obtaining the parent id from a ChildOf relation, the parent
 * (object part of the pair) will have been stored in a 32 bit value, which 
 * cannot store the entity generation. This function can retrieve the identifier
 * with the current generation for that id.
 *
 * If the provided identifier is not alive, the function will return 0.
 *
 * @param world The world.
 * @param e The for which to obtain the current alive entity id.
 * @return The alive entity id if there is one, or 0 if the id is not alive.
 */
FLECS_API
ecs_entity_t ecs_get_alive(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Ensure id is alive.
 * This operation ensures that the provided id is alive. This is useful in
 * scenarios where an application has an existing id that has not been created
 * with ecs_new (such as a global constant or an id from a remote application).
 *
 * Before this operation the id must either not yet exist, or must exist with
 * the same generation as the provided id. If the id has been recycled and the
 * provided id does not have the same generation count, the function will fail.
 *
 * If the provided entity is not alive, and the provided generation count is
 * equal to the current generation (which is the future generation when the id
 * will be recycled) the id will become alive again.
 *
 * If the provided id has a non-zero generation count and the id does not exist
 * in the world, the id will be created with the specified generation.
 *
 * This behavior ensures that an application can use ecs_ensure to track the
 * lifecycle of an id without explicitly having to create it. It also protects
 * against reviving an id with a generation count that was not yet due.
 *
 * @param world The world.
 * @param entity The entity id to make alive.
 */
FLECS_API
void ecs_ensure(
    ecs_world_t *world,
    ecs_entity_t e);

/** Test whether an entity exists.
 * Similar as ecs_is_alive, but ignores entity generation count.
 *
 * @param world The world.
 * @param e The entity.
 * @return True if the entity exists, false if the entity does not exist.
 */
FLECS_API
bool ecs_exists(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Get the type of an entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The type of the entity, NULL if the entity has no components.
 */
FLECS_API
ecs_type_t ecs_get_type(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get the typeid of an entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The typeid of the entity.
 */
FLECS_API
ecs_entity_t ecs_get_typeid(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Get the name of an entity.
 * This will return the name as specified in the EcsName component.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The type of the entity, NULL if the entity has no name.
 */
FLECS_API
const char* ecs_get_name(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Convert type role to string.
 * This operation converts a type role to a string.
 * 
 * @param world The world.
 * @param entity The entity containing the type role.
 * @return The type role string, or NULL if no type role is provided.
 */
FLECS_API
const char* ecs_role_str(
    ecs_entity_t entity);

/** Convert id to string.
 * This operation interprets the structure of an id and converts it to a string.
 *
 * @param world The world.
 * @param id The id to convert to a string.
 * @param buffer The buffer in which to store the string.
 * @param buffer_len The length of the provided buffer.
 * @return The number of characters required to write the string.
 */
FLECS_API
size_t ecs_id_str(
    const ecs_world_t *world,
    ecs_id_t entity,
    char *buffer,
    size_t buffer_len);

/** Get the object of an entity.
 * This will return a object of the entity that has the specified component. If
 * the component is 0, the operation will return the first object that it finds
 * in the entity type.
 *
 * @param world The world.
 * @param entity The entity.
 * @param rel The relation between the entity and the object.
 * @param id The entity id of a component that the object must have.
 * @return The object that has the specified id.
 */
FLECS_API
ecs_entity_t ecs_get_object_w_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    ecs_id_t id);

/** Get the parent of an entity.
 * Same as ecs_get_parent_w_id but accepts a component typename.
 *
 * @param world The world.
 * @param entity The entity.
 * @param rel The relation between the entity and the object.
 * @param component A component that the parent must have.
 * @return The parent of the entity, 0 if no parent was found.
 */
#define ecs_get_object(world, entity, rel, component)\
    ecs_get_parent_w_id(world, entity, ecs_id(component))


/** Enable or disable an entity.
 * This operation enables or disables an entity by adding or removing the
 * EcsDisabled tag. A disabled entity will not be matched with any systems,
 * unless the system explicitly specifies the EcsDisabled tag.
 *
 * @param world The world.
 * @param entity The entity to enable or disable.
 * @param enabled true to enable the entity, false to disable.
 */
FLECS_API
void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool enabled);

/** Count entities that have the specified id.
 * Returns the number of entities that have the specified id.
 *
 * @param world The world.
 * @param entity The id to search for.
 * @return The number of entities that have the id.
 */
FLECS_API
int32_t ecs_count_id(
    const ecs_world_t *world,
    ecs_id_t entity);

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
FLECS_API
int32_t ecs_count_filter(
    const ecs_world_t *world,
    const ecs_filter_t *filter);

/** @} */


/**
 * @defgroup lookup Lookups
 * @{
 */

/** Lookup an entity by name.
 * Returns an entity that matches the specified name. Only looks for entities in
 * the current scope (root if no scope is provided).
 *
 * @param world The world.
 * @param name The entity name.
 * @return The entity with the specified name, or 0 if no entity was found.
 */
FLECS_API
ecs_entity_t ecs_lookup(
    const ecs_world_t *world,
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
FLECS_API
ecs_entity_t ecs_lookup_child(
    const ecs_world_t *world,
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
FLECS_API
ecs_entity_t ecs_lookup_path_w_sep(
    const ecs_world_t *world,
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
FLECS_API
ecs_entity_t ecs_lookup_symbol(
    const ecs_world_t *world,
    const char *name);

/* Add alias for entity to global scope */
FLECS_API
void ecs_use(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name);

/** @} */


/**
 * @defgroup paths Paths
 * @{
 */

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
FLECS_API
char* ecs_get_path_w_sep(
    const ecs_world_t *world,
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
FLECS_API
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
FLECS_API
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

/** @} */


/**
 * @defgroup scopes Scopes
 * @{
 */

/** Does entity have children.
 *
 * @param world The world
 * @param entity The entity
 * @return True if the entity has children, false if not.
 */
FLECS_API
int32_t ecs_get_child_count(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Return a scope iterator.
 * A scope iterator iterates over all the child entities of the specified 
 * parent.
 *
 * @param world The world.
 * @param parent The parent entity for which to iterate the children.
 * @return The iterator.
 */
FLECS_API
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
FLECS_API
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
FLECS_API
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
FLECS_API
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
FLECS_API
ecs_entity_t ecs_get_scope(
    const ecs_world_t *world);

/** Set a name prefix for newly created entities.
 * This is a utility that lets C modules use prefixed names for C types and
 * C functions, while using names for the entity names that do not have the 
 * prefix. The name prefix is currently only used by ECS_COMPONENT.
 *
 * @param world The world.
 * @param prefix The name prefix to use.
 * @return The previous prefix.
 */
FLECS_API
const char* ecs_set_name_prefix(
    ecs_world_t *world,
    const char *prefix);    

/** @} */


/**
 * @defgroup filters Filters
 * @{
 */

/** Return a filter iterator.
 * A filter iterator lets an application iterate over entities that match the
 * specified filter. If NULL is provided for the filter, the iterator will
 * iterate all tables in the world.
 * 
 * @param world The world.
 * @param filter The filter.
 * @return An iterator that can be used with ecs_filter_next.
 */
FLECS_API
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
FLECS_API
bool ecs_filter_next(
    ecs_iter_t *iter);


/** @} */

/**
 * @defgroup queries Queries
 * @{
 */

/** Create a query.
 * This operation creates a query. Queries are used to iterate over entities
 * that match a signature expression and are the fastest way to find and iterate
 * over entities and their components.
 * 
 * Queries should be created once, and reused multiple times. While iterating a
 * query is a cheap operation, creating and deleting a query is expensive. The
 * reason for this is that queries are "prematched", which means that a query
 * stores state about which entities (or rather, tables) match with the query.
 * Building up this state happens during query creation.
 *
 * Once a query is created, matching only happens when new tables are created.
 * In most applications this is an infrequent process, since it only occurs when
 * a new combination of components is introduced. While matching is expensive,
 * it is importent to note that matching does not happen on a per-entity basis,
 * but on a per-table basis. This means that the average time spent on matching
 * per frame should rapidly approach zero over the lifetime of an application.
 *
 * A query provides direct access to the component arrays. When an application
 * creates/deletes entities or adds/removes components, these arrays can shift
 * component values around, or may grow in size. This can cause unexpected or
 * undefined behavior to occur if these operations are performed while 
 * iterating. To prevent this from happening an application should either not
 * perform these operations while iterating, or use deferred operations (see
 * ecs_defer_begin and ecs_defer_end).
 *
 * Queries can be created and deleted dynamically. If a query was not deleted
 * (using ecs_query_free) before the world is deleted, it will be deleted 
 * automatically.
 *
 * @param world The world.
 * @param sig The query signature expression.
 * @return The new query.
 */
FLECS_API
ecs_query_t* ecs_query_new(
    ecs_world_t *world,
    const char *sig);

/** Create a subquery.
 * A subquery is just like a regular query, except that it is matched against 
 * the matched tables of a parent query. Reducing the number of global (normal)
 * queries can improve performance, as new archetypes have to be matched against
 * fewer queries.
 *
 * Subqueries are cheaper to create than regular queries, because the initial
 * set of tables they have to match against is smaller. This makes subqueries
 * more suitable for creating while the simulation.
 *
 * Subqueries are not registered with tables directly, and instead receive new
 * table notifications from their parent query. This means that there is less
 * administrative overhead associated with subqueries.
 *
 * Subqueries can be nested, which allows for the creation of increasingly more
 * specific query hierarchies that are considerably more efficient than when all
 * queries would be created as global queries.
 *
 * @param world The world.
 * @param parent The parent query.
 * @param sig The query signature expression.
 * @return The new subquery.
 */
FLECS_API
ecs_query_t* ecs_subquery_new(
    ecs_world_t *world,
    ecs_query_t *parent,
    const char *sig);    

/** Cleanup a query.
 * This operation frees a query.
 *
 * @param query The query.
 */
FLECS_API
void ecs_query_free(
    ecs_query_t *query);

/** Return a query iterator.
 * A query iterator lets an application iterate over entities that match the
 * specified query. If a sorting function is specified, the query will check
 * whether a resort is required upon creating the iterator.
 *
 * Creating a query iterator is a cheap operation that does not allocate any
 * resources. An application does not need to deinitialize or free a query 
 * iterator before it goes out of scope.
 *
 * To iterate the iterator, an application should use ecs_query_next to progress
 * the iterator and test if it has data.
 *
 * Query iteration requires an outer and an inner loop. The outer loop uses
 * ecs_query_next to test if new tables are available. The inner loop iterates
 * the entities in the table, and is usually a for loop that uses iter.count to
 * loop through the entities and component arrays.
 *
 * The two loops are necessary because of how data is stored internally. 
 * Entities are grouped by the components they have, in tables. A single query 
 * can (and often does) match with multiple tables. Because each table has its
 * own set of arrays, an application has to reobtain pointers to those arrays
 * for each matching table.
 *
 * @param query The query to iterate.
 * @return The query iterator.
 */
FLECS_API
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
FLECS_API
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
FLECS_API
bool ecs_query_next(
    ecs_iter_t *iter);      

/** Progress the query iterator with filter.
 * This operation is the same as ecs_query_next, but accepts a filter as an
 * argument. Entities not matching the filter will be skipped by the iterator.
 *
 * @param iter The iterator.
 * @param filter The filter to apply to the iterator.
 * @returns True if more data is available, false if not.
 */
FLECS_API
bool ecs_query_next_w_filter(
    ecs_iter_t *iter,
    const ecs_filter_t *filter); 

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
 * @param stage_current Id of current stage.
 * @param stage_count Total number of stages.
 * @returns True if more data is available, false if not.
 */
FLECS_API
bool ecs_query_next_worker(
    ecs_iter_t *it,
    int32_t stage_current,
    int32_t stage_count);

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
FLECS_API
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
FLECS_API
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
FLECS_API
bool ecs_query_changed(
    ecs_query_t *query);

/** Returns whether query is orphaned.
 * When the parent query of a subquery is deleted, it is left in an orphaned
 * state. The only valid operation on an orphaned query is deleting it. Only
 * subqueries can be orphaned.
 *
 * @param query The query.
 * @return true if query is orphaned, otherwise false.
 */
FLECS_API
bool ecs_query_orphaned(
    ecs_query_t *query);

/** @} */


/**
 * @defgroup iterator Iterators
 * @{
 */

/** Obtain data for a query term.
 * This operation retrieves a pointer to an array of data that belongs to the
 * term in the query. The index refers to the location of the term in the query,
 * and starts counting from one.
 *
 * For example, the query "Position, Velocity" will return the Position array
 * for index 1, and the Velocity array for index 2.
 *
 * When the specified term is not owned by the entity this function returns a
 * pointer instead of an array. This happens when the source of a term is not
 * the entity being iterated, such as a shared component (from a prefab), a
 * component from a parent, or another entity. The ecs_term_is_owned operation
 * can be used to test dynamically if a term is owned.
 *
 * The provided size must be either 0 or must match the size of the datatype
 * of the returned array. If the size does not match, the operation may assert.
 * The size can be dynamically obtained with ecs_term_size.
 *
 * @param it The iterator.
 * @param size The size of the returned array.
 * @param index The index of the term in the query.
 * @return A pointer to the data associated with the term.
 */
FLECS_API
void* ecs_term_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t index);

/** Same as ecs_term_w_size, but accepts a type instead of a size. */
#define ecs_term(it, T, index)\
    ((T*)ecs_term_w_size(it, sizeof(T), index))

/** Obtain the component/pair id for a term.
 * This operation retrieves the id for the specified query term. Typically this
 * is the component id, but it can also be a pair id or a role annotated id,
 * depending on the term.
 *
 * @param it The iterator.
 * @param index The index of the term in the query.
 * @return The id associated with te term.
 */
FLECS_API
ecs_id_t ecs_term_id(
    const ecs_iter_t *it,
    int32_t index);

/** Obtain the source for a term.
 * This operation retrieves the source of the specified term. A source is the
 * entity from which the data is retrieved. If the term is owned by the iterated
 * over entity/entities, the function will return id 0.
 *
 * This operation can be useful to retrieve, for example, the id of a parent
 * entity when a component from a parent has been requested, or to retrieve the
 * id from a prefab, in the case of a shared component.
 *
 * @param it The iterator.
 * @param index The index of the term in the query.
 * @return The source associated with te term.
 */
FLECS_API
ecs_entity_t ecs_term_source(
    const ecs_iter_t *it,
    int32_t index);

/** Obtain the size for a term.
 * This operation retrieves the size of the datatype for the term.
 *
 * @param it The iterator.
 * @param index The index of the term in the query.
 * @return The size of the datatype associated with te term.
 */
FLECS_API
size_t ecs_term_size(
    const ecs_iter_t *it,
    int32_t index);

/** Test whether the term is readonly
 * This operation returns whether this is a readonly term. Readonly terms are
 * annotated with [in], or are added as a const type in the C++ API.
 *
 * @param it The iterator.
 * @param index The index of the term in the query.
 * @return Whether the term is readonly.
 */
FLECS_API
bool ecs_term_is_readonly(
    const ecs_iter_t *it,
    int32_t index);    

/** Test whether the term is owned
 * This operation returns whether the term is owned by the currently iterated
 * entity. This function will return false when the term is owned by another
 * entity, such as a parent or a prefab.
 *
 * @param it The iterator.
 * @param index The index of the term in the query.
 * @return Whether the term is owned by the iterated over entity/entities.
 */
FLECS_API
bool ecs_term_is_owned(
    const ecs_iter_t *it,
    int32_t index);   

/** Get the type of the currently entity/entities.
 * This operation returns the type of the current iterated entity/entities. A
 * type is a vector that contains all ids of the components that an entity has.
 *
 * @param it The iterator.
 * @return The type of the currently iterated entity/entities.
 */
FLECS_API
ecs_type_t ecs_iter_type(
    const ecs_iter_t *it);

/** Find the column index for a given id.
 * This operation finds the index of a column in the current type for the 
 * specified id. For example, if an entity has type Position, Velocity, and the
 * application requests the id for the Velocity component, this function will
 * return 1.
 *
 * Note that the column index returned by this function starts from 0, as
 * opposed to 1 for the terms. The reason for this is that the returned index
 * is equivalent to using the ecs_type_get_index function, with as type the
 * value returned by ecs_iter_type.
 *
 * This operation can be used to request columns that are not requested by a
 * query. For example, a query may request Position, Velocity, but an entity
 * may also have Mass. With this function the iterator can request the data for
 * Mass as well, when used in combination with ecs_iter_column.
 *
 * @param it The iterator.
 * @return The type of the currently iterated entity/entities.
 */
FLECS_API
int32_t ecs_iter_find_column(
    const ecs_iter_t *it,
    ecs_id_t id);

/** Obtain data for a column index.
 * This operation can be used with the id obtained from ecs_iter_find_column to
 * request data from the currently iterated over entity/entities that is not
 * requested by the query.
 *
 * The data in the returned pointer can be accessed using the same index as
 * the one used to access the arrays returned by the ecs_term function.
 *
 * The provided size must be either 0 or must match the size of the datatype
 * of the returned array. If the size does not match, the operation may assert.
 * The size can be dynamically obtained with ecs_iter_column_size.
 *
 * Note that this function can be used together with ecs_iter_type to 
 * dynamically iterate all data that the matched entities have. An application
 * can use the ecs_vector_count function to obtain the number of elements in a
 * type. All indices from 0..ecs_vector_count(type) are valid column indices.
 *
 * Additionally, note that this provides unprotected access to the column data.
 * An iterator cannot know or prevent accessing columns that are not queried for
 * and thus applications should only use this when it can be guaranteed that
 * there are no other threads reading/writing the same column data.
 *
 * @param it The iterator.
 * @param size The size of the column.
 * @param index The index of the column.
 * @return The data belonging to the column.
 */
FLECS_API
void* ecs_iter_column_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t index);

/** Same as ecs_iter_column_w_size, but accepts a type instead of a size. */
#define ecs_iter_column(it, T, index)\
    ((T*)ecs_iter_column_w_size(it, sizeof(T), index))

/** Obtain size for a column index.
 * This operation obtains the size for a column. The size is equal to the size
 * of the datatype associated with the column.
 *
 * @param it The iterator.
 * @param index The index of the column.
 * @return The size belonging to the column.
 */
FLECS_API
size_t ecs_iter_column_size(
    const ecs_iter_t *it,
    int32_t index);    

/** @} */


/**
 * @defgroup staging Staging
 * @{
 */

/** Begin frame. 
 * When an application does not use ecs_progress to control the main loop, it
 * can still use Flecs features such as FPS limiting and time measurements. This
 * operation needs to be invoked whenever a new frame is about to get processed.
 *
 * Calls to ecs_frame_begin must always be followed by ecs_frame_end.
 *
 * The function accepts a delta_time parameter, which will get passed to 
 * systems. This value is also used to compute the amount of time the function
 * needs to sleep to ensure it does not exceed the target_fps, when it is set.
 * When 0 is provided for delta_time, the time will be measured.
 *
 * This function should only be ran from the main thread.
 *
 * @param world The world.
 * @param delta_time Time elapsed since the last frame.
 * @return The provided delta_time, or measured time if 0 was provided.
 */
FLECS_API
FLECS_FLOAT ecs_frame_begin(
    ecs_world_t *world,
    FLECS_FLOAT delta_time);

/** End frame. 
 * This operation must be called at the end of the frame, and always after
 * ecs_frame_begin.
 *
 * @param world The world.
 */
FLECS_API
void ecs_frame_end(
    ecs_world_t *world);

/** Begin staging.
 * When an application does not use ecs_progress to control the main loop, it
 * can still use Flecs features such as the defer queue. When an application
 * needs to stage changes, it needs to call this function after ecs_frame_begin.
 * A call to ecs_staging_begin must be followed by a call to ecs_staging_end.
 * 
 * When staging is enabled, modifications to entities are stored to a stage.
 * This ensures that arrays are not modified while iterating. Modifications are
 * merged back to the "main stage" when ecs_staging_end is invoked.
 *
 * While the world is in staging mode, no structural changes (add/remove/...)
 * can be made to the world itself. Operations must be executed on a stage
 * instead (see ecs_get_stage).
 *
 * This function should only be ran from the main thread.
 *
 * @param world The world
 * @return Whether world is currently staged.
 */
FLECS_API
bool ecs_staging_begin(
    ecs_world_t *world);

/** End staging.
 * Leaves staging mode. After this operation the world may be directly mutated
 * again. By default this operation also merges data back into the world, unless
 * automerging was disabled explicitly.
 *
 * This function should only be ran from the main thread.
 *
 * @param world The world
 */
FLECS_API
void ecs_staging_end(
    ecs_world_t *world);

/** Merge world or stage.
 * When automatic merging is disabled, an application can call this
 * operation on either an individual stage, or on the world which will merge
 * all stages. This operation may only be called when staging is not enabled
 * (either after progress() or after staging_end()).
 *
 * This operation may be called on an already merged stage or world.
 *
 * @param world The world.
 */
FLECS_API
void ecs_merge(
    ecs_world_t *world);

/** Defer operations until end of frame. 
 * When this operation is invoked while iterating, operations inbetween the
 * defer_begin and defer_end operations are executed at the end of the frame.
 *
 * This operation is thread safe.
 * 
 * @param world The world.
 * @return true if world changed from non-deferred mode to deferred mode.
 */
FLECS_API
bool ecs_defer_begin(
    ecs_world_t *world);

/** End block of operations to defer. 
 * See defer_begin.
 *
 * This operation is thread safe.
 *
 * @param world The world.
 * @return true if world changed from deferred mode to non-deferred mode.
 */
FLECS_API
bool ecs_defer_end(
    ecs_world_t *world);

/** Enable/disable automerging for world or stage.
 * When automerging is enabled, staged data will automatically be merged with
 * the world when staging ends. This happens at the end of progress(), at a
 * sync point or when staging_end() is called.
 *
 * Applications can exercise more control over when data from a stage is merged
 * by disabling automerging. This requires an application to explicitly call
 * merge() on the stage.
 *
 * When this function is invoked on the world, it sets all current stages to
 * the provided value and sets the default for new stages. When this function is
 * invoked on a stage, automerging is only set for that specific stage.
 *
 * @param world The world.
 * @param automerge Whether to enable or disable automerging.
 */
FLECS_API
void ecs_set_automerge(
    ecs_world_t *world,
    bool automerge);

/** Configure world to have N stages.
 * This initializes N stages, which allows applications to defer operations to
 * multiple isolated defer queues. This is typically used for applications with
 * multiple threads, where each thread gets its own queue, and commands are
 * merged when threads are synchronized.
 *
 * Note that the ecs_set_threads function already creates the appropriate
 * number of stages. The set_stages() operation is useful for applications that
 * want to manage their own stages and/or threads.
 * 
 * @param world The world.
 * @param stages The number of stages.
 */
FLECS_API
void ecs_set_stages(
    ecs_world_t *world,
    int32_t stages);

/** Get number of configured stages.
 * Return number of stages set by ecs_set_stages.
 *
 * @param world The world.
 * @return The number of stages used for threading.
 */
FLECS_API
int32_t ecs_get_stage_count(
    const ecs_world_t *world);

/** Get current stage id.
 * The stage id can be used by an application to learn about which stage it is
 * using, which typically corresponds with the worker thread id.
 *
 * @param world The world.
 * @return The stage id.
 */
FLECS_API
int32_t ecs_get_stage_id(
    const ecs_world_t *world);

/** Get stage-specific world pointer.
 * Flecs threads can safely invoke the API as long as they have a private 
 * context to write to, also referred to as the stage. This function returns a
 * pointer to a stage, disguised as a world pointer.
 *
 * Note that this function does not(!) create a new world. It simply wraps the
 * existing world in a thread-specific context, which the API knows how to
 * unwrap. The reason the stage is returned as an ecs_world_t is so that it
 * can be passed transparently to the existing API functions, vs. having to 
 * create a dediated API for threading.
 *
 * @param world The world.
 * @param stage_id The index of the stage to retrieve.
 * @return A thread-specific pointer to the world. 
 */
FLECS_API
ecs_world_t* ecs_get_stage(
    const ecs_world_t *world,
    int32_t stage_id);

/** Get actual world from world.
 * @param world A pointer to a stage or the world.
 * @return The world.
 */
FLECS_API
const ecs_world_t* ecs_get_world(
    const ecs_world_t *world);

/** Test whether the current world object is readonly.
 * This function allows the code to test whether the currently used world object
 * is readonly or whether it allows for writing.  
 *
 * @param world A pointer to a stage or the world.
 * @return True if the world or stage is readonly.
 */
FLECS_API
bool ecs_stage_is_readonly(
    const ecs_world_t *stage);

/** @} */


/**
 * @defgroup table_functions Public table operations
 * @{
 */

/** Find or create table with specified component string. 
 * The provided string must be a comma-separated list of fully qualified 
 * component identifiers. The returned table will have the specified components.
 * Two lists that are the same but specify components in a different order will
 * return the same table.
 *
 * @param world The world.
 * @param type The components.
 * @return The new or existing table, or NULL if the string contains an error.
 */
FLECS_API
ecs_table_t* ecs_table_from_str(
    ecs_world_t *world,
    const char *type);

/** Find or create table from type.
 * Same as ecs_table_from_str, but provides the type directly.
 *
 * @param world The world.
 * @param type The type.
 * @return The new or existing table.
 */
FLECS_API
ecs_table_t* ecs_table_from_type(
    ecs_world_t *world,
    ecs_type_t type);

/** Get type for table.
 *
 * @param table The table.
 * @return The type of the table.
 */
FLECS_API
ecs_type_t ecs_table_get_type(
    const ecs_table_t *table);

/** Insert record into table.
 * This will create a new record for the table, which inserts a value for each
 * component. An optional entity and record can be provided.
 *
 * If a non-zero entity id is provided, a record must also be provided and vice
 * versa. The record must be created by the entity index. If the provided record 
 * is not created for the specified entity, the behavior will be undefined.
 *
 * If the provided record is not managed by the entity index, the behavior will
 * be undefined.
 *
 * The returned record contains a reference to the table and the table row. The
 * data pointed to by the record is guaranteed not to move unless one or more
 * rows are removed from this table. A row can be removed as result of a delete,
 * or by adding/removing components from an entity stored in the table.
 *
 * @param world The world.
 * @param table The table.
 * @param entity The entity.
 * @param record The entity-index record for the specified entity.
 * @return A record containing the table and table row.
 */
FLECS_API
ecs_record_t ecs_table_insert(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    ecs_record_t *record);

/** Returns the number of records in the table. 
 * This operation returns the number of records that have been populated through
 * the regular (entity) API as well as the number of records that have been
 * inserted using the direct access API.
 *
 * @param world The world.
 * @param table The table.
 * @return The number of records in a table.
 */
FLECS_API
int32_t ecs_table_count(
    const ecs_table_t *table);

/** @} */

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
#ifdef FLECS_DIRECT_ACCESS
#include "flecs/addons/direct_access.h"
#endif
#ifdef FLECS_STATS
#include "flecs/addons/stats.h"
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
