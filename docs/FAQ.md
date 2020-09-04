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
One of the main goals of Flecs is portability. Even though new operating systems support the most recent (and future!) versions of the C and C++ language standards, Flecs is used in a number of legacy systems that do not support modern C/C++. Additionally, the C API is easier to embed in existing frameworks and languages as it provides low-level untyped acces to component arrays, and doesn't require wrapping of template-heavy APIs.

### Should I use the C or C++ API?
It depends. The C++ API is slightly easier to work with as it reduces the amount of boilerplate code significantly. On the other hand, the C API is easier to embed in other frameworks and can more easily be ported to different platforms.

### Is Flecs compatible with modern C++ standards?
Yes. The Flecs C++ API has been written in C++11 to balance portability with modern C++ features, but applications can utilize modern C++ standards.

### Which platforms are supported by Flecs?
Flecs is regularly validated on the following platforms:
- Ubuntu Precise, x64 (gcc)
- Ubuntu Xenial, x64 (gcc)
- Ubuntu Xenial, x64 (clang)
- Ubuntu Xenial, arm (gcc)
- Ubuntu Bionic, x64 (gcc)
- Ubuntu Bionic, x64 (clang)
- MacOS 10.14, x64 (clang)
- MacOS 10.10, x64 (clang)
- Windows 10, x64 (msvc, visual studio 2019)

Additionally, Flecs has been used with emscripten and on iOS.

### Where can I ask questions about Flecs?
You can ask questions on the [Discord channel](https://discord.gg/trycKxA), or by creating an issue in the repository.

### What is an archetype?
The term "archetype" in ECS is typically (though not universally) used to describe the approach an ECS framework uses for storing components. Contrary to what the term might suggest, an "archetype" is typically not somtething you would see in the ECS API, but rather the data structure the ECS framework to store components.

An ECS that implements the archetype storage model stores entities with the same components together in the same set of component arrays (or array, depending on whether the framework uses SoA or AoS). The advantage of this approach is that it guarantees that components are always stored in packed arrays, which allows for code to be more easily vectorized by compilers (or in plain language: code runs faster).

The Flecs storage model is based on the archetypes approach.

### What is a table?
The documentation uses the terms archetype and table interchangably. They are the same. Internally in the Flecs source code an archetype is referred to as a table.

## Development questions

### Why is my system called multiple times per frame?
Flecs stores entities with the same set of components in the same arrays in an "archetype". This means that entities with `Position` are stored in different arrays than entities with `Position, Velocity`. 

Because Flecs systems provide direct access to C arrays, a system is invoked multiple times, once for each set of arrays.

### Why does Flecs abort when I try to use threads?
Flecs has an operating system abstraction API with threading functions that are not set by default. Check (or use) the OS API examples to see how to set the OS API.

### Why am I getting errors like FLECS__TPosition or FLECS_EPosition not found?
Flecs functions need access to component handles before they can do anything. In C++ this is abstracted away behind templates, but in the C API this is the responsibility of the application. See [this section of the manual](https://github.com/SanderMertens/flecs/blob/master/docs/Manual.md#component_handles) on how to pass component handles around in the application.

### How do I pass component handles around in an application?
See the previous question.

### Can I add/remove components in a system?
Yes.

### Why are updates to components made by my system lost?
If you have a system in which you both write to component arrays as well as adding/removing components from your entity, it may happen that you lose your updates. A solution to this problem is to split up your system in two, one that sets the component values, and one that adds/removes the components.

### When should I use queries versus filters?
Queries are the fastest way to iterate over entities as they are "prematched", which means that a query does not need to search as you're iterating it. Queries also provide the most flexible mechanism for selecting entities, with many query operators and other features. Queries are expensive to create however, as they register themselves with other parts of the framework. 

Filters on the other hand are slower to iterate over as they perform searching while you're iterating them, but they are very cheap to create as they are simple stack-objects that require no additional setup.

Often a combination of queries and filters works best. See the `ecs_query_next_w_filter` function.

### How do I create tags in an C++ application?
In the C API there is the `ECS_TAG` macro to create tags, but the C++ API does not have an equivalent function or class. The easiest way to create a tag in the C++ API is to create an empty struct, and use it as a regular component.

