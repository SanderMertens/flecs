# reflecs
Reflecs is an entity component system implemented in C99. Its key features are:
- An API design that is optimized for writing fast vectorized code
- A very efficient and low-overhead storage engine
- Dynamic evaluation of non-empty systems/tables
- 30Kb library footprint
- Job scheduler for running system workloads in parallel
- Periodic, reactive and on-demand systems

## Overview
The following code shows a simple reflecs application:

```c
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;
typedef Vector2D Velocity;

void Move(EcsData *data) {
    void *row;
    for (row = data->first; row < data->last; row = ecs_data_next(data, row)) {
        Position *position = ecs_data_get(data, row, 0);
        Velocity *velocity = ecs_data_get(data, row, 0);
        position->x += velocity->x;
        position->y += velocity->y;
    }
}

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    /* Register components and systems */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_FAMILY(world, Movable, Position, Velocity);
    ECS_SYSTEM(world, Move, EcsPeriodic, Position, Velocity);

    /* Create entity with Movable family */
    EcsHandle e = ecs_new(world, Movable);

    /* Progress world in main loop (invokes Move system) */
    while (true) {
        ecs_progress(world);
    }

    return ecs_fini(world);
}
```
