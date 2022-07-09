# FAQ
Frequently asked questions.

## General questions

### Is Flecs fast?
Yes, very. Flecs is implemented using one of the fastest approaches to implement an entity component system (see "What is an archetype"). As a result of this approach, Flecs applications get direct access to component arrays, which enables performance that is close to the theoretical maximum. On top of that Flecs is implemented in C, which allows for a very efficient implementation as the language doesn't get in the way.

### Can Flecs be used together with existing game engines?
As long as the application is implemented in C or C++, it can use Flecs. Flecs has, for example, been used in projects together with Unreal Engine.

### Is Flecs a game engine?
No. Flecs provides an efficient way to store your game data and run game logic, but beyond that it does not provide the features you would typically expect in a game engine, such as rendering, input management, physics and so on.

### Why is Flecs written in C?
One of the main goals of Flecs is portability. Even though new operating systems support the most recent (and future!) versions of the C and C++ language standards, Flecs is used in a number of legacy systems that do not support modern C/C++. Additionally, the C API is easier to embed in existing frameworks and languages as it provides low-level untyped access to component arrays, and doesn't require wrapping of template-heavy APIs.

### Should I use the C or C++ API?
It depends. The C++ API is slightly easier to work with as it reduces the amount of boilerplate code significantly. On the other hand, the C API is easier to embed in other frameworks and can more easily be ported to different platforms.

### Is Flecs compatible with modern C++ standards?
Yes. The Flecs C++ API has been written in C++11 to balance portability with modern C++ features, but applications can utilize modern C++ standards.

### Which platforms are supported by Flecs?
Flecs is supported and validated on Windows, Linux and macOS and compiles warning free with gcc, clang and msvc. For a full list of tested platforms and compilers, see https://github.com/SanderMertens/flecs/actions/workflows/ci.yml

Additionally Flecs can be compiled as a wasm image: [https://flecs.dev/city](https://flecs.dev/city).

### How do I compile Flecs as a static library?
For the specifics on how to compile the source as a static library, check the documentation of your build system or IDE. When compiling flecs as a static library, make sure to define the `flecs_STATIC` macro while compiling the static library. You can do this in code by adding this line to the top of the flecs header:

```c
#define flecs_STATIC
```

Alternatively you can define `flecs_STATIC` in the build configuration of your project.

### Where can I ask questions about Flecs?
You can ask questions on the [Discord channel](https://discord.gg/trycKxA), or by creating an issue in the repository.

### What is an archetype?
The term "archetype" in ECS is typically (though not universally) used to describe the approach an ECS framework uses for storing components. Contrary to what the term might suggest, an "archetype" is typically not something you would see in the ECS API, but rather the data structure the ECS framework to store components.

An ECS that implements the archetype storage model stores entities with the same components together in the same set of component arrays (or array, depending on whether the framework uses SoA or AoS). The advantage of this approach is that it guarantees that components are always stored in packed arrays, which allows for code to be more easily vectorized by compilers (or in plain language: code runs faster).

The Flecs storage model is based on the archetypes approach.

### What is a table?
The documentation uses the terms archetype and table interchangeably. They are the same. Internally in the Flecs source code an archetype is referred to as a table.

## Development questions

### Why is my system called multiple times per frame?
Flecs stores entities with the same set of components in the same arrays in an "archetype". This means that entities with `Position` are stored in different arrays than entities with `Position, Velocity`.

Because Flecs systems provide direct access to C arrays, a system is invoked multiple times, once for each set of arrays.

### Why is the value of a component not set in an OnAdd observer?
The `OnAdd` observer is invoked before the component value is assigned. If you need to respond to a component value, use an `OnSet` system. For more information on when observers, monitors and `OnSet` systems are invoked, see [this diagram](Manual.md#component-add-flow).

### Why does Flecs abort when I try to use threads?
Flecs has an operating system abstraction API with threading functions that are not set by default. Check (or use) the OS API examples to see how to set the OS API.

### Why am I getting errors like FLECS__TPosition or FLECS_EPosition not found?
Flecs functions need access to component handles before they can do anything. In C++ this is abstracted away behind templates, but in the C API this is the responsibility of the application. See [this section of the manual](Manual.md#component_handles) on how to pass component handles around in the application.

### How do I pass component handles around in an application?
See the previous question.

### When to use each vs. iter when evaluating a query or system?
Queries and systems offer two ways to iterate them, which is using `each` and `iter`. `each` is the simpler version of the two, which iterates each individual entity:

```cpp
world.system<Position, Velocity>()
    .each([](flecs::entity e, Position& p, Velocity& v) {
        p.x += v.x;
        p.y += v.y;
    });
```

`iter` is more complex, but is faster to evaluate and allows for more control over how the loop is executed:

```cpp
world.system<Position, Velocity>()
    .iter([](flecs::iter& it, Position* p, Velocity* v) {
        for (auto i : it) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
        }
    });
```

Additionally, `iter` can be used for more complex queries (see next question).

### Why can I create queries and systems both as template parameters and as strings?
In the C++ API you can express simple queries with plain template parameters like this example:

```cpp
auto q = ecs.query<Position, Velocity>();

q.each([](flecs::entity e, Position& p, Velocity& v) {
    // ...
});
```

That same query can also be specified as a string, at the cost of a slightly more complex API:

```cpp
auto q = ecs.query<>("Position, Velocity");

q.iter([](flecs::iter& it) {
    auto p = it.term<Position>(1);
    auto v = it.term<Velocity>(2);
});
```

In most cases using template parameters is the way to go, as this provides a slightly easier to use and nicer API. However, for more complex queries, template parameters are insufficient. For example, queries can select entities in a hierarchy from top to bottom with the `cascade` modifier:

```cpp
auto q = ecs.query<>("Position(parent|cascade), Position");

q.iter([](flecs::iter& it) {
    auto p_parent = it.term<Position>(1);
    auto p = it.term<Position>(2);
});
```

So in short, for simple queries, use template parameters. For complex queries, use strings.

### How do I attach resources to a system?
If you want to attach data to a system, you can specify a `ctx` parameter in the `ecs_system_desc_t` struct parameter passed to the `ecs_system_init` function:

```c
ecs_system_init(world, &(ecs_system_init_t){
    .entity = {.name = "MySystem"},
    .callback = MySystemCallback,
    .ctx = my_context_ptr
});
```

Alternatively, if you use the ECS_SYSTEM macro you can use `ecs_system_init` with the handle to the existing system to set the context:

```c
ECS_SYSTEM(world, MySystem, EcsOnUpdate, Position, Velocity);

ecs_system_init(world, &(ecs_system_init_t) {
    .entity = {MySystem},
    .ctx = my_context_ptr
})
```

This variable will now be accessible through the `ctx` member of the system iterator:

```c
void MySystem(ecs_iter_t *it) {
    int *my_context_ptr = it->ctx;
    // ...
}
```

Systems in C++ can use the `ctx` method:

```cpp
auto sys = world.system<Position, Velocity>()
    .ctx(my_context_ptr)
    .iter([](flecs::iter& it, Position *p, Velocity *v) {
        int *my_context_ptr = it->ctx();
    });

sys.ctx(another_ptr);
```

### Why is my system (or query) unable to find a component from a module?
When you import a module, components are automatically namespaced to prevent name clashes between modules. In C this namespace is determined by taking the name of the module and convert it from PascalCase to a dot-separated notation, so that `MyModule` becomes `my.module`. If component `Position` is defined in module `MyModule`, a system or query will need to use `my.module.Position` in the query expression.

In C++ the component name is prefixed by the C++ namespace, so that `my::module::Position` in C++ becomes `my.module.Position` in the query expression. Note that this only needs to be specified this way when providing a query expression as a string.

### How do I order my systems?
Systems can be ordered using two mechanisms. The first mechanism is the "phase". By default systems are added to the `EcsOnUpdate` phase, which is usually where all of the gameplay logic is executed. Systems can be assigned to other phases, like `EcsPreUpdate` and `EcsPostUpdate`. For a full list of all phases, see:
https://github.com/SanderMertens/flecs/blob/master/docs/Quickstart.md#pipelines

The second mechanism is declaration order. If two systems are assigned to the same phase, the order in which they are executed is the order in which they are declared.

Additionally you can take full control over when to run your systems by not assigning systems to a phase (instead of `EcsOnUpdate` just specify 0). To run a system, you can use the `ecs_run` function:

```c
// delta_time and some_param may be 0
ecs_run(world, MySystem, delta_time, &some_param);
```

### Are queries order sensitive?
No, a query for `Position, Velocity` matches with the same entities as `Velocity, Position`.

### Can I add/remove components in a system?
Yes, you can use the regular add/remove functions in systems. Note however that these operations will not have an immediate effect, as they are _deferred_ until the end of the frame. See [this diagram](Manual.md#staging-flow) on staging for more information.

### Why are updates to components made by my system lost?
If you have a system in which you both write to component arrays as well as adding/removing components from your entity, it may happen that you lose your updates. A solution to this problem is to split up your system in two, one that sets the component values, and one that adds/removes the components.

### What is the difference between a system and a query?
They are very similar. A system is a query paired up with a function that is executed automatically by the framework each frame.

### When should I use queries versus filters?
Queries are the fastest way to iterate over entities as they are "pre-matched", which means that a query does not need to search as you're iterating it. Queries also provide the most flexible mechanism for selecting entities, with many query operators and other features. Queries are expensive to create however, as they register themselves with other parts of the framework.

Filters on the other hand are slower to iterate over as they perform searching while you're iterating them, but they are very cheap to create as they are simple stack-objects that require no additional setup.

Often a combination of queries and filters works best. See the `ecs_query_next_w_filter` function.

### How do I create tags in an C++ application?
In the C API there is the `ECS_TAG` macro to create tags, but the C++ API does not have an equivalent function or class. The easiest way to create a tag in the C++ API is to create an empty struct, and use it as a regular component.
