# Flecs Manual

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
Flecs has a design that is optimized for minimizing cache misses by loading only data in cache that is required by the application, while also storing data in arrays to ensure that an application makes optimal usage of cache lines. In many cases, applications can access raw arrays directly, wich is as fast as iterating a native array in C and, if the code permits it, lets applications be compiled with Single Instruction, Multiple Data (SIMD) instructions.

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
void Move(ecs_rows_t rows) {
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
    
    // Enumeration constants ('EcsOnFrame') use CamelCase
    ECS_SYSTEM(world, Move, EcsOnFrame, Position, Velocity);
    
    // Entity names use CamelCase
    ECS_ENTITY(world, MyEntity, Position, Velocity);
    
    // Imperative macro's (function wrappers) use snake_case
    ecs_set(world, MyEntity, Position, {10, 20});
    
    return ecs_fini(world);
}
```

#### Handles
The Flecs API creates and uses handles (integers) to refer to entities, systems and components. Most of the times these handles are transparently created and used by the API, but in some cases the API may need to access the handles directly, in which case it is useful to know their naming conventions.

The Flecs API has entity handles (of type `ecs_entity_t`) and type handles (of type `ecs_type_t`). Entity handles are used to refer to a single entity. Systems and components (amongst others) obtain identifiers from the same id pool, thus handles to systems and components are also of type `ecs_entity_t`. Types are identifiers that uniquely identify a set of entities (or systems, components). Types are commonly used to add/remove one or more components to/from an entity, or enable/disable one or more systems at once.

Type handles are automatically created by API macro's (like `ECS_COMPONENT`) and are always prefixed by a `T`. Functions like `ecs_new` or `ecs_add` are actually macro's which automatically add the `T` to the type that is being passed to the function. This automatically enforces the naming convention, and makes the API more readable.

The following code example demonstrates the various handles and their naming conventions:

```c
ecs_world_t *world = ecs_init();

// Declares an entity handle 'EPosition' and type handle 'TPosition'
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity); // Ditto for Velocity

// Declares an entity handle 'Movable' and type handle 'TMovable'
ECS_TYPE(world, Movable, Position, Velocity);

// Declares an entity handle 'Move'
ECS_SYSTEM(world, Move, EcsOnFrame, Position, Velocity);

// ecs_new automatically adds the T to Movable so it receives the type handle
ecs_entity_t e = ecs_new(world, Movable);

// ecs_remove automatically adds the T to Position, so it receives the type handle
ecs_remove(world, e, Position);

// Print entity handle and type handle for Position
printf("Position entity: %ld, Position type: %u\n", EPosition, TPosition);
```

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

This does not mean that the API cannot fail. It relies on mechanisms like memory allocation and thread creation amongst others which can fail. It is also possible that an application corrupts memory, or Flecs contains a bug, which can also result in errors. In any of these situations, Flecs is unable to fulfill the post condition of an operation **and will assert or abort**, resulting in the termination of the application.

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

EcsArrayParams params = {.element_size = sizeof(int)};

void InitDynamicBuffer(EcsRows *rows) {
    DynamicBuffer *data = ecs_column(rows, DynamicBuffer, 1);
    for (int i = rows->begin; i < rows->end; i ++) {
        data[i] = ecs_array_new(&params, 0);
    }
}

void DeinitDynamicBuffer(EcsRows *rows) {
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


