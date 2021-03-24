#include <on_demand_system.h>

typedef struct {
    double x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        printf("Move  {%f, %f}\n", p[i].x, p[i].y);
    }
}

void PrintPosition(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("Print {%f, %f}\n", p[i].x, p[i].y);
    }    
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Printable);

    /* The 'Move' system has the 'EcsOnDemand' tag which means Flecs will only
     * run this system if there is interest in any of its [out] columns. In this
     * case the system will only be ran if there is interest in Position. */
    ECS_SYSTEM(world, Move, EcsOnUpdate, [out] Position, Velocity, SYSTEM:EcsOnDemand);

    /* The 'PrintPosition' is a regular system with an [in] column. This signals
     * that the system will not write Position, and relies on another system to
     * provide a value for it. If there are any OnDemand systems that provide
     * 'Position' as an output, they will be enabled. */
    ECS_SYSTEM(world, PrintPosition, EcsOnUpdate, [in]  Position, Printable);

    /* Create entity, set components */
    ecs_entity_t e = 
    ecs_set(world, 0, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    /* No systems will be executed. The PrintPosition system is enabled, but it
     * has no matching entities. As a result, there is no demand for the 
     * Position component, and the Move system won't be executed either, even
     * though the entity does match with it. */
    printf("First iteration: PrintPosition is inactive\n");
    ecs_progress(world, 0);

    /* Add Printable to the entity */
    ecs_add(world, e, Printable);

    /* Both systems will now be executed. The entity matches with PrintPosition
     * meaning there is demand for Position, and thus the Move system will be
     * enabled. */
    printf("\nSecond iteration: PrintPosition is active\n");
    ecs_progress(world, 0);

    /* Disable the PrintPosition system. Now there is no longer demand for the
     * Position component, so the Move on-demand system will be disabled. */
    printf("\nThird iteration: PrintPosition is disabled\n");
    ecs_enable(world, PrintPosition, false);
    ecs_progress(world, 0);   

    /* Cleanup */
    return ecs_fini(world);
}
