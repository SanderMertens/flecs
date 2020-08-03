# Flecs Quickstart
This document provides a quick overview of Flecs features.

## World
The world is the container for all of your ECS data. An application can have multiple worlds. To create & delete a world, simply do:

```c
ecs_world_t *world = ecs_init();

/* Application */

ecs_fini(world);
```
```cpp
flecs::world world();

/* Application */
```

## Entities
An entity (`ecs_entity_t`) is a 64-bit integer that uniquely identifies a thing or object in your application. Entities are created like this:

```c
ecs_entity_t e = ecs_new(world, 0);
```
```cpp
auto e = flecs::entity(world);
```

You can also use plain numbers:

```c
ecs_entity_t e = 1000;
```
```cpp
auto e = flecs::entity(world, 1000);
```

When not using an explicit id, the framework guarantees that the returned id is not in use.

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
```cpp
// Components can be defined from regular types
struct Position {
    float x, y;
};

int main() {
    flecs::world world();

    // Register the component with the world
    flecs::component<Position>(world);
}
```

Once registered, a component can be added to an entity using `add`:

```c
ecs_add(world, e, Position);
```
```cpp
e.add<Position>();
```

You can also create an entity with a component already added:

```c
ecs_entity_t e = ecs_new(world, Position);
```
```cpp
auto e = flecs::entity(world).add<Position>();
```

An application can also use `set` to assign a value to the component. If the component was not added yet, `set` will add it implicitly:

```c
ecs_set(world, e, Position, {10, 20});
```
```cpp
e.set<Position>({10, 20});
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
ECS_SYSTEM(world, Transform, EcsOnUpdate, CASCADE:Position, Position);
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

ecs_iter_t it = ecs_scope_iter(world, Parent);

while (ecs_scope_next(&it)) {
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

## Traits
Traits are a special kind of component that is added to an entity,component tuple. Trait components can be useful for implementing functionality that is not specific to one component. A typical example is implementing a timer after which a component should be deleted. We can define the trait component type like this:

```c
typedef struct ExpiryTimer {
    float expiry_time;
    float t;
} ExpiryTimer;
```

We then create a system that increases the value of t every frame until it matches or exceeds expiry_time, after which we will remove our component. Before looking at the system, let's first look at how we can add a trait to an entity:

```c
ecs_entity_t e = ecs_new(world, 0);

// Add HealthBuff, set the ExpiryTimer trait for HealthBuff to 10 seconds
ecs_add(world, e, HealthBuff);
ecs_set_trait(world, e, HealthBuff, ExpiryTimer, {
    .expiry_time = 10
});

// Add StaminaBuff, set the ExpiryTimer trait for StaminaBuff to 5 seconds
ecs_set_trait(world, e, StaminaBuff, ExpiryTimer, {
    .expiry_time = 5
});
```

Now we need to write a system to increase the timer and execute the remove logic. The system definition looks almost like a regular system:

```c
ECS_SYSTEM(world, ExpireComponents, EcsOnUpdate, TRAIT | ExpiryTimer);
```

Note that the `ExpiryTimer` has the `TRAIT` role. This lets the system know it should match this component as a trait, not as a regular component. Now lets look at the implementation of this system:

```c
void ExpireComponents(ecs_iter_t *it) {
    /* Get the trait component just like a normal component */
    ExpiryTimer *et = ecs_column(it, ExpiryTimer, 1);

    /* Get the trait handle */
    ecs_entity_t trait = ecs_column_entity(it, 1);

    /* Obtain the component handlem, which is the lower 32 bits
     * of the trait handle, which can be obtained with the 
     * ecs_entity_t_lo macro. */
    ecs_entity_t comp = ecs_entity_t_lo(trait);

    /* Iterate trait component as usual ... */
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        /* When timer hits expiry time, remove component */
        et[i].t += it->delta_time;
        if (et[i].t >= et[i].expiry_time) {
            /* Remove component */
            ecs_remove_entity(it->world, it->entities[i], comp);

            /* Removes trait, so system won't be invoked again */
            ecs_remove_entity(it->world, it->entities[i], trait);
        }
    }
```

Note that this system doesn't contain any code that is specific for the components to which the traits were added. This means this system can be applied to any component.

## Queries
Queries are like systems in that they let applications iterate over entities, but without having to create a separate function. Systems use queries internally however, so their APIs are similar:

A query can be used like this:

```c
// Create a query for all entities with Position, Velocity
ecs_query_t *query = ecs_query_new(world, "Position, Velocity");

// Create iterator for query
ecs_iter_t it = ecs_query_iter(query);

// Iterate all the matching archetypes
while (ecs_query_next(&it)) {
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

The opposite of an `EcsOnSet` system is an `EcsUnSet` system:

```c
ECS_SYSTEM(world, UnSetP, EcsUnSet, Position);
```

An UnSet system is invoked when an entity no longer has a value for the specified component:

```c
ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

// The UnSet system is invoked
ecs_remove(world, e, Position);
```

OnSet and UnSet systems are typically invoked when components are set and removed, but there are two edge cases:

- A component is removed but the entity inherits a value for the component from a base entity. In this case OnSet is invoked, because the value for the component changed.
- The entity does not have the component, but the base that has the component is removed. In this case UnSet is invoked, since the entity no longer has the component.

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
ECS_CTOR(String, ptr, {
    ptr->value = NULL;
});

// Component destructor
ECS_DTOR(String, ptr, {
    free(ptr->value);
});

// Component copy
ECS_COPY(String, dst, src, {
    free(dst->value);
    dst->value = strdup(src->value);
});

// Component move
ECS_MOVE(String, dst, src, {
    dst->value = src->value;
    src->value = NULL;
});
```

The component lifecycle callbacks can be registered like this:

```c
ECS_COMPONENT(world, String);

ecs_set_component_actions(world, ecs_entity(String), 
    &(EcsComponentLifecycle){
        .ctor = ecs_ctor(String),
        .dtor = ecs_dtor(String),
        .copy = ecs_copy(String),
        .move = ecs_move(String)
    });
```

## Modules
Modules are used to add organization to components and systems, and to enable reusability across multiple applications. Modules are typically imported at the start of an application like this:

```c
// Import module called MyModule. This declares handles to the
// module contents in the current C scope.
ECS_IMPORT(world, MyModule);
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
    ecs_world_t *world);

// This macro declares the module handles in the current scope
#define MyModuleImportHandles(module)\
    ECS_IMPORT_COMPONENT(module, Position);\
    ECS_IMPORT_COMPONENT(module, Position);\
    ECS_IMPORT_ENTITY(module, Move);
```

The corresponding module source file looks like this:

```c
void MyModuleImport(
    ecs_world_t *world)
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

Modules are namespaced, which emans that the components and system identifiers within a module will be prefixed with the module name. The module name is translated from PascalCase into lowercase separated by dots, such that `MyModule` becomes `my.module`.

If an application wants to create a system that uses components from the module, it has to include the namespace in the signature, for example:

```c
ECS_SYSTEM(world, Move, EcsOnUpdate, 
    my.module.Position, 
    my.module.Velocity);
```

This prevents modules from accidentally creating components or systems that clash with each other. If a module wants however, it can override the scope:

```c
ECS_MODULE(world, MyModule);

// Set scope to root
ecs_entity_t old_scope = ecs_set_scope(world, 0);

// Define components as usual
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);

// Change scope back to the module scope
ecs_set_scope(world, old_scope);
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

## Staging
Staging is a feature that lets applications use the normal API while iterating through entities in a system. Adding & removing components changes their arrays, and because in Flecs systems iterate the underlying component arrays directly, this could introduce undefined behavior. Staging solves this problem by introducing a temporary storage where data is stored while an application is iterating.

The following few examples provide an introduction to staging. To see how this works, let's take the implementation of a normal system and look at what happens exactly:

```c
void Move(ecs_iter_t *it) {
    /* Obtain component arrays from the "main stage" */
    Position *p = ecs_column(it, Position, 1);
    Velocity *v = ecs_column(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        /* Modify values of the component arrays in place. This changes the data
         * of the components in the main stage directly, and nothing is stored
         * in a temporary store. */
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

// System definition
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
```

Now let's look at a system that adds "Velocity" to each entity with a Position:

```c
void AddVelocity(ecs_iter_t *it) {
    /* To add the Velocity component, we need to get the handle first */
    ecs_type_t ecs_type(Velocity) = ecs_column_type(it, 2);

    for (int i = 0; i < it->count; i ++) {
        /* This adds the "Velocity" component. Adding and removing components
         * changes underlying arrays, but since changes are written to the stage
         * the system can keep on iterating. */
        ecs_add(it->world, it->entities[i], Velocity);

        /* While we are staged, we can also query it with the regular API, like so: */
        if (ecs_has(world, it->entities[i], Velocity)) {
            printf("Yes, Velocity was added\n");
        }
    }
}

// System definition, only pass in Velocity handle
ECS_SYSTEM(world, AddVelocity, EcsOnUpdate, Position, :Velocity);
```

In many cases, this means that a system can use the regular API and not worry about how the data from the temporary stage gets merged back into the main stage. However, there is one exception, which is when a system both writes to the component arrays directly as well as stages changes with the regular API:

```c
void AddVelocity(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);
    ecs_type_t ecs_type(Velocity) = ecs_column_type(it, 2);

    for (int i = 0; i < it->count; i ++) {
        /* Add velocity to stage. This copies the entire value of the entity to
         * the stage, including the current value of Position. */
        ecs_add(it->world, it->entities[i], Velocity);

        /* Write to the main stage after the entity has been staged. This change
         * will be lost, because eventually the stage is merged back into the
         * main stage, and the main stage data will be overwritten. */
        p[i].x ++;
        p[i].y ++;        
    }
}
```

This can result in confusing bugs. To make sure this does not happen, as a rule of thumb systems should not write to both the main stage and to the stage. Instead, systems can be split up in two systems. Flecs automatically merges data back to the stage when it is necessary, as long as systems indicate their intents in their signatures. Here are a few examples of correct ways to annotate a system:

```c
// Read/write Position from main stage, read Velocity from main stage
ECS_SYSTEM(world, SystemA, EcsOnUpdate, Position, [in] Velocity);

// Read Position from main stage, write Velocity to stage. For more information
// on what the colon (:) means, see "Signatures" in the manual
ECS_SYSTEM(world, SystemB, EcsOnUpdate, [in] Position, [out] :Velocity);

// Read Position from the main stage, write Position to the stage
ECS_SYSTEM(world, SystemB, EcsOnUpdate, [in] Position, [out] :Position);
```

When a system writes to the stage, and the subsequent system reads the same component from the main stage, Flecs will make sure the component is merged:

```c
// Write Velocity to the stage
ECS_SYSTEM(world, SystemA, EcsOnUpdate, Position, [out] :Velocity);

// Read Velocity from the main stage. Flecs will merge Velocity before running this system
ECS_SYSTEM(world, SystemB, EcsOnUpdate, Position, [in] Velocity);
```

Staging is also the mechanism that allows multiple threads to concurrently make modifications to the store, as Flecs assigns a stage to each thread. 
