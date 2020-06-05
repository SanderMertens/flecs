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

void Move(ecs_view_t *view) {
    ECS_COLUMN(view, Position, p, 1);
    ECS_COLUMN(view, Speed, s, 2);
    
    for (int i = 0; i < view->count; i ++) {
        p[i].x += s[i] * view->delta_time;
        p[i].y += s[i] * view->delta_time;
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
        .each([](flecs::entity e, Position& p, Speed& s) {
            p.x += s.value * e.delta_time();
            p.y += s.value * e.delta_time();
        });

    flecs::entity(world, "MyEntity")
        .set<Position>({0, 0})
        .set<Speed>({1});

    while (world.progress()) { }
}
```

## Documentation
- [Quickstart](docs/Quickstart.md) - A quick introduction to Flecs
- [Examples](https://github.com/SanderMertens/flecs/tree/master/examples) - C and C++ code examples
- [Manual](docs/Manual.md) - Everything you want to know about Flecs and more

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
