
#ifndef REFLECS_H
#define REFLECS_H

#include <stdint.h>
#include <alloca.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* -- Export macro's -- */

#if REFLECS_IMPL && defined _MSC_VER
#define REFLECS_EXPORT __declspec(dllexport)
#elif REFLECS_IMPL
#define REFLECS_EXPORT __attribute__((__visibility__("default")))
#elif defined _MSC_VER
#define REFLECS_EXPORT __declspec(dllimport)
#else
#define REFLECS_EXPORT
#endif

/* The following symbols are used in the time complexity annotations:
 * e: number of entities
 * c: number of components for a specified entity
 * s: number of systems
 * C: average number of components per entity in a world
 * t: number of entities in a matching tables
 * r: combined size of all components for one entity (a 'row')
 */

/* -- The API uses the native bool type in C++, or a custom one in C -- */

#ifndef __cplusplus
#undef bool
#undef true
#undef false
#define bool char
#define false 0
#define true !false
#endif

#define ECS_HANDLE_NIL (0)

#ifdef __cplusplus
extern "C" {
#endif


/* -- Supporting types -- */

typedef struct EcsWorld EcsWorld;

/** A handle identifies an entity */
typedef uint64_t EcsHandle;

/** Function return values */
typedef enum EcsResult {
    EcsOk,
    EcsError
} EcsResult;

/** System kinds determine when and how systems are ran */
typedef enum EcsSystemKind {
    EcsPeriodic,
    EcsOnInit,
    EcsOnDeinit,
    EcsOnDemand
} EcsSystemKind;

/** Data passed to system action callback, used for iterating entities */
typedef struct EcsRows {
    EcsHandle system;
    EcsWorld *world;
    int32_t *columns;
    void *first;
    void *last;
    void **refs;
    void *param;
    EcsHandle *components;
    uint32_t element_size;
    uint32_t column_count;
} EcsRows;

/** System action callback type */
typedef void (*EcsSystemAction)(
    EcsRows *data);

/** Handles to builtin components */
#define EcsComponent_h (1)
#define EcsFamily_h (2)
#define EcsPrefab_h (3)
#define EcsSystem_h (4)
#define EcsId_h (5)


/* -- World API -- */

/** Create a new world.
 * A world manages all the ECS objects. Applications must have at least one
 * world. Entities, component and system handles are local to a world and
 * cannot be shared between worlds.
 *
 * @time-complexity: O(1)
 * @returns A new world object
 */
REFLECS_EXPORT
EcsWorld* ecs_init(void);

/** Delete a world.
 * This operation deletes the world, and all entities, components and systems
 * within the world.
 *
 * @time-complexity: O(n)
 * @param world The world to delete.
 */
REFLECS_EXPORT
EcsResult ecs_fini(
    EcsWorld *world);

/** Progress a world.
 * This operation progresses the world by running all systems that are both
 * enabled and periodic on their matching entities.
 *
 * @time-complexity: O(s * t)
 * @param world The world to progress.
 */
REFLECS_EXPORT
void ecs_progress(
    EcsWorld *world);

/** Set number of worker threads.
 * This operation sets the number of worker threads to which to distribute the
 * processing load. If this function is called multiple times, the total number
 * of threads running will reflect the number specified in the last call.
 *
 * This function should not be called while processing an iteration, but should
 * only be called before or after calling ecs_progress.
 *
 * The initial value is zero, which means that ecs_progress will only use the
 * mainthread.
 *
 * @time-complexity: O(n)
 * @param world: The world.
 * @param threads: The number of threads.
 * @returns: EcsOk if successful, or EcsError if failed.
 */
REFLECS_EXPORT
EcsResult ecs_set_threads(
    EcsWorld *world,
    uint32_t threads);

/** Set a world context.
 * This operation allows an application to register custom data with a world
 * that can be accessed anywhere where the application has the world object.
 *
 * A typical usecase is to register a struct with handles to the application
 * entities, components and systems.
 *
 * @time-complexity: O(1)
 * @param world The world.
 * @param ctx A pointer to a user defined structure.
 */
REFLECS_EXPORT
void ecs_set_context(
    EcsWorld *world,
    void *ctx);

/** Get the world context.
 * This operation retrieves a previously set world context.
 *
 * @time-complexity: O(1)
 * @param world The world.
 * @returns The context set with ecs_set_context. If no context was set, the
 *          function returns NULL.
 */
REFLECS_EXPORT
void* ecs_get_context(
    EcsWorld *world);

/** Dimension the world for a specified number of entities.
 * This operation will preallocate memory in the world for the specified number
 * of entities. Specifying a number lower than the current number of entities in
 * the world will have no effect.
 *
 * When using this operation, note that reflecs uses entities for storing
 * systems, components and builtin components. For an exact calculation of
 * entities, do user_entity_count + component_count + system_count + 3. The 3
 * stands for the number of builtin components.
 *
 * Note that this operation does not allocate memory in tables. To preallocate
 * memory in a table, use ecs_dim_family. Correctly using these functions
 * prevents reflecs from doing dynamic memory allocations in the main loop.
 *
 * @time-complexity: O(1)
 * @param world The world.
 * @param entity_count The number of entities to preallocate.
 */
REFLECS_EXPORT
void ecs_dim(
    EcsWorld *world,
    uint32_t entity_count);

/** Dimension a family for a specified number of entities.
 * This operation will preallocate memory for a family (table) for the
 * specified number of entites. Specifying a number lower than the current
 * number of entities in the table will have no effect.
 *
 * If no table exists yet for this family (when no entities have been committed
 * for the family) it will be created, even if the entity_count is zero. This
 * operation can thus also be used to just preallocate empty tables.
 *
 * If the specified family is unknown, the behavior of this function is
 * unspecified. To ensure that the family exists, use ecs_family_get or
 * ECS_FAMILY.
 *
 * @time-complexity: O(1)
 * @param world The world.
 * @param family Handle to the family, as obtained by ecs_family_get.
 * @param entity_count The number of entities to preallocate.
 */
REFLECS_EXPORT
void ecs_dim_family(
    EcsWorld *world,
    EcsHandle family,
    uint32_t entity_count);

/** Dump contents of world
 * This operation prints current tables and entities to the console.
 *
 * @time-complexity: O(e)
 * @param world The world.
 */
REFLECS_EXPORT
void ecs_dump(
    EcsWorld *world);

/* -- Entity API -- */

/** Create a new entity.
 * Entities are light-weight objects that represent "things" in the application.
 * Entities themselves do not have any state or logic, but instead are composed
 * out of a set of zero or more components.
 *
 * The memory occupied by an entity is the combined size of its components plus
 * approximately 40 bytes. Component lists are shared between entities,
 * thus adding a component to one entity does not necessarily increase
 * footprint. Each active combination of components is stored exactly once.
 *
 * Component data is stored in tables, with one table for each combination of
 * components. An entity is stored in the table that matches its component list.
 * When components are added or removed from an entity, the entity is moved to
 * another table. See ecs_add, ecs_stage and ecs_commit for more information.
 *
 * Entities are accessed through handles instead of direct pointers. Certain
 * operations may move an entity in memory. Handles provide a safe mechanism for
 * addressing entities. The average lookup complexity for a handle is O(1).
 *
 * @time-complexity: O(1)
 * @param world: The world to which to add the entity.
 * @param type: Zero if no type, or handle to a component, family or prefab.
 * @returns: A handle to the new entity.
 */
REFLECS_EXPORT
EcsHandle ecs_new(
    EcsWorld *world,
    EcsHandle type);

/** Create a new prefab entity.
 * Prefab entities allow entities to share a set of components. Components of
 * the prefab will appear on the specified entity when using any of the API
 * functions and ECS systems.
 *
 * A prefab is a regular entity, with the only difference that it has the
 * EcsPrefab component. That means that all the regular API functions like
 * ecs_get_ptr, ecs_add, ecs_commit etc. can be used on prefabs.
 *
 * The ECS_PREFAB macro wraps around this function.
 *
 * Changing the value of one of the components on the prefab will change the
 * value for all entities that added the prefab, as components are stored only
 * once in memory. This makes prefabs also a memory-saving mechanism; there can
 * be many entities that reuse component records from the prefab.
 *
 * Entities can override components from a prefab by adding the component with
 * ecs_add. When a component is overridden, its value will be copied from the
 * prefab. This technique can be combined with families to automatically
 * initialize entities, like this:
 *
 * ECS_PREFAB(world, MyPrefab, Foo);
 * ECS_FAMILY(world, MyFamily, MyPrefab, Foo);
 * EcsHandle e = ecs_new(world, MyFamily);
 *
 * In this code, the entity will be created with the prefab and directly
 * override 'Foo', which will copy the value of 'Foo' from the prefab.
 *
 * Prefabs are explicitly stored on the component list of an entity. This means
 * that two entities with the same set of components but a different prefab are
 * stored in different tables.
 *
 * Prefabs can be part of the component list of other prefabs. This allows for
 * creating hierarchies of prefabs, where the leaves are the most specialized.
 *
 * Only one prefab may be added to an entity.
 */
REFLECS_EXPORT
EcsHandle ecs_new_prefab(
    EcsWorld *world,
    const char *id,
    const char *sig);

/** Delete an existing entity.
 * Deleting an entity in most cases causes the data of another entity to be
 * copied. This happens to prevent memory fragmentation. It means that for
 * entities with lots of components (or few large ones) deleting can be an
 * expensive operation.
 *
 * After this operation the handle will be invalidated and should no longer be
 * used. Handles are not recycled, which implies a finite number of new / delete
 * operations. In the current implementation that number is 2^64 - 1, which
 * should keep code running until the sun runs out of fuel. Possibly longer.
 *
 * The only post condition for this function is that no entity with the
 * specified handle will exist after the operation. If a handle is provided to
 * the function that does not resolve to an entity, this function is a no-op.
 *
 * @time-complexity: O(r)
 * @param world: The world.
 * @param entity: A handle to the entity to delete.
 */
REFLECS_EXPORT
void ecs_delete(
    EcsWorld *world,
    EcsHandle entity);

/** Stage a component for adding.
 * Staging a component will register a component with an entity, but will not
 * yet commit the component to memory. Committing components to memory is an
 * expensive operation, and when adding multiple components this can add up.
 * Staging is relatively cheap, and is therefore the preferred way to add
 * multiple components to an entity at once.
 *
 * After all the components that are to be added are staged, they can be
 * committed to memory with ecs_commit.
 *
 * This operation does not check whether the component handle is valid. If an
 * invalid component handle is provided, the ecs_commit operation will fail.
 *
 * @time-complexity: O(1)
 * @param world: The world.
 * @param entity: Handle to the entity for which to stage the component.
 * @param component: Handle to the component.
 * @returns: EcsOk if succeeded, or EcsError if the operation failed.
 */
REFLECS_EXPORT
EcsResult ecs_add(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component);

/** Stage a component for removing.
 * This operation stages a remove for a component from an entity. The remove
 * will not yet be committed to memory. Similar to ecs_stage, this operation
 * requires calling ecs_commit to actually commit the changes to memory.
 *
 * The post condition for this operation is that the entity will not have
 * the component after invoking ecs_commit. If the entity does not have the
 * component when this operation is called, it is a no-op.
 *
 * @time-complexity: O(1)
 * @param world: The world.
 * @param entity: Handle to the entity from which to remove the component.
 * @param component: The component to remove.
 */
REFLECS_EXPORT
EcsResult ecs_remove(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component);

/** Commit components to memory.
 * This operation commits all staged components to memory. If no components were
 * staged since creation of the entity or since the last ecs_commit, this
 * operation is a no-op. Detection of this scenario is very fast.
 *
 * This operation uses the list of components for this entity to determine which
 * table it should be inserted in. Each active combination of components will
 * have its own table in memory. If this operation commits a new combination,
 * a new table will be created as part of the commit.
 *
 * Once a table is either found or created, a new row is inserted for the
 * specified entity. If the entity was already committed to another table, the
 * entity is moved from that table to the new table. This will likely cause the
 * moving of another entity in the old table, to prevent fragmentation.
 *
 * Matching EcsOnInit and EcsOnDeinit systems will be ran as part of the commit
 * if one or more added / removed components matches their signature. Thus, if
 * a EcsOnInit system is registered for component A and B, and an entity already
 * had A but is now also committing B, the system will be invoked. A quadratic
 * loop is required to determine for which components init / deinit systems have
 * to be invoked. This comparison will be skipped if there are no init / deinit
 * systems on the new / old table.
 *
 * @time-complexity: O(2 * r + c)
 * @param world: The world.
 * @param entity: The entity to commit.
 * @returns: EcsOk if succeeded, or EcsError if the operation failed.
 */
REFLECS_EXPORT
EcsResult ecs_commit(
    EcsWorld *world,
    EcsHandle entity);

/** Get pointer to component data.
 * This operation obtains a pointer to the component data of an entity. If the
 * component was not added for the specified entity, the operation will return
 * NULL.
 *
 * Note that the returned pointer has temporary validity. Operations such as
 * delete and commit may invalidate the pointer as data is potentially moved
 * to different locations. After one of these operations is invoked, the pointer
 * will have to be re-obtained.
 *
 * This function is wrapped by the ecs_get convenience macro, which can be
 * used like this:
 *
 * Foo value = ecs_get(world, e, Foo);
 *
 * @time-complexity: O(c)
 * @param world: The world.
 * @param entity: Handle to the entity from which to obtain the component data.
 * @param component: The component to retrieve the data for.
 * @returns: A pointer to the data, or NULL of the component was not found.
 */
REFLECS_EXPORT
void* ecs_get_ptr(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component);

#define ecs_get(world, entity, component)\
  *(component*)ecs_get_ptr(world, entity, component##_h)

/* Set value of component.
 * This function sets the value of a component on the specified entity. If the
 * component does not yet exist, it will be added to the entity.
 *
 * This function can be used like this:
 * Foo value = {.x = 10, .y = 20};
 * ecs_set_ptr(world, e, Foo_h, &value);
 *
 * This function is wrapped by the ecs_set convenience macro, which can be used
 * like this:
 *
 * ecs_set(world, e, Foo, {.x = 10, .y = 20});
 *
 * @param world: The world.
 * @param entity: The entity on which to set the component.
 * @param component: The component to set.
 */
REFLECS_EXPORT
void ecs_set_ptr(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component,
    void *ptr);

#define ecs_set(world, entity, component, ...)\
  { component v = __VA_ARGS__; ecs_set_ptr(world, entity, component##_h, &v); }

/** Check if entity has the specified type.
 * This operation checks if the entity has the components associated with the
 * specified type. It accepts component handles, families and prefabs.
 *
 * For example, if an entity has component 'Foo' and a family has 'Foo, Bar'
 * invoking this function with the entity and family as type will return false
 * because the entity does not have 'Bar'. Invoking the entity with the 'Bar'
 * component, or a family that contains only 'Bar' will return true.
 *
 * @time-complexity: O(c)
 * @param world: The world.
 * @param entity: Handle to a entity.
 * @param type: Handle to a component, family or prefab.
 * @returns: true if entity has type, otherwise false.
 */
REFLECS_EXPORT
bool ecs_has(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle type);

/** Check if entity has any of the components in the specified type.
 * This operation checks if the entity has any of the components associated with
 * the specified type. It accepts component handles, families and prefabs.
 *
 * For example, if an entity has component 'Foo' and a family has 'Foo, Bar'
 * invoking this function with the entity and family as type will return true
 * because the entity has one of the components.
 *
 * @time-complexity: O(c)
 * @param world: The world.
 * @param entity: Handle to a entity.
 * @param type: Handle to a component, family or prefab.
 * @returns: true if entity has one of the components, otherwise false.
 */
REFLECS_EXPORT
bool ecs_has_any(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle type);

/** Return if the entity is valid.
 * This returns whether the provided entity handle is valid. An entity that has
 * never been returned by ecs_new (or variants) or that has been deleted with
 * ecs_delete is not valid.
 *
 * @param world: The world.
 * @param entity: The entity handle.
 * @returns: true if valid, false if not valid.
 */
REFLECS_EXPORT
bool ecs_valid(
    EcsWorld *world,
    EcsHandle entity);

/* -- Id API -- */

/** Return the entity id.
 * This returns the string identifier of an entity, if the entity has the EcsId
 * component. By default, all component, family, system and prefab entities add
 * the EcsId component if they have been created with the ecs_new_* functions.
 *
 * If the entity does not contain the EcsId component, this function will return
 * NULL.
 *
 * @param world: The world.
 * @param entity: The entity for which to resolve the id.
 * @returns: The id of the entity.
 */
REFLECS_EXPORT
const char* ecs_id(
    EcsWorld *world,
    EcsHandle entity);

/** Lookup an entity by id.
 * This operation is a convenient way to lookup entities by string identifier
 * that have the EcsId component. It is recommended to cache the result of this
 * function, as the function must iterates over all entities and all components
 * in an entity.
 *
 * @time-complexity: O(t * c)
 * @param world The world.
 * @param id The id to lookup.
 * @returns The entity handle if found, or ECS_HANDLE_NIL if not found.
 */
REFLECS_EXPORT
EcsHandle ecs_lookup(
    EcsWorld *world,
    const char *id);


/* -- Component API -- */

/** Create a new component.
 * This operation creates a new component with a specified id and size. After
 * this operation is called, the component can be added to entities by using
 * the returned handle with ecs_stage or ecs_add.
 *
 * Components represent the data of entities. An entity can be composed out of
 * zero or more components. Internally compoments are stored in tables that
 * are created for active combinations of components in a world.
 *
 * This operation accepts a size, which is the size of the type that contains
 * the component data. Any native type can be used, and the size can be
 * obtained with the built-in sizeof function. For convenience, an application
 * may use the ECS_COMPONENT macro instead of calling this function directly.
 *
 * Components are stored internally as entities. This operation is equivalent
 * to creating an entity with the EcsComponent and EcsId components. The
 * returned handle can be used in any function that accepts an entity handle.
 *
 * @time-complexity: O(2 * r + c)
 * @param world: The world.
 * @param id: A unique component identifier.
 * @param size: The size of the component type (as obtained by sizeof).
 * @returns: A handle to the new component, or ECS_HANDLE_NIL if failed.
 */
REFLECS_EXPORT
EcsHandle ecs_new_component(
    EcsWorld *world,
    const char *id,
    size_t size);


/* -- Family API -- */

/** Get handle to family.
 * This operation obtains a handle to a family that can be used with
 * ecs_new. Predefining families has performance benefits over using
 * ecs_new, ecs_stage and ecs_commit separately. It also provides constant
 * creation time regardless of the number of components. This function will
 * internally create a table for the family.
 *
 * If a family had been created for this set of components before with the same
 * identifier, the existing family is returned. If the family had been created
 * with a different identifier, this function will fail.
 *
 * The ECS_FAMILY macro wraps around this function.
 *
 * @time-complexity: O(c)
 * @param world The world.
 * @param components A comma-separated string with the component identifiers.
 * @returns Handle to the family, zero if failed.
 */
REFLECS_EXPORT
EcsHandle ecs_new_family(
    EcsWorld *world,
    const char *id,
    const char *components);


/* -- System API -- */

/** Create a new system.
 * This operation creates a new system with a specified id, kind and action.
 * After this operation is called, the system will be active. Systems can be
 * created with three different kinds:
 *
 * - EcsPeriodic: the system is invoked when ecs_progress is called.
 * - EcsOnInit: the system is invoked when a component is committed to memory.
 * - EcsOnDeinit: the system is invoked when a component is removed from memory.
 * - EcsOnDemand: the system is only invoked on demand (ecs_run)
 *
 * The signature of the system is a string formatted as a comma separated list
 * of component identifiers. For example, a system that wants to receive the
 * Location and Speed components, should provide "Location, Speed" as its
 * signature.
 *
 * The action is a function that is invoked for every entity that has the
 * components the system is interested in. The action has three parameters:
 *
 * - EcsHandle system: Handle to the system (same as returned by this function)
 * - EcsHandle entity: Handle to the current entity
 * - void *data[]: Array of pointers to the component data
 *
 * Systems are stored internally as entities. This operation is equivalent to
 * creating an entity with the EcsSystem and EcsId components. The returned
 * handle can be used in any function that accepts an entity handle.
 *
 * @time-complexity: O(2 * r + c)
 * @param world: The world.
 * @param id: The identifier of the system.
 * @param kind: The kind of system.
 * @param signature: The signature that describes the components.
 * @param action: The action that is invoked for matching entities.
 * @returns: A handle to the system.
 */
REFLECS_EXPORT
EcsHandle ecs_new_system(
    EcsWorld *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    EcsSystemAction action);

/** Enable or disable a system.
 * This operation enables or disables a system. A disabled system will not be
 * ran during ecs_progress or when components must be initialized or
 * deinitialized. Systems are enabled by default.
 *
 * This operation expects a valid system handle, or in other words, an entity
 * with the EcsSystem component. If a handle to an entity is provided that does
 * not have this component, the operation will fail.
 *
 * @time-complexity: O(c)
 * @param world: The world.
 * @param system: The system to enable or disable.
 * @param enabled: true to enable the system, false to disable the system.
 * @returns: EcsOk if succeeded, EcsError if the operation failed.
 */
REFLECS_EXPORT
EcsResult ecs_enable(
    EcsWorld *world,
    EcsHandle system,
    bool enabled);

/** Returns the enabled status for a system / entity.
 * This operation will return whether a system is enabled or disabled. Currently
 * only systems can be enabled or disabled, but this operation does not fail
 * when a handle to an entity is provided that is not a system. If this
 * operation is called on a non-system entity, the operation will return true.
 *
 * @time-complexity: O(c)
 * @param world: The world.
 * @param system: The system to check.
 * @returns: True if the system is enabled, false if the system is disabled.
 */
REFLECS_EXPORT
bool ecs_is_enabled(
    EcsWorld *world,
    EcsHandle system);

/** Run a specific component manually.
 * This operation runs a single system on demand. It is an efficient way to
 * invoke logic on a set of entities, as on demand systems are only matched to
 * tables at creation time or after creation time, when a new table is created.
 *
 * On demand systems can be used as alternative to requesting lists of entities
 * repeatedly inside periodic/reactive systems, which is not efficient.
 *
 * @time-complexity: O(t)
 * @param world: The world.
 * @param system: The system to run.
 * @param param: A user-defined parameter to pass to the system.
 */
REFLECS_EXPORT
void ecs_run_system(
    EcsWorld *world,
    EcsHandle system,
    void *param);

/* -- Utility API -- */

typedef struct EcsIter EcsIter;
struct EcsIter {
    void *ctx;
    void *data;
    bool (*hasnext)(EcsIter*);
    void* (*next)(EcsIter*);
    void (*release)(EcsIter*);
};

REFLECS_EXPORT
bool ecs_iter_hasnext(
    EcsIter* iter);

REFLECS_EXPORT
void* ecs_iter_next(
    EcsIter* iter);

REFLECS_EXPORT
void ecs_iter_release(
    EcsIter* iter);


/* -- Convenience macro's -- */

/** Wrapper around ecs_new_component.
 * This macro provides a convenient way to register components with a world. It
 * can be used like this:
 *
 * ECS_COMPONENT(world, Location);
 *
 * In this example, "Location" must be a valid C type name. After the macro,
 * the application will have access to a Location_h variable which holds the
 * handle to the new component.
 */
#define ECS_COMPONENT(world, id) \
    EcsHandle id##_h = ecs_new_component(world, #id, sizeof(id));\
    assert (id##_h != 0)

/** Same as component, but no size */
#define ECS_TAG(world, id) \
    EcsHandle id##_h = ecs_new_component(world, #id, 0);\
    assert (id##_h != 0)

/** Wrapper around ecs_new_system.
 * This macro provides a convenient way to register systems with a world. It can
 * be used like this:
 *
 * ECS_SYSTEM(world, Move, EcsPeriodic, Location, Speed);
 *
 * In this example, "Move" must be the identifier to a C function that matches
 * the signature of EcsSystemAction. The signature of this component will be
 * "Location, Speed".
 *
 * After the macro, the application will have access to a Move_h variable which
 * holds the handle to the new system.
 */
#define ECS_SYSTEM(world, id, kind, ...) \
    void id(EcsRows*);\
    EcsHandle id##_h = ecs_new_system(world, #id, kind, #__VA_ARGS__, id);\
    assert (id##_h != 0)

/** Wrapper around ecs_new_family.
 * This macro provides a convenient way to register a family with the world.
 * It can be used like this:
 *
 * ECS_FAMILY(world, MyFamily, ComponentA, ComponentB);
 * EcsHandle h = ecs_new(world, MyFamily_h);
 *
 * Creating a family and using it with ecs_new is faster
 * than calling ecs_new, ecs_add and ecs_commit separately. This method also
 * provides near-constant creation time for entities regardless of the number of
 * components, whereas using ecs_add and ecs_commit takes longer for larger
 * numbers of components.
 */
#define ECS_FAMILY(world, id, ...) \
    EcsHandle id##_h = ecs_new_family(world, #id, #__VA_ARGS__);\
    assert (id##_h != 0)

/** Wrapper around ecs_new_prefab.
 * This macro provides a convenient way to register a prefab with the world. It
 * can be used like this:
 *
 * ECS_PREFAB(world, MyPrefab, ComponentA, ComponentB);
 * EcsHandle h = ecs_new(world, MyPrefab_h);
 *
 * For more specifics, see description of ecs_new_prefab.
 */
#define ECS_PREFAB(world, id, ...) \
    EcsHandle id##_h = ecs_new_prefab(world, #id, #__VA_ARGS__);\
    assert (id##_h != 0)

#define ecs_next(data, row) ECS_OFFSET(row, (data)->element_size)

#define ecs_column(data, row, column) \
  ((data)->columns[column] >= 0 \
    ? ECS_OFFSET(row, (data)->columns[column]) \
    : data->refs[-((data)->columns[column]) - 1])

#define ecs_entity(row) *(EcsHandle*)ECS_OFFSET(row, -sizeof(EcsHandle))

/** Utility macro's */
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))

#ifdef __cplusplus
}
#endif

#endif
