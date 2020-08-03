![flecs](https://user-images.githubusercontent.com/9919222/84740976-2ecc8580-af63-11ea-963e-c5da3be54101.png)

[![Discord Chat](https://img.shields.io/discord/633826290415435777.svg)](https://discord.gg/MRSAZqb) 
![CI build](https://github.com/SanderMertens/flecs/workflows/CI/badge.svg)
[![Build Status](https://travis-ci.org/SanderMertens/flecs.svg?branch=master)](https://travis-ci.org/SanderMertens/flecs)
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
ECS (Entity Component System) is a design pattern often found in gaming and simulation which produces code that is fast and reusable. Dynamic omposition is a first-class citizen in ECS, and there is a strict separation between data and behavior. A framework is an Entity Component System if it:

- Has _entities_ that are unique identifiers
- Has _components_ that are plain data types
- Has _systems_ which are behavior matched with entities based on their components

## Documentation
- [Quickstart](docs/Quickstart.md)
- [Manual](docs/Manual.md)
- [Migration guide](docs/MigrationGuide.md)
- [ECS FAQ](https://github.com/SanderMertens/ecs-faq)
- [C examples](examples/c)
- [C++ examples](examples/cpp)

## Example
This is a simple flecs example in the C++11 API:

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
The easiest way to add Flecs to a project is to add [flecs.c](https://github.com/flecs/master/flecs.c) and [flecs.h](https://github.com/flecs/master/flecs.h) to your source code. These files can be added to both C and C++ projects (the C++ API is embedded in flecs.h). Alternatively you can also build Flecs as a library by using the cmake, meson, bazel or bake buildfiles.

### Custom builds
Whether you're looking for a minimal ECS library or a full-fledged system runtime, customizable builds let you remove Flecs features you don't need. By default all features are included. To customize a build, follow these steps:

- define `FLECS_CUSTOM_BUILD`. This removes all optional features from the build.
- define constants for the features you want to include (see below)
- remove the files of the features you don't need

Features are split up in addons and modules. Addons implement a specific Flecs feature, like snapshots. Modules are like addons but register their own components and systems, and therefore need to be imported.

#### Addons
Addons are located in the `src/addons` and `include/addons` folders. The following addons are available:

Addon         | Description                                      | Constant            |
--------------|--------------------------------------------------|---------------------|
Bulk          | Efficient operations that run on many entities   | FLECS_BULK          | 
Dbg           | Debug API for inspection of internals            | FLECS_DBG           |
Module        | Organize components and systems in modules       | FLECS_MODULE        | 
Queue         | A queue data structure                           | FLECS_QUEUE         |
Reader_writer | Serialize components to series of bytes          | FLECS_READER_WRITER | 
Snapshot      | Take a snapshot that can be restored  afterwards | FLECS_SNAPSHOT      |

#### Builtin modules
Modules are located in the `src/modules` and `include/modules` folders. The following modules are available:

Module        | Description                                      | Constant            |
--------------|--------------------------------------------------|---------------------|
System        | Support for systems, monitors and triggers       | FLECS_SYSTEM        | 
Pipeline      | Run systems each frame and/or multithreaded      | FLECS_PIPELINE      |
Timer         | Run systems at intervals, timeouts or fixed rate | FLECS_TIMER         | 
Stats         | Collect statistics on entities and systems       | FLECS_STATS         |

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

