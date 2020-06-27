# Flecs Manual

## Introduction
Nobody likes to read manuals, and you should be able to get up and running with Flecs by using the quickstart, by looking at examples and by checking the documentation in the flecs header files. However, if you truly want to know how something works, or why it works that way, the manual is the right place to go. With that said, the manual is not exhaustive, and it complements the other sources of documentation.

## Design Goals

### 1. Performance
Flecs is designed from the ground up to provide blazing fast iteration speeds in systems that can be vectorized by default, while minimizing cache misses. In addition, Flecs has a unique graph-based storage engine that allows for extremely fast add, remove and bulk operations. These features, amongst others, ensure that applications can get the most out of the underlying hardware.

### 2. Portability
Flecs has been implemented in C99 and features an external interface that is C89 compatible to ensure it is portable to a wide range of platforms. The framework contains a flexible operating system abstraction API that enables an application to easily port the library to new platforms.

### 3. Reusability
ECS has the potential for being a platform for the development of reusable, loosely coupled, plug and play features like input, physics and rendering. Flecs modules enable such features to be packaged in a loosely coupled way so that applications can simply import them, while guaranteeing a correct execution order. In addition, Flecs has features like time management that ensure a consistent baseline across modules.

### 4. Usability
Flecs is designed first and foremost to be a framework that simplifies the development of games and simulations. Rather than just providing a vanilla ECS implementation, Flecs provides many features that are commonly found in game development frameworks such as hierarchies, prefabs and time management, all integrated seamlessly with the core ECS system.

### 5. Extensibility
Flecs is used with other frameworks and game engines, and as such not all of its features are useful in each application. For that reason Flecs has a modular design, so that applications can easily remove features from the core that they do not need. Additionally, since many features are built on top of the ECS core, applications can easily extend or reimplement them.

### 6. Have fun!
There are few things as satisfying as building games. If nothing else, Flecs has been built to enable creative visions both big and small. I'm having a lot of fun building Flecs, I hope you will have fun using it, and that your users will have fun playing your games :)

## Concepts
This section describes the high level concepts of Flecs.

### World
The world is the container in which a simulation lives. It stores all of the simulation data, metadata and systems required to run the simulation. An application can have one or more worlds.

### Entity
An entity is a unique, 64-bit identifier that represents a single thing or object in your game. Entities have no behavior, and strictly speaking do not even have a lifecycle. Rather, you should see an entity as a numeric "primary key" that is associated with data in the store. In Flecs, the entity type (`ecs_entity_t`) is just that, a 64-bit integer.

### Component
A component is a datatype that is registered with the world. They can be added to, and removed from an entity. An entity can contain at most a single instance of a component, and can have an infinite number of components (in theory). Flecs internally stores component metadata on an entity, which is why component handles are of type `ecs_entity_t`.

### Tag
A tag is similar to a component, but is not associated with a data type. Just like components, tags are stored as an entity, which is why tag handles are of type `ecs_entity_t`.

### Query
A query allows for high performance iteration of entities that match a certain signature. The signature at a minimum contains information about which components an entity needs to have in order to be matched. Signatures can however be much more complex, and can exclude components, optionally match with components, match with components from specific sources and more.

### System
A system is a function that iterates over entities that, just like queries, match a signature. Systems can either be standalone or part of a _pipeline_, in which case they are executed periodically as part of running the main loop. Flecs internally stores system metadata on an entity, which is why system handles are of type `ecs_entity_t`.

### Phase
A phase identifies a moment in a frame when a system should be executed. A system can only be associated with a single phase. A phase is implemented as a tag, which is why phase handles are of type `ecs_entity_t`. A system can be made part of a phase by adding the phase to the system entity.

### Pipeline
A pipeline contains all the phases that should be executed by the world when a frame is computed. Flecs has a builtin pipeline which contains builtin phase tags, but applications can define their own custom pipelines. Flecs stores pipelin metadata on an entity, which is why pipeline handles are of type `ecs_entity_t`.

### Monitor
A monitor is a special kind of system that is executed when an entity goes from not matching the signature to matching the signature. Monitors can be used to monitor whether an entity has a specific set of entities.

### Set systems
Set systems are a special kind of system that are executed when one or more components are set or unset. Set systems are a reliable way of getting notified when the value of a component changes.

### Trigger
A trigger is a function that is executed when a component is added or removed. Triggers are simpler than systems, and can only be registered for a single component. Flecs stores triggers internally as an entity which is why trigger handles are of type `ecs_entity_t`.

### Type
A type is a collection (vector) of entity identifiers (`ecs_entity_t`) that can describe anything from a set of components, systems and tags, to plain entities. Every entity is associated with exactly one type that describes which components, tags, or other it has.

### Type role
A type role is a flag that indicates which role an entity fulfills in a type. By default this flag is `0`, indicating that the entity should be interpreted as component or tag. Type roles are used amongst others to indicate hierarchies (the `CHILDOF` role) or instancing (the `INSTANCEOF` flag).

### Scope
A scope is a virtual container that contains all of the child entities for a specific parent entity. Scopes are not recursive. Scopes are identified by their parent, which is why scope handles are of type `ecs_entity_t`.

### Namespace
Namespace is a different word for scope, and the terms may be used interchangeably. Typically the term namespace is used in relationship to components, systems and modules, whereas the term scope is used in relation to regular entities.

### Module
A module groups components, systems and more into reusable units that can be imported by an application. Contents of a module are by default stored in the scope (or namespace) of the module. Modules can import other modules. A module is only loaded once, regardless of how many times an application imports it.

### Stage
A stage is a temporary storage where structural changes to entities (new, add, remove, delete) are stored while an application is iterating. Flecs systems access raw C arrays, and this ensures that these arrays are not modified as the application is iterating over them. Data in a stage is merged at least once per frame, or more when necessary.

## API design

### Naming conventions

```c
// Component names ('Position') use PascalCase
typedef struct Position {
    float x;
    float y; // Component members ('y') use snake_case
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

// System names ('Move') use PascalCase. API types use snake_case_t
void Move(ecs_iter_t *it) {
    // Functions use snake_case
    Position *p = ecs_column(it, Position, 1);
    Velocity *v = ecs_column(it, Velocity, 2);
    
    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();
    
    // Declarative function-style macro's use SCREAMING_SNAKE_CASE
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    // Module names are PascalCase
    ECS_IMPORT(world, MyModule, 0);

    // Enumeration constants ('EcsOnUpdate') use PascalCase
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    // Function wrapper macro's use snake_case
    ecs_entity_t e = ecs_new(world, 0);

    // Builtin entities use PascalCase
    ecs_add(world, EcsSingleton, Position);
    
    return ecs_fini(world);
}
```

### Idempotence
Many operations in the Flecs API are idempotent, meaning that invoking an operation once has the same effect as invoking an operation multiple times with the same parameters. For example:

```c
ecs_add(world, e, Position);
```

Has the same effect as:

```c
ecs_add(world, e, Position);
ecs_add(world, e, Position);
```

This simplifies application code as it can be written in a declarative style, where the only thing that matters is that after the operation has been invoked, the post condition of the operation is satisfied.

Some operations are idempotent but have side effects, like `ecs_set`:

```c
ecs_set(world, e, Position, {10, 20});
ecs_set(world, e, Position, {10, 20});
```

The effect of invoking this operation once is the same as invoking the operation multiple times, but both invocations can trigger an OnSet system which can introduce side effects.

All declarative macro's (`ECS_COMPONEN`, `ECS_SYSTEM`, ...) are idempotent:

```c
{
    ECS_COMPONENT(world, Position);
}
{
    ECS_COMPONENT(world, Position);
}
```

The second time the `ECS_COMPONENT` macro is evaluated, the first instance will be found and returned. Note that because these macro's may declare variables, they cannot be defined twice in the same C scope.

### Error handling
As a result of the idempotent design of many operations, the API has a very small error surface. There are essentially two conditions under which an operation is unable to fulfill its postcondition:

- The application provides invalid inputs to an operation
- The operating system is unable to fulfill a request, like a failure to allocate memory

When either of those conditions occur, the library will throw an assertion in debug mode (the source is not compiled with `NDEBUG`). Except for errors caused by the OS, errors are almost always caused by the invocation of a single operation, which makes applications easy to debug. 

This approach has several advantages. Application code does not need to check for errors. If an error occurs, the assertion will cause application execution to halt. As a result of this, application code is cleaner and more robust, as it is impossible to forget to handle an error condition. 

### Memory ownership
Most of the API is handle based, as many API constructs are implemented using entities. There are a few instances where an application will interface with memory managed by the framework, or when an application needs to provide memory it manages to the API. In these scenarios there are four rules:

- If an operation accepts a `const T*`, the application retains ownership of the memory
- If an operation accepts a `T*`, ownership is transferred from application to framework
- If an operation returns a `const T*`, the framework retains ownership of the memory
- If an operation returns a `T*`, ownership is transferred from framework to application

The `ecs_get_name` operation is an example where the framework retains ownership:

```c
const char *name = ecs_get_name(world, e);
```

The `ecs_get_fullpath` operation is an example where the ownership is transferred to the application:

```c
char *path = ecs_get_fullpath(world, e);
```

Memory for which ownership has been transferred to the application will need to be freed by the application. This should be done by the `ecs_os_free` operation:

```c
ecs_os_free(path);
```

### Entity names
An application can assign names to entities by setting the `EcsName` component. This component accepts a `const char*` for storing a name, which means that the string is application managed:

```c
// "MyEntity" is application managed
ecs_set(world, e, EcsName, {.value = "MyEntity"});
```

Sometimes this can be inconvenient as the lifecycle of a name needs to match that of the entity. For these scenario's, the `EcsName` component provides the `alloc_value` member:

```c
// "MyEntity" will be copied and framework managed
ecs_set(world, EcsName, {.alloc_value = "MyEntity"});
```

When the `EcsName` component is set, the framework will make a copy of the `alloc_value` member and free the memory when the component is removed. If an application has a heap-allocated string, it can be freed after `ecs_set`:

```c
char *str = strdup("MyEntity");
ecs_set(world, EcsName, {.alloc_value = str});
free(str); // Safe, framework made a copy of the string
```

## Good practices
Writing code for an Entity Component System is different from object oriented code, and while the concepts are not very complex, it can be counter intuitive if you have never used one. This section provides a couple of guidelines on how to write code for ECS and Flecs specifically to get you started.

### ECS guidelines
The following guidelines apply to ECS in general.

- When building a new feature, start with the components. Spend time thinking about how the components will be used: how often are they written/read, who owns them, how many instances etc.

- Design components so that they have a single purpose. It is much easier / cheaper to combine two components in a query than it is to split up components, which causes a lot of refactoring.

- Don't over-generalize. If your code has lots of branches it is possible that you're trying to do too much with a single component. Consider splitting up components. It's ok to have multiple components with the same fields, if this makes your business logic simpler.

- Minimize the number of branches (`if`, `while`, `for`) in a system. Branches are expensive and prevent vectorization of code.

- Think twice before adding collections to components. Collections are OK if the contents of the collection are meant to be interpreted as an atomic unit, but if the individual elements must be interpreted in a standalone way, consider creating separate entities instead.

- Avoid dependencies / direct interaction between systems. Components are the only things that should be used for inter-system communication.

- Don't store function pointers in components. If you need to provide different (mutually exclusive) implementations for a component, use systems & tags.

- Don't feel like you should store everything in ECS. ECS is great for iterating large sets of entities linearly, but things more specialized than that (like spatial data structures) are better implemented separately. You can always cross-reference entity handles from your specialized data structure.

### Performance guidelines
When starting with ECS, it is important to build intuition around how expensive different operations are in ECS. Here are a few general observations. Note that these may vary across ECS implementations:

- Iteration over large sets of similar entities is super fast

- Random access (`ecs_get`) is comparatively slow

- Entity creation / destruction is cheap when compared with OOP

- Adding/removing components is comparable to creation/deletion

- Creating systems and queries is slow, evaluating systems and queries is very fast

- Creating a filter is fast, evaluating a filter is slow when compared to a query

### Flecs guidelines
The following guidelines apply to Flecs specifically. Following these guidelines will ensure that your applications perform well, are well organized and that their systems can be reused in multiple projects.

- Write periodic logic in systems. Systems provide a unified mechanism for running logic in the main loop and make features like time management available to the application. Writing logic in systems makes it easy to organize features in composable units.

- Always use `delta_time` in simulation systems to progress component values. Aside from making sure the execution of logic is decoupled from the number of frames per second, an application may want to pause or slow down the simulation, which is only possible if all simulation systems consistently use `delta_time`.

- Use POD (plain old data) components wherever possible. 

- Use component lifecycle actions for managing memory owned by a component.

- Preallocate memory where possible with `ecs_dim` and `ecs_dim_type`, as this makes application performance more predictable.

- Decide what your pipeline looks like. A pipeline defines the phases your main loop will go through, and determines where your systems should run. You can use the Flecs builtin pipeline, which enables you to use the flecs module ecosystem, or you can define your own.

- Understand how different components are accessed in each phase of the pipeline. This will help you determine how to assign systems to different pipelines.

- Annotate system signatures with `[in]` and `[out]` where possible. This will not only make it easier for someone to reason about dataflow in an application, but will also allow Flecs to optimize system execution. The default is `[inout]`.

- Organize high level features into modules. Features are often implemented with multiple systems and components, and keeping them together in a module makes it easier to import features, while keeping code organized in large applications.

- Put components that are not specific to any feature in particular (for example: `Position`) in a their own module. Component-only modules provide the bedrock for an ecosystem where different modules can coexist while working on the same data.

- Build declarative, component-driven APIs where possible. Flecs allows applications to define behavior when a component value changes. Using components instead of normal functions enables external tools to introspect and configure your applications, especially when used in combination with modules like `flecs.meta` and `flecs.dash`.

- Use `ecs_progress` to run the main loop. This will make it possible to plug & play new features into application by importing modules from the Flecs ecosystem, and also enable applications to use automatic FPS control, time management and multithreading.

- Use component references (`ecs_get_ref`) when repeatedly accessing the same component. This is a faster alternative to `ecs_get`, with as only downside that a little bit of state has to be stored.

