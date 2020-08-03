/**
 * @file hierarchy.h
 * @brief Hierarchy API.
 */

#ifndef FLECS_HIERARCHY_H
#define FLECS_HIERARCHY_H

#ifdef __cplusplus
extern "C" {
#endif

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


#ifdef __cplusplus
}
#endif

#endif
