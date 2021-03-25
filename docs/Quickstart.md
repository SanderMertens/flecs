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
flecs::world world;

/* Application */
```

## Entities
An entity (`ecs_entity_t`) is a 64-bit integer that uniquely identifies a thing or object in your application. Entities are created like this:

```c
ecs_entity_t e = ecs_new(world, 0);
```
```cpp
auto e = world.entity();
```

You can also use plain numbers:

```c
ecs_entity_t e = 1000;
```
```cpp
auto e = world.entity(1000);
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
    flecs::world world;

    // Register the component with the world
    world.component<Position>();
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
auto e = world.entity().add<Position>();
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
```cpp
const Position *p = e.get<Position>();
```

If you need a pointer that you can modify, use `ecs_get_mut`:

```c
Position *p = ecs_get_mut(world, e, Position, NULL);
```
```cpp
const Position *p = e.get_mut<Position>();
```

Components can be removed with `ecs_remove`:

```c
ecs_remove(world, e, Position);
```
```cpp
e.remove<Position>();
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
```cpp
struct Enemy { };

int main() {
    flecs::world world;

    // Register the tag with the world
    world.component<Enemy>();
}
```

Once registered, you can now add & remove the tag just like a regular component:

```c
// Add the Enemy tag
ecs_add(world, e, Enemy);

// Remove the Enemy tag
ecs_remove(world, e, Enemy);
```
```cpp
// Add the Enemy tag
e.add<Enemy>();

// Remove the Enemy tag
e.remove<Enemy>();
```

Note that since a tag does not have data, you cannot use `ecs_set` or `ecs_get`.

## Systems
Systems are functions that are matched with entities that have a specific set of components. A typical system is defined like this:

```c
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
```
```cpp
world.system<Position, Velocity>("Move").iter(Move);
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
```cpp
void Move(flecs::iter& it, Position *p, Velocity *v) {
    for (auto i : it) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}
```

The `it` argument contains all the information the system needs to iterate the components. The `ecs_column` function (in C) returns a C array for the subscribed for component. The numbers `1` and `2` indicate where in the system signature the components can be found.

The system will be invoked by `ecs_progress`, which runs the main loop:

```c
// Progress frame, exit loop when application should quit
while (ecs_progress(world, 0)) { }
```
```cpp
while (world.progress()) { }
```

The `Move` function will be invoked once per unique [type](#Type) that matched with the system. For example if a system matched with entities that have `Position, Velocity` and `Position, Velocity, Mass`, the function would be invoked twice.

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
```cpp
// Create a query for all entities with Position, Velocity
auto query = world.query<Position, const Velocity>();

query.each([](flecs::entity e, Position& p, const Velocity &v) {
    p.x += v.x;
    p.y += v.y;
});
```

Queries are registered with the world, and entities (types) are continuously matched with a query. This means that when an application iterates over a query, matching has already happened, which makes it very fast.

## Traits
Traits are a special kind of component that is added to an entity,component tuple. Pair components can be useful for implementing functionality that is not specific to one component. A typical example is implementing a timer after which a component should be deleted. We can define the trait component type like this:

```c
typedef struct ExpiryTimer {
    float expiry_time;
    float t;
} ExpiryTimer;
```
```cpp
struct ExpiryTimer {
    float expiry_time;
    float t;
};
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
```cpp
auto e = world.entity();

// Add HealthBuff, set the ExpiryTimer trait for HealthBuff to 10 seconds
e.add<HealthBuff>();
e.set<ExpiryTimer, HealthBuff>({ 10 });

// Add StaminaBuff, set the ExpiryTimer trait for StaminaBuff to 5 seconds
e.set<ExpiryTimer, StaminaBuff>({ 5 });
```

Now we need to write a system to increase the timer and execute the remove logic. The system definition looks almost like a regular system:

```c
ECS_SYSTEM(world, ExpireComponents, EcsOnUpdate, PAIR | ExpiryTimer);
```
```cpp
world.system<>("ExpireComponents", "PAIR | ExpiryTimer").iter(ExpiryTimer);
```

Note that the `ExpiryTimer` has the `PAIR` role. This lets the system know it should match this component as a trait, not as a regular component. Now lets look at the implementation of this system:

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
    for (int32_t i = 0; i < it->count; i ++) {
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
```cpp
void ExpireComponents(flecs::iter& it) {
    /* Get the trait component */
    ExpiryTimer *et = it.term<ExpiryTimer>(1);

    /* Get the trait handle */
    auto trait = it.column_entity(1);

    /* Obtain the component handlem, which is the lower 32 bits
     * of the trait handle, which can be obtained with the 
     * ecs_entity_t_lo macro. */
    auto comp = trait.lo();

    /* Iterate trait component as usual ... */
    for (auto i : it) {
        /* When timer hits expiry time, remove component */
        et[i].t += it.delta_time();
        if (et[i].t >= et[i].expiry_time) {
            /* Remove component */
            it.entity(i).remove(comp);

            /* Removes trait, so system won't be invoked again */
            it.entity(i).remove(trait);
        }
    }
```

Note that this system doesn't contain any code that is specific for the components to which the traits were added. This means this system can be applied to any component.

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

ecs_set_component_actions(world, ecs_typeid(String), 
    &(EcsComponentLifecycle){
        .ctor = ecs_ctor(String),
        .dtor = ecs_dtor(String),
        .copy = ecs_copy(String),
        .move = ecs_move(String)
    });
```

The C++ API automatically uses the component type constructor, destructor and copy, move assignment operators.
