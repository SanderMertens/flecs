#include <task.h>

typedef struct TaskContext {
    int value;
} TaskContext;

void MyTask(ecs_iter_t *it) {
    printf("Task executed every second\n");
}

void My2ndTask(ecs_iter_t *it) {
    printf("Task executed every 2 seconds\n");
}

void My3rdTask(ecs_iter_t *it) {
    ECS_COLUMN(it, TaskContext, ctx, 1);

    printf("Task with context: %d\n", ctx->value);
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    // Tasks are systems that are not matched with any entities.

    ECS_COMPONENT(world, TaskContext);

    // Basic task
    ECS_SYSTEM(world, MyTask, EcsOnUpdate, 0);

     // Task that is executed every 2 seconds
    ECS_SYSTEM(world, My2ndTask, EcsOnUpdate, 0);
    ecs_set_interval(world, My2ndTask, 2.0);

    // It is possible to add components to a task, just like regular systems
    ECS_SYSTEM(world, My3rdTask, EcsOnUpdate, SYSTEM:TaskContext);
    ecs_set(world, My3rdTask, TaskContext, {10});

    /* Set target FPS for main loop */
    ecs_set_target_fps(world, 1);

    printf("Application task is running, press CTRL-C to exit...\n");

    /* Run systems */
    while ( ecs_progress(world, 0));

    /* Cleanup */
    return ecs_fini(world);
}
