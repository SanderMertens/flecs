# reflecs
Reflecs is an entity component system implemented in C99. It's design goal is to pack as much punch as possible into a tiny library with a minimal API and zero dependencies. The result: a lightning fast, feature-packed ECS framework in a library no larger than 30Kb. Here's what Reflecs has to offer:

- Stupid simple C99 API
- Super fast and memory efficient storage engine
- Periodic, on demand and on component init/deinit systems
- Job scheduler for multi-threaded execution of systems
- Fine-grained control over preallocation of memory to prevent allocations in main loop
- Adaptive optimizations that remove unused code from the critical path

## Overview
The following code shows a simple reflecs application:

```c
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;
typedef Vector2D Velocity;

void Move(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *position = ecs_column(rows, row, 0);
        Velocity *velocity = ecs_column(rows, row, 1);
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
    ecs_new(world, Movable);

    /* Progress world in main loop (invokes Move system) */
    while (true) {
        ecs_progress(world);
    }

    return ecs_fini(world);
}
```
