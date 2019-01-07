# reflecs
Reflecs is an entity component system implemented in C99. It's design goal is to
pack as much punch as possible into a small library with a minimal API and zero
dependencies. The result: a fast, feature-rich ECS framework in a library no
larger than 80Kb. Here are the feature highlights:

- Periodic, reactive and on demand systems
- A job scheduler for executing systems in multiple threads
- A module system for organizing components and systems, and importing them from libraries
- An ecosystem of modules that include a physics engine and a web dashboard
- Automatic FPS limiter so application does not consume more CPU than needed
- Adaptive optimizations that remove unused systems from critical path
- Fine-grained memory preallocation API to prevent allocations in the main loop

In addition, reflecs has a flexible architecture with many features that let you
write powerful applications with just a few lines of code:

- Prefab entities let you store components in memory once, and reuse it many times
- Expressive system signatures, with AND, OR, NOT and optional operators
- Use entities as components to create hierarchies, indexes and DAGs
- Features group systems so they can be enabled/disabled with a single API call
- Time management ensures frames progress at a constant pace, regardless of FPS
- Create/delete entities and add/remove components while iterating in a system
- Add components to systems to build stateful systems

Check out the [examples](https://github.com/SanderMertens/reflecs/tree/master/examples) and [documentation](https://github.com/SanderMertens/reflecs/blob/master/include/reflecs.h) to learn more!

## Building
To build reflecs, you need to install bake which is currently only supported on
Linux and macOS. See the bake README for installation instructions:

https://github.com/SanderMertens/bake/blob/master/README.md

Then to clone, build and install reflecs, do:

```
bake clone SanderMertens/reflecs
```

## Getting started
To create a new reflecs application, first create a new project:

```
bake init app
```

To add reflecs as a dependency to your application, modify the `project.json` so
that it looks like this:

```json
{
    "id":"app",
    "type":"application",
    "value": {
        "use": ["reflecs"]
    }
}
```

You can now use the reflecs API in your source code. If you get compiler errors for 
missing definitions, make sure that your project includes the (generated) file 
`bake_config.h`, as it contains the include statements for your dependencies.

## Modules
To use a module in reflecs, first install it on your machine:

```
bake clone SanderMertens/reflecs-components-transform
```

Then, add it as a dependency to the `project.json` of your project:

```json
{
    "id":"app",
    "type":"application",
    "value": {
        "use": ["reflecs", "reflecs.components.transform"]
    }
}
```

Then import it in code:

```c
ECS_IMPORT(world, EcsComponentsTransform);
```

After that, you will be able to use the systems and components that are defined
by the module.

Reflecs has a growing ecosystem of modules. The following modules are currently
available:

Module      | Description      
------------|------------------
[reflecs.components.transform](https://github.com/SanderMertens/reflecs-components-transform) | Components for positioning, rotating and scaling entities
[reflecs.components.physics](https://github.com/SanderMertens/reflecs-components-physics) | Components for moving entities
[reflecs.components.graphics](https://github.com/SanderMertens/reflecs-components-graphics) | Components for describing a drawing canvas
[reflecs.components.geometry](https://github.com/SanderMertens/reflecs-components-geometry) | Components for describing geometry
[reflecs.components.http](https://github.com/SanderMertens/reflecs-components-http) | Components for describing an HTTP server with endpoints
[reflecs.systems.physics](https://github.com/SanderMertens/reflecs-systems-physics) | Simple 2D physics engine with limited 3D features
[reflecs.systems.civetweb](https://github.com/SanderMertens/reflecs-systems-civetweb) | A civetweb-based implementation of components-http
[reflecs.systems.admin](https://github.com/SanderMertens/reflecs-systems-admin) | A web-based dashboard for monitoring reflecs performance
[reflecs.systems.sdl2](https://github.com/SanderMertens/reflecs-systems-sdl2) | An SDL2-based renderer

## Example
The following code shows a simple reflecs application:

```c
typedef struct Position {
    float x;
    float y;
} Position;

typedef int32_t Speed;

void Move(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_column(rows, row, 0);
        Speed *s = ecs_column(rows, row, 1);
        p->x += *s * rows->delta_time;
        p->y += *s * rows->delta_time;
    }
}

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    /* Register components and systems */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Speed);
    ECS_FAMILY(world, Movable, Position, Speed);
    ECS_SYSTEM(world, Move, EcsOnFrame, Position, Speed);

    /* Create entity with Movable family */
    ecs_new(world, Movable_h);

    /* Limit application to 100 FPS */
    ecs_set_target_fps(world, 100);

    /* Progress world in main loop (invokes Move system) */
    while (true) {
        ecs_progress(world, 0);
    }

    return ecs_fini(world);
}
```
