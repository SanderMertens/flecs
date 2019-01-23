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

To run the project, do:

```
bake run app
```

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
[reflecs.components.graphics](https://github.com/SanderMertens/reflecs-components-graphics) | Components for describing a drawing canvas and camera
[reflecs.components.geometry](https://github.com/SanderMertens/reflecs-components-geometry) | Components for describing geometry
[reflecs.components.input](https://github.com/SanderMertens/reflecs-components-input) | Components for describing keyboard and mouse input
[reflecs.components.http](https://github.com/SanderMertens/reflecs-components-http) | Components for describing an HTTP server with endpoints
[reflecs.systems.transform](https://github.com/SanderMertens/reflecs-systems-transform) | Compute transformation matrices from transform components
[reflecs.systems.physics](https://github.com/SanderMertens/reflecs-systems-physics) | Simple 2D physics engine with limited 3D features
[reflecs.systems.civetweb](https://github.com/SanderMertens/reflecs-systems-civetweb) | A civetweb-based implementation of components-http
[reflecs.systems.admin](https://github.com/SanderMertens/reflecs-systems-admin) | A web-based dashboard for monitoring reflecs performance
[reflecs.systems.sdl2](https://github.com/SanderMertens/reflecs-systems-sdl2) | An SDL2-based renderer
[reflecs.math](https://github.com/SanderMertens/reflecs-math) | Matrix and vector math functions
[reflecs.util](https://github.com/SanderMertens/reflecs-util) | Utility functions and datastructures

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
        Position *p = ecs_data(rows, row, 0);
        Speed *s = ecs_data(rows, row, 1);
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

## Concepts
This section describes the high-level concepts used in the reflecs API.

### World
A world is a container in which entities, components and systems can be stored and evaluated. An application can create any number of worlds. Data between worlds is not shared. If the application wants to share data between worlds, this has to be done manually. A world in ECS can be created with the `ecs_init` function:

```c
EcsWorld *world = ecs_init();
```

### Entity
An entity is an integer that uniquely identifies an "object" in a system. Entities do not contain data or logic. To add data to an entity, an application should add components to an entity. An entity may have 0..n components. Each component can be added only once. An entity can be created in reflecs with the `ecs_new` function:

```c
EcsEntity e = ecs_new(world, 0);
```

Entities can have an optional string identifier, which allows entities to be looked up in the world, and can make debugging easier. To add an identifier to an entity, an application needs to set the `EcsId` component, like this:

```c
ecs_set(world, e, EcsId, {"MyEntity"});
```

### Component
Components are datatypes that can be added to an entity. Any C datatype can be registered as a component within reflecs. To register a component, you can use the `ECS_COMPONENT` macro, which wraps around the `ecs_new_component` function:

```c
typedef struct Point {
   int x;
   int y;
} Point;

ECS_COMPONENT(world, Point);
```

The macro will define the `Point_h` variable, which the application can then use to add the component to an entity with the `ecs_add` function:

```c
ecs_add(world, e, Point_h);
```

Components in reflecs are stored internally as entities, which is why you will notice handles to components are of the `EcsEntity` type.

### System
A system is logic (a function) that is executed for every entity that has a set of components that match a system's interest. In reflecs, systems specify their interest, and when they should run. To define a system, you can use the `ECS_SYSTEM` macro, which wraps around the `ecs_new_system` function:

```c
ECS_SYSTEM(world, LogPoints, EcsOnFrame, Point);
```

In this statement, `LogPoints` refers to a C function that will be associated with the system. `EcsOnFrame` identifies the stage in which the system is executed. The `{Point}` identifies the component interest expression. After the macro, an application can use the `LogPoints_h` variable to refer to the system. The system is implemented as a regular C function, like this:

```c
void LogPoints(EcsRows *rows) {
    for (void *row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Point *p = ecs_data(rows, row, 0);
        printf("Log point (%d, %d)\n", p->x, p->y);
    }
}
```

Systems can be enabled / disabled. By default a system is enabled. To enable or disable a system, you can use the `ecs_enable` function:

```c
ecs_enable(world, LogPoints_h, false);
```

Systems in reflecs are stored as entities internally, which is why you will notice handles to systems are of the `EcsEntity` type.

### Family
A family is a combination of 1..n entities. Since components and systems are stored as entities by reflecs, families can also be used to group components and systems. Typical uses for familes are:

- Group components so that they can be added to an entity with a single `ecs_add` call
- Group systems so that they can be enabled or disabled with a single `ecs_enable` call

To define a family, you can use the `ECS_FAMILY` macro, which wraps the `ecs_new_family` function:

```c
ECS_FAMILY(world, Object, Foo, Bar);
```

This defines a family called `Object` that contains `Foo` and `Bar`. Note that in order to be able to refer to an entity in a family, the entity must have a string identifier, as defined by the `EcsId` component. Components and systems automatically register themselves with string identifiers. The macro will define the `Object_h` variable, which the application can use to refer to the family:

```
ecs_add(world, e, Object_h);
```

Families in reflecs are stored as entities internally, which is why you will notice handles to families are of the `EcsEntity` type.

### Feature
A feature is a family that contains solely out of systems. To create features, use the `ECS_FAMILY` macro or `ecs_new_family` function.

### Tag
A tag is a component that does not contain any data. Internally it is represented as a component with data-size 0. Tags can be useful for subdividing entities into categories, without adding any data. A tag can be defined with the `ECS_TAG` macro:

```c
ECS_TAG(world, MyTag);
```
The macro will define the `MyTag_h` variable, which an application can then use as a regular component, like with the ecs_add function:

```
ecs_add(world, e, MyTag_h);
```

### Container
A container is an entity that can contain other entities. Since components are stored as entities in reflecs, and components can be added with the `ecs_add` function, it is similarly possible to add entities with the `ecs_add` function. The entity that is added must however be a container. To turn an entity in a container, add the builtin `EcsContainer` component. Consider this example:

```c
EcsEntity my_container = ecs_new(world, EcsContainer_h);
EcsEntity my_child = ecs_new(world, my_container);
```

The above code constructs a hierarchy with a parent and a child. For an example of how to walk over this hierarchy, see the `dag` example in the examples directory.

### Prefab

### Module

