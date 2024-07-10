# FAQ
Frequently asked questions.

## What is an ECS?
See the [ECS FAQ](https://github.com/SanderMertens/ecs-faq)

## Why is Flecs written in C?
There are a lot of reasons, but the main ones are:
- Faster compile times, especially when compared with header-only C++ libraries
- C can be called from almost any programming language
- C is super portable and has a tiny runtime so you can run it on a toaster if you want
- The rules and limitations of the ECS aren't dictated by any type system
- You can create a zero-overhead C++ API on top of C, but not the other way around

## Can I use Flecs with C++14 or higher?
You can! Even though the C++ API is C++11, you can use it with any revision at or above 11.

## Can I use std::vector or other types inside components?
You can! Components can contain almost any C++ type.

## What is an archetype?
Archetype-based refers to the way the ECS stores components. It means that all entities with the same components are stored together in an archetype. This provides efficient CPU cache utilization and allows for things like vectorization and fast querying.

Other examples of archetype implementations are Unity DOTS, Unreal Mass and Bevy ECS.

For more information, see this blog: https://ajmmertens.medium.com/building-an-ecs-2-archetypes-and-vectorization-fe21690805f9

## How does Flecs compare with EnTT?
Flecs and EnTT both are ECS libraries, but other than that they are different in almost every way, which can make comparing the two frameworks tricky. When you are comparing Flecs and EnTT, you can _generally_ expect to see the following:

- Add/remove operations are faster in EnTT
- Single component queries are faster in EnTT
- Multi-component queries are faster in Flecs
- Bulk-creating entities are faster in Flecs
- Entity destruct are faster in Flecs (especially for worlds/registries with lots of components)
- Iterating a single entity's components are faster in Flecs

When doing a benchmark comparison don't rely on someone else's numbers, always test for your own use case!

## Is Flecs used for commercial projects?
Yes, Flecs is used commercially (see the README).

## Why are queries slow?
This is likely because queries (`flecs::query`) are created repeatedly in a loop or system. Queries are the fastest way to iterate over entities, but are expensive to create, so make sure to create them in advance:

```cpp
// GOOD
flecs::query<Position> q = world.query<Position>();

world.system()
    .run([=](flecs::iter& it) {
        q.each([&](Position& p) {
            // ...
        });
    });
```
```cpp
// BAD
world.system()
    .run([](flecs::iter& it) {
        flecs::query<Position> q = world.query<Position>();
        q.each([&](Position& p) {
            // ...
        });
    });
```

## Why are queries taking up a lot of RAM?
Likely because of the same reason as above. Queries are registered with the world, and unless they are deleted explicitly they will take up space. To delete a query, do:

```cpp
q.destruct();
```

## Why is my system called multiple times per frame?
System functions are called for each matching archetype (see above for "What is an archetype"). In short, the reason for this is that it provides systems with direct access to component arrays.

If you have code that needs to run only once per frame or you want to take control over the entire iteration, take a look the `custom_runner` examples, which show how to use the `run` callback.

## Can Flecs be compiled to web assembly?
Yes it can! See the [quickstart manual](Quickstart.md) for more information.

## Why am I getting an “use of undeclared identifier 'FLECS_ID …’” compiler error?
This happens in C if the variable that holds the component id can't be found. This example shows how to fix it: https://github.com/SanderMertens/flecs/tree/master/examples/c/entities/fwd_declare_component

## Why are my entity ids so large?
When you inspect the integer value of an entity you may see that this value is very large, usually around 4 billion. This is not a bug, and instead means that the entity id has been recycled. This example shows when recycling happens:

```cpp
flecs::entity e1 = world.entity(); // small id
e1.destruct(); // id is made available for recycling

flecs::entity e2 = world.entity(); // recycles e1
e1.is_alive(); // false
e2.is_alive(); // true

std::cout << e2.id(); // large id, upper 32 bits contains liveliness count
```

## What is the difference between add & set? Why do both exist?
An `add` just adds a component without assigning a value to it. A `set` assigns a value to the component. Both operations ensure that the entity will have the component afterwards.

An `add` is used mostly for adding things that don't have a value, like empty types/tags and most relationships. If `add` is used with a component that has a constructor, adding the component will invoke its constructor.

Additionally you can use `emplace` to construct a component in place in the storage (similar to `std::vector::emplace`).

## Can Flecs serialize components?
Yes it can! See the reflection examples:

- https://github.com/SanderMertens/flecs/tree/master/examples/c/reflection
- https://github.com/SanderMertens/flecs/tree/master/examples/cpp/reflection

## Why is Flecs so large?
If you look at the size of the flecs.c and flecs.h files you might wonder why they are so large. There are a few reasons:

- The files contain a _lot_ of comments and in-code documentation!
- Flecs has a small core with a lot of addons. The flecs.c and flecs.h files are the full source code, including addons.
- The flecs.h file contains the full C++ API.
- Flecs implements its own data structures like vectors and maps, vs. depending on something like the STL.
- C tends to be a bit more verbose than other languages.

Not all addons are useful in any project. You can customize a Flecs build to only build the things you need, which reduces executable size and improves build speed. See the quickstart on how to customize a build.

## Why does the explorer not work?
Make sure that:
- The REST API is enabled (see the [Remote API manual](FlecsRemoteApi.md))
- You can reach the REST API by testing http://localhost:27750/entity/flecs
- You call `ecs_progress`/`world::progress` in your main loop

If that doesn't work, see the [README of the explorer](https://github.com/flecs-hub/explorer) for potential issues with browser security settings and how to run a local instance of the explorer.

## Does the explorer collect data from my application?
No! The https://flecs.dev/explorer page is a 100% client side application that does not talk to a backend. When you navigate to the page it will attempt to connect to http://localhost:27750 to find a running Flecs application. The data that the explorer fetches never leaves your machine.

## Why can't I see component values in the explorer?
The explorer can only display component values if the reflection data has been registered for the component. See the [C reflection examples](https://github.com/SanderMertens/flecs/tree/master/examples/c/reflection) and [C++ reflection examples](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/reflection) for more information.

## How do I detect which entities have changed?
Flecs has builtin change detection. Additionally, you can use an `OnSet` observer to get notified of changes to component values. See the query change detection and observer examples for more information.

## Are relationships just a component with an entity handle?
No, relationships are a deeply integrated feature that is faster in many ways than a component with an entity handle. See the [relationship manual](Relationships.md) for more information.

## Can I create systems outside of the main function?
You can! Systems can be created from any function, except other systems.

## Can I use my own scheduler implementation?
You can! The flecs scheduler (implemented in the pipeline addon) is fully optional and is built on top of Flecs. You can create a custom pipeline, or a custom schedule implementation that does something else entirely than what Flecs provides.

## Can I use Flecs without using systems?
You can! Systems are an optional addon that can be disabled. You can build applications that just use Flecs queries.

## Why does the lookup function not find my entity?
This is likely because the entity has a parent. A lookup by name requires you to provide the full path to an entity, like:

```c
ecs_lookup(world, "parent.child");
```

or in C++:

```cpp
world.lookup("parent::child");
```

## Can I add or remove components from within a system?
You can! By default ECS operations are deferred when called from inside a system, which means that they are added to a queue and processed later when it's safe to do so. Flecs does not have a dedicated command API, if you call an operation from a system it will be automatically added to the command queue!

## What does a LOCKED_STORAGE error mean?
A LOCKED_STORAGE error means that you're trying to add or remove an entity to an archetype that is currently being accessed, usually during query iteration. An example:

```cpp
q.each([](flecs::entity e) {
    e.destruct(); // LOCKED_STORAGE error: removes entity from table
});
```

Most LOCKED_STORAGE errors can be resolved by putting `defer_begin` and `defer_end` around the iteration, which postpones the table-changing operations until after the iteration:

```cpp
world.defer_begin();
q.each([](flecs::entity e) {
    e.destruct();
});
world.defer_end(); // OK: entities are deleted here
```
