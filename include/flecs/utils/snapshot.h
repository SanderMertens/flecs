/**
 * @file snapshot.h
 * @brief Snapshot API.
 */

#ifndef FLECS_SNAPSHOT_H
#define FLECS_SNAPSHOT_H

#ifdef __cplusplus
extern "C" {
#endif

/** Create a snapshot.
 * This operation makes a copy of all component in the world that matches the 
 * specified filter.
 *
 * @param world The world to snapshot.
 * @param filter A filter that specifies which components to snapshot.
 * @param return The snapshot.
 */
FLECS_EXPORT
ecs_snapshot_t* ecs_snapshot_take(
    ecs_world_t *world,
    const ecs_filter_t *filter);

/** Restore a snapshot.
 * This operation restores the world to the state it was in when the specified
 * snapshot was taken. A snapshot can only be used once for restoring, as its
 * data replaces the data that is currently in the world.
 * This operation also resets the last issued entity handle, so any calls to
 * ecs_new may return entity ids that have been issued before restoring the 
 * snapshot.
 *
 * The world in which the snapshot is restored must be the same as the world in
 * which the snapshot is taken.
 *
 * @param world The world to restore the snapshot to.
 * @param snapshot The snapshot to restore. 
 */
FLECS_EXPORT
void ecs_snapshot_restore(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot);

/** Copy a snapshot.
 * This operation creates a copy of the provided snapshot. An application can
 * optionally filter the tables to copy.
 *
 * @param world The world.
 * @param snapshot The snapshot to copy.
 * @param filter Filter to apply to the copy (optional)
 * @return The duplicated snapshot.
 */
FLECS_EXPORT
ecs_snapshot_t* ecs_snapshot_copy(
    ecs_world_t *world,
    const ecs_snapshot_t *snapshot,
    const ecs_filter_t *filter);

/** Free snapshot resources.
 * This frees resources associated with a snapshot without restoring it.
 *
 * @param world The world.
 * @param snapshot The snapshot to free. 
 */
FLECS_EXPORT
void ecs_snapshot_free(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot);
    
#ifdef __cplusplus
}
#endif

#endif
