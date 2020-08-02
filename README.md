![flecs](https://user-images.githubusercontent.com/9919222/84740976-2ecc8580-af63-11ea-963e-c5da3be54101.png)

[![Discord Chat](https://img.shields.io/discord/633826290415435777.svg)](https://discord.gg/MRSAZqb) 
![CI build](https://github.com/SanderMertens/flecs/workflows/CI/badge.svg)
[![Build Status](https://travis-ci.org/SanderMertens/flecs.svg?branch=master)](https://travis-ci.org/SanderMertens/flecs)
[![Build status](https://ci.appveyor.com/api/projects/status/t99p1per439ctg1a/branch/master?svg=true)](https://ci.appveyor.com/project/SanderMertens/flecs/branch/master)
[![codecov](https://codecov.io/gh/SanderMertens/flecs/branch/master/graph/badge.svg)](https://codecov.io/gh/SanderMertens/flecs)

Flecs is a fast and lightweight [Entity Component System](#what-is-an-entity-component-system) for C89 / C99 / C++11 that packs a lot of punch in a small footprint:

- Blazing fast iteration speeds with direct access to raw C arrays across multiple components
- Built-in support for entity hierarchies, prefabs, nested prefabs and prefab variants
- An efficient lock-free architecture allows for modifying entities across multiple threads
- Expressive entity queries with support for and, or, not and optional operators
- Systems that are time triggered, rate triggered, run every frame or run only when needed
- A customizable core that lets you include only the features you need

This is Flecs v2, which is a breaking change from v1. For the last v1 release, see:
https://github.com/SanderMertens/flecs/releases/tag/v1.3

## What is an Entity Component System?
ECS (Entity Component System) is a way to organize code that is mostly used in gaming and simulation projects. ECS code generally performs better than traditional OOP, and is typically easier to reuse. The main differences between ECS and OOP are composition is a first class citizen in ECS, and that data is represented as plain data types rather than encapsulated classes.  A framework is an Entity Component System if it:

- Has _entities_ that are unique identifiers (integers)
- Has _components_ that are plain data types which can be added to entities
- Has _systems_ that are functions which are matched against entities with a set of components

## Documentation
- [Quickstart](docs/Quickstart.md)
- [Manual](docs/Manual.md)
- [Migration guide](docs/MigrationGuide.md)
- [ECS FAQ](https://github.com/SanderMertens/ecs-faq)
- [C examples](examples/c)
- [C++ examples](examples/cpp)

## Example
This is a simple flecs example in the C99 and C++11 APIs:

```c
typedef struct Position {
    float x;
    float y;
} Position;

typedef float Speed;

void Move(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    Speed *s = ecs_column(it, Speed, 2);
    
    for (int i = 0; i < it->count; i ++) {
        p[i].x += s[i] * it->delta_time;
        p[i].y += s[i] * it->delta_time;
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Speed);
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Speed);

    ecs_entity_t e = ecs_new(world, 0);    
    ecs_set(world, e, Position, {0, 0});
    ecs_set(world, e, Speed, {1});
    
    while (ecs_progress(world, 0));

    return ecs_fini(world);
}
```

```c++
struct Position {
    float x;
    float y;
};

struct Speed {
    float value;
};

int main(int argc, char *argv[]) {
    flecs::world world;

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

## Building
Add flecs.c and flecs.h to your project.

## Modules
The following modules are available in [flecs-hub](https://github.com/flecs-hub) and are compatible with v2:

Module      | Description      
------------|------------------
[flecs.meta](https://github.com/flecs-hub/flecs-meta) | Reflection for Flecs components
[flecs.json](https://github.com/flecs-hub/flecs-json) | JSON serializer for Flecs components
[flecs.rest](https://github.com/flecs-hub/flecs-rest) | A REST interface for introspecting & editing entities
[flecs.player](https://github.com/flecs-hub/flecs-player) | Play, stop and pause simulations
[flecs.dash](https://github.com/flecs-hub/flecs-dash) | Web-frontend for remote monitoring and debugging of Flecs apps
[flecs.components.geometry](https://github.com/flecs-hub/flecs-components-geometry) | Components that describe geometry
[flecs.components.graphics](https://github.com/flecs-hub/flecs-components-graphics) | Components used for computer graphics
[flecs.components.gui](https://github.com/flecs-hub/flecs-components-gui) | Components used to describe GUI components
[flecs.components.http](https://github.com/flecs-hub/flecs-components-http) | Components describing an HTTP server
[flecs.components.input](https://github.com/flecs-hub/flecs-components-input) | Components that describe keyboard and mouse input
[flecs.components.physics](https://github.com/flecs-hub/flecs-components-physics) | Components that describe physics and movement
[flecs.systems.civetweb](https://github.com/flecs-hub/flecs-systems-civetweb) | A civetweb-based implementation of flecs.components.http

