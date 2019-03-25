# Flecs Manual

## Design Goals
Flecs is designed with the following goals in mind, in order of importance:

### Portability
Flecs is implemented in C99 which makes it easy to port to a variety of platforms and (legacy) compilers. To further improve portability, Flecs has no mandatory external dependencies. For certain optional features, like threading and running the web-based dashboard, Flecs relies on external libraries like pthreads (or equivalent), civetweb and bake.util, but Flecs can be easily used without them.

### Reusability
Flecs has been designed so that it is easy to package systems and components in a way that can be easily reused across applications. The module design allows applications to import modules with a single line of code, after which the imported components and systems can be immediately used. To facilitate this, Flecs has an architecture that imposes a well-defined order on systems, so that imported systems are always executed in the right order, regardless of in wich order they are imported.

To further improve reusability, Flecs allows for writing code that is agnostic to how data is stored by the framework. While applications may make decisions to optimize storage for a particular usage pattern, Flecs allows systems to be written in a way that works across different storage modes.

### Clean interfaces
Flecs aims to provide clear and simple interfaces, by staying close to the core principles of Entity Component Systems. Someone who has worked with Entity Component Systems before should find it easy to read and write code written with Flecs. Flecs promotes a declarative design, where applications only need to state their intent.

Many Entity Component System frameworks put restrictions on the operations that can be performed while iterating over data, which makes APIs harder to use. In Flecs, there are no restrictions on which operations can be used.

### Performance
Flecs has a design that is optimized for minimizing cache misses by loading only data in cache that is required by the application, while also storing data in arrays to ensure that an application makes optimal usage of cache lines. In many cases, applications can access raw arrays directly, wich is as fast as iterating a native array in C and, if the code permits it, lets applications be compiled with Single Instruction, Multiple Data (SIMD) instructions.

Furthermore, Flecs automatically optimizes performance where it can, by removing systems from the critical path if they are unused. This further improves reusability of code, as it lets applications import modules of which only a subset of the systems is used, without increasing overhead of the framework.

## Naming conventions
The Flecs API adheres to a set of well-defined naming conventions, to make it easier to read and write Flecs code. The basic naming conventions are illustrated in this code example:

```c
// Component names ('Position') use CamelCase
typedef struct Position {
    float x;
    float y; // component members use snake_case
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

// System names ('Move') use CamelCase. Supporting API types use snake_case_t
void Move(ecs_rows_t rows) {
    Position *p = ecs_column(rows, Position, 1); // API functions use snake_case
    Velocity *v = ecs_column(rows, Velocity, 2);
    
    for (int i = 0; i < rows->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();
    
    // Declarative macro's use SCREAMING_SNAKE_CASE
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    // Enumeration constants ('EcsOnFrame') use CamelCase
    ECS_SYSTEM(world, Move, EcsOnFrame, Position, Velocity);
    
    // Entity names use CamelCase
    ECS_ENTITY(world, MyEntity, Position, Velocity);
    
    // Imperative macro's (function wrappers) use snake_case
    ecs_set(world, MyEntity, Position, {10, 20});
    
    return ecs_fini(world);
}
```
