/**
 * @file snapshot.h
 * @brief Snapshot addon.
 *
 * A snapshot records the state of a world in a way so that it can be restored
 * later. Snapshots work with POD components and non-POD components, provided
 * that the appropriate lifecycle actions are registered for non-POD components.
 *
 * A snapshot is tightly coupled to a world. It is not possible to restore a
 * snapshot from world A into world B.
 */

#ifdef FLECS_SNAPSHOT

#ifndef FLECS_SNAPSHOT_H
#define FLECS_SNAPSHOT_H

#ifdef __cplusplus
extern "C" {
#endif

/** A snapshot stores the state of a world in a particular point in time. */
typedef struct ecs_snapshot_t ecs_snapshot_t;

/** Create a snapshot.
 * This operation makes a copy of all component in the world that matches the 
 * specified filter.
 *
 * @param world The world to snapshot.
 * @return The snapshot.
 */
FLECS_API
ecs_snapshot_t* ecs_snapshot_take(
    ecs_world_t *world);

/** Create a filtered snapshot.
 * This operation is the same as ecs_snapshot_take, but accepts an iterator so
 * an application can control what is stored by the snapshot. 
 *
 * @param iter An iterator to the data to be stored by the snapshot.
 * @return The snapshot.
 */
FLECS_API
ecs_snapshot_t* ecs_snapshot_take_w_iter(
    ecs_iter_t *iter);

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
FLECS_API
void ecs_snapshot_restore(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot);

/** Obtain iterator to snapshot data.
 *
 * @param snapshot The snapshot to iterate over.
 * @return Iterator to snapshot data. */
FLECS_API
ecs_iter_t ecs_snapshot_iter(
    ecs_snapshot_t *snapshot);

/** Progress snapshot iterator.
 * 
 * @param iter The snapshot iterator.
 * @return True if more data is available, otherwise false.
 */
FLECS_API
bool ecs_snapshot_next(
    ecs_iter_t *iter);

/** Free snapshot resources.
 * This frees resources associated with a snapshot without restoring it.
 *
 * @param snapshot The snapshot to free. 
 */
FLECS_API
void ecs_snapshot_free(
    ecs_snapshot_t *snapshot);
    
#ifdef __cplusplus
}
#endif

#endif

#endif
