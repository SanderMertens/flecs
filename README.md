![flecs](docs/img/logo.png)

[![Version](https://img.shields.io/github/v/release/sandermertens/flecs?include_prereleases&style=for-the-badge)](https://github.com/SanderMertens/flecs/releases)
[![MIT](https://img.shields.io/badge/license-MIT-blue.svg?style=for-the-badge)](https://github.com/SanderMertens/flecs/blob/master/LICENSE)
[![Documentation](https://img.shields.io/badge/docs-flecs-blue?style=for-the-badge&color=blue)](https://www.flecs.dev/flecs/md_docs_2Docs.html)
[![actions](https://img.shields.io/github/actions/workflow/status/SanderMertens/flecs/ci.yml?branch=master&style=for-the-badge)](https://github.com/SanderMertens/flecs/actions?query=workflow%3ACI)
[![Discord Chat](https://img.shields.io/discord/633826290415435777.svg?style=for-the-badge&color=%235a64f6)](https://discord.gg/BEzP5Rgrrp)

Flecs is a fast and lightweight Entity Component System that lets you build games and simulations with millions of entities ([join the Discord!](https://discord.gg/BEzP5Rgrrp)). Here are some of the framework's highlights:

- Fast and [portable](#language-bindings) zero dependency [C99 API](https://www.flecs.dev/flecs/group__c.html)
- Modern type-safe [C++11 API](https://www.flecs.dev/flecs/group__cpp.html) that doesn't use STL containers
- First open source ECS with full support for [Entity Relationships](https://www.flecs.dev/flecs/md_docs_2Relationships.html)!
- Fast native support for [hierarchies](https://www.flecs.dev/flecs/md_docs_2Relationships.html#the-childof-relationship) and [prefabs](https://www.flecs.dev/flecs/md_docs_2Relationships.html#the-isa-relationship)
- Code base that builds in less than 5 seconds
- Runs [in the browser](https://flecs.dev/city) without modifications with emscripten
- Cache friendly [archetype/SoA storage](https://ajmmertens.medium.com/building-an-ecs-2-archetypes-and-vectorization-fe21690805f9) that can process millions of entities every frame
- Supports entities with hundreds of components and applications with tens of thousands of archetypes
- Automatic component registration that works out of the box across shared libraries/DLLs
- Write free functions with [queries](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/queries/basics) or run code automatically in [systems](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/systems/pipeline)
- Run games on multiple CPU cores with a fast lockless scheduler
- Verified on all major compilers and platforms with [CI](https://github.com/SanderMertens/flecs/actions) running more than 6000 tests
- Integrated [reflection framework](https://www.flecs.dev/flecs/group__c__addons__meta.html) with [JSON serializer](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/reflection/basics_json) and support for [runtime components](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/reflection/runtime_component)
- [Unit annotations](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/reflection/units) for components
- Powerful [query language](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/rules) with support for [joins](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/rules/setting_variables) and [inheritance](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/rules/component_inheritance)
- [Statistics addon](https://www.flecs.dev/flecs/group__c__addons__stats.html) for profiling ECS performance
- A web-based UI for monitoring & controlling your apps ([demo](https://flecs.dev/explorer), [code](https://github.com/flecs-hub/explorer)):

[![Flecs Explorer](docs/img/explorer.png)](https://flecs.dev/explorer)

To support the project, give it a star 🌟 !

## What is an Entity Component System?
ECS is a way of organizing code and data that lets you build games that are larger, more complex and are easier to extend. Something is called an ECS when it:
- Has _entities_ that uniquely identify objects in a game
- Has _components_ which are datatypes that can be added to entities
- Has _systems_ which are functions that run for all entities matching a component _query_

For more information, check the [ECS FAQ](https://github.com/SanderMertens/ecs-faq)!

## Try it out!
The [Flecs playground](https://www.flecs.dev/explorer/?local=true&wasm=https://www.flecs.dev/explorer/playground.js) lets you try Flecs without writing any C/C++ code!

[![Flecs playground](docs/img/playground.png)](https://www.flecs.dev/explorer/?local=true&wasm=https://www.flecs.dev/explorer/playground.js)

To learn how to use the playground, check the [Flecs Script Tutorial](https://www.flecs.dev/flecs/md_docs_2FlecsScriptTutorial.html).

## Documentation
- [Quickstart](https://www.flecs.dev/flecs/md_docs_2Quickstart.html)
- [FAQ](https://www.flecs.dev/flecs/md_docs_2FAQ.html)
- [Examples](https://github.com/SanderMertens/flecs/tree/master/examples)
- [All Documentation](https://www.flecs.dev/flecs/md_docs_2Docs.html)

## Performance
For a list of regularly tracked benchmarks, see the [ECS Benchmark](https://github.com/SanderMertens/ecs_benchmark) project.

## Show me the code!
C99 example:
```c
typedef struct {
  float x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
  Position *p = ecs_field(it, Position, 1);
  Velocity *v = ecs_field(it, Velocity, 2);

  for (int i = 0; i < it->count; i++) {
    p[i].x += v[i].x;
    p[i].y += v[i].y;
  }
}

int main(int argc, char *argv[]) {
  ecs_world_t *ecs = ecs_init();

  ECS_COMPONENT(ecs, Position);
  ECS_COMPONENT(ecs, Velocity);

  ECS_SYSTEM(ecs, Move, EcsOnUpdate, Position, Velocity);

  ecs_entity_t e = ecs_new_id(ecs);
  ecs_set(ecs, e, Position, {10, 20});
  ecs_set(ecs, e, Velocity, {1, 2});

  while (ecs_progress(ecs, 0)) { }
}
```

Same example in C++11:

```cpp
struct Position {
  float x, y;
};

struct Velocity {
  float x, y;
};

int main(int argc, char *argv[]) {
  flecs::world ecs;

  ecs.system<Position, const Velocity>()
    .each([](Position& p, const Velocity& v) {
      p.x += v.x;
      p.y += v.y;
    });

  auto e = ecs.entity()
    .set([](Position& p, Velocity& v) {
      p = {10, 20};
      v = {1, 2};
    });

  while (ecs.progress()) { }
}
```

## Projects using Flecs
If you have a project you'd like to share, let me know on [Discord](https://discord.gg/BEzP5Rgrrp)!

### [Hytale](https://hytale.com/)
[![Hytale](docs/img/projects/hytale.png)](https://hytale.com/)

> We knew that we wanted to build Hytale around an Entity-Component-System (ECS). When we analyzed the options, FLECS rose to the top. FLECS provides the backbone of the Hytale Game Engine. Its flexibility has allowed us to build highly varied gameplay while supporting our vision for empowering Creators.

-- Dann Webster, Hypixel studios

### [Tempest Rising](https://store.steampowered.com/app/1486920/Tempest_Rising/)
[![Tempest Rising](docs/img/projects/tempest_rising.png)](https://store.steampowered.com/app/1486920/Tempest_Rising/)

### [Territory Control](https://store.steampowered.com/app/690290/Territory_Control_2/)
[![image](docs/img/projects/territory_control.jpeg)](https://store.steampowered.com/app/690290/Territory_Control_2/)

### [The Forge](https://github.com/ConfettiFX/The-Forge)
[![image](docs/img/projects/the_forge.jpg)](https://github.com/ConfettiFX/The-Forge)

### [Extermination Shock](https://store.steampowered.com/app/2510820/Extermination_Shock/)
[![image](docs/img/projects/extermination_shock.jpeg)](https://store.steampowered.com/app/2510820/Extermination_Shock/)

### [Sol Survivor](https://nicok.itch.io/sol-survivor-demo)
[![image](docs/img/projects/sol_survivor.png)](https://nicok.itch.io/sol-survivor-demo)

### [Equilibrium Engine](https://github.com/clibequilibrium/EquilibriumEngine)
[![image](docs/img/projects/equilibrium_engine.png)](https://github.com/clibequilibrium/EquilibriumEngine)

### [Gravitas](https://thepunkcollective.itch.io/gravitas)
[![image](docs/img/projects/gravitas.png)](https://thepunkcollective.itch.io/gravitas)

### [After Sun](https://github.com/foxnne/aftersun)
[![image](docs/img/projects/after_sun.png)](https://github.com/foxnne/aftersun)

### Flecs Demo's
https://www.flecs.dev/tower_defense/etc ([repository](https://github.com/SanderMertens/tower_defense))
![image](docs/img/projects/tower_defense.png)

https://www.flecs.dev/city ([repository](https://github.com/flecs-hub/city))
![image](docs/img/projects/city.png)

## Resources

### Resources provided by the community :heart:
- [Unreal Minimum Viable Flecs Project](https://github.com/PreyK/Unreal-Minimum-Viable-Flecs)
- [Bgfx/Imgui module](https://github.com/flecs-hub/flecs-systems-bgfx/tree/bgfx_imgui)
- [Tower defense example](https://gist.github.com/oldmanauz/b4ced44737bf9d248233538fa06a989e)
- [Unreal + Flecs example](https://github.com/PreyK/Unreal-Minimum-Viable-Flecs)
- [Building a space battle with Flecs in UE4](https://twitter.com/ajmmertens/status/1361070033334456320)
- [Flecs + SDL + Web ASM example](https://github.com/HeatXD/flecs_web_demo) ([live demo](https://heatxd.github.io/flecs_web_demo/))
- [Flecs + Raylib example](https://github.com/Lexxicon/FlecsRaylib)
- [Flecs + gunslinger example](https://github.com/MrFrenik/gs_examples/blob/main/ex_demos/flecs/source/main.c)
- [Flecs based 3D game engine with editor](https://bit.ly/3T9cc1o)

### Flecs around the web
- [Discord](https://discord.gg/BEzP5Rgrrp)
- [Medium](https://ajmmertens.medium.com)
- [Twitter](https://twitter.com/ajmmertens)
- [Reddit](https://www.reddit.com/r/flecs)

## Flecs Hub
[Flecs Hub](https://github.com/flecs-hub) is a collection of repositories that show how Flecs can be used to build game systems like input handling, hierarchical transforms and rendering.

Module      | Description
------------|------------------
[flecs.components.cglm](https://github.com/flecs-hub/flecs-components-cglm) | Component registration for cglm (math) types
[flecs.components.input](https://github.com/flecs-hub/flecs-components-input) | Components that describe keyboard and mouse input
[flecs.components.transform](https://github.com/flecs-hub/flecs-components-transform) | Components that describe position, rotation and scale
[flecs.components.physics](https://github.com/flecs-hub/flecs-components-physics) | Components that describe physics and movement
[flecs.components.geometry](https://github.com/flecs-hub/flecs-components-geometry) | Components that describe geometry
[flecs.components.graphics](https://github.com/flecs-hub/flecs-components-graphics) | Components used for computer graphics
[flecs.components.gui](https://github.com/flecs-hub/flecs-components-gui) | Components used to describe GUI components
[flecs.systems.transform](https://github.com/flecs-hub/flecs-systems-transform) | Hierarchical transforms for scene graphs
[flecs.systems.physics](https://github.com/flecs-hub/flecs-systems-physics) | Systems for moving objects and collision detection
[flecs.systems.sokol](https://github.com/flecs-hub/flecs-systems-sokol) | Sokol-based renderer
[flecs.game](https://github.com/flecs-hub/flecs-game) | Generic game systems, like a camera controller

## Language bindings
The following language bindings have been developed with Flecs! Note that these are projects built and maintained by helpful community members, and may not always be up to date with the latest commit from master!
- C#:
  - [flecs-hub/flecs-cs](https://github.com/flecs-hub/flecs-cs)
  - [BeanCheeseBurrito/Flecs.NET](https://github.com/BeanCheeseBurrito/Flecs.NET)
- Rust:
  - [flecs-rs](https://github.com/jazzay/flecs-rs)
  - [flecs-polyglot](https://github.com/flecs-hub/flecs-polyglot)
- Zig:
  - [michal-z/zig-gamedev](https://github.com/michal-z/zig-gamedev/tree/main/libs/zflecs)
  - [foxnne/zig-flecs](https://github.com/foxnne/zig-flecs)
  - [prime31/zig-flecs](https://github.com/prime31/zig-flecs)
- Lua:
  - [sro5h/flecs-luajit](https://github.com/sro5h/flecs-luajit)
  - [flecs-hub/flecs-lua](https://github.com/flecs-hub/flecs-lua)
