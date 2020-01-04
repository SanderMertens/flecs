[![Join the chat at https://gitter.im/flecsdev/community](https://badges.gitter.im/flecsdev/community.svg)](https://gitter.im/flecsdev/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Discord Chat](https://img.shields.io/discord/633826290415435777.svg)](https://discord.gg/MRSAZqb) [![Build Status](https://travis-ci.org/SanderMertens/flecs.svg?branch=master)](https://travis-ci.org/SanderMertens/flecs)
[![Build status](https://ci.appveyor.com/api/projects/status/t99p1per439ctg1a/branch/master?svg=true)](https://ci.appveyor.com/project/SanderMertens/flecs/branch/master)
[![codecov](https://codecov.io/gh/SanderMertens/flecs/branch/master/graph/badge.svg)](https://codecov.io/gh/SanderMertens/flecs)

![flecs](https://user-images.githubusercontent.com/9919222/54175082-b107f900-4446-11e9-9cbc-91c096f7c0b1.png)

Flecs is a [Fast](https://github.com/SanderMertens/ecs_benchmark) and Lightweight ECS ([Entity Component System](#what-is-an-entity-component-system)). Flecs packs as much punch as possible into a small library with a tiny C99 API and zero dependencies. Here are some of the things it can do:

- Process entities on multiple threads with a lock-free, zero-overhead staging architecture [[learn more](Manual.md#staging)]
- Organize components & systems in reusable, library-friendly modules [[learn more](Manual.md#modules)]
- Run systems every frame, periodically, on demand or on change events [[learn more](Manual.md#reactive-systems)]

Additionally, flecs has a flexible engine that lets you do many things, like:

- A prefab system with variants, overrides and prefab nesting [[learn more](Manual.md#prefabs)]
- Create system expressions with AND, OR, NOT and optional operators [[learn more](Manual.md#system-signatures)]
- Create hierarchies, indexes and [DAGs](https://en.wikipedia.org/wiki/Directed_acyclic_graph) with container entities [[learn more](Manual.md#containers)]

Make sure to check the flecs [dashboard](https://github.com/SanderMertens/flecs-systems-admin):

![dashboard](https://user-images.githubusercontent.com/9919222/54180572-309ec380-4459-11e9-9e48-1a08de57ff91.png)

## What is an Entity Component System?
ECS (Entity Component System) is a way to organize code that is mostly used in gaming and simulation projects. ECS code generally performs better than traditional OOP, and is typically easier to reuse. The main differences between ECS and OOP are composition is a first class citizen in ECS, and that data is represented as plain data types rather than encapsulated classes.  A framework is an Entity Component System if it:

- Has _entities_ that are unique identifiers (integers)
- Has _components_ that are plain data types which can be added to entities
- Has _systems_ that are functions which are matched against entities with a set of components

For more information, check [the Entity Component System FAQ](https://github.com/SanderMertens/ecs-faq)!

## Example
This is a simple flecs example using the C99 API:

```c
typedef struct Position {
    float x;
    float y;
} Position;

typedef float Speed;

void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Speed, s, 2);
    
    for (int i = 0; i < rows->count; i ++) {
        p[i].x += s[i] * rows->delta_time;
        p[i].y += s[i] * rows->delta_time;
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Speed);
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Speed);
    ECS_ENTITY(world, MyEntity, Position, Speed);
    
    ecs_set(world, MyEntity, Position, {0, 0});
    ecs_set(world, MyEntity, Speed, {1});
    
    while (ecs_progress(world, 0));

    return ecs_fini(world);
}
```

This is the same example in the C++11 API:

```c++
struct Position {
    float x;
    float y;
};

struct Speed {
    float value;
};

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);

    flecs::component<Position>(world, "Position");
    flecs::component<Speed>(world, "Speed");

    flecs::system<Position, Speed>(world)
        .action([](const flecs::rows& rows, 
            flecs::column<Position> p, 
            flecs::column<Speed> s) 
        {    
            for (auto row : rows) {
                p[row].x += s[row].value * rows.delta_time();
                p[row].y += s[row].value * rows.delta_time();
            }
        });

    flecs::entity(world, "MyEntity")
        .set<Position>({0, 0})
        .set<Speed>({1});

    while (world.progress()) { }
}
```

For more examples, go to [the flecs examples folder](https://github.com/SanderMertens/flecs/tree/master/examples) or the [flecs-hub organization](https://github.com/flecs-hub).

## Manual
[Click here](Manual.md) to view the Flecs manual.

## Contents
* [Building](#building)
* [Getting started](#getting-started)
* [Built with flecs](#built-with-flecs)
* [Modules](#modules)
* [Concepts](#concepts)
  * [entity](#entity)
  * [component](#component)
  * [system](#system)
  * [identifier](#identifier)
  * [type](#type)
  * [feature](#feature)
  * [tag](#tag)
  * [container](#container)
  * [prefab](#prefab)
  * [module](#module)

## Building
You can build flecs with either CMake, Meson, [Bake](https://github.com/SanderMertens/bake) or embed the sources into your own project.

### Embedding:
Flecs can be easily embedded into projects, as it does not require complex build instructions. The following build instructions are enough to build a functioning Flecs library with gcc:

```
gcc src/*.c -Iinclude --shared -o libflecs.so
```

### CMake
```
git clone https://github.com/SanderMertens/flecs
cd flecs
mkdir build
cd build
cmake ..
make
```

### Meson

```
git clone https://github.com/SanderMertens/flecs
cd flecs
meson build --default-library=both
cd build
ninja
```

### Bake
Install bake first:
```
git clone https://github.com/SanderMertens/bake
make -C bake/build-$(uname)
bake/bake setup
```

To then install Flecs, do:
```
bake clone https://github.com/SanderMertens/flecs
```

### Building notes

#### Operating system abstraction API
Most of Flecs can run without relying on operating system specific functionality. However, some features require threading and timing, amongst others. Out of the box, Flecs implements abstractions for these functions for common platforms. If you however want to run Flecs on a platform that is not implemented, you can easily provide Flecs with a platform specific set of functions through the OS API interface.

The OS API is an interface that contains function pointers for all the functions Flecs needs from the underlying platform which can be easily overridden by an application. These functions include support for:

- Heap memory management
- Threading
- Timing
- Logging
- Exception handling

[This section of the manual](Manual.md#operating-system-abstraction-api) describes how to override functions in OS API.

#### Modules
Flecs has optional [modules](#modules) which are created as bake packages. It is possible to use modules in a non-bake environment, but this is still a work in progress and likely requires manual labor. 

If you want to use modules, but you do not want to use bake as the build tool for your own applications, the recommended way of accomplishing this is to first build Flecs and the modules with bake, and use the generated binaries with your own build system. 

After building with bake, you will have a folder called `bake` in your home directory which contains binaries and include files. Depending on your operating system, you may need to set `LD_LIBRARY_PATH` (Linux), `DYLD_LIBRARY_PATH` (MacOS) or `PATH` (Windows) to the path where the libraries are stored.

## Getting started
To create a new flecs application, first create a new project:

```
bake new my_app -t flecs
```

You now have a project which contains a simple flecs application! To run the project, do:

```
bake run my_app
```

### Getting started with the dashboard
To create an application that uses the flecs web dashboard, first install the `admin` and `civetweb` modules:

```
bake clone SanderMertens/flecs-systems-admin
bake clone SanderMertens/flecs-systems-civetweb
```
Currently this is only possible out of the box with [bake](https://github.com/SanderMertens/bake). Future versions may also support modules with CMake.

After cloning the packages, create a new project like so:

```
bake new my_app -t flecs
```
This creates a new flecs application. To now run your application with the dashboard, run it like this:

```
bake run my_app -a --admin 9090
```
This runs the application, and passes `--admin 9090` as its arguments. You can now navigate to http://localhost:9090 to see the dashboard. Any systems that you add to your application will now show up in the dashboard, and can be turned on/off.

## Built with flecs

#### [ecs_graphics](https://github.com/SanderMertens/ecs_graphics)
Basic rendering and user input. 

#### [ecs_nbody](https://github.com/SanderMertens/ecs_nbody)
An nbody simulation that uses flecs multithreading.

#### [ecs_collisions](https://github.com/SanderMertens/ecs_collisions)
A simple application demonstrating collision detection with flecs.

#### [ecs_inheritance](https://github.com/SanderMertens/ecs_inheritance)
A simple application demonstrating inheritance with flecs.

#### [ecs_pong](https://github.com/SanderMertens/ecs_pong)
An implementation of pong in flecs.

#### [ecs_solar](https://github.com/SanderMertens/ecs_solar)
An intermediate application that demonstrates hierarchies and particle effects

#### [ecs_benchmark](https://github.com/SanderMertens/ecs_benchmark)
ECS performance benchmark that tests various operations and iterations.

## Modules
Flecs has a growing ecosystem of modules. The following modules are currently
available:

Module      | Description      
------------|------------------
[flecs.components.transform](https://github.com/SanderMertens/flecs-components-transform) | Components for positioning, rotating and scaling entities
[flecs.components.physics](https://github.com/SanderMertens/flecs-components-physics) | Components for moving entities
[flecs.components.graphics](https://github.com/SanderMertens/flecs-components-graphics) | Components for describing a drawing canvas and camera
[flecs.components.geometry](https://github.com/SanderMertens/flecs-components-geometry) | Components for describing geometry
[flecs.components.input](https://github.com/SanderMertens/flecs-components-input) | Components for describing keyboard and mouse input
[flecs.components.http](https://github.com/SanderMertens/flecs-components-http) | Components for describing an HTTP server with endpoints
[flecs.components.meta](https://github.com/SanderMertens/flecs-components-meta) | Reflection components for flecs components
[flecs.systems.transform](https://github.com/SanderMertens/flecs-systems-transform) | Compute transformation matrices from transform components
[flecs.systems.physics](https://github.com/SanderMertens/flecs-systems-physics) | Simple 2D physics engine with limited 3D features
[flecs.systems.civetweb](https://github.com/SanderMertens/flecs-systems-civetweb) | A civetweb-based implementation of components-http
[flecs.systems.admin](https://github.com/SanderMertens/flecs-systems-admin) | A web-based dashboard for monitoring flecs performance
[flecs.systems.sdl2](https://github.com/SanderMertens/flecs-systems-sdl2) | An SDL2-based renderer
[flecs.math](https://github.com/SanderMertens/flecs-math) | Matrix and vector math functions
[flecs.util](https://github.com/SanderMertens/flecs-util) | Utility functions and datastructures

## Concepts
This section describes the high-level concepts used in flecs, and how they are represented in the API. Rather than providing an exhaustive overview of the API behavior, this section is intended as an introduction to the different API features of flecs.

### World
A world is a container in which entities, components and systems can be stored and evaluated. An application can create any number of worlds. Data between worlds is not shared. If the application wants to share data between worlds, this has to be done manually. A world in ECS can be created with the `ecs_init` function:

```c
ecs_world_t *world = ecs_init();
```

### Entity
An entity is an integer that uniquely identifies an "object" in a system. An entity may have `0..n` components, and each component can be added only once. Entities can be created in flecs with the `ecs_new` function:

```c
ecs_entity_t e = ecs_new(world, 0);
```

[Learn more](Manual.md#entities)

### Component
Components are datatypes that can be added to an entity. Any C datatype can be registered as a component within flecs. To register a component, you can use the `ECS_COMPONENT` macro, which wraps around the `ecs_new_component` function:

```c
typedef struct Point {
   int x;
   int y;
} Point;

ECS_COMPONENT(world, Point);
```

After this macro, you are able to add the `Point` component using `ecs_add`:

```c
ecs_add(world, e, Point);
```

Additionally, the component can be added and initialized with the `ecs_set` function:

```c
ecs_set(world, e, Point, {.x = 10, .y = 20});
```

Flecs components are stored internally as entities, which is why handles to components are of the `ecs_entity_t` type.

[Learn more](Manual.md#components-and-types)

### System
A system is logic (a function) that is executed for every entity that has a set of components that match a system's interest. In flecs, systems specify their interest, and when they should run. To define a system, you can use the `ECS_SYSTEM` macro, which wraps around the `ecs_new_system` function:

```c
ECS_SYSTEM(world, LogPoints, EcsOnUpdate, Point);
```

In this statement, `LogPoints` refers to a C function that will be associated with the system. `EcsOnUpdate` identifies the stage in which the system is executed. The `Point` identifies the component interest expression. The system is implemented as a regular C function, like this:

```c
void LogPoints(ecs_rows_t *rows) {
    Point *p = ecs_column(rows, Point, 1);
    for (int i = 0; i < rows->count; i ++) {
        printf("Log point (%d, %d)\n", p[i].x, p[i].y);
    }
}
```

Systems can be enabled / disabled. By default a system is enabled. To enable or disable a system, you can use the `ecs_enable` function:

```c
ecs_enable(world, LogPoints, false);
```

[Learn more](Manual.md#systems)

### Identifier
Entities in flecs may have an optional string-based identifier. An identifier can be added to an entity by setting the `EcsId` component, like this:

```c
ecs_set(world, e, EcsId, {"MyEntity"});
```

After a string identifier is added, the entity can be looked up like this:

```c
ecs_entity_t e = ecs_lookup(world, "MyEntity");
```

Additionally, applications can define entities with the `ECS_ENTITY` macro, which automatically adds `EcsId` and initializes it with the provided name:

```c
ECS_ENTITY(world, MyEntity, Point);
```

Components, systems, tasks, types and prefabs automatically register the `EcsId` component when they are created, and can thus be looked up with `ecs_lookup`.

### Task
A task is a system that has no interest expression. Tasks are run once every frame. Tasks are defined the same way as normal systems, but instead of an interest expression, you specify `0`:

```c
ECS_SYSTEM(world, MyTask, EcsOnUpdate, 0);
```

### Type
A type identifies a collection of `1..n` entities. In flecs, components and systems are assigned unique identifiers from the same pool as entities, and therefore a type may contain identifiers to entities, components and systems. Typical usecases for types are:

- Group components so that they can be added to an entity with a single `ecs_add` call
- Group systems so that they can be enabled or disabled with a single `ecs_enable` call

To define a type, you can use the `ECS_TYPE` macro, which wraps the `ecs_new_type` function:

```c
ECS_TYPE(world, Circle, EcsCircle, EcsPosition2D);
```

This defines a type called `Circle` that contains `EcsCircle` and `EcsPosition2D`. After this macro, you can use the `Circle` type with functions like `ecs_add` and `ecs_remove`:

```c
ecs_add(world, e, Circle);
```

[Learn more](Manual.md#components-and-types)

### Feature
A feature is a type that contains solely out of systems. To create features, use the `ECS_TYPE` macro or `ecs_new_type` function. This can be used to enable/disable multiple systems with a single API call, like so:

```c
ECS_TYPE(world, MyFeature, SystemA, SystemB);

ecs_enable(World, MyFeature, true);
```

A useful property of features (types) is that they can be nested, like so:

```c
ECS_TYPE(world, MyNestedFeatureA, SystemA, SystemB);
ECS_TYPE(world, MyNestedFeatureB, SystemC);
ECS_TYPE(world, MyFeature, MyNestedFeatureA, MyNestedFeatureB);

ecs_enable(World, MyFeature, true);
```

[Learn more](Manual.md#features)

### Tag
A tag is a component that does not contain any data. Internally it is represented as a component with data-size 0. Tags can be useful for subdividing entities into categories, without adding any data. A tag can be defined with the `ECS_TAG` macro:

```c
ECS_TAG(world, MyTag);
```

Tags can be added/removed like any other component:

```c
ecs_add(world, e, MyTag);
```

[Learn more](Manual.md#tags)

### Container
A container is an entity that can contain other entities. There are several methods to add a child entity to a container entity. The easiest way is with the `ecs_new_child` function:

```c
ecs_entity_t parent = ecs_new(world, 0);
ecs_entity_t child = ecs_new_child(world, parent, 0);
```

Alternatively, you can add an entity to a container entity after its creation using `ecs_adopt`:

```c
ecs_entity_t parent = ecs_new(world, 0);
ecs_entity_t child = ecs_new(world, 0);
ecs_adopt(world, child, parent);
```

With the `ecs_contains` function you can check whether an entity contains another entity:

```c
if (ecs_contains(world, parent, child) {
    printf("entity %u is a child of %u\n", child, parent);
}
```

Systems can request components from containers. If a system requests component `EcsPosition2D` from a container, but an entity does not have a container, or the container does not have `EcsPosition2D`, the system will not match the entity. This system definition shows an example of how a system can access container components:

```c
ECS_SYSTEM(world, MySystem, EcsOnUpdate, CONTAINER.Foo, Bar);
```

[Learn more](Manual.md#containers)

### Prefab
Prefabs are a special kind of entity that enable applications to reuse components values across entities. To create a prefab, you can use the `ECS_PREFAB` macro, or `ecs_new_prefab` function:

```c
ECS_PREFAB(world, CirclePrefab, EcsCircle, EcsPosition2D);
```

This defines a prefab with the `EcsCircle` and `EcsPosition2D` components. We can now add this prefab to regular entities:

```c
ecs_entity_t e1 = ecs_new(world, CirclePrefab);
ecs_entity_t e2 = ecs_new(world, CirclePrefab);
```

This will make the `EcsCircle` and `EcsPosition2D` components available on entities `e1` and `e2`, similar to a family. In contrast to types, component values of `EcsCircle` and `EcsPosition2D` are now shared between entities, and stored only once in memory. Since a prefab can be used as a regular entity, we can change the value of a prefab component with the `ecs_set` function:

```c
ecs_set(world, CirclePrefab, EcsCircle, {.radius = 10});
```

This will change the value of `EcsCircle` across all entities that have the prefab. Entities can override component values from a prefab, by either adding or setting a component on themselves, using `ecs_add` or `ecs_set`. When a component is added using `ecs_add`, it will be initialized with the component value of the prefab.

[Learn more](Manual.md#prefabs)

### Module
Modules are used to group entities / components / systems. They can be imported with the `ECS_IMPORT` macro:

```c
ECS_IMPORT(world, EcsComponentsTransform, 0);
```

This will invoke the `EcsComponentsTransform` function, which will define the entities / components / systems. Furthermore, the macro will declare the variables to the entity / component / system handles to the local scope, so that they can be accessed by the code. 

In large code bases modules can be used to organize code and limit exposure of internal systems to other parts of the code. Modules may be implemented in separate shared libraries, or within the same project. The only requirements for using the `ECS_IMPORT` macro is that the name of the module (`EcsComponentsTransform`) can be resolved as a C function with the right type. For an example on how to implement modules, see the implementation of one of the flecs modules (see above).

Modules can be imported multiple times without causing side effects.

[Learn more](Manual.md#modules)

