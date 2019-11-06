# Flecs Manual

## Contents

- [Design goals](#design-goals)
  - [Portability](#portability)
  - [Reusability](#reusability)
  - [Clean interfaces](#clean-interfaces)
  - [Performance](#performance)
- [API design](#api-design)
  - [Naming conventions](#naming-conventions)
  - [Handles](#handles)
  - [Error handling](#error-handling)
  - [Memory management](#memory-management)
- [Good practices](#good-practices)
  - [Minimize usage of ecs_get, ecs_set](#minimize-the-usage-of-ecs_get-ecs_set)
  - [Never compare types with anything](#never-compare-entity-types-with-anything)
  - [Write logic in systems](#write-logic-in-systems)
  - [Organize code in modules](#organize-code-in-modules)
  - [Do not depend on systems in other modules](#do-not-depend-on-systems-in-other-modules)
  - [Expose features, not systems in modules](#expose-features-not-systems-in-modules)
  - [Use types where possible](#use-types-where-possible)
  - [Use declarative statements for static data](#use-declarative-statements-for-static-data)
  - [Create entities in bulk](#create-entities-in-bulk)
  - [Limit usage of ecs_lookup](#limit-usage-of-ecs_lookup)
  - [Use ecs_quit to signal that your application needs to exit](#use-ecs_quit-to-signal-that-your-application-needs-to-exit)
  - [Update components proportionally to delta_time](#update-components-proportionally-to-delta_time)
  - [Set a target FPS for applications](#set-a-target-fps-for-applications)
  - [Never store pointers to components](#never-store-pointers-to-components)
- [Entities](#entities)
  - [Entity basics](#entity-basics)
    - [Creating entities](#creating-entities)
    - [Create entities in bulk](#create-entities-in-bulk)
    - [Deleting entities](#deleting-entities)
  - [Hierarchies](#hierarchies)
    - [Creating child entities](#creating-child-entities)
    - [Adopting entities](#adopting-entities)
    - [Orphaning entities](#orphaning-entities)
    - [Hierarchies and types](#hierarchies-and-types)
  - [Inheritance](#inheritance)
    - [Creating instances](#creating-instances)
    - [Adding inheritance relationships](#adding-inheritance-relationships)
    - [Removing inheritance relationships](#adding-inheritance-relationships)
    - [Inheritance and types](#inheritance-and-types)
    - [Multiple inheritance](#multiple-inheritance)
    - [Overriding components](#overriding-components)
    - [Specialization](#Specialization)
    - [Prefabs](#prefabs)
    - [Prefab nesting](#prefab-nesting)
- [Components and Types](#components-and-types)
  - [Add components](#add-components)
  - [Remove components](#remove-components)
  - [Set components](#set-components)
  - [Tags](#tags)
  - [Builtin components](#builtin-components)
- [Systems](#systems)
   - [System signatures](#system-signatures)
     - [Column operators](#column-operators)
       - [OR operator](#or-operator)
       - [NOT operator](#not-operator)
       - [Optional operator](#optional-operator)
     - [Column source modifiers](#column-source-modifiers)
       - [SELF source](#self-source)
       - [OWNED source](#owned-source)
       - [SHARED source](#shared-source)
       - [NOTHING source](#nothing-source)
       - [CONTAINER source](#container-source)
       - [CASCADE source](#cascade-source)
       - [SYSTEM source](#system-source)
       - [SINGLETON source](#singleton-source)
       - [ENTITY source](#entity-source)
   - [System API](#system-api)
     - [The ECS_COLUMN macro](#the-ecs_column-macro)
     - [The ECS_COLUMN_COMPONENT macro](#the-ecs_column_component-macro)
     - [The ECS_COLUMN_ENTITY macro](#the-ecs_column_entity-macro)
   - [System phases](#system-phases)
     - [The EcsOnLoad phase](#the-ecsonload-phase)
     - [The EcsPostLoad phase](#the-ecspostload-phase)
     - [The EcsPreUpdate phase](#the-ecspreupdate-phase)
     - [The EcsOnUpdate phase](#the-ecsonupdate-phase)
     - [The EcsOnValidate phase](#the-ecsonvalidate-phase)
     - [The EcsPostUpdate phase](#the-ecspostupdate-phase)
     - [The EcsPreStore phase](#the-ecsprestore-phase)
     - [The EcsOnStore phase](#the-ecsonstore-phase)
     - [System phases example](#system-phases-example)
   - [Reactive systems](#reactive-systems)
     - [EcsOnAdd event](#ecsonadd-event)
     - [EcsOnRemove event](#ecsonremove-event)
     - [EcsOnSet event](#ecsonset-event)
   - [Features](#features)
- [Staging](#staging)
  - [Staged vs. inline modifications](#staged-vs-inline-modifications)
  - [Staging and ecs_get_ptr](#staging-and-ecs_get_ptr)
  - [Overwriting the stage](#overwriting-the-stage)
  - [Staging and EcsOnAdd, EcsOnSet and EcsOnRemove](#staging-and-ecsonadd-ecsonset-and-ecsonremove)
  - [Staging and system phases](#staging-and-system-phases)
  - [Staging and threading](#staging-and-threading)
  - [Manually merging stages](#manually-merging-stages)
  - [Limitations of staging](#limitations-of-staging)
- [Modules](#modules)
  - [Importing modules](#importing-modules)
    - [Module content handles](#module-content-handles)
    - [Dynamic imports](#dynamic-imports)
  - [Creating modules](#creating-modules)
- [Internals](#internals)
  - [Archetypes](#archetypes)
  - [System internals](#system-internals)
  - [Type internals](#type-internals)
  - [Entity internals](#entity-internals)
- [Operating system abstraction API](#operating-system-abstraction-api)

## Design Goals
Flecs is designed with the following goals in mind, in order of importance:

### Portability
Flecs is implemented in C99 which makes it easy to port to a variety of platforms and (legacy) compilers. To further improve portability, Flecs has no mandatory external dependencies. For certain optional features, like threading and running the web-based dashboard, Flecs relies on external libraries like pthreads (or equivalent), civetweb and bake.util, but Flecs can be easily used without them.

### Reusability
Flecs has been designed so that it is easy to package systems and components in a way that can be easily reused across applications. The module design allows applications to import modules with a single line of code, after which the imported components and systems can be immediately used. To facilitate this, Flecs has an architecture that imposes a well-defined order on systems, so that imported systems are always executed in the right order, regardless of in wich order they are imported.

To further improve reusability, Flecs allows for writing code that is agnostic to how data is stored by the framework. While applications may make decisions to optimize storage for a particular usage pattern, Flecs allows systems to be written in a way that works across different storage modes.

### Clean interfaces
Flecs aims to provide clear and simple interfaces, by staying close to the core principles of Entity Component Systems. Someone who has worked with Entity Component Systems before should find it easy to read and write code written with Flecs. Flecs promotes a declarative design, where applications only need to state their intent.

### Performance
Flecs has a design that is optimized for minimizing cache misses by loading only data in cache that is required by the application, while also storing data in arrays (SoA) to ensure that an application makes optimal usage of cache lines. In many cases, applications can access raw arrays directly, wich is as fast as iterating a native array in C and, if the code permits it, lets applications be compiled with Single Instruction, Multiple Data (SIMD) instructions.

Furthermore, Flecs automatically optimizes performance where it can, by removing systems from the critical path if they are unused. This further improves reusability of code, as it lets applications import modules of which only a subset of the systems is used, without increasing overhead of the framework.

## API design

### Naming conventions
The Flecs API adheres to a set of well-defined naming conventions, to make it easier to read and write Flecs code. The basic naming conventions are illustrated in this code example:

```c
// Component names ('Position') use CamelCase
typedef struct Position {
    float x;
    float y; // Component members ('y') use snake_case
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

// System names ('Move') use CamelCase. Supporting API types use snake_case_t
void Move(ecs_rows_t *rows) {
    // Declarative macro's use SCREAMING_SNAKE_CASE
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Velocity, v, 2);
    
    for (int i = 0; i < rows->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

int main(int argc, char *argv[]) {
    // Functions use snake_case
    ecs_world_t *world = ecs_init();
    
    // Declarative macro's use SCREAMING_SNAKE_CASE
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    // Enumeration constants ('EcsOnUpdate') use CamelCase
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
    
    // Entity names use CamelCase
    ECS_ENTITY(world, MyEntity, Position, Velocity);
    
    // Imperative macro's (function wrappers) use snake_case
    ecs_add(world, MyEntity, Position);
    
    return ecs_fini(world);
}
```

Flecs symbols and macro's use the `ecs_`, `ECS_` and `Ecs` prefixes.

#### Handles
The API creates and uses handles to refer to entities, systems and components. Most of the times these handles are transparently created and used by the API, and most code can be written without explicitly being aware of how they are managed. However, in some cases the API may need to access the handles directly, in which case it is useful to know their conventions.

The API has entity handles (of type `ecs_entity_t`) and type handles (of type `ecs_type_t`). Entity handles are used to refer to a single entity. Systems and components (amongst others) obtain identifiers from the same id pool as entities, thus handles to systems and components are also of type `ecs_entity_t`. Types are vectors of entity identifiers that are used to describe which components an entity has (components are identified by entity identifiers), amongst others.

Type handles are automatically created by API macro's like `ECS_COMPONENT`, `ECS_TYPE` and `ECS_PREFAB`. To obtain a handle to a type, use the `ecs_type` macro and provide as argument the identifier of the component or entity. Entity handles in most cases have the same identifier that is provided to the macro. For example:

```c
ECS_TYPE(world, MyType, Position);
```

This statement makes the entity handle available as `MyType`. To access the type handle directly, use `ecs_type(MyType)`. There is one exception to this rule, which is components. Entity handles of components are prefixed, so that the names do not collide with the component type name. To obtain the entity handle of a component, use the `ecs_entity` macro. For example:

```c
ECS_COMPONENT(world, Position);
```

This statement makes the entity handle available as `ecs_entity(Position)`, and the type handle as `ecs_type(Position)`, where `ecs_entity` and `ecs_type` again are the macro's that translate from the component type name to the respective entity and type handles. If one were to fully write out what the `ECS_COMPONENT` macro does, it would look like this (replace 'Type' with a C type):

```c
ecs_entity_t ecs_entity(Type) = ecs_new_component(world, "Type", sizeof(Type));
ecs_type_t ecs_type(Type) = ecs_type_from_entity(ecs_entity(Type));
```

The `ecs_type_from_entity` function is an API function that can obtain a type handle from any entity handle.

### Error handling
The API has been designed in a way where operations have no preconditions on the (ECS) state of the application. Instead, they only ensure that a post condition of an operation is fulfilled. In practice this means that an operation _cannot_ fail unless invalid input is provided (e.g. a `NULL` pointer as world parameter). Take for example this code example:

```c
ecs_add(world, e, Position);
ecs_add(world, e, Position);
```

The `ecs_add` function has no precondition on the entity not having the component. The only thing that matters is that _after_ the operation is invoked, the entity has the `Position` component, which for both invocations is the case, thus the API will not throw an error. Another example:

```c
ecs_delete(world, e);
ecs_delete(world, e);
```

The post condition of `ecs_delete` is that the provided entity is deleted. In both invocations this is the case, thus the second time the `ecs_delete` operation is invoked is not an error. Another, slightly more interesting example:

```c
ecs_delete(world, e);
ecs_add(world, e, Position);
```

This, perhaps surprisingly, also does not result in an error. The reason is that entities in Flecs are never really deleted, they are only _emptied_. A deleted entity in Flecs is equivalent to an empty entity. Thus the post condition of `ecs_delete` is actually that the entity is empty. Adding `Position` subsequently to `e` is no different than adding `Position` to an empty entity, which is also not an error.

This does not mean that the API cannot fail. It relies on mechanisms like memory allocation and thread creation amongst others which can fail. It is also possible that an application corrupts memory, or Flecs contains a bug, which can also result in errors. In any of these situations, Flecs is unable to fulfill the post condition of an operation **and will assert or abort**, resulting in the termination of the application. Let me repeat that:

**Flecs will terminate your application when it encounters an error**.

This is a very conscious decision: rather than relying on the application to check (or not check) the return code of an operation, and making a decision based on incomplete information, the API does the only sensible thing it can do, which is stop. Note that this will _never_ happen as a result of a regular operation, but is _always_ the result of Flecs being in a state from which it cannot (or does not know how to) recover. It should be noted that explicit checks (asserts) are disabled when Flecs is built in release mode.

As a result of this API design, application code can be written declaratively and without error handling. Furthermore, return values of functions can actually be used to return useful information, which allows for a clean API, and results in concise code.

### Memory Management
The Flecs API has been designed to be ultra-simple and safe when it comes to managing memory (pointers):

**The API _never_ assumes ownership of passed in pointers & _never_ requires freeing a returned pointer**. 

This approach makes it unlikely for Flecs applications to run into memory corruption issues as a result of API misuse. There is only one exception to this rule, which is the creation / deletion of the world:

```c
ecs_world_t *world = ecs_init();

ecs_fini(world);
```

This also means that the application is fully responsible for ensuring that if a component contains pointers, these pointers are kept valid, and are cleaned up. In some cases this is straightforward, if the memory outlives a component as is often the case with entity identifiers:

```c
ecs_set(world, e, EcsId, {"MyEntity"}); 
```

This sets the `EcsId` component on an entity which is used by Flecs to assign names to entities. The `"MyEntity"` string is a literal and will certainly outlive the lifespan of the component, as it is tied to the lifecycle of the process, therefore it is safe to assign it like this. It can subsequently be obtained with this function:

```c
const char *id = ecs_get_id(world, e);
```

This function returns the verbatim address that is stored in the `EcsId` component, and thus should not be freed.

If memory is tied to the lifecycle of a component, applications can use `OnAdd` and `OnRemove` components to initialize and free the memory when components are added/removed. This example shows how to create two systems for a dynamic buffer that automatically allocate/free the memory for the dynamic buffer when it is added to an entity:

```c
typedef ecs_vector_t *DynamicBuffer;

ecs_vector_params_t params = {.element_size = sizeof(int)};

void InitDynamicBuffer(ecs_rows_t *rows) {
    ECS_COLUMN(rows, DynamicBuffer, data, 1);
    
    for (int i = rows->begin; i < rows->end; i ++) {
        data[i] = ecs_vector_new(&params, 0);
    }
}

void DeinitDynamicBuffer(ecs_rows_t *rows) {
    ECS_COLUMN(rows, DynamicBuffer, data, 1);
    
    for (int i = rows->begin; i < rows->end; i ++) {
        ecs_vector_free(data[i]);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, DynamicBuffer);
    ECS_SYSTEM(world, InitDynamicBuffer, EcsOnAdd, DynamicBuffer);
    ECS_SYSTEM(world, DeinitDynamicBuffer, EcsOnRemove, DynamicBuffer);
    
    // This adds DynamicBuffer, and invokes the InitDynamicBuffer system
    ecs_entity_t e = ecs_new(world, DynamicBuffer);
    
    // This removes DynamicBuffer, and invokes the DeinitDynamicBuffer system
    ecs_delete(world, e);
    
    return ecs_fini(world);
}
```

## Good Practices
Flecs is an Entity Component System, and it is important to realize that ECS is probably quite different from how you are used to write code. Thus when you are just getting started with Flecs, you may run into some unforeseen problems, and you may wonder more than once how something is supposed to work. Additionally, Flecs also has its own set of rules and mechanisms that require getting used to. This section is not a comprehensive guide into writing code "the ECS way", but intends to provide a few helpful tips to guide you on the way.

### Minimize the usage of ecs_get, ecs_set
An ECS framework is only as efficient as the way it is used, and the most inefficient way of accomplishing something in an ECS framework is by extensively using `ecs_get` and `ecs_set`. This always requires the framework to do lookups in the set of components the entity has, which is quite slow. It also is an indication that code relies to much on individual entities, whereas in ECS it is more common practice to write code that operates on entity collections. The preferred way to do this in Flecs is with [_systems_](#systems), which can access components directly, without requiring a lookup.

### Never compare entity types with anything
The [type](#types) (of type `ecs_type_t`) of an entity is a handle that uniquely identifies the components an entity has. Even though the API provides a function to get the type of a specific entity (`ecs_get_type`) it is big code smell to compare this type for anything other than for debugging. In ECS applications the type of an entity may change any moment, and directly comparing the entity type is almost guaranteed to break at some point. Instead, use `ecs_has` if you want to check whether an entity has a component (or set of components, by providing a type to `ecs_has`).

### Write logic in systems
If you find yourself writing lots of code in the main loop of your application that is not executed in a system, it could be a sign of code smell. Logic in ECS runs best when it is part of a system. A system ensures that your code has a clear interface, which makes it easy to reuse the system in other applications. Flecs adds additional benefits to using systems like being able to automatically or manually (remotely!) enable/disable them, and schedule them to run on different threads.

### Organize code in modules
For small applications it is fine to create a few systems in your main source file, but for larger projects you will want to organize your systems and components in modules. Flecs has a module system that lets you easily import systems and components that are defined in other files in your project, or even other libraries. Ideally, the main function of your application only consists of importing modules and the creation of entities.

### Do not depend on systems in other modules
Flecs has been designed to allow applications define logic in a way that can be easily reused across a wide range of projects. This however only works if a small but important set of guiding principles is followed. As a general rule of thumb, a system should never depend on a _system_ that is not in the same module. Modules may import other modules, but should never directly refer systems from that module. The reason is, that individual systems are often small parts of a bigger whole that implements a certain _capability_. While it is fine to rely on the module implementing that capability, systems should not rely on _how_ that capability is implemented.

If you find that your application has this need and you cannot work around it, this could be an indication of modules that have the wrong granularity, or missing information in components shared between the modules. 

### Expose features, not systems in modules
Modules need to expose handles to the things they implement, to let the user interact with the contents of a module. A module should however be designed in such a way that it only exposes things that will not break compatibility between the application and the module in the future. When a module implements components, this is often not a problem, as component definitions are unlikely to change. When a module implements systems, this is less straightforward.

Modules often organize and define systems in a way so that multiple systems achieve a single feature. Only in the case of simple features, will a feature map to a single system. The way that systems are mapped to features is implementation specific, and subject to change as the module evolves. Therefore, explicitly exposing system handles in a module is generally not a good idea.

Instead, a module can expose [features](#features) which allow a module to group the systems that belong to a certain feature. This way a module can expose a single feature (for example, `CollisionDetection`) and group all systems that belong to that feature (for example, `AddCollider`, `TestColliders`, `CleanCollisions`) under that feature. Features are unlikely to change, and thus this ensures that future versions of a module won't break compatibility with the applications that uses it.

### Use types where possible
The sooner you can let Flecs know what components you will be setting on an entity, the better. Flecs can add/remove multiple components to/from your entity in a single `ecs_add` or `ecs_remove` call with types (see `ECS_TYPE`), and this is much more efficient than calling these operations for each individual component. It is even more efficient to specify a type with `ecs_new`, as Flecs can take advantage of the knowledge that the entity to which the component is going to be added is empty.

### Use declarative statements for static data
Declarative statements in Flecs can be recognized by their capitalized names. Declarative statements define the "fabric" of your application. They declare components, systems, prefabs, types and can even define entities. They improve the readability of applications as someone only needs to quickly glance over the code to get a good impression of which components, entities and systems an application has. An example of a declarative piece of code is:

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);
ECS_SYSTEM(world, Move, EcsOnFrame, Position, Velocity);
ECS_ENTITY(world, Player, Position, Velocity);
```

You may find that certain things cannot be expressed through declarative statements yet, like setting component values on individual entities (you need `ecs_set` for that). These use cases represent areas in the API that we want to improve. Eventually, we would like applications to be able to define applications fully declaratively (except for the system implementations, of course!). 

### Create entities in bulk
It is much more efficient to [create entities in bulk](#create-entities-in-bulk) (using the `ecs_new_w_count` function) than it is to create entities individually. When entities are created in bulk, memory for N entities is reserved in one operation, which is much faster than repeatedly calling `ecs_new`. What can provide an even bigger performance boost is that when entities are created in bulk with an initial set of components, the `EcsOnAdd` handler for initializing those components is called with an array that contains the new entities vs. for each entity individually. If your application heavily relies on `EcsOnAdd` systems to initialize data, bulk creation is the way to go!

### Limit usage of ecs_lookup
You can use `ecs_lookup` to find entities, components and systems that are named (that have the `EcsId` component). This operation is however not cheap, and you will want to limit the amount of times you call it in the main loop, and preferably avoid it alltogether. A better alternative to `ecs_lookup` is to specify entities in your system expression with the `NOTHING` modifier, like so:

```c
ECS_SYSTEM(world, MySystem, EcsOnUpdate, Position, .MyEntity);
```

This will lookup the entity in advance, instead of every time the system is invoked. Obtaining the entity from within the system can be done with the `ecs_column_entity` function.

### Use ecs_quit to signal that your application needs to exit
You can use `ecs_progress` to control the flow of your application, by running it in a while loop, and making the result of the function the condition of the while loop. This will keep your application running until you call `ecs_quit`. Using this pattern provides a common approach to signalling your application needs to exit across modules.

### Update components proportionally to delta_time
The Flecs API provides your systems with a `delta_time` variable in the `ecs_rows_t` type wich contains the time passed since the previous frame. This lets you update your entity values proportional to the time that has passed, and is a good idea when you want to decouple the speed at which your logic is running from the FPS of your appplication. You can let Flecs determine `delta_time` automatically, by specifying `0` to `ecs_progress`, or manually by providing a non-zero value to `ecs_progress`.

### Set a target FPS for applications
When you run `ecs_progress` in your main loop, you rarely want to run your application as fast as possible. It is good practice to set a target FPS (a good default is 60) so that your application does not consume all of your CPU bandwidth. When you set a target FPS with the `ecs_set_target_fps` function, the `ecs_progress` function will automatically insert sleeps to make sure your application runs at the specified FPS. It may run slower if not enough CPU bandwidth is available, but it will never run faster than that.

### Never store pointers to components
In ECS frameworks, adding, removing, creating or deleting entities may cause memory to move around. This is it is not safe to store pointers to component values. Functions like `ecs_get_ptr` return a pointer which is guaranteed to remain valid until one of the aforementioned operations happens.

## Entities
Entities are the most important API primitive in any ECS framework, and even more so in Flecs. Entities by themselves are nothing special, just a number that identifies a specific "thing" in your application. What makes entities useful, is that they can be composed out of multiple _components_, which are data types describing the various _aspects_ or capabilities of an entity.

In Flecs, entities take on an even more prominent role, as Flecs internally uses entities to store many of the framework primitives, like components and systems. How this works exactly is an advanced topic, and the only reason it is mentioned here is so that you do not get confused when you see a signature like this:

```c
ecs_set(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t component);
```

The "component" is, unexpectedly, of the `ecs_entity_t` type, and this looks weird until you realize that a component _handle_ is actually an entity with _builtin components_. That is all you need to know about this, unless you want to contribute to Flecs yourself (which is encouraged :-).

The next sections describe how applications can use Flecs to work with entities.

### Entity basics
Entities in Flecs do not have an explicit lifecycle. This may seem confusing when you come from an OOP background, or perhaps even if you worked with other ECS Frameworks. An entity in Flecs is just an integer, nothing more. You can pick any integer you want as your entity identifier, and start assigning components to it. You cannot "create" or "delete" an entity, in the same way you cannot create or delete the number 10. That means in code, you can run this line by itself and it works:

```c
ecs_add(world, 10, Position);
```

This design emphasises the data-oriented nature of an ECS framework. Whereas in object oriented programming code is quite literally built to work with _individial_ objects, in ECS code is built around _a collection_ of components. In Flecs entities are an orthogonally designed feature that is only there to associate components.

Having said that the API does provide `ecs_new` and `ecs_delete` operations, which on the surface seem to imply entity lifecycle. The next sections will describe these operations in more detail, and explain how it fits with the above.

#### Creating entities
In Flecs, the `ecs_new` operation returns an entity handle that is guaranteed to not have been returned before by `ecs_new`. Note how this decsription explicitly avoids terminology such as "create". In Flecs, the total entity addressing space (which is 64 bit) is "alive" at all times. The `ecs_new` operation is merely a utility to obtain unused entity handles in a convenient way. The simplest way to invoke `ecs_new` is like this:

```c
ecs_entity_t e = ecs_new(world, 0);
```

This returns a handle to an empty entity. It is possible to provide an initial component, or type to `ecs_new`.  This is in many ways equivalent to first calling `ecs_new`, followed by `ecs_add`, but has the added advantage that since the API knows the entity is empty, there is no need to lookup the existing set of components, which has better performance. To specify a component to `ecs_new`, do:

```c
ecs_entity_t e = ecs_new(world, Position);
```

As it is functionally equivalent to first calling `ecs_new` followed by `ecs_add`, `EcsOnAdd` systems will be invoked for the `Position` component as a result of this operation.

#### Creating entities in bulk
When creating a large number of new entities, it can be much faster do this them in bulk, especially when adding initial components. This can be achieved with the `ecs_new_w_count` function. This function is equivalent to `ecs_new` but it has an additional `count` parameter which indicates the number of entities to be returned. An application can use it like this:

```c
ecs_entity_t e = ecs_new_w_count(world, Position, 100);
```

This operation is functionally equivalent to calling `ecs_new` 100 times, but has the added benefit that all the resource allocations, updating of the internal administration and invoking of reactive systems can all happen in bulk. The function returns the first of the created entities, and it is guaranteed that the entity identifiers are consecutive, so that if the first entity identifier is 1, the last entity identifier is 101.

#### Deleting entities
The `ecs_delete` operation does not actually delete the entity identifier (as this is impossible) but it guarantees that after the operation, no more resources are being held by flecs that are associated to the entity. After invoking the `ecs_delete` operation, the entity will have no more components, and will not be stored in any internal data structures. An application can use it like this:

```c
ecs_delete(world, e);
```

Since the entity identifier itself is not invalidated after the `ecs_delete`, it is legal to continue using it, like this:

```c
ecs_delete(world, e); // empty entity
ecs_add(world, e, Position); // add Position to empty entity
```

### Hierarchies
Flecs allows applications to create entities that are organized in hierarchies, or more accurately, directed acyclic graphs (DAG). This feature can be used, for example, when transforming entity coordinates to world space. An application can create a parent entity with child entities that specify their position relative to their parent. When transforming coordinates to world space, the transformation matrix can then be cascadingly applied from parents to children, according to the hierarchy.

This is just one application of using entity hierarchies. The `flecs.components.http` and `flecs.systems.civetweb` modules both rely on the usage of parent-child relationships to express a web server (parent) with endpoints (children). Other APIs that feature hierarchical designs can benefit from this feature, like DDS (with `DomainParticipant` -> `Publisher` -> `DataWriter`) or UI development (`Window` -> `Group` -> `Button` -> `Label`). 

The container API in Flecs is fully composed out of existing (public) API functionality. The API operations related to containers are intended to codify a pattern that provides a consistent way to implement parent-child relationships across applications. This approach has as side effect that container/contained entities behave no different from other entities, aside from the fact that they _can_ be accessed in a different way if the application so desires.

The next sections describe how to work with containers.

#### Creating child entities
Flecs has an API to create a new entity which also specifies a parent entity. The API can be invoked like this:

```c
ecs_entity_t my_root = ecs_new(world, 0);
ecs_entity_t my_child = ecs_new_child(world, my_root, 0);
```

Any entity can be specifed as a parent entity (`my_root`, in this example).

#### Adopting entities
The API allows applications to adopt entities by containers after they have been created with the `ecs_adopt` operation. The `ecs_adopt` operation is almost equivalent to an `ecs_add`, with as only difference that it accepts an `ecs_entity_t` (instead of an `ecs_type_t`), and it adds the `EcsContainer` component to the parent if it didn't have it already. The operation can be used like this:

```c
ecs_entity_t my_root = ecs_new(world, 0);
ecs_entity_t e = ecs_new(world, 0);
ecs_adopt(world, e, my_root);
```

If the entity was already a child of the container, the operation has no side effects.

#### Orphaning entities
The API allows applications to orphan entities from containers after they have been created with the `ecs_orphan` operation. The `ecs_orphan` operation is almost equivalent to an `ecs_remove`, with as only difference that it accepts an `ecs_entity_t` (instead of an `ecs_type_t`). The operation can be used like this:

```c
ecs_orphan(world, e, my_root);
```

If the entity was not a child of the container, the operation has no side effects. This operation will not add the `EcsContainer` tag to `my_root`.

#### Hierarchies and types
Parent-child relationships are encoded in entity types using the `CHILDOF` entity flag. It is possible to create a type that describes what the parent will be of an entity created with that type. Consider the following example:

```c
ECS_ENTITY(world, MyParent, Position);
ECS_TYPE(world, MyType, CHILDOF | MyParent, Position);

// This entity will be a child of MyParent
ecs_entity_t MyChild = ecs_new(world, MyType);
```

Alternatively, the `CHILDOF` entity flag can also be specified directly into the ECS_ENTITY macro:

```c
ECS_ENTITY(world, MyParent, Position);
ECS_ENTITY(world, MyChild, CHILDOF | MyParent, Position);
```

Note that in order to be able to use entity flags, the parent entity must be named (it must have an `EcsId` component). When not considering the entity identifiers, the above examples are equivalent to:

```c
ecs_entity_t MyParent = ecs_new(world, Position);
ecs_entity_t MyChild = ecs_new_child(world, MyParent, Position);
```

### Inheritance
Inheritance is a mechanism in flecs that allows entities to "inherit", or share components from another entity. Inherited components will appear as if they are added to an entity, even though they are actually part of another entity. Inheritance relationships can be specified at creation time, or it can be added / removed at a later point in time. An entity may inherit from multiple other entities at the same time.

The entity that inherits components is called the "instance". The entity from which the components are inherited is called the "base" entity.

#### Creating instances
Flecs has an API to create a new entity which also specifies a base entity. The API can be invoked like this:

```c
ecs_entity_t my_base = ecs_new(world, Position);
ecs_entity_t my_instance = ecs_new_instance(world, my_base, Velocity);
```

In this example, `my_instance` will now share the `Position` component from the `my_base` entity. If an application were to retrieve the Position component from both `my_base` and `my_instance`, it would observe that they are the same. Consider the following code snippet:

```c
Position *p_base = ecs_get_ptr(world, my_base, Position);
Position *p_instance = ecs_get_ptr(world, my_instance, Position);
assert(p_base == p_instance); // condition will be true
```

From this follows that modifying the component value of the base will also modify components of all its instances. Any entity can be specifed as a base entity (`my_root`, in this example).

#### Adding inheritance relationships
Inheritance relationships can be added after entities have been created with the `ecs_inherit` method. Consider:

```c
ecs_entity_t my_base = ecs_new(world, Position);
ecs_entity_t my_instance = ecs_new(world, Velocity);

// Create inheritance relationship where my_instance inherits from my_base
ecs_inherit(world, my_instance, my_base);
```

If the inheritance relationship was already added to the entity, this operation will have no effects.

#### Removing inheritance relationships
Inheritance relationships can be removed after they have been added with the `ecs_disinherit` method. Consider:

```c
ecs_entity_t my_base = ecs_new(world, Position);
ecs_entity_t my_instance = ecs_new_instance(world, my_base, Velocity);

// Remove inheritance relationship
ecs_disinherit(world, my_instance, my_base);
```

If the inheritance relationship was not added to the entity, this operation will have no effects.

#### Inheritance and types
Inheritance relationships are encoded in entity types using the `INSTANCEOF` entity flag. It is possible to create a type that describes what the base entity will be of an entity created with that type. Consider the following example:

```c
ECS_ENTITY(world, MyBase, Position);
ECS_TYPE(world, MyType, INSTANCEOF | MyParent, Velocity);

// This entity will be an instance of MyBase
ecs_entity_t MyInstance = ecs_new(world, MyType);
```

Alternatively, the `INSTANCEOF` entity flag can also be specified directly into the ECS_ENTITY macro:

```c
ECS_ENTITY(world, MyBase, Position);
ECS_ENTITY(world, MyInstance, INSTANCEOF | MyBase, Velocity);
```

Note that in order to be able to use entity flags, the parent entity must be named (it must have an `EcsId` component). When not considering the entity identifiers, the above examples are equivalent to:

```c
ecs_entity_t MyBase = ecs_new(world, Position);
ecs_entity_t MyInstance = ecs_new_instance(world, MyBase, Velocity);
```

#### Multiple inheritance
An entity may inherit from multiple base entities. Consider the following example:

```c
ECS_ENTITY(world, MyBase1, Position);
ECS_ENTITY(world, MyBase2, Velocity);
ECS_ENTITY(world, MyInstance, INSTANCEOF | MyBase1, INSTANCEOF | MyBase2, Mass);
```

In this scenario, the `MyInstance` entity will inherit components from both `MyBase1` and `MyBase2`. If two base entities contain the same component, the component will be shared from the base entity with the highest id (typically the one that is created last).

#### Overriding components
An instance may override the components of a base entity. Consider the following example:

```c
ecs_entity_t MyBase = ecs_new(world, Position);
ecs_entity_t MyInstance = ecs_new_instance(world, MyBase, 0);

// Override Position from MyBase
ecs_add(world, MyInstance, Position);
```

When an instance overrides a component, it obtains a private copy of the component which it can modify without changing the value of the base component. An instance can remove an override by removing the component:

```c
// Revert to Position shared from MyBase
ecs_remove(world, MyInstance, Position);
```

When an instance overrides a component from a base, the value from the base component is copied to the instance component.

#### Overriding and initialization
Component overrides enable a useful pattern for initializing components with default values when an entity is created. Consider the following example:

```c
ecs_entity_t MyBase = ecs_new(world, Position);
    ecs_set(world, MyBase, Position, {10, 20});

// Override Position on creation, copies the value from MyBase into MyInstance
ecs_entity_t MyInstance = ecs_new_instance(world, MyBase, Position);
```

After this operation, `MyInstance` will have a private `Position` component that is initialized to `{10, 20}`. The utility of this pattern becomes more obvious when combined when used in combination with types in which the inheritance relationship is encoded:

```c
ECS_ENTITY(world, MyBase, Position, Velocity);
  ecs_set(world, MyBase, Position, {0, 0});
  ecs_set(world, MyBase, Velocity, {1, 1});
  
ECS_TYPE(world, MyType, INSTANCEOF | MyBase, Position, Velocity);

// Creates an instance of MyBase and overrides Position & Velocity
ecs_entity_t MyInstance = ecs_new(world, MyType);
```

Applications can leverage this pattern by creating "template" entities for which the sole purpose is to provide initial values for other entities. Often such template entities should not be matched with systems by default. To accomplish this, applications can add the `EcsDisabled` flag to entities:

```c
ECS_ENTITY(world, MyTemplate, EcsDisabled, Position, Velocity);
```

Alternatively, applications can choose to create the template entities as [prefabs](#prefabs). Prefab entities are ignored by default by systems, and offer additional features for creating template hierarchies.

#### Specialization
It is possible to create inheritance trees, where base entities themselves inherit from other base entities. This allows applications to create base entities that range from very generic to very specialized while allowing for code reuse. Consider the following example:

```c
ECS_ENTITY(world, MyRoot, Position);
ECS_ENTITY(world, MyBase, INSTANCEOF | MyRoot, Velocity);
ECS_ENTITY(world, MyInstance, INSTANCEOF | MyBase, Mass);
```

In this example, `MyInstance` will inherit `Position` and `Velocity` from its base entities, and will own `Mass`. The `MyBase` entity will own `Velocity` and shared `Position` from `MyRoot`.

It is possible to override components in inheritance trees to specialize components:

```c
ECS_ENTITY(world, Planet, Mass);
ECS_ENTITY(world, LightPlanet, INSTANCEOF | Planet, Mass);
    ecs_set(world, LigthPlanet, Mass, {5.972 ^ 24});
ECS_ENTITY(world, HeavyPlanet, INSTANCEOF | Planet, Mass);
    ecs_set(world, LigthPlanet, Mass, {1898 ^ 27});
```

#### Prefabs
Prefabs are entities that are solely meant to be used as templates for other entities. Prefabs behave and can be accessed just like ordinary entities, but they are ignored by systems by default. Furthermore, prefabs can be combined with hierarchies to create entity trees that can be reused by instantiating the top-level prefab.

The only thing that distinguishes a prefab from an ordinary entity is the `EcsPrefab` component. Entities with this component are ignored by default by systems. An application can create a prefab like this:

```c
ECS_ENTITY(world, MyPrefab, EcsPrefab, Position);
```

Or by simply using the `ECS_PREFAB` macro:

```c
ECS_PREFAB(world, MyPrefab, Position);
```

Prefabs can be instantiated just like normal base entities:

```c
ECS_ENTITY(world, MyEntity, INSTANCEOF | MyPrefab);
```

or:

```c
ecs_entity_t e = ecs_new_instance(world, MyPrefab);
```

#### Prefab nesting
Prefabs can be created as children of other prefabs. This lets applications create prefab hierarchies that can be instantiated by creating an entity with the top-level prefab. To create a prefab hierarchy, applications must explicitly set the value of the builtin `EcsPrefab` component:

```c
ECS_PREFAB(world, ParentPrefab, EcsPosition2D);
  ECS_PREFAB(world, ChildPrefab, EcsPosition2D);
     ecs_set(world, ChildPrefab, EcsPrefab, {.parent = ParentPrefab});
     
ecs_entity_t e = ecs_new_instance(world, ParentPrefab);
```

After running this example, entity `e` will contain a child entity called `ChildPrefab`. All components of `e` will be shared with `ParentPrefab`, and all components of `e`'s child will be shared with `ChildPrefab`. Just like with regular prefabs, component values can be overridden. To override the component of a child entity, an application can use the following method:

```c
ecs_entity_t child = ecs_lookup_child(world, e, "ChildPrefab"); // Resolve the child entity created by the prefab instantiation
ecs_set(world, child, Position, {10, 20}); // Override the component of the child
```

An application can also choose to instantiate a child prefab directly:

```c
ecs_entity_t e = ecs_new_instance(world, ChildPrefab);
```

Applications may want to compose a prefab out of various existing prefabs. This can be achieved by combining nested prefabs with prefab variants, as is shown in the following example:

```c
// Prefab that defines a wheel
ECS_PREFAB(world, Wheel, EcsPosition2D, EcsCircle2D);
  ECS_PREFAB(world, Tire, EcsPosition2D, EcsCircle2D, Pressure);
      ecs_set(world, Tire, EcsPrefab, {.parent = Wheel});

// Prefab that defines a car
ECS_PREFAB(world, Car, EcsPosition2D);
  ECS_PREFAB(world, FrontWheel, INSTANCEOF | Wheel);
     ecs_set(world, FrontWheel, EcsPrefab, {.parent = Car});
     ecs_set(world, FrontWheel, EcsPosition, {-100, 0});
  ECS_PREFAB(world, BackWheel, INSTANCEOF | Wheel);
     ecs_set(world, BackWheel, EcsPrefab, {.parent = Car});
     ecs_set(world, BackWheel, EcsPosition, {100, 0});     
```

In this example, the `FrontWheel` and `BackWheel` prefabs of the car inherit from `Wheel`. Children of the base (`Wheel`) are also automatically inherited by the instances (`FrontWheel`, `BackWheel`), thus for every entity that is created with `Car`, 4 additional child entities will be created:

```c
// Also creates FrontWheel, FrontWheel/Tire, BackWheel, BackWheel/Tire
ecs_entity_t e = ecs_new_instance(world, Car); 
```

## Components and Types
Components are important building blocks for applications in Flecs. Components do not contain any logic, and can be added or removed at any point in the application. A component can be registered like this:

```c
ECS_COMPONENT(world, Position);
```

Here, `Position` has to be an existing type in your application. You can register the same type twice with a typedef. If you have the following types:

```c
struct Vector {
    float x;
    float y;
};

typedef struct Vector Position;
typedef struct Vector Velocity;
```

they can be simply registered like this:

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);
```

With the API, you can even register the same type twice:

```c
ecs_new_component(world, "Position", sizeof(struct Vector));
ecs_new_component(world, "Velocity", sizeof(struct Vector));
```

A _type_ in Flecs is any set of 1..N components. If an entity has components `A` and `B`, then the _type_ of that entity is `A, B`. Types in Flecs are a first class citizen, in that they are just as prominently featured in the API as components, and in many cases they can be used interchangeably. An application can explicitly create a type, like this:

```c
ECS_TYPE(world, Movable, Position, Velocity);
```

This creates a type called `Movable` with the `Position` and `Velocity` components. Components have to be defined with either the `ECS_COMPONENT` macro or the `ecs_new_component` function before they can be used in an `ECS_TYPE` declaration.

Types handles be used interchangeably with components for most API operations. Using types instead of components can be useful for various reasons. A common application is to use a type to define an initial set of components for new entities. A type can be used together with `ecs_new` to accomplish this, like so:

```c
ecs_new(world, Movable);
```

The `ecs_new` operation actually accepts an argument of `ecs_type_t`, which means that even when a single component (like `Position`) is passed to the function, actually a _type_ is passed with only the `Position` component. 

This will create an entity with the `Position` and `Velocity` components, when using the previous definition of `Movable`. This approach is faster than individually adding the `Position` and `Velocity` components. Furthermore it allows applications to define reusable templates that can be managed in one location, as opposed to every location where entities are being created.

Types can also be nested. For example:

```c
ECS_TYPE(world, Movable, Position, Velocity);
ECS_TYPE(world, Unit, HealthPoints, Attack, Defense);
ECS_TYPE(world, Magic, Mana, ManaRecharge);
ECS_TYPE(world, Warrior, Movable, Unit);
ECS_TYPE(world, Wizard, Movable, Unit, Magic);
```

When added to entities, types themselves are not associated with the entity. Instead, only the resulting set of components is. For example, creating an entity with the `Wizard` type would be equivalent to creating an entity and individually adding `Position`, `Velocity`, `Attack`, `Defense`, `Mana` and `ManaRecharge`.

Types can be used with other operations as well, like `ecs_add`, `ecs_remove` and `ecs_has`. Whenever an application wants to add or remove multiple components, it is always faster to do this with a type as they approximate constant time performance (`O(1)`), whereas individually adding components is at least `O(n)`.

Types are not limited to grouping components. They can also group entities or systems. This is a key enabler for powerful features, like [prefabs](#prefabs), [containers](#containers) and [features](#features).

### Add components
In Flecs, an application can add owned components to an entity with the `ecs_add` operation. The operation accepts an entity and a _type_ handle, and can be used like this:

```c
ecs_add(world, e, Position);
```

After the operation, it is guaranteed that `e` will have the component. It is legal to call `ecs_add` multiple times. When a component is already added, this operation will have no side effects. When the component is added as a result of this operation, any `EcsOnAdd` systems subscribed for `Position` will be invoked.

It is also possible to use types with `ecs_add`. If an application defined a type `Movable` with the `ECS_TYPE` macro, it can be used with `ecs_add` like so:

```c
ecs_add(world, e, Movable);
```

After the operation, it is guaranteed that `e` will have all components that are part of the type. If the entity already had a subset of the components in the type, only the difference is added. If the entity already had all components in the type, this operation will have no side effects.

#### A quick recap on handles
The signature of `ecs_add` looks like this:

```c
void ecs_add(ecs_world_t *world, ecs_entity_t entity, ecs_type_t type);
```

Note that the function accepts a _type handle_ as its 3rd argument. Type handles are automatically defined by the API when an application uses the `ECS_COMPONENT`, `ECS_ENTITY`, `ECS_PREFAB` or `ECS_TYPE` macro's. When a component is defined with the `ECS_COMPONENT` macro a type handle is generated (or looked up, if it already existed) just with that one component. If the set of added components matches any `EcsOnAdd` systems, they will be invoked.

For more details on how handles work, see [Handles](#handles).

### Remove components
Flecs allows applications to remove owned components with the `ecs_remove` operation. The operation accepts an entity and a _type_ handle, and can be used like this:

```c
ecs_remove(world, e, Position);
```

After the operation, it is guaranteed that `e` will not have the component. It is legal to call `ecs_remove` multiple times. When the component was already removed, this operation will have no side effects. When the component is removed as a result of this operation, any `EcsOnRemove` systems that match with the `Position` component will be invoked.

It is also possible to use types with `ecs_remove`. If an application defined a type `Movable` with the `ECS_TYPE` macro, it can be used with `ecs_remove` like so:

```c
ecs_remove(world, e, Movable);
```

After the operation, it is guaranteed that `e` will not have any of the components that are part of the type. If the entity only had a subset of the types, that subset is removed. If the entity had none of the components in the type, this operation will have no side effects. If the set of components that was part of this operation matched any `EcsOnRemove` systems, they will be invoked.

### Set components
With the `ecs_set` operation, Flecs applications are able to set a component on an entity to a specific value. Other than the `ecs_add` and `ecs_remove` operations, `ecs_set` accepts a `_component` (entity) handle, as it is only possible to set a single component at the same time. The `ecs_set` operation can be used like this:

```c
ecs_set(world, e, Position, {10, 20});
```

After the operation it is guaranteed that `e` has `Position`, and that it is set to `{10, 20}`. If the entity did not yet have `Position`, it will be added by the operation. If the entity already had `Position`, it will only assign the value. If there are any `EcsOnSet` systems that match with the `Position` component, they will be invoked after the value is assigned.

### Tags
Tags are components that do not contain any data. Internally it is represented as a component with data-size 0. Tags can be useful for subdividing entities into categories, without adding any data. A tag can be defined with the ECS_TAG macro:

```c
ECS_TAG(world, MyTag);
```

Tags can be added/removed just like regular components with `ecs_new`, `ecs_add` and `ecs_remove`:

```c
ecs_entity_t e = ecs_new(world, MyTag);
ecs_remove(world, e, MyTag);
```

A system can subscribe for instances of that tag by adding the tag to the system signature:

```c
ECS_SYSTEM(world, Foo, EcsOnUpdate, Position, MyTag);
```

This introduces an additional column to the system which has no data associated with it, but systems can still access the tag handle with the `ECS_COLUMN_COMPONENT` marco:

```c
void Foo(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, 1);
    ECS_COLUMN_COMPONENT(rows, MyTag, 2);
    
    for (int i = 0; i < rows->count; i ++) {
        ecs_remove(rows->world, rows->entities[i], MyTag);
    }
}
```
### Builtin components
Flecs uses a set of builtin components to implement some of its features. Some of these components can be accessed by the application, while others have opaque data types. The builtin components are:

Name | Description | Access
-----|-------------|-------
EcsComponent | Stores the size of a component | Read
EcsTypeComponent | Stores information about a named type | Opaque 
EcsPrefab | Indicates that entity can be used as prefab, stores optional prefab parent | Read / Write
EcsPrefabParent | Internal data for prefab parents | Opaque 
EcsPrefabBuilder | Internal data for prefab parents | Opaque 
EcsRowSystem | Internal data for row systems | Opaque
EcsColSystem | Internal data for column systems | Opaque
EcsId | Stores the name of an entity | Read / Write
EcsHidden | Tag that indicates an entity should be hidden by UIs | Read / Write
EcsDisabled | Tag that indicates an entity should not be matched with systems | Read / Write

Builtin components can be get/set just like normal components. Writing a component that is read only can however cause undefined behavior.

## Systems
Systems let applications execute logic on a set of entities that matches a certain component expression. The matching process is continuous, when new entities (or rather, new _entity types_) are created, systems will be automatically matched with those. Systems can be ran by Flecs as part of the built-in frame loop, or by invoking them individually using the Flecs API.

### System signatures
A system signature specifies which components the system is interested in. By default, it will match with entities that have all of the specified components in its expression. An example of a valid system signature is:

```
Position, Velocity
```

The two elements are the components the system is interested in. Within a signature they are called "columns", and they can be thought of as elements in the `SELECT` clause of an SQL query. The order in which components are specified is important, as the system implementation will need to access component in this exact order. Care must be taken that when changing the order of columns, the implementation is updated to reflect this. More on this in "System API".

The system signature is the only mechanism for specifying the input for the system. Any information that the system needs to run must therefore be captured in the system signature. This strict enforcement of the interface can sometimes feel like a constraint, but it makes it possible to reuse systems across different applications. As you will see, system signatures have a number of features that make it easier to specify a range of possible input parameters.

#### Column operators
System signatures may contain operators to express optionality or exclusion of components. The most common one is the `,` (comma) which is equivalent to an AND operator. Only if an entity satisfies each of the expressions separated by the `,`, it will be matched with the system. In addition to the `,` operator, signatures may contain a number of other operators:

##### OR operator
The OR operator (`|`) allows the system to match with one component in a list of components. An example of a valid signature with an OR operator is:

```
Position, Velocity | Rotation
```

Inside of the system implementation, an application has the possibility to determine which component in the OR expression was the one that caused the system to match. An OR expression may contain any number of components.

##### NOT operator
The NOT operator (`!`) allows the system to exclude entities that have a certain component. An example of a valid signature with a NOT operator is:

```
Position, !Velocity
```

Inside the system implementation an application will be able to obtain metadata for the column (it will be able to see the component type for `Velocity`), but no actual data will be associated with it.

##### Optional operator
The optional operator (`?`) allows a system to optionally match with a component. An example of a valid signature with an optional operator is:

```
Position, ?Velocity
```

Inside the system implementation, an application will be able to check whether the component was available or not.

#### Column source modifiers
System signatures can request components from a variety of sources. The most common and default source is from an entity. When a system specifies `Position, Velocity` as its signature, it will match _entities_ that have `Position, Velocity`. A system may however require components from other entities than the one being iterated over. To streamline this use case, reduce `ecs_get` API calls within systems, and prevent excessive checking on whether components are available on external entities, the system signature can capture these requirements. A signature may contain the folllowing modifiers:

##### SELF source
This is the default source, and is implied when no source is explicitly provided. A system will match SELF components against the entities to be iterated over. An example of a signature with `SELF` as source is:

```
Position, Velocity
```

This system will match with any entities that have the `Position, Velocity` components. The components will be available to the system as owned (non-shared) columns, except when a component is provided by an inherited entity, in which case the component will be shared.

The `SELF` source gives no guarantees about whether the component is owned or shared. Therefore the component data passed into the system may be either an array or a pointer to a single value. If a system does not know in advance whether a for example `Velocity` is owned or shared, it can use the following code to safely access the component data:

```c
ECS_COLUMN(rows, Position, p, 1);
ECS_COLUMN(rows, Velocity, v, 2);

if (ecs_is_shared(rows, 2)) {
    for (int i = 0; i < rows->count; i ++) {
        p[i].x += v->x;
        p[i].y += v->x;
    }
} else {
    for (int i = 0; i < rows->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].x;
    }
}
```

Alternatively a system can use the `ecs_field` function, which abstracts away the difference between owned and shared columns:

```c
ECS_COLUMN(rows, Position, p, 1);

for (int i = 0; i < rows->count; i ++) {
    Velocity *v = ecs_field(rows, Velocity, 2, i);
    p[i].x += v->x;
    p[i].y += v->x;
}
```

##### OWNED source
OWNED is similar to SELF in that the component is matched with the entities to be iterated over, but will only match entities that own the component. Components from base entities (added either with `ecs_new_instance` or `ecs_inherit`) will not be matched.

```
Position, OWNED.Velocity
```

This system will match with any entities that have the `Position, Velocity` components. The `Position` component can be either owned or shared, where the `Velocity` component is guaranteed to be owned by the entity, and cannot come from a base entity.

An owned column is passed as an array into a system, and can always be safely accessed like this:

```c
ECS_COLUMN(rows, Position, p, 1);
ECS_COLUMN(rows, Velocity, v, 2);

for (int i = 0; i < rows->count; i ++) {
    p[i].x += v[i].x;
    p[i].y += v[i].x;
}
```

##### SHARED source
SHARED is similar to SELF in that the component is matched with the entities to be iterated over, but will only match entities that do not own the component. Only components from base entities (added either with `ecs_new_instance` or `ecs_inherit`) will be matched.

```
Position, SHARED.Velocity
```

This system will match with any entities that have the `Position, Velocity` components. The `Position` component can be either owned or shared, where the `Velocity` component is guaranteed to be shared, and will come from a base entity.

A shared column is passed in as a pointer into the system, and can always be safely accessed like this:

```c
ECS_COLUMN(rows, Position, p, 1);
ECS_COLUMN(rows, Velocity, v, 2);

for (int i = 0; i < rows->count; i ++) {
    p[i].x += v->x;
    p[i].y += v->y;
}
```

##### NOTHING source
The NOTHING source passes components to a system that are not matched with any entities. This is a convenient method for passing component/entity handles into systems. If for example a system wants to add the `Velocity` component to entities with `Position`, the `Velocity` handle can be passed into the system like this:

```
Position, .Velocity
```

A component passed in with a `NOTHING` source can be accessed like this:

```c 
ECS_COLUMN(rows, Position, p, 1);
ECS_COLUMN_COMPONENT(rows, Velocity, 2);

for (int i = 0; i < rows->count; i ++) {
    ecs_add(rows->world, rows->entities[i], Velocity);
}
```

Any entity identifier can be passed into a system with a `NOTHING` modifier. This can often be useful when combined with manual systems:

```
Position, .MyManualSystem
```

`MyManualSystem` can be accessed like this:

```c
ECS_COLUMN(rows, Position, p, 1);
ECS_COLUMN_ENTITY(rows, MyManualSystem, 2);

for (int i = 0; i < rows->count; i ++) {
    ecs_run(rows->world, MyManualSystem, rows->delta_time, &rows->entities[i]);
}
```

Empty columns have no data, and as such should not be accessed as owned or shared columns. Instead, the system should only attempt to obtain the handle to the component or component type.

##### CONTAINER source
The `CONTAINER` source allows a system to select a component from the entity that contains the currently iterated over entity. An example of the `CONTAINER` modifier is:

```
CONTAINER.Position, Position, Velocity
```

This will match all entities that have `Position, Velocity`, _and_ that have a container (parent) entity that has the `Position` component. This facilitates building systems that must traverse entities in a hierarchical manner.

`CONTAINER` columns can be accessed the same way as `SHARED` columns.

##### CASCADE source
The `CASCADE` source is similar to an _optional_ `CONTAINER` column, but in addition it ensures that entities are iterated over in the order of the container hierarchy. 

For a hierarchy like this:

```
   A
  / \
 B   C
    / \
   D   E 
```

the order in which entities will be visited by the system will be `A, B, C, D, E`. Note how the system also matches the root entities (`A`) that do not have a container (hence "optional"). An example of a `CASCADE` modifier is:

```
CASCADE.Position, Position, Velocity
```

The order will be determined by the container that has the specified component (`Position` in the example). Containers of the entity that do not have this component will be ignored. 

`CASCADE` columns can be accessed the same way as `SHARED` columns.

##### SYSTEM source
The `SYSTEM` modifier adds components or tags to a system and passes them into the system. This can be useful when a system needs additional context. An example:

```
Position, Velocity, SYSTEM.DbConnection
```

This will match all entities with `Position, Velocity`, and automatically add the `DbConnection` component to the system. Often this pattern is paired with an `EcsOnAdd` system for the `DbConnection` component, which would be immediately executed when the system is defined, and set up the database connection (or any functionality) accordingly.

Another common pattern is to add the `EcsHidden` tag to a system, which is used to hide a system from UIs (if the UI supports it) and is often added to private systems in a module:

```
Position, Velocity, SYSTEM.EcsHidden
```

Another common tag to add is the `EcsOnDemand` tag, which marks the system as an on-demand system, meaning it will only be enabled if its outputs are required by another system:

```
[out] Position, Velocity, SYSTEM.EcsOnDemand
```

`SYSTEM` columns can be accessed the same way as `SHARED` columns.

##### SINGLETON source
The `SINGLETON` source (`$`) enables passing in components from the singleton entity. It can be used like this:

```
Position, $.Velocity
```

The system will only be invoked if the singleton entity has the `Velocity` component.

`SINGLETON` columns can be accessed the same way as `SHARED` columns.

##### ENTITY source
The `ENTITY` source enables passing in components from arbitrary entiites. It can be used like this:

```
Position, MyEntity.Velocity
```

The system will only be invoked if the entity has the `Velocity` component.

`ENTITY` columns can be accessed the same way as `SHARED` columns.

### System API
Now that you now how to specify system signatures, it is time to find out how to use the columns specified in a signature in the system itself! First of all, lets take a look at the anatomy of a system. Suppose we define a system like this in our application `main`:

```
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
```

How would this system actually be implemented? First of all, the application needs to define the function itself. The name of the function should exactly match the name in the definition, and should have the following signature:

```c
void Move(ecs_rows_t *rows) {
    for (int i = 0; i < rows->count; i ++) {
    }
}
```

The `rows` parameter provides access to the entities that matched with the system, and a lot of other useful information. This example already has the typical `for` loop that defines many system implementations, and the application can get the number of entities to iterate over from the `rows->count` member.

#### The ECS_COLUMN macro
To access the data of matched entities, the system function needs to obtain pointers to the component arrays. In order to do this, the code needs to look at the system signature, which in this case is `Position, Velocity`. This signature has two columns, and the components can be accessed from the system by using the corresponding column index:

```c
void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, position, 1);
    ECS_COLUMN(rows, Velocity, velocity, 2);
    
    for (int i = 0; i < rows->count; i ++) {
    }
}
```

This macro requires the `rows` parameter, to get access to the matched entities, the type of the component (`Position`) a name for the variable which will point to the component array (`position`) and the column index (`1`). Note how the column index starts counting from `1`. This is because column index `0` is reserved for the column that stores the _entity identifiers_.

Now the system logic can be written, by using the `position` and `velocity` variables. Because they point to arrays of the component types (`Position`, `Velocity`) the application can simply use them as C arrays, like so:

```c
void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, position, 1);
    ECS_COLUMN(rows, Velocity, velocity, 2);
    
    for (int i = 0; i < rows->count; i ++) {
        position[i].x += velocity[i].x;
        position[i].y += velocity[i].y;
    }
}
```

When a column contains shared data, a system should not access the data as an array. Colummns that are guaranteed to be shared are columns with the `CONTAINER` modifier, `SYSTEM` modifier or `ENTITIY` modifier. In this case, a system should treat the column pointer as an ordinary pointer instead of an array:

```c
void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, position, 1);
    ECS_COLUMN(rows, Velocity, velocity, 2); // Assuming column 2 is shared
    
    for (int i = 0; i < rows->count; i ++) {
        // Don't access velocity as array
        position[i].x += velocity->x;
        position[i].y += velocity->y;
    }
}
```

In some cases a normal system signature can still cause shared components to be matched with systems, which can happen if entities inherit from base entities. If an application uses inheritance, a system needs to be made robust against this. Consider the following example:

```c
void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, position, 1);
    ECS_COLUMN(rows, Velocity, velocity, 2);
    
    bool velocity_is_shared = ecs_is_shared(rows, 2);
    
    for (int i = 0; i < rows->count; i ++) {
        if (velocity_is_shared) {
            position[i].x += velocity->x;
            position[i].y += velocity->y;        
        } else {
            position[i].x += velocity[i].x;
            position[i].y += velocity[i].y;
        }
    }
}
```

For systems with large numbers of components for which it is unknown whether a column is shared or not, it can be unwieldy to have to check all columns and create if statements that switch between access methods. In this case a system can choose to use the `ecs_field` function, which has a larger overhead than accessing the data directly, but abstracts away from the difference between an owned and a shared column. This is the same code with `ecs_field`:

```c
void Move(ecs_rows_t *rows) {    
    for (int i = 0; i < rows->count; i ++) {
        Position *p = ecs_field(rows, Position, 1, i);
        Velocity *v = ecs_field(rows, Velocity, 2, i);
        position->x += velocity->x;
        position->y += velocity->y;        
    }
}
```

#### The ECS_COLUMN_COMPONENT macro
When a system needs a component handle to one of its components, the `ECS_COLUMN_COMPONENT` will declare a handle to the component in the system. This is useful when a system needs to use the Flecs API, like the `ecs_set` function. When the `ecs_set` function is called, it expects the handle for a component to be there. In the application `main` function this happens automatically when a component is defined with `ECS_COMPONENT` or when it is imported with `ECS_IMPORT`, but a system needs to do this itself.

To import a component handle in a system, an application can use the  `ECS_COLUMN_COMPONENT` macro, like so:

```c
void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, position, 1);
    ECS_COLUMN(rows, Velocity, velocity, 2);
    ECS_COLUMN_COMPONENT(rows, Position, 1);
    
    for (int i = 0; i < rows->count; i ++) {
      ecs_set(rows->world, rows->entities[i], Position, {
          .x = position[i].x + velocity[i].x,
          .y = position[i].y + velocity[i].y
      });
    }
}
```

This code may look a bit weird as it introduces a few things that haven't been covered yet. First of all, note how the `world` object is passed into the system through the `rows` parameter. This lets a system call Flecs API functions, as all functions at least require a reference to an `ecs_world_t` instance. Secondly, note how the system obtains the entity id of the currently iterated over entity with `rows->entities`. Finally, note how the `ecs_set` function is able to use the `Position` component. The function requires a handle to the `Position` component to be defined, or it will result in a compiler error (try removing the `ECS_COLUMN_COMPONENT` macro).

This macro can also be used when a column uses the `NOTHING` modifier. For example, if a system has the following signature:

```
Position, .Velocity
```

Then the system can obtain the handle to the `Velocity` component with the following statement:

```c
ECS_COLUMN_COMPONENT(rows, Velocity, 2);
```

#### The ECS_COLUMN_ENTITY macro
Where the `ECS_COLUMN_COMPONENT` macro obtains a _type_ handle (of `ecs_type_t`), the `ECS_COLUMN_ENTITY` macro obtains an _entity_ handle (of type `ecs_entity_t`). This macro is useful when a system needs access to a specific entity, as is shown in this example:

```c
void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, position, 1);
    ECS_COLUMN(rows, Velocity, velocity, 2);
    ECS_COLUMN_COMPONENT(rows, Position, 1);
    ECS_COLUMN_ENTITY(rows, e, 3);
    
    for (int i = 0; i < rows->count; i ++) {
      if (ecs_has(rows->world, e, Position) {
          position[i].x += velocity[i].x;
          position[i].y += velocity[i].y;
      }
    }
}
```

Aside from this being a highly contrived example, it demonstrates the difference in how entity handles and type handles are used. When unsure about when to use an entity handle or a type handle, refer to the API documentation.

### System phases
Each frame in Flecs is computed in different phases, which define the execution order in which certain systems are executed. Phases are necessary to guarantee interoperability between systems in different modules, as it imposes a high-level order on the systems that need to be executed each frame. A simple example is a system that updates game state, and a system that renders a frame. The first system needs to be executed before the second one, to ensure the correct data is rendered. 

By using phases in the correct way, module developers can ensure that their systems work correctly across applications. There are no strictly enforced rules on how these phases must be used, and an application may decide to not use them at all. However, if an application or module defines logic that must be reused, phases are necessary to ensure systems run at the right moment.

The following sections describe the different phases in the order of execution, and what kind of systems are expected to run in them.

#### The EcsOnLoad phase
The `EcsOnLoad` phase is the first phase that is executed every frame. In this phase any data that needs to be loaded from external sources should be inserted into the ECS world (`ecs_world_t` instance). An example could be a system that streams data from disk based on the game state.

#### The EcsPostLoad phase
The `EcsPostLoad` phase "primes" the loaded data so that it is ready for usage. This often involves automatically adding components to entities that have a certain set of components. For example, a transformation module may automatically add a `TransformMatrix` component to each entity that has a `Position`, `Rotation` or `Scale`. 

#### The EcsPreUpdate phase
The `EcsPreUpdate` phase prepares the frame for the game logic. This phase typically contains systems that cleanup data from the previous frame, or initialize components for the next frame. An example could be a system that resets the transformation matrices to the identity matrix, or remove components that describe collisions detected in the previous frame.

#### The EcsOnUpdate phase
During the `EcsOnUpdate` phase the game logic is executed. In this phase entities are moved, AI is executed, input is processed and so on. Most of the game-specific systems are executed in this phase.

#### The EcsOnValidate phase
The `EcsOnValidate` phase checks if any constrains have been violated during the `EcsOnUpdate` phase. A typical example is an entity that moved out of bounds, or two entities that collided and are now partially overlapping. An example of a system that is executed during the `EcsOnValidate` phase is collision detection.

#### The EcsPostUpdate phase
The `EcsPostUpdate` phase evaluates constraints and corrects entities where required. For example, a system in the `EcsPostUpdate` phase may evaluate collisions detected during the `EcsOnValidate` phase, and correct entity positions so that they are no longer overlapping. Whereas the systems in the `EcsOnValidate` stage are typically from imported modules (like physics) the systems in the `EcsPostUpdate` phase are typically application specific (insert explosion at location of collision).

#### The EcsPreStore phase
The `EcsPreStore` phase allows imported modules a last chance to react to application logic before rendering a frame. For example, if an entity was moved during the `EcsPostUpdate` phase, its transformation matrix should be updated before it is rendered to reflect its latest position.

#### The EcsOnStore phase
During the `EcsOnStore` phase the data computed in the frame is rendered, or stored in an external data source. 

#### System phases example
This is an example of how a typical application that loads data dynamically, uses collision detection and rendering (identifiers between parenthesis are component expressions):

- **EcsOnLoad**
  - StreamFromDisk (`Game`, `Player`)

- **EcsPostLoad**
  - AddTransformMatrix (`Position | Rotation | Scale`, `!TransformMatrix`)
  - CleanCollisions (`Collision`)
  
- **EcsPreUpdate**
  - ResetTransformMatrix (`TransformMatrix`)
  
- **EcsOnUpdate**
  - Move (`Position`, `Velocity`)

- **EcsOnValidate**
  - Translate (`Position`, `TransformMatrix`)
  - Rotate (`Rotation`, `TransformMatrix`)
  - Scale (`Scale`, `TransformMatrix`)
  - TransformColliders (`Collider`, `TransformMatrix`)
  - TestForCollisions (`Collider`) -> `Collision`
  
- **EcsPostFrame**
  - OnCollision (`Collision`)
  
- **EcsPreStore**
  - CorrectTransform (`Collision`, `TransformMatrix`)
  
- **EcsOnStore**
  - Render

### Reactive systems
When a system is assigned to one of the various [system phases](#system-phases) systems are executed every frame (when `ecs_progress` is called), or periodically at a specified time interval. Alternatively, applications can define systems that are ran whenever a specific _event_ occurs. Events that can be intercepted are adding/removing components, and setting a value. The following sections describe these events.

#### EcsOnAdd
The `EcsOnAdd` event is triggered whenever a component is added to an entity. This can happen as a result of an `ecs_new` (when a component is specified), an `ecs_add` or an `ecs_set`. The `EcsOnAdd` event is typically used to build systems that initialize components, as flecs does not automatically initialize components itself.

Systems that respond to `EcsOnAdd` events are executed immediately after the component is added, before any of the aforementioned calls (`ecs_new`, `ecs_add`, `ecs_set`) return. This ensures that right after the component is added, it can be initialized and safe to use.

#### EcsOnRemove event
The `EcsOnRemove` event is triggered whenever a component is removed from an entity. This can happen as a result of an `ecs_remove` or an `ecs_delete` (which removes all components). The `EcsOnRemove` event is typically used to build systems that deinitialize components, which often involves releasing resources or freeing memory.

Systems that respond to `EcsOnRemove` events are normally executed immediately after the component is removed, before any of the aforementioned calls (`ecs_remove`, `ecs_delete`) return. The exception to this rule is when `ecs_remove` is invoked by a system, in which case the system is executed at the end of a phase, after all systems in that phase have been executed.

#### EcsOnSet event
The `EcsOnSet` event is triggered whenever a component is set to a value. This can happen as a result of an `ecs_set`. Systems responding to an `EcsOnSet` typically react to the data in a component, and require data to be initialized. During the lifecycle of a component, multiple `EcsOnSet` events may be generated, for every time the `ecs_set` function is called.

### Features
In many situations, a particular feature is realized by multiple systems. For example, a feature that does matrix transformations may have three systems, for automatically adding the `TransformMatrix` component, resetting it to the identity, and doing the actual transform. The exact details on how a feature is split up between systems highly depends on its implementation, and this makes it more complicated for applications to tell which systems are associated with a feature. An application may, for example, want to enable/disable certain features that it does not need.

To reduce this complexity, and to prevent applications from having to depend on specific implementation details, systems can be organized in features. Features combine multiple systems that fulfill a common goal under a single handle, which can be used to enable/disable all systems at the same time. A feature can be defined with the `ECS_TYPE` macro:

```c
ECS_TYPE(world, MyFeature, SystemA, SystemB);
```

An application can then enable/disable both `SystemA` and `SystemB` with a single call to `ecs_enable`:

```c
ecs_enable(world, MyFeature, false); // disables feature
```

Features can also be nested, like so:

```c
ECS_TYPE(world, ChildFeature1, Foo, Bar);
ECS_TYPE(world, ChildFeature2, Hello, World);
ECS_TYPE(world, ParentFeature, ChildFeature1, ChildFeature2);
```

When designing modules it is good practice to not directly expose handles to systems, but instead only expose feature handles. This decreases the chance that an application has to be modified because the implementation of a module changed.

### Staging
ECS frameworks typically store data in contiguous arrays where systems iterate over these arrays, and Flecs is no exception. A challenge for ECS frameworks is how to allow for mutations, such as adding/removing components and removing entities which alter the array. To address this, ECS frameworks often have limitations on which operations are allowed while iterating. A common solution to this problem is to implemement something called a _command buffer_, which stores a list of operations that are executed after an iteration.

Command buffers however have several disadvantages. First of all, mutations are not visible until the next iteration. When a system adds a component, and subsequently tests if the component has been added, the test would return false, which is not intuitive. Another disadvantage is that applications need a different API while iterating.

Flecs uses an alternative solution called "staging". Staging uses a data structure (called a "stage" or "staging area") which stores the _result_ of operations instead of the operations themselves. The data structure behaves like a "branch" of the main (not staged) data store, and can be queried by the flecs API in much the same way as the main data store can be queried. This allows the Flecs API to mutate data without limitations, while still having access to the mutated data while iterating. This capability is also a key enabling feature for [multithreading](#staging-and-multithreading).

The following code shows an example of a system that relies on staging:

```c
void System(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Velocity, 2);

    for(int i = 0; i < rows->count; i ++) {
        // Add component Velocity to stage, set its value to {1, 1}
        ecs_set(rows->world, rows->entities[i], Velocity, {1, 1});
        
        // Operation returns true
        ecs_has(rows->world, rows->entities[i], Velocity);
        
        // Operation returns pointer to the initialized Velocity
        ecs_get_ptr(rows->world, rows->entities[i], Velocity);
    }
}

int main(int argc, const char *argv[]) {
    ecs_world_t *world = ecs_init();
    
    ECS_SYSTEM(world, System, EcsOnUpdate, Position, !Velocity);
    
    // API calls done while in ecs_progress are staged
    ecs_progress(world, 1);
    // Stage is merged after executing systems
    
    return ecs_fini();
}
```

Note how you cannot tell from the API calls that staging is used. Flecs "knows" that the application is iterating over data, and when it is in this mode, all operations will automatically query and write to the stage.

#### Staged vs. inline modifications
When a system is iterating, it receives contiguous arrays with component data. These arrays are not staged, as they provide direct access to the components in the main data store. Applications can change the contents of these arrays, which is referred to as "inline modifications". An important decision system implementors have to make is whether to modify data inline, or whether to use staging. The following code example shows the difference between the two:

```c
void System(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);
    ECS_COLUMN(rows, Velocity, v, 2);

    for(int i = 0; i < rows->count; i ++) {
        // Inline modification
        v[i].x *= 0.9;
        v[i].y *= 0.9;
        
        // Staged modification
        ecs_set(rows->world,  rows->entities[i], Position, {p[i].x + v[i].x, p[i].y + v[i].y});
    }
}
```

A staged modification, while much slower than an inline modification, has as advantage that the component is not changed until data is merged, which means that subsequent systems can still have access to the previous value. If a subsequently executed system wants to read the value set in the stage, it has to use the `ecs_get` API.

A system _always_ receives the component arrays from the main data store. If a previously executed system added components to the stage, the only way to access these values is through the `ecs_get` API.

Using `ecs_get` and `ecs_set` has a performance penalty however as nothing beats the raw performance of inline reads/writes on a contiguous array. Expect application performance to take a significant hit when using these API calls versus using inline modifications.

#### Staging and ecs_get
When an application uses `ecs_get` while iterating, the operation may return data from to the main data store or to the stage, depending on whether the component has been added to the stage. Consider the following example:

```c
void System(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Velocity, 2);

    for(int i = 0; i < rows->count; i ++) {
        // Get data from main Flecs store
        Velocity *v = ecs_get_ptr(rows->world, rows->entities[i]);
        
        // Explicitly set component in the stage
        ecs_set(rows->world, rows->entities[i], Velocity, {1, 1});
        
        // ecs_get_ptr now returns component from the stage
        v = ecs_get_ptr(rows->world, rows->entities[i], Velocity);
        
        // Change value in the stage
        v[i].x ++;
    }
}
```

The `ecs_get_ptr` operation returns a pointer from the main data store _unless_ the component is staged.

#### Overwriting the stage
A single iteration may contain multiple operations on the same component. Consider the following example:

```c
void System(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Velocity, 2);

    for(int i = 0; i < rows->count; i ++) {
        // Set component Velocity in the stage
        ecs_set(rows->world, rows->entities[i], Velocity, {1, 1});
        
        // Overwrite component Velocity in stage
        ecs_set(rows->world, rows->entities[i], Velocity, {2, 2});
    }
}
```

The stage behaves the same as the main data store, in that it stores entities with their type, and tables in which all components for a type are stored. When a component is written twice in an iteration, it just means that its record in the staged table will be overwritten. This example is contrived, but it is not inconceivable that two subsequent systems modify the same component in a single iteration.

#### Staging and EcsOnAdd, EcsOnSet and EcsOnRemove
Systems that are executed after adding, setting and removing components (`EcsOnAdd`, `EcsOnSet`, `EcsOnRemove`) work also on staged components. When a component is added while iterating, the applicable `EcsOnAdd` systems will be called and the staged component will be exposed as a regular `ECS_COLUMN`. Note that this is different from regular (`EcsOnUpdate` etc) systems, where an `ECS_COLUMN` always returns an array from the main data store.

The `EcsOnAdd` and `EcsOnSet` systems are executed by the `ecs_add`/`ecs_set` operation. This means that data is always initialized by an `EcsOnAdd` system before `ecs_add` returns, and that any actions executed by an `EcsOnSet` system are executed before `ecs_set` returns.

`EcsOnRemove` systems however are not immediately ran by `ecs_remove`. Such systems are typically used as destructors which clean up/free any resources that a component owns. As such, `EcsOnRemove` systems can invalidate component values, making accessing a component value after executing an `EcsOnRemove` unsafe. For this reason, `EcsOnRemove` systems are executed during the merge, and not while iterating. This ensures that for the duration of the iteration, it is guaranteed that component values passed into systems are valid.

The folllowing code shows an example of an `EcsOnAdd` and `EcsOnRemove` system, and when they are executed:

```c
void AddVelocity(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Velocity, 1);

    for(int i = 0; i < rows->count; i ++) {
        // ...
    }
}

void RemoveVelocity(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Velocity, 1);

    for(int i = 0; i < rows->count; i ++) {
        // ...
    }
}

void System(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Velocity, 1);

    for(int i = 0; i < rows->count; i ++) {
        // AddVelocity will be invoked before ecs_add returns
        ecs_add(rows->world, rows->entities[i], Velocity);
        
        // RemoveVelocity will not be invoked until iteration finishes
        ecs_remove(rows->world, rows->entities[i], Velocity);
    }
}

int main(int argc, const char *argv[]) {
    ecs_world_t *world = ecs_init();
    
    ECS_SYSTEM(world, AddVelocity, EcsOnAdd, Velocity);
    ECS_SYSTEM(world, RemoveVelocity, EcsOnRemove, Velocity);
    ECS_SYSTEM(world, System, EcsOnUpdate, Velocity);

    ecs_progress(world, 1);
    
    return ecs_fini();
}
```

#### Staging and system phases
Systems in Flecs can be be assigned to different phases (see [System phases](#system-phases)). To ensure that data is available between phases, a merge is performed between each phase. This means for example that any components added or set during the `EcsPreUpdate` phase, will be merged with the main data store before the `EcsOnUpdate` phase starts. This by default limits the effects of staging to a single phase, and can improve reusability of systems as they do not need to be aware of how systems in another phase used staging.

While merging after each phase is the default behavior, applications can choose to [manually merge data](#manually-merging-stages).

#### Staging and threading
Staging is the key enabler for multithreading in Flecs, as it provides a mechanism for mutating state in a thread-safe way without requiring locking. When an application is ran on multiple threads (using the `ecs_set_threads` operation) each thread receives its own private stage which the thread can read and write without having to synchronize with other threads.

As a result, there are no limitations on adding/removing components, and creating/deleting entities from a thread. Invoking `ecs_add` or `ecs_set` from a thread is not significantly more expensive than calling it from a system executed on a single thread, as both use the same mechanism to prevent operations from mutating the Flecs data store while iterating. At the end of every phase, stages are merged with the main data store just like a normal iteration, with the only difference that Flecs needs to merge N stages as opposed to just a single one.

This capability does not absolve applications from the responsibility of making sure access to component data is thread safe. The component data that systems receive through `ECS_COLUMN` always comes from the main Flecs store, and is shared between threads. To understand when it is safe and when it is not safe to access (and modify) this data, it is important to understand how Flecs splits up the workload for a systems.

Flecs distributes load in jobs, where the number of jobs generated is equal to the number of threads, and where each job will (roughly) process `number of matched entities / number of threads` entities. This distribution is stable, meaning that in the absense of mutations, threads are guaranteed to process the same entities in subsequent iterations.

With this knowledge, applications can write systems that can safely modify component values inline without having to go through the stage, as every job will only read and write the entities that are allocated to it. Things get trickier when systems _in the same phase_ access _and write_ the same components, and have _different component signatures_. Lets unpack this statement. 

Each phase introduces a sychronization point for Flecs. After running systems in a phase, all threads are synchronized and data is merged. Inside the phase however, system execution is not synchronized, and different systems in that phase may (and will) run at any particular point in time. Imagine phase `EcsOnUpdate` defines systems `Foo` and `Bar` in that order, and that the systems are ran on two threads. System `Foo` writes- and system `Bar` reads data from component `A`. Suppose `Foo` subscribes for `A`, while `Bar` subscribes for `A, B`.

Now suppose thread 1 finishes the `Foo` job before thread 2. In that scenario, thread 1 starts processing the job for `Bar` before thread 2 has finished processing the job for `Foo`. Because system `Bar` has a different signature than `Foo`, it is matched to different entities. This means that an entity may be processed on thread 1 for `Foo`, but on thread 2 for `Bar`. As a result the `Bar` job of thread 1 may start reading data from `A` that thread 2 is still writing, which can result in race conditions.

A straightforward solution to this problem could be to put `Foo` and `Bar` in different phases. This guarantees that `Bar` can access data from `Foo` in a reliable way within the same iteration. If `Bar` however should only use data from the main store, this problem could be addressed by making `Foo` write to the stage instead. That way, changes made by `Foo` will not be visible until the next phase, and `Bar` can safely access the data from `A` inline.

To take the most advantage of staging and multithreading, it is important to understand how both mechanisms work. While Flecs makes it easier to write applications in a way that allows for fast processing of data by multiple threads, applications still need to take care to prevent race conditions. Simply adding more threads with `ecs_set_threads` in an application that is not written with multithreading in mind, will almost surely result in undefined behavior.

#### Manually merging stages
By default, Flecs merges data each iteration, for each phase. While a merge is relatively cheap (especially when there is not much to merge) merging may still incur overhead when there is lots of data to merge. For certain applications with high-throughput requirements, an application may want to take control over when Flecs performs a merge.

Note that this feature is still experimental, and that the API for this feature may change in the future.

An application can indicate that it wants to take control of merging by calling this operation:

```c
ecs_set_automerge(world, false);
```

This will cause Flecs to stop performing automatic merges. An application now has to manually perform a merge, which it can do with this operation:

```c
ecs_merge(world);
```

To write an application that only merges each frame (as opposed to each phase) an application could do:

```c
ecs_set_automerge(world, false);

while (ecs_progress(world, 1)) {
    ecs_merge(world);
}
```

It is even possible to merge every N frames, as the stage is retained across iterations. This also applies to applications that use multiple threads, as the stage for each thread is also maintained across iterations. An example:

```c
ecs_set_automerge(world, false);
ecs_set_threads(world, 4);

int count = 0;
while (ecs_progress(world, 1)) {
    // Merge every 10 frames
    if (!(ecs_get_tick(world) % 10)) {
        ecs_merge(world);
    }
}
```

There are no limitations on how the API can be used when performing merges manually, but there are some things to consider. First of all, staged data is not available as a contiguous buffer, which means that applications can only read/write it with `ecs_get` and `ecs_set`, which has a performance penalty. This is no different than when using automatic merging, but application logic has to be robust against large deltas between the main store and the stage.

Another consideration is that while data within a thread can be accessed through the regular API operations, data between threads is not shared until it is merged. Thread stages are _eventually consistent_, and the delta between the thread stages can be large. Application logic will have to be made explicitly robust against this, as this is not automatic. Turning off automatic merging on an existing codebase will likely break an application.

Lastly, Flecs modules rely on automatic merging (data between phases needs to be synchronized). Only if you use Flecs as a standalone library and do not use any modules, manual merging is possible.

#### Limitations of staging
The staging mechanism was adopted because it provides a fast, transparent way to manage mutations while iterating while being zero-cost when there is no data to merge. While there are no limitations on the operations you can do, there are some considerations that are common-sense once you understand the concept, but not necessarily trivial:

**You cannot call ecs_progress from a system**
When you are iterationg, you cannot start another iteration. If you need to recursively call systems, use the `ecs_run` API.

**You cannot merge from a system**
You cannot invoke `ecs_merge` when you are iterating. Merging may only be called outside of a call to `ecs_progress`.

**You cannot set the number of threads from a system**
Just don't do this.

**You cannot create new systems from a system**
Creating a new system updates administration that is used while invoking the systems.

**You cannot enable/disable systems from a system**
For the same reason you cannot create systems, you can also not enable/disable systems. It changes adminstration that is in use while iterating.

**You cannot preallocate memory from a system**
Preallocating memory directly modifies the component buffers of the main store, and thus is not possible while iterating.

**ecs_count may return unreliable numbers while iterating**
The `ecs_count` operation does not take into account staged data and only counts entities from the main store. 

**You cannot make changes from an EcsOnRemove system beyond the to be removed component**
`EcsOnRemove` systems are invoked during merging. As these systems also iterate over components, any mutations performed while in an `EcsOnRemove` system would require nested staging, which would reduce performance and increase complexity by a lot.

## Modules
Modules are a simple concept used to organize components, systems and entities into units that can be easily reused across applications, but also as a way to organize code in large applications. Modules can be located inside a project, inside a statically linked, dynamically linked, or runtime loaded library. See [good practices](#good-practices) for some tips around how to organize code in modules.

### Importing modules
The default way to import an existing module is by using the `ECS_IMPORT` macro. This will invoke the module loader that instantiates all contents of the module in the Flecs data store. After importing a module, components can be used, entities will be created and systems will be matched against any entities in the application. To import a module with `ECS_IMPORT`, do:

```c
ECS_IMPORT(world, MyModule, flags);
```

Here, `MyModule` is the module identifier. The `flags` parameter is an integer that is passed to the module loader, which allows an application to specify whether only specific parts of the module should be included. For example, a module may contain systems for moving entities in both 2D and 3D, and the application may only be interested in the 2D systems. Flecs defines a few flags out of the box, but modules are not required to use them. The application that imports the module should refer to the module documentation to find out which flags are supported. The flags defined by Flecs are:

- ECS_2D
- ECS_3D
- ECS_REFLECTION

The same module may be imported more than once without introducing duplicate definitions into the Flecs world.

#### Module content handles
To interact with module contents, the application needs handles to the content. Handles are needed to, for example, add or remove components (component handle is required) or enabling/disabling systems (system handle is required). When a module is imported, handles are automatically imported into the current application scope. In other words, if a module is imported with the `ECS_IMPORT` macro in the main function, the handles of the contents in the module will be available to code in the main function.

To enable access to module handles from outside of the function in which it was imported, Flecs registers a type with the module handles as a singleton component. To obtain a struct with the module handles, an application can use the `ecs_get_singleton` operation:

```c
ecs_entity_t ecs_type(MyModule) = ecs_lookup(world, "MyModule");
MyModule module_handles = ecs_get_singleton(world, MyModule);
```

Additionally, applications can get access to this struct with the `ecs_module()` macro after an `ECS_IMPORT`:

```c
ecs_module(MyModule); // returns struct with handles
```

The returned `MyModule` struct contains handles to the module components, but for the Flecs API to work correctly, they need to be imported in the current function scope. This will define a local variable for each handle inside the struct, which are used by the Flecs API. To do this, use the module specific `ImportHandles` macro:

```c
MyModuleImportHandles(module_handles);
```

A full code example that uses `ecs_module`:

```c
void do_stuff(ecs_world_t *world, MyModule *handles) {
    MyModuleImportHandles(*handles);
    return ecs_new(world, ComponentFromMyModule);
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, MyModule, 0);

    do_stuff(world, &ecs_module(MyModule));

    ecs_fini(world);
}
```

#### Dynamic imports
Modules can be dynamically imported from a shared library. This is only possible when an application does not require compile-time knowledge about the contents of a module, but can be a powerful mechanism for dynamically loading functionality into an application. To dynamically load a module, applications can use the `ecs_import_from_library` function:

```c
ecs_import_from_library(world, "flecs.systems.admin", "FlecsSystemsAdmin");
```

The `flecs.systems.admin` argument is the name of the library or package. The `FlecsSystemsAdmin` argument is the name of the module, as a library may contain more than one module. If the module name is `NULL`, Flecs will automatically attempt to derive the module name from the library name. Currently this operation is only supported when Flecs is built with the bake build system, as Flecs reuses package management functions from the bake runtime. Future versions of Flecs will support this functionality for other build systems as well.

### Creating modules
A few steps are required to create a module from scratch:

- Create a type which contains the _public_ handles
- Create a function that instantiates the module contents
- Create a macro that declares handle variables

These steps all have to conform to a specific naming convention of them to work correctly. Fortunately, For each of these steps, Flecs has convenience macro's that simplify this process.

Suppose a module called `MyTransformModule` defines a component called `Position`. First of all, we have to create a struct that exposes the handle to `Position`. To do this, create the following struct in a publicly available header of the module:

```c
typedef struct MyTransformModuleHandles {
    ECS_DECLARE_COMPONENT(Position);
} MyTransformModuleHandles;
```

Make sure that the struct name is the module name (`MyTransformModule`) plus `Handles`. In that same header, a function must be declared that will be responsible for importing the module contents into Flecs:

```c
void MyTransformModuleImport(ecs_world_t *world, int flags);
```

Again, make sure that the name of the function is the module name plus `Import`, and that it follows this exact function signature. Now we need to create a macro that declares handle variables. This macro is used by the `ECS_IMPORT` macro to make the `Position` handle available to the application function where this module is imported. Creating this macro is straightforward, and looks like this:

```c
#define MyTransformModuleImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, Position)
```

When this macro is invoked by `ECS_IMPORT`, it will receive an initialized instance to the `MyTransformModuleHandles` struct.

Lastly, the function that imports the contents into Flecs needs to be created. This function needs to accomplish a few things, outlined in this example:

```c
void MyTransformModuleImport(ecs_world_t *world, int flags)
{
    // Create the module in Flecs
    ECS_MODULE(world, MyTransformModule);

    // Create the component
    ECS_COMPONENT(world, Position);

    // Export the component
    ECS_EXPORT_COMPONENT(Position);
}
```

With that in place, an application can now use `ECS_IMPORT` to import the module:

```c
ECS_IMPORT(world, MyTransformModule, 0);
```

When a module defines other kinds of things besides components, a different set of macro's is used. The following code needs to be added to a header of a module that exports a system:

```c
// To declare a system handle, use ECS_DECLARE_ENTITY
typedef struct MyTransformModuleHandles {
    ECS_DECLARE_COMPONENT(Position);
    ECS_DECLARE_ENTITY(MySystem);
} MyTransformModuleHandles;

// This does not change
void MyTransformModuleImport(ecs_world_t *world, int flags);

// To import a system handle, use ECS_IMPORT_ENTITY 
#define MyTransformModuleImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, Position)\
    ECS_IMPORT_ENTITY(handles, MySystem)
```

The `MyTransformModuleImport` function then needs to be changed to this:

```c
void MyTransformModuleImport(ecs_world_t *world, int flags)
{
    // Create the module in Flecs
    ECS_MODULE(world, MyTransformModule);
    
    // Create the component
    ECS_COMPONENT(world, Position);

    // Create the component
    ECS_SYSTEM(world, MySystem, EcsOnUpdate, Position);

    // Export the component
    ECS_EXPORT_COMPONENT(Position);

    // Export the system
    ECS_EXPORT_ENTITY(MySystem);
}
```

Prefabs, types and tags can all be exported with the ECS_EXPORT_ENTITY macro.

### Internals
There is a lot of variation between different ECS frameworks. As with any data structure or library, understanding the characteristics of a particular implementation will let you write code that runs faster, occupies less memory and behaves more predictably. This section is interesting for someone who already knows how to write ECS code, but wants to know more about what Flecs does behind the scenes.

#### Archetypes
Flecs is a variation of a so called "archetype" based entity component system. The name "archetypes" is used to indicate that entities that have the same "type" are stored together, where a type is the set of components an entity has. Types are created dynamically, based on the components an application adds and removes from entities. A simple example: consider an application with 10 entities containing `[Position]`, and 10 entities containing `[Position, Velocity]`. `[Position]` and `[Position, Velocity]` are the two archetypes, and entities belonging to the same archetype are stored together. In practice, the actual storage will look something like this:

```c
struct Archetype1 {
    Position position[10];
};

struct Archetype2 {
    Position position[10];
    Velocity velocity[10];
};
```

This approach to storing data is often referred to as "structs of arrays", or SoA. There are several advantages and disadvantages to storing entities this way. Lets start with the biggest benefit: entity data can always be stored in tightly packed arrays, which allows for efficient loading of data from RAM into the CPU caches. As the index for an entity is the same for each component array, it is easy and cheap to iterate over multiple components at the same time.

Archetype-based implementations are unique in that they guarantee direct array access for any combination of entities and components. Other approaches to implementing an ECS have either arrays with gaps, or have arrays where components for one entity are stored on different indices. These approaches rely on additional data structures to find the set of components for a specific entity. Hybrid approaches exist such as EnTT groups, which rely on sorting entities with a certain archetype to offer direct array access.

There are two noteworthy downsides to using archetypes that cannot be avoided. The first one is that when components are added and removed to an entity, it has to be moved from one archetype to another archetype, which involves copying memory between arrays. The second downside is that in applications with lots of different combinations of components, the large number of archetypes creates many arrays for the same component, which fragments the data space. Excessive fragmentation is bad, as each time an application has to switch from one array to another, it is likely to incur a cache miss.

These downsides can be easily mitigated once understood. Applications should avoid frequently adding/removing components to entities with lots of data. In such cases, performance can be hugely improved by splitting components over multiple entities. Fragmentation is seldomly a problem except in extreme cases, where there is only a handful of entities per archetype, and there are hundreds, if not thousands of archetypes that share the same component(s).

When properly used, archetypes based implementations are easily one of the fastest ways to implement an Entity Component System. Their ability to self-organize the entity storage for speed makes it easy to write applications that perform well. This combination of speed and usability is why Flecs has adopted the archetypes-based approach.

#### System internals
In an archetypes-based implementation, systems are matched with a set of archetypes that matches a system interest expression. Flecs systems are no exception to this rule. When a system is created in Flecs, it is matched with the existing archetypes. The matching archetypes are stored in administration that belongs to the system, so that when the system is invoked, there is no need to perform the matching again. When new archetypes are created, they are matched with the existing systems, so that at any point in time, each system has the full set of matching archetypes independent of the order in which they are defined.

Systems in Flecs keep track of which archetypes are empty and which archetypes are not. Empty archetypes are marked as inactive by systems so that they are not evaluated in the main loop. This improves performance as there often can be empty archetypes that served as an intermediate, if multiple components were added or removed from an entity.

Similarly, Flecs keeps track of systems that have no matching entities. If a system does not match with any archetypes, or all of its archetypes are empty, the system is marked as inactive which will also cause Flecs to not evaluate it in the `ecs_progress` call. This is particularly useful, as Flecs modules can define many systems that are not active, and may never be active for the set of entities in an application.

#### Type internals
Types in Flecs are the data structure that describes a set of components, or more accurately, a set of _entities_. Types are a simple yet powerful mechanism in Flecs that enables many features, from plain ECS, to hierarchies, to shared components. Understanding how to use types can greatly improve efficiency of code, and will let applications do things that are ordinarily not possible in ECS frameworks.

An example of a type is `[Position, Velocity]`. They are internally stored as a vector (`ecs_vector_t`) where the element type is an entity id (`ecs_entity_t`). Components in Flecs are stored as entities with the `EcsComponent` component. This causes their handle to be of the `ecs_entity_t` type, and as a result, types are vectors of `ecs_entity_t`. The type of an entity dynamically changes when components are added and removed from an entity.

Because the elements in a type are of `ecs_entity_t`, it is possible to add regular entities, instead of just components, to a type. When a regular entity is added to a type, it will act as a component without data, which is equivalent to a tag. Entities in a type can however be annotated with a so called "type flag". A type flag indicates a special kind of role that the entity in a type has. Flecs uses this role to indicate if an entity is used as a parent or as a base.

There are currently two type flags in Flecs: `CHILDOF` and `INSTANCEOF`. The values of these flags are single bits, starting from the MSB and counting backwards. This makes the addressing space for type flags limited: with each new flag the entity addressing space is halved. However, since the `ecs_entity_t` type is a 64-bit integer, this will not quickly become an issue.

A type with a `CHILDOF` flag could look like `[Position, Velocity, CHILDOF | my_parent]`, where `my_parent` can be any regular entity. This indicates to Flecs that `my_parent` should be treated as a parent of entities of this type. This knowledge is used to optimize storage for hierarchies, and to implement certain features like `CONTAINER` and `CASCADE` columns in queries (see [Hierarchies](#hierarches)).

Similarly, a type with an `INSTANCEOF` flag looks like `[Position, Velocity, INSTANCEOF | my_base]`, where `my_base` can be any regular entity. This indicates to flecs that components of `my_base` should be shared with entities of this type (see [Inheritance](#inheritance)).

Types are like the DNA of entities. They are used extensively throughout application and Flecs code, and provide a cheap mechanism for learning everything there is to know about a specific entity.

#### Entity internals
Entities are stored in archetypes which lets them to be iterated with systems, but that does not allow them to be accessed randomly. For example, an application may want to get or set component values on one specific entity using the `ecs_get` or `ecs_set` APIs. To enable this functionality, Flecs has the entity index, which is a data structure that maps entity identifiers to where they are stored in an archetype.

In Flecs the entity index is implemented as a sparse set, where the entity identifier is the index in the sparse array. The dense array of the sparse set is used to test if an entity identifier is alive, and allows for iterating all entities. The data stored in the sparse set is a pointer to the archetype the entity is stored in, combined with an _row_ (array index) that points to where in the component arrays the entity is stored.

Flecs has a mechanism whereby it can monitor specific entities for changes. This is required for ensuring that the set of archetypes matched with systems that have `CONTAINER` columns remains correct and up to date. For example, a system with `CONTAINER.Position` column can unmatch a previously matched archetype when the `Position` component is removed from one of the parents of the entities matched with the system. It would be expensive to reevaluate matched archetypes after updating _any_ entity, so instead Flecs needs a mechanism to monitor specific entities for updates. Monitored entities are stored with a negative row in the entity index. The actual index of an entity can be found by multiplying the row with -1. This allows Flecs to monitor entities for changes efficiently without having to do additional lookups.

Systems will occasionally need access to the entity identifier. Because systems access the entities directly from the archetypes and not from the entity index, they need to obtain the entity identifier in another way. Flecs accomplishes this by storing the entity identifiers as an additional column columns in an archetype. Applications can access the entity identifiers using `row->entities`, or by requesting the column at index 0:

```c
ECS_COLUMN(rows, ecs_entity_t, entities, 0);
```

### Operating system abstraction API
Flecs relies on functionality that is not standardized across platforms, like threading and measuring high resolution time. While the essential features of Flecs work out of the box, some of its features require additional effort. To keep Flecs as portable as possible, it does not contain any platform-specific API calls. Instead, it requires the application to provide them.

Note that when Flecs is built with bake, it automatically uses the platform specific API from the bake runtime. Applications do not need to manually set these, unless they want to provide their own implementations.

The OS API is defined by a struct in `util/os_api.h`. The default pattern for providing custom abstraction functions is:

```c
// Set default calls, like malloc, free
ecs_os_set_api_defaults();

// Obtain a private writable copy of the os_api struct
ecs_os_api_t os_api = ecs_os_api;

// Override any calls
os_api.thread_new = my_thread_new;
os_api.thread_join = my_thread_join;

// Set the custom API callbacks
ecs_os_set_api(&os_api);
```

These APIs callbacks are set automatically when calling `ecs_os_set_api_defaults`:

- malloc
- free
- realloc
- calloc
- log
- log_error
- log_debug
- abort

When Flecs is built with bake, these additional API callbacks are set to the corresponding functions from the bake runtime:

- thread_new
- thread_join
- mutex_new
- mutex_free
- mutex_lock
- mutex_unlock
- cond_new
- cond_free
- cond_signal
- cond_broadcast
- cond_wait
- sleep
- get_time
