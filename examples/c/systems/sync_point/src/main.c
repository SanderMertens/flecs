#include <sync_point.h>
#include <stdio.h>

typedef struct {
    double x, y;
} Position, Velocity;

// Forward declare components so they can be accessed from systems
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);

void SetVelocity(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Velocity, {1, 2});
    }
}

void Move(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    const Velocity *v = ecs_term(it, const Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

void PrintPosition(ecs_iter_t *it) {
    const Position *p = ecs_term(it, const Position, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("%s: {%f, %f}\n", ecs_get_name(it->world, it->entities[i]), 
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT_DEFINE(ecs, Position);
    ECS_COMPONENT_DEFINE(ecs, Velocity);

    // System that sets velocity using set<T> for entities with Position.
    // While systems are progressing, operations like set<T> are deferred until
    // it is safe to merge. By default this merge happens at the end of the
    // frame, but we can annotate systems to give the scheduler more information
    // about what it's doing, which allows it to insert sync points earlier.
    // 
    // The parentheses after Velocity indicate that the component is not used
    // to match entities, while [out] indicates that it is written. A subsequent
    // system that accesses the component will cause the scheduler to insert a
    // sync point.
    //
    // Note that sync points are never necessary/inserted for systems that write
    // components provided by their signature, as these writes directly happen
    // in the ECS storage and are never deferred.
    //
    // The [filter] annotation for Position tells the scheduler that while we 
    // want to match entities with Position, we're not interested in reading or 
    // writing the component value.
    ECS_SYSTEM(ecs, SetVelocity, EcsOnUpdate, [filter] Position, [out] Velocity());

    // This system reads Velocity, which causes the insertion of a sync point.
    ECS_SYSTEM(ecs, Move, EcsOnUpdate, Position, [in] Velocity);

    // Print resulting Position
    ECS_SYSTEM(ecs, PrintPosition, EcsPostUpdate, [in] Position);

    // Create a few test entities for a Position, Velocity query
    ecs_entity_t e1 = ecs_new_entity(ecs, "e1");
    ecs_set(ecs, e1, Position, {10, 20});
    ecs_set(ecs, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_entity(ecs, "e2");
    ecs_set(ecs, e2, Position, {10, 20});
    ecs_set(ecs, e2, Velocity, {3, 4});

    // Run systems. Debug logging enables us to see the generated schedule
    ecs_log_set_level(1);
    ecs_progress(ecs, 0.0f);
    ecs_log_set_level(-1); // Restore so we don't get world cleanup logs

    // Output:
    //  info: pipeline rebuild:
    //  info: | schedule: threading: 0, staging: 1:
    //  info: | | system SetVelocity
    //  info: | | merge
    //  info: | schedule: threading: 0, staging: 1:
    //  info: | | system Move
    //  info: | | system PrintPosition
    //  info: | | merge
    //  e1: {11.000000, 22.000000}
    //  e2: {11.000000, 22.000000}
    //
    // The "merge" lines indicate sync points.
    //
    // Removing the '[out] Velocity()' annotation from the system will remove
    // the first sync point from the schedule.
    //
    // To create the same system with ecs_system_init, do:
    //  ecs_system_init(ecs, &(ecs_system_desc_t){
    //      .query.filter.terms = {
    //          { 
    //              .id = ecs_id(Position), 
    //              .inout = EcsInOutFilter 
    //          },
    //          { 
    //              .id = ecs_id(Velocity), 
    //              .inout = EcsOut, 
    //              .subj.set.mask = EcsNothing 
    //          }
    //      },
    //      .entity = {
    //          .name = "SetVelocity",
    //          .add = {ecs_dependson(EcsOnUpdate)}
    //      },
    //      .callback = SetVelocity
    //  });

    return ecs_fini(ecs);
}
