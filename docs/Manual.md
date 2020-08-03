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
    ECS_IMPORT(world, MyModule);

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

### Macro's
The C99 API heavily relies on function-style macro's, probably more than you would see in other libraries. The number one reason for this is that an ECS framework needs to work with user-defined types, and C does not provide out of the box support for generics. A few strategies have been employed in the API to improve its overall ergonomics, type safety and readability. Let's start with a simple example:

```c
typedef struct Position {
    float x;
    float y;
} Position;

ECS_COMPONENT(world, Position);

ecs_entity_t e = ecs_new(world, Position);
```

From a readability perspective this code looks fine as we can easily tell what is happening here. Though if we take a closer look, we can see that a typename is used where we expect an expression, and that is not possible in plain C. So what is going on?

Let's first remove the `ECS_COMPONENT` macro and replace it with equivalent code (details are omitted for brevity):

```c
ecs_entity_t ecs_entity(Position) = ecs_new_component(world, "Position", sizeof(Position));
ecs_type_t ecs_type(Position) = ecs_type_from_entity(world, ecs_entity(Position));
```

The first line actually registers the component with Flecs, and captures its name and size. The result is stored in a variable with name `ecs_entity(Position)`. Here, `ecs_entity` is a macro that translates the typename of the component to a variable name. The actual name of the variable is:

```c
FLECS__EPosition
```

The second thing that happens is that a type variable is declared with the name `ecs_type(Position)`, which translates to `FLECS__TPosition`. A type is a vector of components. In this case, the type only contains the component id for `Position`. We will see in a moment why this is necessary.

The next statement creates a new entity with the `Position` component. The `ecs_new` function is a macro in disguise, and when it is replaced with the actual code, it looks like this:

```c
ecs_entity_t e = ecs_new_w_type(world, ecs_type(Position));
```

We can see that the actual plain C function that is called is `ecs_new_w_type`, and that the macro is passing in the type variable into the function. When creating a new entity, it can be initialized with multiple components which is why it accepts a type. Other operations, like `ecs_get` only accept a single component, and use the entity variable:

```c
Position *p = ecs_get(world, e, Position);
```

Translates into:

```c
Position *p = (Position*)ecs_get_w_entity(world, e, ecs_entity(Position));
```

As you can see, the `ecs_get` macro casts the result of the function to the correct type, so a compiler will throw a warning when an application tries to assign the result of the operation to a variable of the wrong type.

Similarly, `ecs_set` is a macro that ensures that anything we pass into it is of the right type:

```c
ecs_set(world, e, Position, {10, 20});
```

Translates into:

```c
ecs_set_ptr_w_entity
    (world, e, ecs_entity(Position), sizeof(Position), 
    &(Position){10, 20});
```

In addition to casting the value to the right type and passing in the component, this macro also captures the size of the type, which saves Flecs from having to do a component data lookup.

Understanding how the macro's work will go a long way in being able to write effective code in Flecs, and will lead to less surprises when debugging the code.

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

## Entities
Entities are uniquely identifiable objects in a game or simulation. In a real time strategy game, there may be entities for the different units, buildings, UI elements and particle effects, but also for exmaple the camera, world and player. An entity does not contain any state, and is not of a particular type. In a traditional OOP-based game, you may expect a tank in the game is of class "Tank". In ECS, an entity is simply a unique identifier, and any data and behavior associated with that entity is implemented with components and systems.

In Flecs, an entity is represented by a 64 bit integer, which is also how it is exposed on the API:

```c
typedef uint64_t ecs_entity_t;
```

Zero indicates an invalid entity. Applications can create new entities with the `ecs_new` operation:

```c
ecs_entity_t e = ecs_new(world, 0);
```

This operation guarantees to return an unused entity identifier. The first entity returned is not 1, as Flecs creates a number of builtin entities during the intialization of the world. The identifier of the first returned entity is stored in the `EcsFirstUserEntityId` constant.


### Named entities
In Flecs, entities can be named. A named entity is an entity that has the `EcsName` component. Setting this component is straightforward:

```c
ecs_entity_t e = ecs_new(world, 0);
ecs_set(world, e, EcsName, {"MyEntity"});
```

Alternatively, named entities can be declared with the `ECS_ENTITY` macro, which also allows for the entity to be initialized with a set of components:

```c
ECS_ENTITY(world, e, Position, Velocity);
```

Named entities can be looked up with `ecs_lookup`:

```c
ecs_entity_t e = ecs_lookup(world, "MyEntity");
```

An application can also request the name of a named entity:

```c
const char *name = ecs_get_name(world, e);
```

### Id recycling
Entity identifiers are reused when deleted. The `ecs_new` operation will first attempt to recycle a deleted identifier before producing a new one. If no identifier can be recycled, it will return the last issued identifier + 1. 

Entity identifiers can only be recycled if they have been deleted with `ecs_delete`. Repeatedly calling `ecs_delete` with `ecs_new` will cause unbounded memory growth, as the framework stores a list of entity identifiers that can be recycled. An application should never delete an identifier that has not been returned by `ecs_new`, and never delete the returned identifier more than once.

When using multiple threads, the `ecs_new` operation guarantees that the returned identifiers are unique, by using atomic increments instead of a simple increment operation. Ids will not be recycled when using multiple threads, since this would require locking global administration.

When creating entities in bulk, the returned entity identifiers are guaranteed to be contiguous. For example, the following operation:

```c
ecs_entity_t e = ecs_bulk_new(world, 0, 1000);
```

is guaranteed to return entity identifiers `e .. e + 1000`. As a consequence, this operation also does not recycle ids, as ids are unlikely to be deleted in contiguous order.

### Manual id generation
Applications do not have to rely on `ecs_new` and `ecs_delete` to create and delete entity identifiers. Entity ids may be used directly, like in this example:

```c
ecs_add(world, 42, Position);
```

This is particularly useful when the lifecycle of an entity is managed by another data source (like a multiplayer server) and prevents networking code from having to check whether the entity exists. This also allows applications to reuse existing identifiers, as long as these fit inside a 64 bit integer.

When not using manual ids, id recycling mechanisms are bypassed as these are only invoked by the `ecs_new` and `ecs_delete` operations. Combining manual ids with `ecs_new` and `ecs_delete` can result in unexpected behavior, as `ecs_new` may return an identifier that an application has already used.

### Id ranges
An application can instruct Flecs to issue ids from a specific offset and up to a certain limit with the `ecs_set_entity_range` operation. This example ensures that id generation starts from id 5000:

```c
ecs_set_entity_range(world, 5000, 0);
```

If the last issued id was higher than 5000, the operation will not cause the last id to be reset to 5000. An application can also specify the highest id that can be generated:

```c
ecs_set_entity_range(world, 5000, 10000);
```

If invoking `ecs_new` would result in an id higher than `10000`, the application would assert. If `0` is provided for the maximum id, no uppper bound will be enforced.

It is possible for an application to enforce that entity operations (`ecs_add`, `ecs_remove`, `ecs_delete`) are only allowed for the configured range with the `ecs_enable_range_check` operation:

```c
ecs_enable_range_check(world, true);
```

This can be useful for enforcing that an application is not modifying entities that are owned by another datasource.


## Types

### Basic usage
A type is typically used to describe the contents (components) of an entity. A simple example:

```c
// Create entity with type Position
ecs_entity_t e = ecs_new(world, Position);

// Add Velocity to the entity
ecs_add(world, e, Velocity);
```

After running this code, the type can be printed:

```c
// Print the type of the entity
ecs_type_t type = ecs_get_type(world, e);
char *str = ecs_type_str(world, type);
```

Which will produce:

```
Position, Velocity
```

Types can be used to add multiple components in one operation:

```c
ecs_entity_t e2 = ecs_new_w_type(world, type);
```

Alternatively, the `ECS_TYPE` macro can be used to create a type:

```c
ECS_TYPE(world, MyType, Position, Velocity);

ecs_entity_t e = ecs_new(world, MyType);
```

### Advanced usage
A type is stored as a vector of identifiers. Because components are stored as entities in Flecs, a type is defined as (pseudo, not actual definition):

```cpp
typedef vector<ecs_entity_t> ecs_type_t;
```

As a result, an application is able to do this:

```c
ecs_entity_t tag_1 = ecs_new(world, 0);
ecs_entity_t tag_2 = ecs_new(world, 0);

ecs_entity_t e = ecs_new(world, 0);
ecs_add_entity(world, e, tag_1);
ecs_add_entity(world, e, tag_2);
```

Printing the contents of the type of `e` now would produce something similar to:

```
256, 257
```

When the type contained components the names of the components were printed. This is because the component entities contained an `EcsName` component. The following  example does the same for `tag_1` and `tag_2`:

```c
ecs_set(world, tag_1, EcsName, {"tag_1"});
ecs_set(world, tag_2, EcsName, {"tag_2"});
```

Printing the type again will now produce:

```
tag_1, tag_2
```

### Type roles
The ability to add entities, and not just components to entities is the cornerstone of many features such as tags, hierarchies and instancing. To be able to define whether an entity in a type is a parent or not, type flags are used.

This example shows how to add an entity as a parent:

```c
ecs_entity_t parent = ecs_new(world, 0);
ecs_entity_t child = ecs_new_w_entity(world, ECS_CHILDOF | parent);
```

Here, `ECS_CHILDOF` is the type flag. This is an overview of the different type flags:

| Flag | Description |
|------|-------------|
| ECS_INSTANCEOF | The entity is a base |
| ECS_CHILDOF | The entity is a parent |

Entities with type flags can be dynamically added or removed:

```c
ecs_add_entity(world, child, ECS_CHILDOF | parent);
ecs_remove_entity(world, child, ECS_CHILDOF | parent);
```

Additionally, type flags can also be used inside of type and signature expressions, such as in the `ECS_TYPE` and `ECS_ENTITY` macro's:

```c
ECS_ENTITY(world, Base, Position);
ECS_ENTITY(world, Parent, Position, INSTANCEOF | Base);
ECS_TYPE(world, MyType, CHILDOF | Parent);
```

Note that when used inside a type expression, there is no need to provide the `ECS` prefix.

### Type constraints
Type constraints are special type flags that allow an application to put constraints on what entities a type can contain. Type constraints apply to type
entities, typically created with the `ECS_TYPE` macro. An example:

```c
// Sandwich toppings
ECS_TAG(world, Bacon);
ECS_TAG(world, Lettuce);
ECS_TAG(world, Tomato);

// A type that contains all sandwich toppings
ECS_TYPE(world, Toppings, Bacon, Lettuce, Tomato);

// Create a sandwich entity, enforce it has at least one topping
ECS_ENTITY(world, Sandwich, Bacon, Lettuce, OR | Toppings);
```

The `Sandwich` entity contains an `OR` type constraint that is applied to the `Toppings` type. This enforces that the entity must have _at least one_ of the entities in the `Toppings` type in its type. An overview of the constraints:

| Constraint | Description |
|------|-------------|
| ECS_AND | Entity must have all entities from provided type |
| ECS_OR | Entity must have at least one entity from provided type |
| ECS_NOT | Entity must have no entities from provided type | 
| ECS_XOR | Entity must have exactly one entity from provided type |

Type constraints can be added and removed like other type flags:

```c
ecs_add_entity(world, child, ECS_OR | Toppings);
ecs_remove_entity(world, child, ECS_OR | Toppings);
```

## Components
A component is a plain datatype that can be attached to an entity. An entity can contain any number of components, and each component can be added only once per entity. Components are registered with a world using the `ECS_COMPONENT` macro, after which they can be added and removed to and from entities. Components can be of any datatype. The following example shows how to register and use components:

```c
// Components can be defined from regular types
typedef struct Position {
    float x, y;
} Position;

int main() {
    ecs_world_t *world = ecs_init();

    // Register the component with the world
    ECS_COMPONENT(world, Position);

    // Create a new entity with the component
    ecs_entity_t e = ecs_new(world, Position);

    // Remove the component from the entity
    ecs_remove(world, e, Position);

    // Add the component again
    ecs_add(world, e, Position);
}
```

Component values can be set with the `ecs_set` operation. If the entity did not yet have the component, it will be added:

```c
ecs_set(world, e, Position, {10, 20});
```

Applications can get the value of a component with the `ecs_get` function:

The value of a component can be requested with `ecs_get`, which will return `NULL` if the entity does not have the component:

```c
const Position *p = ecs_get(world, e, Position);
```

The `ecs_get` operation returns a const pointer which should not be modified by the application. An application can obtain a mutable pointer with `ecs_get_mut`. The `ecs_get_mut` operation ensures that, even when using multiple threads, an application obtains a pointer to a component that can be safely modified, whereas the `ecs_get` operation might return a pointer to memory that is shared between threads. When an application modified a component obtained with `ecs_get_mut`, it should invoke `ecs_modified` to let the framework know the component value was changed. An example:

```c
Position *p = ecs_get_mut(world, e, Position);
p->x ++;
ecs_modified(world, p, Position);
```

### Component handles
In order to be able to add, remove and set components on an entity, the API needs access to the component handle. A component handle uniquely identifies a component and is passed to API functions. There are two types of handles that are accepted by API functions, a type handle and an entity handle. These handles are automatically defined as variables by the `ECS_COMPONENT` macro, however if an application wants to use the component in another scope, the handle will have to be passed to that scope.

Some operations can process multiple components in a single operation, like `ecs_add` and `ecs_remove`. Such operations require a handle of `ecs_type_t`. The `ECS_COMPONENT` macro defines a variable of `ecs_type_t` that contains only the id of the component. The variable defined by `ECS_COMPONENT` can be accessed with `ecs_type(ComponentName)`. This escapes the component name, which is necessary as it would otherwise conflict with the C type name. The following example shows how to pass a type handle to another function:

```c
typedef struct Position {
    float x, y;
} Position;

void new_w_position(ecs_world_t *t, ecs_type_t ecs_type(Position)) {
    // ecs_new uses an ecs_type_t
    ecs_new(world, Position);
}

int main() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    new_w_position(world, ecs_type(Position));

    ecs_fini(world);
}
```

The `ecs_new`, `ecs_add` and `ecs_remove` (not exhaustive) functions are wrapper macro's arround functions functions that accept a type. The following code is equivalent to the previous example:

```c
typedef struct Position {
    float x, y;
} Position;

void new_w_position(ecs_world_t *t, ecs_type_t p_handle) {
    // Use plain variable name with the ecs_new_w_type operation
    ecs_new_w_type(world, p_handle); 
}

int main() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    new_w_position(world, ecs_type(Position));

    ecs_fini(world);
}
```

There are also operations which operate on a single component at a time, like `ecs_get` and `ecs_set`. These operations require a component handle of type `ecs_entity_t`. The `ECS_COMPONENT` macro defines a variable of type `ecs_entity_t`that contains the id of the component. The variable defined by `ECS_COMPONENT` can be accessed by the application with `ecs_entity(ComponentName)`. The following example shows how to pass an entity handle to another function:

```c
typedef struct Position {
    float x, y;
} Position;

void set_position(ecs_world_t *t, ecs_entity_t ecs_entity(Position)) {
    ecs_entity_t e = ecs_new(world, 0);
    ecs_set(world, e, Position, {10, 20});
}

int main() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    set_position(world, ecs_entity(Position));

    ecs_fini(world);
}
```

The `ecs_set`, `ecs_get` (not exhaustive) functions are wrapper macro's arround functions functions that accept a type. The following code shows how to use the underlying function for `ecs_get`, `ecs_get_w_entity`:

```c
typedef struct Position {
    float x, y;
} Position;

const Position* get_position(ecs_world_t *t, ecs_entity_t e, ecs_entity_t p_handle) {
    return ecs_get_w_entity(world, e, p_handle);
}

int main() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    Position *p = get_position(world, e, ecs_entity(Position));

    ecs_fini(world);
}
```

## Tagging
Tags are much like components, but they are not associated with a data type. Tags are typically used to add a flag to an entity, for example to indicate that an entity is an Enemy:

```c
int main() {
    ecs_world_t *world = ecs_init();

    // Register the tag with the world. There is no Enemy type
    ECS_TAG(world, Enemy);

    // Add the Enemy tag
    ecs_add(world, e, Enemy);

    // Remove the Enemy tag
    ecs_remove(world, e, Enemy);    
}
```

### Tag handles
Just like components, the API needs a handle to a tag before it can use it, and just like `ECS_COMPONENT`, the `ECS_TAG` macro defines two variables, one of type `ecs_type_t` and one of `ecs_entity_t`. Passing a handle of an `ecs_type_t` into a function looks similar to a component:

```c
void new_w_tag(ecs_world_t *t, ecs_type_t ecs_type(Tag)) {
    // ecs_new uses an ecs_type_t
    ecs_new(world, Tag);
}

int main() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    new_w_tag(world, ecs_type(Tag));

    ecs_fini(world);
}
```

For functions that require an `ecs_entity_t` handle, the tag variable names are not escaped, since they do not clash with a C type name. An example:

```c
void add_tag(ecs_world_t *t, ecs_entity_t e, ecs_entity_t Tag) {
    ecs_add_entity(world, e, Tag);
}

int main() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world, 0);
    add_tag(world, e, Tag);

    ecs_fini(world);
}
```

Anyone who paid careful attention to this example will notice that the `ecs_add_entity` operation accepts two regular entities. 

## Queries
Queries allow an application to iterate entities that match a component expression, called a signature (see "Signatures"). Queries are stateful, in that they are registered with the world, and keep track of a list of entities (archetypes) that they match with. Whenever a new combination of entities is introduced (usually through an `ecs_add` or `ecs_remove` operation) it will be matched with the system, and if it matches, stored in a list with matched tables. This continuous matching process means that when an application starts iterating the query, it does not need to evaluate the query signature, which makes queries the most performant way to iterate entities.

A query can be used like this:

```c
// Create a query for all entities with Position, Velocity
ecs_query_t *query = ecs_query_new(world, "Position, Velocity");

// Create iterator for query
ecs_iter_t it = ecs_query_iter(query);

// Iterate all the matching archetypes
while (ecs_query_next(&it)) {
    // Get the component arrays
    Position *p = ecs_column(&it, Position, 1);
    Velocity *v = ecs_column(&it, Velocity, 2);

    // Iterate the entities in the archetype
    for (int i = 0; i < it.count, i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}
```

When an application is iterating the query, it can obtain pointers to the component arrays using the `ecs_column` function, which accepts the iterator, component name and the index of the component in the signature, which is offset by one. In the above example, `1` points to `Position`, which is the first component in the signature. and `2` points to `Velocity` which is the second.

Each time the `ecs_query_next` function returns true, the iterator contains entities that all have the same set of components, or belong to the same archetype, or table. The `count` member of the iterator contains the number of entities in the current table. An application can access the entity identifiers being iterated over with the `entities` member of the iterator:

```c
while (ecs_query_next(&it)) {
    for (int i = 0; i < it.count, i ++) {
        printf("Entity %s\n", ecs_get_name(world, it.entities[i]));
    }
}
```

### Change tracking
An application is able to see whether the entities and components matched with a query have changed since the last iteration with the `ecs_query_changed` function. When this function is invoked for the first time for a query it will always return true. The function should be invoked before obtaining an iterator to the query, as obtaining an iterator resets the state required for change tracking. An example:

```c
if (ecs_query_changed(q)) {
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        // ...
    }
}
```

## Signatures
The query signature accepts a wide range of operators and options that allow an application to determine with fine granularity which entities to iterate over. The most common kind of signature is one that subscribes for entities with a set of components. An example of such a signature looks like a comma delimited list of component or tag identifiers:

```
Position, Velocity
```

Component identifiers can be namespaced, as is often the case with components from a module. If so, a signature will have to include the full path to the component, separated by dots:

```
my.namespace.Position, my.namespace.Velocity
```

Signatures may contain type roles. Type roles have the same identifier as the API constants, without the `ECS` prefix.

```
INSTANCEOF | base, my.namespace.Position
```

The subset of signatures described so far are called "type expressions", and can be used not only for queries, but also for defining types, usually in combination with the `ECS_TYPE` macro:

```c
ECS_TYPE(world, MyType, INSTANCEOF | base, my.namespace.Position);
```

The features described from here are not allowed in type expressions, and may only occur for queries and systems.

### Source modifiers
By default, components in a query are matched against the entities in a table. Source modifiers allow a query to request data from other entities, typically ones that have a special relationship with the matched entities. A source modifier can be specified with the colon (`:`) character. The following source modifiers can be provided in a signature:

Modifier | Description
---------|------------
OWNED    | Match only owned components (default)
SHARED   | Match only shared components
ANY      | Match owned or shared components
PARENT   | Match component from parent
CASCADE  | Match component from parent, iterate depth-first
Nothing  | Do not get the component from an entity, just pass in handle

This is an example of a query that requests the `Position` component from both the entity and its parent:

```
PARENT:Position, Position
```

#### OWNED
The `OWNED` modifier matches a component if the entity owns the component. Whenever a component is added to an entity, it is said that the entity owns the component:

```c
ecs_entity_t e = ecs_new(world, 0);

// Entity 'e' owns Position
ecs_add(world, e, Position);
```

Components can be shared, when an entity has an `INSTANCEOF` relationship, in which case an entity may share components with a base entity:

```c
ecs_entity_t base = ecs_new(world, 0);
ecs_add(world, base, Position);

// Entity 'e' does not own Position, Position is shared
ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | base);
```

An example of a signature expression with `OWNED:

```
OWNED:Position, OWNED:Velocity
```

`OWNED` is the default modifier, which means that if an application does not provide a source modifier, `OWNED` is used.

#### SHARED
The `SHARED` modifier only matches shared components. A shared component is a component that the entity inherits from a base entity, through an `INSTANCEOF` relationship (see the examples in `OWNED`).

When a query or system matches with a `SHARED` component, the `ecs_column` function does not provide an array. Instead it provides a pointer to the shared component which is shared with the entities in the table being iterated over:

```c
// Request Position, Velocity where Velocity must be shared
ecs_query_t *query = ecs_query_new(world, "Position, SHARED:Velocity");

ecs_iter_t it = ecs_query_iter(query);

while (ecs_query_next(&it)) {
    // Pointer is an array, Velocity is a pointer
    Position *p = ecs_column(&it, Position, 1);
    Velocity *v = ecs_column(&it, Velocity, 2);

    for (int i = 0; i < it.count, i ++) {
        // Access velocity as pointer, not as array
        p[i].x += v->x;
        p[i].y += v->y;
    }
}
```

#### ANY
The `ANY` modifier matches both `OWNED` and `SHARED` components. When `ANY` is used, a system should use the `ecs_is_owned` function to test whether it is owned or shared:

```c
// Request Position, Velocity where Velocity must be shared
ecs_query_t *query = ecs_query_new(world, "Position, ANY:Velocity");

ecs_iter_t it = ecs_query_iter(query);

while (ecs_query_next(&it)) {
    Position *p = ecs_column(&it, Position, 1);
    Velocity *v = ecs_column(&it, Velocity, 2);

    // Test outside of loop for better performance
    if (ecs_is_owned(it, 2)) {
        // Velocity is owned, access as array
        for (int i = 0; i < it.count, i ++) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
        }
    } else {
        // Velocity is shared, access as pointer
        for (int i = 0; i < it.count, i ++) {
            p[i].x += v->x;
            p[i].y += v->y;
        }        
    }
}
```

#### PARENT
The `PARENT` modifier requests a component from the parent of an entity. This works with entities that have a `CHILDOF` relationship:

```c
ecs_entity_t parent = ecs_new(world, 0);
ecs_add(world, parent, Position);

ecs_entity_t child = ecs_new_w_entity(world, ECS_CHILDOF | parent);
```

Queries that use a `PARENT` column should access the column data as if it were a `SHARED` column, as a pointer and not as an array:

```c
ecs_query_t *query = ecs_query_new(world, "Position, PARENT:Position");

ecs_iter_t it = ecs_query_iter(query);

while (ecs_query_next(&it)) {
    // 1st Position is array, 2nd one is pointer
    Position *p = ecs_column(&it, Position, 1);
    Position *p_parent = ecs_column(&it, Position, 2);

    for (int i = 0; i < it.count, i ++) {
        p[i].x += p_parent->x;
        p[i].y += p_parent->y;
    }
}
```

If an entity does not have a parent with the specified component, the query will not match with the entity.

#### CASCADE
The `CASCADE` modifier is like the `PARENT` modifier, except that it iterates entities depth-first, calculated by counting the number of parents from an entity to the root. Another difference with `PARENT` is that `CASCADE` matches with the root of a tree, which does not have a parent with the specified component. This requires `CASCADE` queries to check if the parent component is available:

```c
ecs_query_t *query = ecs_query_new(world, "Position, CASCADE:Position");

ecs_iter_t it = ecs_query_iter(query);

while (ecs_query_next(&it)) {
    Position *p = ecs_column(&it, Position, 1);
    Position *p_parent = ecs_column(&it, Position, 2);

    if (p_parent) {
        for (int i = 0; i < it.count, i ++) {
            p[i].x += p_parent->x;
            p[i].y += p_parent->y;
        }
    }
}
```

The `CASCADE` modifier is useful for systems that need a certain parent component to be written before the child component is written, which is the case when, for example, transforming from local coordinates to world coordinates.

#### Nothing
The nothing modifier does not get the component from an entity, but instead just passes its identifier to a query or system. An example:

```c
ecs_query_t *query = ecs_query_new(world, "Position, :Velocity");

ecs_iter_t it = ecs_query_iter(query);

while (ecs_query_next(&it)) {
    Position *p = ecs_column(&it, Position, 1);

    // Get component identifier from column
    ecs_entity_t vel_id = ecs_column_entity(&it, 2);
}
```

### Operators
Signatures may contain operators, which allow queries to make more granular selections of entities. The following operators are avaialble:

Operator | Symbol | Description
---------|--------|------------
And      | `,`    | All elements in AND expression must match
Or       | `||`   | At least one components in OR expression must match
Not      | `!`    | Entity should not have component
Optional | `?`    | Entity may have component

Operators can be combined with source modifiers:

```
Position, !PARENT:Velocity
```

#### AND
And is the most common operator, and allows a query to request a set of components that an entity must have in order to be matched. The AND operator takes lowest precedence, and elements in an AND expression may contain other operators. An example of a signature with an `AND` operator is:

```
Position, Velocity
```

#### OR
OR expressions allow a signature to match with one of the components in the OR expression. An example of a signature with an `OR` operator is:

```
Postion, Velocity || Speed
```

In this example, any entity that has `Position`, and `Velocity` OR `Speed` will match the signature. Components in an OR expression may contain SOURCE modifiers, but the source modifier must be the same for all elements:

```
Postion, PARENT:Velocity || PARENT:Speed
```

#### NOT
Not expressions allow a signature to exclude entities that have a component. An example of a signature with a `NOT` operator is:

```
Position, !Velocity
```

This signature matches all entities that have `Position`, but not have `Velocity`. An expression with a `NOT` column does not pass any data into a system, which means that the `ecs_column` function is guaranteed to return `NULL` for a column.

#### Optional
The optional operator allows a signature to both match entities with and without a specific component. An exampole with an optional operator is:

```
Position, ?Velocity
```

A query with an optional column should test if the component is set before using it:

```c
ecs_query_t *query = ecs_query_new(world, "Position, CASCADE:Position");

ecs_iter_t it = ecs_query_iter(query);

while (ecs_query_next(&it)) {
    Position *p = ecs_column(&it, Position, 1);
    Velocity *v = ecs_column(&it, Velocity, 2);

    if (v) {
        for (int i = 0; i < it.count, i ++) {
            p[i].x += v[i].x
            p[i].y += v[i].y
        }
    }
}
```

### Access modifiers
A signature can contain access modifiers, which lets the framework know whether a system or query will read or write the component. The following access modifiers are supported:

Modifier | Description
---------|------------
in       | The component is read only
out      | The component is write only
inout    | The component can both be read and written

Access modifiers are added to a signature using angular brackets:

```
[out] Position, [in] Velocity
```

The default access modifier is `[inout]`, which by default allows a system to read and write a component, but also means Flecs cannot make optimizations in, for example, how systems can be executed in parallel. For this reason, while not mandatory, applications are encouraged to add access modifiers to systems where possible.

## Sorting
Applications are able to access entities in order, by using sorted queries. Sorted queries allow an application to specify a component that entities should be sorted on. Sorting is enabled with the `ecs_query_order_by` function:

```c
ecs_query_t q = ecs_query_new(world, "Position");
ecs_query_order_by(world, q, ecs_entity(Position), compare_position);
```

This will sort the query by the `Position` component. The function also accepts a compare function, which looks like this:

```c
int compare_position(ecs_entity_t e1, Posiiton *p1, ecs_entity_t e2, Position *p2) {
    return p1->x - p2->x;
}
```

Once sorting is enabled for a query, the data will remain sorted, even after the underlying data changes. The query keeps track of any changes that have happened to the data, and if changes could have invalidated the ordering, data will be resorted. Resorting does not happen when the data is modified, which means that sorting will not decrease performance of regular operations. Instead, the sort will be applied when the application obtains an iterator to the query:

```c
ecs_entity_t e = ecs_new(world, Position); // Does not reorder
ecs_set(world, e, Position, {10, 20}); // Does not reorder
ecs_iter_t it = ecs_query_iter(q); // Reordering happens here
```

The following operations mark data dirty can can trigger a reordering:
- Creating a new entity with the ordered component
- Deleting an entity with the ordered component
- Adding the ordered component to an entity
- Removing the ordered component from an entity
- Setting the ordered component
- Running a system that writes the ordered component (through an [out] column)

Applications iterate a sorted query in the same way they would iterate a regular query:

```c
while (ecs_query_next(&it)) {
    Position *p = ecs_column(&it, Position, 1);

    for (int i = 0; i < it.count; i ++) {
        printf("{%f, %f}\n", p[i].x, p[i].y); // Values printed will be in order
    }
}
```

### Sorting algorithm
The algorithm used for the sort is a quicksort. Each table that is matched with the query will be sorted using a quicksort. As a result, sorting one query affects the order of entities in another query. However, just sorting tables is not enough, as the list of ordered entities may have to jump between tables. For example:

Entitiy | Components (table) | Value used for sorting
--------|--------------------|-----------------------
E1      | Position           | 1
E2      | Position           | 3
E3      | Position           | 4
E4      | Position, Velocity | 5
E5      | Position, Velocity | 7
E6      | Position, Mass     | 8
E7      | Position           | 10
E8      | Position           | 11

To make sure a query iterates the entities in the right order, it will iterate entities in the ordered tables to determine the largest slice of ordered entities in each table, which the query will iterate in order. Slices are precomputed during the sorting step, which means that the performance of query iteration is similar to a regular iteration. For the above set of entities, these slices would look like this:

Table              | Slice
-------------------|-------
Position           | 0..2
Position, Velocity | 3..4
Position, Mass     | 5
Position           | 6..7

This process is transparent for applications, except that the slicing will result in smaller contiguous arrays being iterated by the application.

### Sorting by entity id
Instead of sorting by a component value, applications can sort by entity id by not specifying a component to the `ecs_query_order_by` function:

```c
ecs_query_order_by(world, q, 0, compare_entity);
```

The compare function would look like this:

```c
int compare_position(ecs_entity_t e1, Posiiton *p1, ecs_entity_t e2, Position *p2) {
    return e1 - e2;
}
```

When no component is provided in the `ecs_query_order_by` function, no reordering will happen as a result of setting components or running a system with `[out]` columns.

## Filters
Filters allow an application to iterate through matching entities in a way that is similar to queries. Contrary to queries however, filters are not prematched, which means that a filter is evaluated as it is iterated over. Filters are therefore slower to evaluate than queries, but they have less overhead and are (much) cheaper to create. This makes filters less suitable for repeated-, but useful for ad-hoc searches where the application doesn't know beforehand which set of entities it will need.

A filter can be used like this:

```cpp
ECS_COMPONENT(world, Position);

/* Create filter */
ecs_filter_t filter = {
    .include = ecs_type(Position),
    .include_kind = EcsMatchAll
};

/* Create iterator to filter */
ecs_iter_t it = ecs_filter_iter(world, &filter);

while (ecs_filter_next(&it)) {
    /* Because a filter does not have a signature, we need to get the component
     * array by finding it in the current table */
    ecs_type_t table_type = ecs_iter_type(&it);

    /* First Retrieve the column index for Position */
    int32_t p_index = ecs_type_index_of(table_type, ecs_entity(Position));

    /* Now use the column index to get the Position array from the table */
    Position *p = ecs_table_column(&it, p_index);

    /* Iterate as usual */
    for (int i = 0; i < it.count; i ++) {
        printf("{%f, %f}\n", p[i].x, p[i].y);
    }
}
```

A filter can provide an `include` and an `exclude` type, where the `include` type specifies the components the entities must have in order to match the filter, and the `exclude` type specifies the components the entity should not have. In addition to these two fields, the filter provides a `kind` field for both `include` and `exclude` types which can be one of these values:

Option          | Description
----------------|------------------------------------------------------------------------
EcsMatchDefault | Default matching: ECsMatchAny for include, and EcsMatchAll for exclude
EcsMatchAll     | The type must include/excldue all components
EcsMatchAny     | The type must include/exclude one of the components
EcsMatchExact   | The type must match exactly with the components of the entity

## Systems
Systems allow the application to run logic that is matched with a set of entities every frame, periodically or as the result of some event. An example of a simple system definition is:

```c
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
```

In this definition, `Move` is the name of the system and also fo the function that will be registered with the system. `EcsOnUpdate` is the system "phase" which indicates when the system is ran in a frame (see "Pipelines"). The part that follows is the system expression, which follows the rules as described in "Signatures").

A system implementation is a function with the following signature:

```c
void Move(ecs_iter_t *it) { }
```

The implementation of a system is a regular query iteration:

```c
Position *p = ecs_column(it, Position, 1);
Velocity *v = ecs_column(it, Velocity, 2);

for (int i = 0; i < it->count, i ++) {
    p[i].x += v[i].x;
    p[i].y += v[i].y;
}
```

### Using delta_time
A system provides a `delta_time` which contains the time passed since the last frame:

```c
Position *p = ecs_column(it, Position, 1);
Velocity *v = ecs_column(it, Velocity, 2);

for (int i = 0; i < it->count, i ++) {
    p[i].x += v[i].x * it->delta_time;
    p[i].y += v[i].y * it->delta_time;
}
```

This is the value passed into `ecs_progress`:

```c
ecs_progress(world, delta_time);
```

If 0 was provided for `delta_time`, flecs will automatically measure the time passed between the last frame and the current. 

A system may also use the `delta_system_time` member, which is the time elapsed since the last time the system was invoked. This can be useful when a system is not invoked each frame, for example when using a timer.

### Systems and tables
A system may be invoked multiple times per frame. The reason this happens is because entities are stored in different "tables", where each table stores entities of a specific set of components. For example, all entities with components `Position, Velocity` will be stored in table A, where all entities with components `Position, Mass` are stored in table B. Tables ensure that component data is stored in contiguous arrays, and that the same index can be used for a particular entity in all component arrays. Because systems iterate component arrays directly, and because a component can be stored in more than one array, systems need to be invoked once for each table.

The total number of tables a system will iterate over is stored in the `table_count` member of the iterator. Additionally, the `table_offset` member contains the current table being iterated over, so that a system can keep track of where it is in the iteration:

```c
void Move(ecs_iter_t *it) { 
    printf("Iterating table %d / %d\n", it->table_offset, it->table_count);
    // ...
}
```

## Triggers
Triggers are callbacks that are executed when a component is added or removed from an entity. Triggers are similar to systems, but unlike systems they can only match a single component. This is an example of a trigger that is executed when the Position component is added:

```c
ECS_TRIGGER(world, AddPosition, EcsOnAdd, Position);
```

The implementation of the trigger looks similar to a system:

```c
void AddPosition(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);

    for (int i = 0; i < it->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;
        printf("Position added\n");
    }
}
```

## Modules
Modules allow an application to split up systems and components into separate decoupled units. The purpose of modules is to make it easier to organize systems and components for large projects. Additionally, modules also make it easier to split off functionality into separate compilation units.

A module consists out of a few parts:

- A module type (struct) that stores handles to the contents in the modules
- A macro to declare module contents as local variables in the scope where it is imported
- An import function that loads the module contents for a world

The module type and macro are typically located in the a separate module header file, and look like this for a module named "Vehicles":

```c
typedef struct Car {
    float speed;
} Car;

typedef struct Bus {
    float speed;
} Bus;

typedef struct MotorCycle {
    float speed;
} MotorCycle;

typedef struct Vehicles {
    /* Components are declared with ECS_DECLARE_COMPONENT */
    ECS_DECLARE_COMPONENT(Car);
    ECS_DECLARE_COMPONENT(Bus);
    ECS_DECLARE_COMPONENT(MotorCycle);

    /* Tags are declared with ECS_DECLARE_ENTITY */
    ECS_DECLARE_ENTITY(Moving);

    /* Systems are also declared with ECS_DECLARE_ENTITY */
    ECS_DECLARE_ENTITY(Move);
};

/* Forward declaration to the import function */
void VehiclesImport(ecs_world_t *world);

/* The ImportHandles macro mimics the module struct */
#define VehiclesImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, Car);\
    ECS_IMPORT_COMPONENT(handles, Bus);\
    ECS_IMPORT_COMPONENT(handles, MotorCycle);\
    ECS_IMPORT_ENTITY(handles, Moving);\
    ECS_IMPORT_ENTITY(handles, Move);
```

The import function for this module would look like this:

```c
void VehiclesImport(ecs_world_t *world) {
    /* Define the module */
    ECS_MODULE(world, Vehicles);

    /* Declare components, tags and systems as usual */
    ECS_COMPONENT(world, Car);
    ECS_COMPONENT(world, Bus);
    ECS_COMPONENT(world, MotorCycle);
    ECS_TAG(world, Moving);
    ECS_SYSTEM(world, Move, EcsOnUpdate, Car, Moving);

    /* Export them so that they are assigned to the module struct */
    ECS_EXPORT_COMPONENT(world, Car);
    ECS_EXPORT_COMPONENT(world, Bus);
    ECS_EXPORT_COMPONENT(world, Motorcycle);
    ECS_EXPORT_ENTITY(world, Moving);
    ECS_EXPORT_ENTITY(world, Move);
}
```

After the module has been defined, it can be imported in an application like this:

```c
ecs_world_t *world = ecs_init();

/* Import module, which invokes the module import function */
ECS_IMPORT(world, Vehicles);

/* The module contents can now be used */
ecs_entity_t e = ecs_new(world, Car);
```

Module contents are namespaced, which means that the identifiers of the contenst of the module (components, tags, systems) are stored in the scope of the module. For the above example module, everything would be stored in the `vehicles` scope. To resolve the `Car` component by name, an application would have to do:

```c
ecs_entity_t car_entity = ecs_lookup_fullpath(world, "vehicles.Car");
```

Note that even though the module name is specified with uppercase, the name is stored with lowercase. This is because the naming convention for modules in C is PascalCase, whereas the stored identifiers use snake_case. If a module name contains several uppercase letters, this will be translated to a nested module. For example, the C module name `MySimpleModule` will be translated to `my.simple.module`.

### Modules in C++
A module in C++ is defined as a class where the module contents are defined in the constructor. The above Vehicles module would look like this in C++:

```cpp
/* In C++ it is more convenient to define tags as empty structs */
struct Moving { };

/* Module implementation */
class vehicles {
public:
    vehicles(flecs::world& world) {
        flecs::module<Vehicles>(world, "vehicles");

        m_car = flecs::component<Car>(world, "Car");
        m_bus = flecs::component<Bus>(world, "Bus");
        m_motor_cycle = flecs::component<MotorCycle>(world, "MotorCycle");

        m_moving = flecs::component<Moving>(world, "Moving");
        m_move = flecs::system<Car, Moving>(world, "Move")
            .each([](flecs::entity e, Car &car, Moving&) {
                /* System implementation */
            });
    }

    flecs::entity m_car;
    flecs::entity m_bus;
    flecs::entity m_motor_cycle;
    flecs::entity m_moving;
    flecs::entity m_move;
}
```

An application can import the module in C++ like this:

```cpp
flecs::world world;
flecs::import<vehicles>(world);
```

## Hierarchies

## Instancing

## Traits

## Pipelines

## Time management

## Timers

## Snapshots

## Serialization

## Bulk operations

## Statistics

## Staging

## Threading

## Tracing

## Debug API

## OS Abstraction API
