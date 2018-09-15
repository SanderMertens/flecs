
#ifndef REFLECS_H
#define REFLECS_H

#include <stdint.h>
#include <alloca.h>

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
 * c: chunk size of entity vector
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

typedef uint64_t EcsHandle;

typedef enum EcsResult {
    EcsOk,
    EcsError
} EcsResult;

typedef enum EcsSystemKind {
    EcsPeriodic,
    EcsOnInit,
    EcsOnDeinit
} EcsSystemKind;

typedef void (*EcsSystemAction)(
    EcsHandle h_system,
    EcsHandle h_entity,
    void *data[]);


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
void ecs_fini(
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

/** Lookup an entity by id.
 * This operation is a convenient way to lookup entities by string identifier
 * that have the EcsId component. It is recommended to cache the result of this
 * function, as the function must iterates over all entities and all components
 * in an entity.
 *
 * @time-complexity: O(e * c)
 * @param world The world.
 * @param id The id to lookup.
 * @returns The entity handle if found, or ECS_HANDLE_NIL if not found.
 */
REFLECS_EXPORT
EcsHandle ecs_lookup(
    EcsWorld *world,
    const char *id);


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
 * @time-complexity: O(1) (average, O(e) when rehashing lookup map)
 * @param world: The world to which to add the entity.
 * @returns: A handle to the new entity.
 */
REFLECS_EXPORT
EcsHandle ecs_new(
    EcsWorld *world);

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
 * @time-complexity: O(1 + r + e / c)
 * @param world: The world.
 * @param entity: A handle to the entity to delete.
 */
REFLECS_EXPORT
void ecs_delete(
    EcsWorld *world,
    EcsHandle entity);

/** Add a component to an entity and return a pointer to its data.
 * This operation is a combination of ecs_stage, ecs_commit and ecs_get. It
 * adds a component to an entity, commits it to memory and subsequently
 * retrieves a pointer to the component, so that its data can be initialized by
 * an application.
 *
 * This operation is expensive as it results in moving component data and entity
 * data around in memory. When adding multiple components to an entity, it is
 * more efficient to use ecs_stage and ecs_commit directly.
 *
 * If the specific combination of components did not yet exist in the world, a
 * new table will be created to store the entity. Subsequent entities with the
 * same set of components will be stored in the new table as well.
 *
 * @time-complexity: O(2 * r + c)
 * @param world: The world.
 * @param entity: Handle to the entity to which to add the component.
 * @param component: Handle to the component to add.
 * @returns: A pointer to the component data, or NULL if the operation failed.
 */
REFLECS_EXPORT
void* ecs_add(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component);

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
 * @time-complexity: O(c)
 * @param world: The world.
 * @param entity: Handle to the entity for which to stage the component.
 * @param component: Handle to the component.
 * @returns: EcsOk if succeeded, or EcsError if the operation failed.
 */
REFLECS_EXPORT
EcsResult ecs_stage(
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
 * @time-complexity: O(2 * r + 2 * c ^ 2)
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
 * @time-complexity: O(c)
 * @param world: The world.
 * @param entity: Handle to the entity from which to obtain the component data.
 * @param component: The component to retrieve the data for.
 * @result: A pointer to the data, or NULL of the component was not found.
 */
REFLECS_EXPORT
void* ecs_get(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component);

/** Remove a component from an entity.
 * This operation stages a remove for a component from an entity. The remove
 * will not yet be committed to memory. Similar to ecs_stage, this operation
 * requires calling ecs_commit to actually commit the changes to memory.
 *
 * The post condition for this operation is that the entity will not have
 * the component after invoking ecs_commit. If the entity does not have the
 * component when this operation is called, it is a no-op.
 *
 * @time-complexity: O(c)
 * @param world: The world.
 * @param entity: Handle to the entity from which to remove the component.
 * @param component: The component to remove.
 */
REFLECS_EXPORT
void ecs_stage_remove(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component);


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
 * @time-complexity: O(1 + 2 * c + 2 * r)
 * @param world: The world.
 * @param id: A unique component identifier.
 * @param size: The size of the component type (as obtained by sizeof).
 * @returns: A handle to the new component, or ECS_HANDLE_NIL if failed.
 */
REFLECS_EXPORT
EcsHandle ecs_component_new(
    EcsWorld *world,
    const char *id,
    size_t size);


/* -- System API -- */

/** Create a new system.
 * This operation creates a new system with a specified id, kind and action.
 * After this operation is called, the system will be active. Systems can be
 * created with three different kinds:
 *
 * - EcsPeriodic: the system is invoked when ecs_progress is called.
 * - EcsOnInit: the system is invoked when a component is committed to memory.
 * - EcsOnDeinit: the system is invoked when a component is removed from memory.
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
 * @param world: The world.
 * @param id: The identifier of the system.
 * @param kind: The kind of system.
 * @param signature: The signature that describes the components.
 * @param action: The action that is invoked for matching entities.
 * @returns: A handle to the system.
 */
REFLECS_EXPORT
EcsHandle ecs_system_new(
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
 * @param world: The world.
 * @param system: The system to check.
 * @returns: True if the system is enabled, false if the system is disabled.
 */
REFLECS_EXPORT
bool ecs_is_enabled(
    EcsWorld *world,
    EcsHandle system);


/* -- Iterator utility API -- */

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

/** Wrapper around ecs_component_new.
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
    EcsHandle id##_h = ecs_component_new(world, #id, sizeof(id)); (void)id##_h;

/** Wrapper around ecs_system_new.
 * This macro provides a convenient way to register systems with a world. It can
 * be used like this:
 *
 * ECS_SYSTEM(world, Move, EcsPeriodic, Location, Speed)
 *
 * In this example, "Move" must be the identifier to a C function that matches
 * the signature of EcsSystemAction. The signature of this component will be
 * "Location, Speed".
 *
 * After the macro, the application will have access to a Move_h variable which
 * holds the handle to the new system.
 */
#define ECS_SYSTEM(world, id, kind, ...) \
    void id(EcsHandle, EcsHandle, void*[]);\
    EcsHandle id##_h = ecs_system_new(world, #id, kind, #__VA_ARGS__, id);\
    (void)id##_h;


#ifdef __cplusplus
}
#endif

#endif
