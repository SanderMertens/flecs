# Flecs Manual

## Contents

- [Design goals](#design-goals)
  - [Portability](#portability)
  - [Reusability](#reusability)
  - [Clean interfaces](#clean-interfaces)
  - [Performance](#performance)
- [API design](#api-design)
  - [Naming conventions](#naming-conventions)
  - [Error handling](#error-handling)
  - [Memory management](#memory-management)
- [Good practices](#good-practices)
  - [Minimize usage of ecs_get, ecs_set](#minimize-the-usage-of-ecs_get-ecs_set)
  - [Never compare types with anything](#never-compare-entity-types-with-anything)
  - [Write logic in systems](#write-logic-in-systems)
  - [Organize code in modules](#organize-code-in-modules)
  - [Use types where possible](#use-types-where-possible)
  - [Create entities in bulk](#create-entities-in-bulk)
  - [Limit usage of ecs_lookup](#limit-usage-of-ecs_lookup)
  - [Use ecs_quit to signal that your application needs to exit](#use-ecs_quit-to-signal-that-your-application-needs-to-exit)
  - [Update components proportionally to delta_time](#update-components-proportionally-to-delta-time)
  - [Set a target FPS for applications](#set-a-target-fps-for-applications)
  - [Never store pointers to components](#never-store-pointers-to-components)
- [Systems](#systems)
  - [System queries](#system-queries)
   - [Column operators](#columm-operators)
    - [OR operator](#or-operator)
    - [NOT operator](#not-operator)
    - [Optional operator](#optional-operator)
   - [Column source modifiers](#column-source-modifiers)
    - [SELF modifier](#id-modifier)
    - [ID modifier](#id-modifier)
    - [CONTAINER modifier](#container-modifier)
    - [SYSTEM modifier](#system-modifier)
    - [SINGLETON modifier](#singleton-modifier)
    - [ENTITY modifier](#entity-modifier)

## Design Goals
Flecs is designed with the following goals in mind, in order of importance:

### Portability
Flecs is implemented in C99 which makes it easy to port to a variety of platforms and (legacy) compilers. To further improve portability, Flecs has no mandatory external dependencies. For certain optional features, like threading and running the web-based dashboard, Flecs relies on external libraries like pthreads (or equivalent), civetweb and bake.util, but Flecs can be easily used without them.

### Reusability
Flecs has been designed so that it is easy to package systems and components in a way that can be easily reused across applications. The module design allows applications to import modules with a single line of code, after which the imported components and systems can be immediately used. To facilitate this, Flecs has an architecture that imposes a well-defined order on systems, so that imported systems are always executed in the right order, regardless of in wich order they are imported.

To further improve reusability, Flecs allows for writing code that is agnostic to how data is stored by the framework. While applications may make decisions to optimize storage for a particular usage pattern, Flecs allows systems to be written in a way that works across different storage modes.

### Clean interfaces
Flecs aims to provide clear and simple interfaces, by staying close to the core principles of Entity Component Systems. Someone who has worked with Entity Component Systems before should find it easy to read and write code written with Flecs. Flecs promotes a declarative design, where applications only need to state their intent.

Many Entity Component System frameworks put restrictions on the operations that can be performed while iterating over data, which makes APIs harder to use. In Flecs, there are no restrictions on which operations can be used.

### Performance
Flecs has a design that is optimized for minimizing cache misses by loading only data in cache that is required by the application, while also storing data in arrays (AoS) to ensure that an application makes optimal usage of cache lines. In many cases, applications can access raw arrays directly, wich is as fast as iterating a native array in C and, if the code permits it, lets applications be compiled with Single Instruction, Multiple Data (SIMD) instructions.

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
    // API functions ('ecs_column') use snake_case
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v = ecs_column(rows, Velocity, 2);
    
    for (int i = 0; i < rows->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();
    
    // Declarative macro's use SCREAMING_SNAKE_CASE
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    // Enumeration constants ('EcsOnUpdate') use CamelCase
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
    
    // Entity names use CamelCase
    ECS_ENTITY(world, MyEntity, Position, Velocity);
    
    // Imperative macro's (function wrappers) use snake_case
    ecs_set(world, MyEntity, Position, {10, 20});
    
    return ecs_fini(world);
}
```

#### Handles
The Flecs API creates and uses handles (integers) to refer to entities, systems and components. Most of the times these handles are transparently created and used by the API, and most code can be written without explicitly being aware of how they are managed. However, in some cases the API may need to access the handles directly, in which case it is useful to know their conventions.

The Flecs API has entity handles (of type `ecs_entity_t`) and type handles (of type `ecs_type_t`). Entity handles are used to refer to a single entity. Systems and components (amongst others) obtain identifiers from the same id pool as entities, thus handles to systems and components are also of type `ecs_entity_t`. Types are identifiers that uniquely identify a set of entities (or systems, components). Types are commonly used to add/remove one or more components to/from an entity, or enable/disable one or more systems at once.

Type handles are automatically created by API macro's like `ECS_COMPONENT`, `ECS_TYPE` and `ECS_PREFAB`. To obtain a handle to a type, use the `ecs_to_type` function and provide as argument the identifier of the component or entity. Entity handles in most cases have the same identifier that is provided to the macro. For example:

```c
ECS_TYPE(world, MyType, Position);
```

This statement makes the entity handle available as `MyType`. To access the type handle directly, use `ecs_to_type(MyType)`. There is one exception to this rule, which is components. Entity handles of components are prefixed, so that the names do not collide with the component type name. To obtain the entity handle of a component, use the `ecs_to_entity` function. For example:

```c
ECS_COMPONENT(world, Position);
```

This statement makes the entity handle available through `ecs_entity_of`, and the type handle as `ecs_type_of`.

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
const char *id = ecs_id(world, e);
```

This function returns the verbatim address that is stored in the `EcsId` component, and thus should not be freed.

If memory is tied to the lifecycle of a component, applications can use `OnAdd` and `OnRemove` components to initialize and free the memory when components are added/removed. This example shows how to create two systems for a dynamic buffer that automatically allocate/free the memory for the dynamic buffer when it is added to an entity:

```c
typedef ecs_array_t *DynamicBuffer;

ecs_array_params_t params = {.element_size = sizeof(int)};

void InitDynamicBuffer(ecs_rows_t *rows) {
    DynamicBuffer *data = ecs_column(rows, DynamicBuffer, 1);
    for (int i = rows->begin; i < rows->end; i ++) {
        data[i] = ecs_array_new(&params, 0);
    }
}

void DeinitDynamicBuffer(ecs_rows_t *rows) {
    DynamicBuffer *data = ecs_column(rows, DynamicBuffer, 1);
    for (int i = rows->begin; i < rows->end; i ++) {
        ecs_array_free(data[i]);
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
An ECS framework is only as efficient as the way it is used, and the most inefficient way of accomplishing something in an ECS framework is by extensively using `ecs_get` and `ecs_set`. This always requires the framework to do lookups in the set of components the entity has, which is quite slow. It also is an indication that code relies to much on individual entities, whereas in ECS it is more common practice to write code that operates on entity collections. The preferred way to do this in Flecs is with _systems_, which can access components directly, without requiring a lookup.

### Never compare entity types with anything
The type (of type `ecs_type_t`) of an entity is a handle that uniquely identifies the components an entity has. Even though the API provides a function to get the type of a specific entity (`ecs_get_type`) it is big code smell to compare this type for anything other than for debugging. In ECS applications the type of an entity may change any moment, and directly comparing the entity type is almost guaranteed to break at some point. Instead, use `ecs_has` if you want to check whether an entity has a component (or set of components, by providing a type to `ecs_has`).

### Write logic in systems
If you find yourself writing lots of code in the main loop of your application that is not executed in a system, it could be a sign of code smell. Logic in ECS runs best when it is part of a system. A system ensures that your code has a clear interface, which makes it easy to reuse the system in other applications. Flecs adds additional benefits to using systems like being able to automatically or manually (remotely!) enable/disable them, and schedule them to run on different threads.

### Organize code in modules
For small applications it is fine to create a few systems in your main source file, but for larger projects you will want to organize your systems and components in modules. Flecs has a module system that lets you easily import systems and components that are defined in other files in your project, or even other libraries. Ideally, the main function of your application only consists of importing modules and the creation of entities.

### Use types where possible
The sooner you can let Flecs know what entities you will be setting on an entity, the better. Flecs can add/remove multiple components to/from your entity in a single `ecs_add` or `ecs_remove` call with types (see `ECS_TYPE`), and this is much more efficient than calling these operations for each individual component. It is even more efficient to specify a type with `ecs_new`, as Flecs can take advantage of the knowledge that the entity to which the component is going to be added is empty.

### Create entities in bulk
It is much more efficient to create entities in bulk (using the `ecs_new_w_count` function) than it is to create entities individually. When entities are created in bulk, memory for N entities is reserved in one operation, which is much faster than repeatedly calling `ecs_new`. What can provide an even bigger performance boost is that when entities are created in bulk with an initial set of components, the `EcsOnAdd` handler for initializing those components is called with an array that contains the new entities vs. for each entity individually. If your application heavily relies on `EcsOnAdd` systems to initialize data, bulk creation is the way to go!

### Limit usage of ecs_lookup
You can use `ecs_lookup` to find entities, components and systems that are named (that have the `EcsId` component). This operation is however not cheap, and you will want to limit the amount of times you call it in the main loop, and preferably avoid it alltogether. A better alternative to `ecs_lookup` is to specify entities in your system expression with the `ID` modifier, like so:

```c
ECS_SYSTEM(world, MySystem, EcsOnUpdate, Position, ID.MyEntity);
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

## Systems
Systems let applications execute logic on a set of entities that matches a certain component expression. The matching process is continuous, when new entities (or rather, new _entity types_) are created, systems will be automatically matched with those. Systems can be ran by Flecs as part of the built-in frame loop, or by invoking them individually using the Flecs API.

### System queries
A system query specifies which components the system is interested in. By default, it will match with entities that have all of the specified components in its expression. An example of a valid system query is:

```
Position, Velocity
```

The two elements are the components the system is interested in. Within a query they are called "columns", and they can be thought of as elements in the `SELECT` clause of an SQL query. The order in which components are specified is important, as the system implementation will need to access component in this exact order. Care must be taken that when changing the order of columns, the implementation is updated to reflect this. More on this in "System API".

The system query is the only mechanism for specifying the input for the system. Any information that the system needs to run must therefore be captured in the system query. This strict enforcement of the interface can sometimes feel like a constraint, but it makes it possible to reuse systems across different applications. As you will see, system queries have a number of features that make it easier to specify a range of possible input parameters.

#### Column operators
System queries may contain operators to express optionality or exclusion of components. The most common one is the `,` (comma) which is equivalent to an AND operator. Only if an entity satisfies each of the expressions separated by the `,`, it will be matched with the system. In addition to the `,` operator, queries may contain a number of other operators:

##### OR operator
The OR operator (`|`) allows the system to match with one component in a list of components. An example of a valid query with an OR operator is:

```
Position, Velocity | Rotation
```

Inside of the system implementation, an application has the possibility to determine which component in the OR expression was the one that caused the system to match. An OR expression may contain any number of components.

##### NOT operator
The NOT operator (`!`) allows the system to exclude entities that have a certain component. An example of a valid query with a NOT operator is:

```
Position, !Velocity
```

Inside the system implementation an application will be able to obtain metadata for the column (it will be able to see the component type for `Velocity`), but no actual data will be associated with it.

##### Optional operator
The optional operator (`?`) allows a system to optionally match with a component. An example of a valid query with an optional operator is:

```
Position, ?Velocity
```

Inside the system implementation, an application will be able to check whether the component was available or not.

#### Column source modifiers
System queries can request components from a variety of sources. The most common and default source is from an entity. When a system specifies `Position, Velocity` as its query, it will match _entities_ that have `Position, Velocity`. A system may however require components from other entities than the one being iterated over. To streamline this use case, reduce `ecs_get` API calls within systems, and prevent excessive checking on whether components are available on external entities, the system query can capture these requirements. A query may contain the folllowing modifiers:

##### SELF modifier
This is the default modifier, and is implied when no modifiers are explicitly specified. An example of the `SELF` modifier is:

```
Position, Velocity
```

This system will match with any entities that have the `Position, Velocity` components. The components will be available to the system as owned (non-shared) columns, _except_ when a component is provided by a Prefab, in which case the component will be shared.

##### ID modifier
The `ID` modifier lets an application pass handles to components or other systems to a system. This is frequently useful, as systems may need component handles to add or set components on entities that may not be part of the entity yet. Another use case for this feature is passing a handle to another `EcsManual` system to the system, which the system can then execute. This is frequently used when a system needs to evaluate a set of entities for every matching entity. An example of the `ID` modifier is:

```
Position, ID.Velocity
```

This will match any entity that has the `Position` component, and pass the handle to the `Velocity` component to the system. This allows the system implementation to add or set the `Velocity` component on the matching entities.

`ID` columns have no data, and as such should not be accessed as owned or shared columns. Instead, the system should only attempt to obtain the handle to the component or component type.

##### CONTAINER modifier
The `CONTAINER` modifier allows a system to select a component from the entity that contains the currently iterated over entity. An example of the `CONTAINER` modifier is:

```
CONTAINER.Position, Position, Velocity
```

This will match all entities that have `Position, Velocity`, _and_ that have a container (parent) entity that has the `Position` component. This facilitates building systems that must traverse entities in a hierarchical manner.

`CONTAINER` columns are available to the system as a shared component.

##### SINGLETON modifier
The `SINGLETON` or `$` modifier selects components from the singleton entity. As the name suggests, this allows a system to access a single, global (but world-specific) instance of a component. An example of the `SINGLETON` modifier is:

```
$Position, Position, Velocity
```

This will match all entities that have `Position, Velocity`, and make the `Position` component from the singleton entity available to the system.

`SINGLETON` columns are available to the system as a shared component.

##### SYSTEM modifier
In some cases it is useful to have stateful systems that either track progress in some way, or contain information pointing to an external source of data (like a database connection). The `SYSTEM` modifier allows for an easy way to access data associated with the system. An example of the `SYSTEM` modifier is:

```
SYSTEM.DbConnection, Position, Velocity
```

This will match all entities with `Position, Velocity`, and automatically add the `DbConnection` component to the system. Often this pattern is paired with an `EcsOnAdd` system for the `DbConnection` component, which would be immediately executed when the system is defined, and set up the database connection (or other functionality) accordingly.

`SYSTEM` columns are available to the system as a shared component.

##### ENTITY modifier
In some cases, it is useful to get a component from a specific entity. In this case, the source modifier can specify the name of a named entity (that has the `EcsId` component) to obtain a component from that entity. An example of the `ENTITY` modifier is:

```
Position, SomeEntity.Velocity
```

This will match all antities with the `Position` component, and pass the `Velocity` component of `SomeEntity` to the system. The equivalent of this functionality would be to pass handles to `SomeEntity` and `Velocity` with the `ID` component, and then do an `ecs_get` from within the system, like so:

```
Position, ID.SomeEntity, ID.Velocity
```

Using the `ENTITY` modifier is however much less verbose, and can potentially also be optimized as the framework may use a more efficient version of `ecs_get`.

`ENTITY` columns are available to the system as a shared component.
