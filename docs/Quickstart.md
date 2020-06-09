# Flecs Quickstart
This document provides a quick overview of Flecs features.

## World
The world is the container for all of your ECS data. An application can have multiple worlds. To create & delete a world, simply do:

```c
ecs_world_t *world = ecs_init();

/* Application */

ecs_fini(world);
```

## Entities
An entity (`ecs_entity_t`) is a 64-bit integer that uniquely identifies something in your application. Entities are created with the `ecs_new` function:

```c
ecs_entity_t e = ecs_new(world, 0);
```

Entities do not have to be explicitly created with `ecs_new`. You can also use plain numbers:

```c
ecs_entity_t e = 1000;
```

The only difference is that `ecs_new` guarantees to return an id that is not in use.

## Components
A component is a plain datatype that can be attached to an entity. An entity can contain any number of components. Components must be registered with the world like this:

```c
// Components can be defined from regular types
typedef struct Position {
    float x, y;
} Position;

int main() {
    ecs_world_t *world = ecs_init();

    // Register the component with the world
    ECS_COMPONENT(world, Position);
}
```

Once registered, a component can be added to an entity using `ecs_add`:

```c
ecs_add(world, e, Position);
```

You can also create an entity with a component already added:

```c
ecs_entity_t e = ecs_new(world, Position);
```

An application can also use `ecs_set` to assign a value to the component. If the component was not added yet, `ecs_set` will add it implicitly:

```c
ecs_set(world, e, Position, {10, 20});
```

The value of a component can be requested with `ecs_get`, which will return `NULL` if the entity does not have the component:

```c
const Position *p = ecs_get(world, e, Position);
```

If you need a pointer that you can modify, use `ecs_get_mut`:

```c
Position *p = ecs_get_mut(world, e, Position);
```

Components can be removed with `ecs_remove`:

```c
ecs_remove(world, e, Position);
```

## Tags
Tags are much like components, but they are not associated with a data type. Tags are typically used to add a flag to an entity, for example to indicate that an entity is an Enemy:

```c
int main() {
    ecs_world_t *world = ecs_init();

    // Register the tag with the world. There is no Enemy type
    ECS_TAG(world, Enemy);
}
```

Once registered, you can now add & remove the tag just like a regular component:

```c
// Add the Enemy tag
ecs_add(world, e, Enemy);

// Remove the Enemy tag
ecs_remove(world, e, Enemy);
```

Note that since a tag does not have data, you cannot use `ecs_set` or `ecs_get`.

## Entity names
Entities can have an optional name. The easiest way to create a named entity is with `ECS_ENTITY`:

```c
ECS_ENTITY(world, MyEntity, Position, Velocity);
```

This creates an entity called `MyEntity` with components `Position, Velocity`. To get the name of an entity, do:

```c
const char *name = ecs_get_name(world, MyEntity);
```

Alternatively an application can set the name of an entity by using the `EcsName` component:

```c
ecs_set(world, e, EcsName, {"MyName"});
```

Applications can lookup an entity by name with the `ecs_lookup` function:

```c
ecs_entity_t e = ecs_lookup(world, "MyName");
```

## Types
Types are vectors that contain multiple components. Each entity has a type which describes the components it has. To request the type of an entity, do:

```c
ecs_type_t type = ecs_get_type(world, e);
```

There are many things you can do with a type. One useful thing you can do is print it:

```c
char *str = ecs_type_str(world, type);
printf("type = %s\n", str);
free(str);
```
This will print something like:
```
Position,Velocity
```

You can add and remove types to entities in the same way that you can add and remove components. This lets you add/remove multiple components at the same time. One common way to do that is like this:

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);
ECS_TYPE(world, MyType, Position, Velocity);

// Add both Position and Velocity to entity e
ecs_add(world, e, MyType);
```

## Systems
Systems are functions that are matched with entities that have a specific set of components. A typical system is defined like this:

```c
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
```

`Move` is the system name and also the name of the system function, `EcsOnUpdate` indicates when the system runs (see [Pipelines](#Pipelines)). `Position, Velocity` is the system signature, and indicates the components the system is subscribed for.

The implementation of this system could look like this:

```c
void Move(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    Velocity *v = ecs_column(it, Velocity, 2);

    for (int i = 0; i < it->count, i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}
```

The `it` argument contains all the information the system needs to iterate the components. The `ecs_column` function returns a C array for the subscribed for component. The numbers `1` and `2` indicate where in the system signature the components can be found.

The system will be invoked by `ecs_progress`, which runs the main loop:

```c
// Progress frame, exit loop when application should quit
while (ecs_progress(word, 0)) { }
```

The `Move` function will be invoked once per unique [type](#Type) that matched with the system. For example if a system matched with entities that have `Position, Velocity` and `Position, Velocity, Mass`, the function would be invoked twice.

## Instancing
Components from a single entity can be reused across many entities by using instancing. Setting up instancing is similar to adding components and tags to an entity:

```c
// Create a base entity with Position
ecs_entity_t base = ecs_new(world, Position);

// Create an instance of base. Position will be shared with instance
ecs_entity_t instance = ecs_new_w_entity(world, ECS_INSTANCEOF | base);
```

Note that instead of adding a _component_ to an entity, we add an _entity_ to an entity with the `ecs_new_w_entity` function.  `ECS_INSTANCEOF` is the "role" of the entity, in this case we indicate that `instance` is an instance of `base`. We can now get `Position` from `instance`:

```c
// Retrieves Position from base
const Position *p = ecs_get(world, instance, Position);
```

`ecs_add_entity` and `ecs_remove_entity` can be used to add and remove instancing relationships:

```c
ecs_remove_entity(world, instance, ECS_INSTANCEOF | base);
```

Instances can override components, which copies the value from the base to the instance component:

```c
// Create base with initialized Position component
ecs_entity_t base = ecs_new(world, 0);
ecs_set(world, base, Position, {10, 20});

// Create instance of base
ecs_entity_t instance = ecs_new_w_entity(world, ECS_INSTANCEOF | base);

// Override Position. Base value will be copied to instance
ecs_add(world, instance, Position);

// Get Position from instance, will be {10, 20}
const Position *p = ecs_get(world, e, Position);
```

Instance relationships and component overrides can be encoded in a type:

```c
ECS_COMPONENT(world, Position);
ECS_ENTITY(world, base, Position);
ECS_TYPE(world, MyType, INSTANCEOF | base, Position);

// Create instance of base, with override for Position
ecs_entity_t instance = ecs_new(world, MyType);
```

## Hierarchies
Entities can be created in a hierarchy. Setting up hierarchies is similar to adding components / tags to an entity:

```c
// Create regular entity that we'll use as parent
ecs_entity_t parent = ecs_new(world, 0);

// Create child entity of parent
ecs_entity_t child = ecs_new_w_entity(world, ECS_CHILDOF | parent);
```

One useful application of hierarchies is CASCADE systems. CASCADE systems iterate entities ordered by depth, which can be used for (for example) transform systems:

```c
ECS_SYSTEM(world, Transform, CASCADE:Position, Position);
```

Which could be implemented like this:

```c
void Transform(ecs_iter_t *it) {
    // Get the parent component array
    Position *p_parent = ecs_column(it, Position, 1);
    
    // Get the entity component array
    Position *p = ecs_column(it, Position, 2);

    // The root doesn't have a parent, so check if it's NULL
    if (!p_parent)
        return;

    // Add parent position to entity position. Note that the parent
    // Position is not passed an array
    for (int i = 0; i < it->count; i ++) {
        p[i].x += p_parent->x;
        p[i].y += p_parent->y;
    }
}
```

Applications can iterate a hierarchy depth-first, using a tree iterator:

```c
ECS_ENTITY(world, Parent, 0);
ECS_ENTITY(world, Child, CHILDOF | Parent);

ecs_iter_t it = ecs_tree_iter(world, Parent);

while (ecs_tree_next(&it)) {
    for (int i = 0; i < it.count; i ++) {
        printf("%s\n", it.entities[i]);
    }
}
```

It is possible to lookup entities by their full path, and to get the full path from an entity:

```c
// Returns Child
ecs_entity_t e = ecs_lookup_fullpath(world, "Parent.Child");

// Returns "Parent.Child"
char *path = ecs_get_fullpath(world, e);
```

## Prefabs
Hierachies and instancing, when combined, let applications create templates for entity hierarchies. Consider the following example of a hierarchy template:

```c
// Create top-level template entity. The ECS_PREFAB macro creates a
// regular entity with the EcsPrefab tag, which hides it from systems.
ECS_PREFAB(world, Destroyer);

    // Create a child prefab. Children are instantiated for an instance
    // whenever their parent is instantiated
    ECS_PREFAB(world, FrontTurret, CHILDOF | Destroyer, Position);
        ecs_set(world, FrontTurret, Position, {-10, 0});

    // Create another child prefab
    ECS_PREFAB(world, BackTurret, CHILDOF | Destroyer, Position);
        ecs_set(world, BackTurret, Position, {10, 0});        
```

Because these entities are defined as prefabs, they will not be matched with systems. The following code instantiates the template hierarchy by using regular instancing:

```c
// Instantiates the Destroyer base and its children. After this operation the instance will have two children, one for each turret.
ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Destroyer);
```

## Queries
Queries are like systems in that they let applications iterate over entities, but without having to create a separate function. Systems use queries internally however, so their APIs are similar:

A query can be used like this:

```c
// Create a query for all entities with Position, Velocity
ecs_query_t *query = ecs_query_new(world, "Position, Velocity");

// Create iterator for query
ecs_query_iter_t it = ecs_query_iter(query);

// Iterate all the matching archetypes
while (ecs_query_next(&it)) {
    ecs_iter_t *it = &it.it;

    // Get the component arrays
    Position *p = ecs_column(it, Position, 1);
    Velocity *v = ecs_column(it, Velocity, 2);

    // Iterate the entities in the archetype
    for (int i = 0; i < it->count, i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}
```

Queries are registered with the world, and entities (types) are continuously matched with a query. This means that when an application iterates over a query, matching has already happened, which makes it very fast.

## Monitors
A monitor is a special kind of system that is executed once when a condition becomes true. A monitor is created just like a regular system, but with the `EcsMonitor` tag:

```c
ECS_SYSTEM(world, OnPV, EcsMonitor, Position, Velocity);
```

This example illustrates when the monitor is invoked:

```c
// Condition is not true: monitor is not invoked
ecs_entity_t e = ecs_new(world, Position);

// Condition is true for the first time: monitor is invoked!
ecs_add(world, e, Velocity);

// Condition is still true: monitor is not invoked
ecs_add(world, e, Mass);

// Condition is no longer true: monitor is not invoked
ecs_remove(world, e, Position);

// Condition is true again: monitor is invoked!
ecs_add(world, e, Position);
```

Note that monitors are never invoked by `ecs_progress`.

An monitor is implemented the same way as a regular system:

```c
void OnPV(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    Velocity *v = ecs_column(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        /* Monitor code. Note that components may not have
         * been initialized when the monitor is invoked */
    }
}
```

## OnSet Systems
OnSet systems are ran whenever the value of one of the components the system subscribes for changes. An OnSet system is created just like a regular system, but with the `EcsOnSet` tag:

```c
ECS_SYSTEM(world, OnSetPV, EcsOnSet, Position, Velocity);
```

This example illustrates when the monitor is invoked:

```c
ecs_entity_t e = ecs_new(world, 0);

// The entity does not have Velocity, so system is not invoked
ecs_set(world, e, Position, {10, 20});

// The entity has both components, but Velocity is not set
ecs_add(world, e, Velocity);

// The entity has both components, so system is invoked!
ecs_set(world, e, Velocity, {1, 2});

// The entity has both components, so system is invoked!
ecs_set(world, e, Position, {11, 22});
```

An OnSet system is implemented the same way as a regular system:

```c
void OnSetPV(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    Velocity *v = ecs_column(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        /* Trigger code */
    }
}
```

## Triggers
Triggers are callbacks that are invoked when a component is added or removed to/from an entity. Triggers can only be defined on a single component:

```c
ECS_TRIGGER(world, OnAddP, EcsOnAdd, Position);
```

This example illustrates when the trigger is invoked:

```c
// Position is added, trigger is invoked
ecs_entity_t e = ecs_new(world, Position);

// Entity already has Position, trigger is not invoked
ecs_add(world, e, Position);
```

A trigger is implemented the same way as a system:

```c
void OnAddP(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);

    for (int i = 0; i < it->count; i ++) {
        /* Trigger code. Note that components may not have
         * been initialized when the trigger is invoked */
    }
}
```

## Component Lifecycle
An application can register callbacks for when a component is constructed, destructed, copied or moved. This allows applications to manage resources and prevent memory leaks.

This is an example implementation for the component lifecycle callbacks:

```c
typedef struct String {
    char *value;
} String;

// Component constructor
void StringCtor(
    ecs_world_t *world,             // World
    ecs_entity_t component,         // Component id
    const ecs_entity_t *entity_ptr, // Array with entity ids
    void *ptr,                      // Array with component ptrs
    size_t size,                    // Component size
    int32_t count,                  // Number of elements
    void *ctx)                      // User context
{
    String *elements = ptr;

    for (int i = 0; i < count; i ++) {
        elements[i].value = NULL;
    }
}

// Component destructor
void StringDtor(
    ecs_world_t *world,             // World
    ecs_entity_t component,         // Component id
    const ecs_entity_t *entity_ptr, // Array with entity ids
    void *ptr,                      // Array with component ptrs
    size_t size,                    // Component size
    int32_t count,                  // Number of elements
    void *ctx)                      // User context
{
    String *elements = ptr;

    for (int i = 0; i < count; i ++) {
        free(elements[i].value);
    }
}

// Component copy
void StringCopy(
    ecs_world_t *world,             // World
    ecs_entity_t component,         // Component id
    const ecs_entity_t *dst_entity, // Destination entities
    const ecs_entity_t *src_entity, // Source entities
    void *dst_ptr,                  // Destination component array
    const void *src_ptr,            // Source component array   
    size_t size,                    // Component size
    int32_t count,                  // Number of elements
    void *ctx)                      // User context
{
    String *dst_elements = dst_ptr;
    String *src_elements = src_ptr;

    for (int i = 0; i < count; i ++) {
        dst_elements[i].value = strdup(src_elements[i].value);
    }
}

// Component move
void StringMove(
    ecs_world_t *world,             // World
    ecs_entity_t component,         // Component id
    const ecs_entity_t *dst_entity, // Destination entities
    const ecs_entity_t *src_entity, // Source entities
    void *dst_ptr,                  // Destination component array
    void *src_ptr,                  // Source component array
    size_t size,                    // Component size
    int32_t count,                  // Number of elements
    void *ctx)                      // User context
{
    String *dst_elements = dst_ptr;
    String *src_elements = src_ptr;

    for (int i = 0; i < count; i ++) {
        dst_elements[i].value = src_elements[i].value;
        src_elements[i].value = NULL;
    }
}
```

The component lifecycle callbacks can be registered like this:

```c
ECS_COMPONENT(world, String);

ecs_set(world, ecs_entity(String), EcsComponentLifecycle, {
    .ctor = StringCtor,
    .dtor = StringDtor,
    .copy = StringCopy,
    .move = StringMove
});
```

## Modules
Modules are used to add organization to components and systems, and to enable reusability across multiple applications. Modules are typically imported at the start of an application like this:

```c
// Import a module called MyModule. This will declare handles to the
// module contents in the current scope.
ECS_IMPORT(world, MyModule, 0);
```

Modules often have a header and a source file. This is an example module header:

```c
// The module type that stores handles to the module contents
typedef struct MyModule {
    ECS_DECLARE_COMPONENT(world, Position);
    ECS_DECLARE_COMPONENT(world, Velocity);
    ECS_DECLARE_ENTITY(world, Move);
} MyModule;

// The function that is executed when the module is imported
void MyModuleImport(
    ecs_world_t *world, 
    int flags);

// This macro declares the module handles in the current scope
#define MyModuleImportHandles(module)\
    ECS_IMPORT_COMPONENT(module, Position);\
    ECS_IMPORT_COMPONENT(module, Position);\
    ECS_IMPORT_ENTITY(module, Move);
```

The corresponding module source file looks like this:

```c
void MyModuleImport(
    ecs_world_t *world, 
    int flags)
{
    // Declare the module
    ECS_MODULE(world, MyModule);

    // Create the components as usual
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    // Create the system as usual
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    // Export components and system to module (this sets the
    // handles in the module type)
    ECS_EXPORT_COMPONENT(Position);
    ECS_EXPORT_COMPONENT(Velocity);
    ECS_EXPORT_ENTITY(Move);
}
```

## Pipelines
A pipeline defines the different phases that are executed for each frame. By default an application uses the builtin pipeline which has the following phases:

- EcsOnLoad
- EcsPostLoad
- EcsPreUpdate
- EcsOnUpdate
- EcsOnValidate
- EcsPostUpdate
- EcsPreStore
- EcsOnStore

These phases can be provided as an argument to the `ECS_SYSTEM` macro:

```c
// System ran in the EcsOnUpdate phase
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

// System ran in the EcsOnValidate phase
ECS_SYSTEM(world, DetectCollisions, EcsOnValidate, Position);
```

An application can create a custom pipeline, like is shown here:

```c
// Create a tag for each phase in the custom pipeline.
// The tags must be created in the phase execution order.
ECS_TAG(world, BeforeFrame);
ECS_TAG(world, OnFrame);
ECS_TAG(world, AfterFrame);

// Create the pipeline
ECS_PIPELINE(world, MyPipeline, BeforeFrame, OnFrame, AfterFrame);

// Make sure the world uses the correct pipeline
ecs_set_pipeline(world, MyPipeline);
```

Now the application can create systems for the custom pipeline:

```c
// System ran in the OnFrame phase
ECS_SYSTEM(world, Move, OnFrame, Position, Velocity);

// System ran in the AfterFrame phase
ECS_SYSTEM(world, DetectCollisions, AfterFrame, Position);

// This will now run systems in the custom pipeline
ecs_progress(world, 0);
```

