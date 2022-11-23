#include <sync_point_delete.h>
#include <stdio.h>

typedef struct {
    double x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    const Velocity *v = ecs_field(it, const Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

void DeleteEntity(ecs_iter_t *it) {
    const Position *p = ecs_field(it, Position, 1);

    for (int i = 0; i < it->count; i ++) {
        if (p[i].x >= 3) {
            ecs_entity_t e = it->entities[i];
            printf("Delete entity %s\n", ecs_get_name(it->world, e));
            ecs_delete(it->world, e);
        }
    }
}

void PrintPosition(ecs_iter_t *it) {
    const Position *p = ecs_field(it, const Position, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("%s: {%f, %f}\n", ecs_get_name(it->world, it->entities[i]), 
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // This example shows how to annotate systems that delete entities, in a way
    // that allows the scheduler to to correctly insert sync points. See the
    // sync_point example for more details on sync points.
    //
    // While annotating a system for a delete operation follows the same 
    // design as other operations, one key difference is that a system often 
    // does not know which components a to be deleted entity has. This makes it
    // impossible to annotate the system in advance for specific components.
    //
    // To ensure the scheduler is still able to insert the correct sync points,
    // a system can use a wildcard to indicate that any component could be
    // modified by the system, which forces the scheduler to insert a sync.

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    // Basic move system.
    ECS_SYSTEM(ecs, Move, EcsOnUpdate, Position, [in] Velocity);

    // Delete entities when p.x >= 3. Add wildcard annotation to indicate any
    // component could be written by the system. Position itself is added as 
    // const, since inside the system we're only reading it.
    //
    // Note how the system uses the same annotations as the sync_point example.
    ECS_SYSTEM(ecs, DeleteEntity, EcsOnUpdate, [in] Position, [out] *());

    // Print resulting Position
    ECS_SYSTEM(ecs, PrintPosition, EcsPostUpdate, [in] Position);

    // Create a few test entities for a Position, Velocity query
    ecs_entity_t e1 = ecs_new_entity(ecs, "e1");
    ecs_set(ecs, e1, Position, {0, 0});
    ecs_set(ecs, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_entity(ecs, "e2");
    ecs_set(ecs, e2, Position, {1, 2});
    ecs_set(ecs, e2, Velocity, {1, 2});

    // Run systems. Debug logging enables us to see the generated schedule
    ecs_log_set_level(1);
    while (ecs_progress(ecs, 0.0f)) {
        if (!ecs_count(ecs, Position)) {
            break;  // No more entities left with Position
        }
    }
    ecs_log_set_level(-1); // Restore so we don't get world cleanup logs

    // Output:
    //  info: pipeline rebuild:
    //  info: | schedule: threading: 0, staging: 1:
    //  info: | | system Move
    //  info: | | system DeleteEntity
    //  info: | | merge
    //  info: | schedule: threading: 0, staging: 1:
    //  info: | | system PrintPosition
    //  info: | | merge
    //  e1: {1, 2}
    //  e2: {2, 4}
    //  Delete entity e2
    //  e1: {2, 4}
    //  Delete entity e1
    //  info: system Move deactivated
    //  info: system DeleteEntity deactivated
    //  info: system PrintPosition deactivated
    //
    // Removing the wildcard annotation from the DeleteEntity system will
    // remove the first sync point.
    //
    // Note how after both entities are deleted, all three systems are 
    // deactivated. This happens when there are no matching entities for a
    // system. A deactivated system is not ran by the scheduler, which reduces
    // overhead.
    //
    // To create the same system with ecs_system_init, do:
    //  ecs_system_init(ecs, &(ecs_system_desc_t){
    //      .query.filter.terms = {
    //          { 
    //              .id = ecs_id(Position), 
    //              .inout = EcsIn
    //          },
    //          { 
    //              .id = EcsWildcard, 
    //              .inout = EcsOut, 
    //              .src.flags = EcsIsEntity 
    //          }
    //      },
    //      .entity = {
    //          .name = "DeleteEntity",
    //          .add = {ecs_dependson(EcsOnUpdate)}
    //      },
    //      .callback = DeleteEntity
    //  });

    return ecs_fini(ecs);
}
