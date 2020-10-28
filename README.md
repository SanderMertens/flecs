![flecs](https://user-images.githubusercontent.com/9919222/84740976-2ecc8580-af63-11ea-963e-c5da3be54101.png)

[![GitHub version](https://badge.fury.io/gh/sandermertens%2Fflecs.svg)](https://badge.fury.io/gh/sandermertens%2Fflecs)
[![CI build](https://github.com/SanderMertens/flecs/workflows/CI/badge.svg)](https://github.com/SanderMertens/flecs/actions)
[![Build Status](https://travis-ci.org/SanderMertens/flecs.svg?branch=master)](https://travis-ci.org/SanderMertens/flecs)
[![codecov](https://codecov.io/gh/SanderMertens/flecs/branch/master/graph/badge.svg)](https://codecov.io/gh/SanderMertens/flecs)
[![Documentation](https://img.shields.io/badge/docs-docsforge-blue)](http://flecs.docsforge.com/)
[![Discord Chat](https://img.shields.io/discord/633826290415435777.svg)](https://discord.gg/MRSAZqb) 

Flecs is a fast and lightweight [Entity Component System](#what-is-an-entity-component-system) with a focus on high performance game development:

- Easy to integrate, fast to compile with core that is entirely written in C99
- Includes C++11 API that enforces type safety & tightly integrates with modern C++ idioms
- Provides (SoA) access to raw component arrays for optimal cache efficiency and vectorization
- Archetype-storage with unique graph-based design enables entity mutations with overhead measured in nanoseconds
- Flexible API primitives allow for efficient implementation of prefabs, runtime tags and entity graphs
- Supports advanced queries that are evaluated offline so no searching is performed in the main loop
- Lockless threading design allows for efficient execution of systems on multiple threads

**In flecs you only pay for what you use:**
- A customizable core makes it easy to strip unnecessary features from the library
- An adaptive scheduler removes systems that aren't matched with entities from the main loop
- Code paths are designed to minimize branching and bypass unused features

Flecs comes with the following optional utilities:
- Snapshots enable saving & restoring the state of a game
- A reflection-free serializer allows for fast serializing/deserializing of component data to a file/buffer
- A module system for organizing components and systems into reusable units
- A statistics API for monitoring runtime performance of an application
- [A minimal reflection system](https://github.com/flecs-hub/flecs-meta) for introspecting component values
- [A REST API](https://github.com/flecs-hub/flecs-rest) for remotely accessing component data
- [A Lua binding](https://github.com/flecs-hub/flecs-lua) (work in progress)

[Join the Flecs Discord](https://discord.gg/MRSAZqb)!

## What is an Entity Component System?
ECS (Entity Component System) is a design pattern often found in gaming and simulation which produces code that is fast and reusable. Dynamic composition is a first-class citizen in ECS, and there is a strict separation between data and behavior. A framework is an Entity Component System if it:

- Has _entities_ that are unique identifiers
- Has _components_ that are plain data types
- Has _systems_ which are behavior matched with entities based on their components

## Documentation
- [Quickstart](docs/Quickstart.md)
- [FAQ](docs/FAQ.md)
- [Manual](docs/Manual.md)
- [Migration guide](docs/MigrationGuide.md)
- [ECS FAQ](https://github.com/SanderMertens/ecs-faq)
- [C examples](examples/c)
- [C++ examples](examples/cpp)

See [Docsforge](http://flecs.docsforge.com/) for a more readable version of the documentation.

## Example
This is a simple flecs example in the C++11 API:

```c++
struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

int main(int argc, char *argv[]) {
    flecs::world ecs;

    ecs.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x * e.delta_time();
            p.y += v.y * e.delta_time();
            std::cout << "Entity " << e.name() << " moved!" << std::endl;
        });

    ecs.entity("MyEntity")
        .set<Position>({0, 0})
        .set<Velocity>({1, 1});

    while (ecs.progress()) { }
}
```

## Building
The easiest way to add Flecs to a project is to add [flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.c) and [flecs.h](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.h) to your source code. These files can be added to both C and C++ projects (the C++ API is embedded in flecs.h). Alternatively you can also build Flecs as a library by using the cmake, meson, bazel or bake buildfiles.

### Custom builds
The Flecs source has a modular design which makes it easy to strip out code you don't need. At its core, Flecs is a minimalistic ECS library with a lot of optional features that you can choose to include or not. [This section of the manual](https://github.com/SanderMertens/flecs/blob/master/docs/Manual.md#custom-builds) describes how to customize which features to include. 

## Modules
The following modules are available in [flecs-hub](https://github.com/flecs-hub) and are compatible with v2:

Module      | Description      
------------|------------------
[flecs.meta](https://github.com/flecs-hub/flecs-meta) | Reflection for Flecs components
[flecs.json](https://github.com/flecs-hub/flecs-json) | JSON serializer for Flecs components
[flecs.rest](https://github.com/flecs-hub/flecs-rest) | A REST interface for introspecting & editing entities
[flecs.player](https://github.com/flecs-hub/flecs-player) | Play, stop and pause simulations
[flecs.dash](https://github.com/flecs-hub/flecs-dash) | Web-frontend for remote monitoring and debugging of Flecs apps
[flecs.components.input](https://github.com/flecs-hub/flecs-components-input) | Components that describe keyboard and mouse input
[flecs.components.transform](https://github.com/flecs-hub/flecs-components-transform) | Components that describe position, rotation and scale
[flecs.components.physics](https://github.com/flecs-hub/flecs-components-physics) | Components that describe physics and movement
[flecs.components.geometry](https://github.com/flecs-hub/flecs-components-geometry) | Components that describe geometry
[flecs.components.graphics](https://github.com/flecs-hub/flecs-components-graphics) | Components used for computer graphics
[flecs.components.gui](https://github.com/flecs-hub/flecs-components-gui) | Components used to describe GUI components
[flecs.components.http](https://github.com/flecs-hub/flecs-components-http) | Components describing an HTTP server
[flecs.systems.transform](https://github.com/flecs-hub/flecs-systems-transform) | Hierarchical transforms for scene graphs
[flecs.systems.sdl2](https://github.com/flecs-hub/flecs-systems-sdl2) | SDL window creation & input management
[flecs.systems.sokol](https://github.com/flecs-hub/flecs-systems-sokol) | Sokol-based renderer
[flecs.systems.civetweb](https://github.com/flecs-hub/flecs-systems-civetweb) | A civetweb-based implementation of flecs.components.http
