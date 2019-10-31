#include <on_demand_system.h>

typedef struct Position {
    float x;
    float y;
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Velocity, v, 2);

    for (int i = 0; i < rows->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        printf("Move  {%f, %f}\n", p[i].x, p[i].y);
    }
}

void PrintPosition(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);

    for (int i = 0; i < rows->count; i ++) {
        printf("Print {%f, %f}\n", p[i].x, p[i].y);
    }    
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* The 'Move' system has the 'EcsOnDemand' tag which means Flecs will only
     * run this system if there is interest in any of its [out] columns. In this
     * case the system will only be ran if there is interest in Position. */
    ECS_SYSTEM(world, Move,          EcsOnUpdate, [out] Position, Velocity, SYSTEM.EcsOnDemand);

    /* The 'PrintPosition' is a regular system with an [in] column. This signals
     * that the system will not write Position, and relies on another system to
     * provide a value for it. If there are any OnDemand systems that provide
     * 'Position' as an output, they will be enabled. */
    ECS_SYSTEM(world, PrintPosition, EcsOnUpdate, [in]  Position);

    /* Create entity, set components */
    ecs_entity_t e = 
    ecs_set(world, 0, Position, {0, 0});
    ecs_set(world, e, Velocity, {1, 2});

    /* If this line is uncommented, the PrintPosition system will be disabled.
     * As a result there will be no more enabled systems with interest in the 
     * 'Position' component, and therefore the 'Move' system will be disabled
     * as a result as well. */

    //ecs_enable(world, PrintPosition, false);

    /* Run slowly so we won't flood the console */
    ecs_set_target_fps(world, 1);

    printf("Application on_demand_system is running, press CTRL-C to exit...\n");

    /* Run systems */
    while ( ecs_progress(world, 0));

    /* Cleanup */
    return ecs_fini(world);
}
