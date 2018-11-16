# reflecs
Reflecs is an entity component system implemented in C99. It's design goal is to
pack as much punch as possible into a tiny library with a minimal API and zero
dependencies. The result: a lightning fast, feature-rich ECS framework in a
library no larger than 50Kb. Here's what Reflecs has to offer:

- Stupid simple, single header API (documentation [here](https://github.com/SanderMertens/reflecs/blob/master/include/reflecs.h#L92))
- Memory efficient storage engine optimized to take full advantage of CPU cache lines
- Periodic, on demand and on component init/deinit systems
- Fast job scheduler for multi-threaded execution of systems
- Fine-grained control over every aspect of the framework, like preallocation of memory.
- Adaptive optimizations that remove unused code from the critical path
- Self-describing design that enables reflection and keeps implementation small

## Example
The following code shows a simple reflecs application:

```c
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;
typedef Vector2D Velocity;

void Move(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *position = ecs_column(rows, row, 0);
        Velocity *velocity = ecs_column(rows, row, 1);
        position->x += velocity->x;
        position->y += velocity->y;
    }
}

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    /* Register components and systems */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_FAMILY(world, Movable, Position, Velocity);
    ECS_SYSTEM(world, Move, EcsPeriodic, Position, Velocity);

    /* Create entity with Movable family */
    ecs_new(world, Movable_h);

    /* Progress world in main loop (invokes Move system) */
    while (true) {
        ecs_progress(world);
    }

    return ecs_fini(world);
}
```

## Dynamically assigning components
The above example demonstrates how you can use a family to specify the
components for an entity. This is useful when you know in advance which
components you need. Sometimes you may want to add components dynamically, in
which case you can use `ecs_add` and `ecs_commit`. These functions can be used
like this:

```c
EcsHandle e = ecs_new(world, 0); // No family
ecs_add(world, e, Position_h);   // Stage Position component
ecs_add(world, e, Velocity_h);   // Stage Velocity component
ecs_commit(world, e);            // Commit components to memoy
```

Whenever the components of an entity change, the entity data will move between
internal tables. To minimize moving the entity around, components are first
*staged* before they are *committed*. No components actually get added until you
call `ecs_commit`. Removing a component can be done similarly with `ecs_remove`:

```c
ecs_remove(world, e, Velocity_h); // Stage removing the Velocity component
ecs_commit(world, e);             // Actually remove the Velocity component
```

You can mix calling `ecs_add` and `ecs_remove` within a single commit:
```c
ecs_add(world, e, Size_h);        // Stage adding the Size component
ecs_remove(world, e, Velocity_h); // Stage removing the Velocity component
ecs_commit(world, e);             // Remove Velocity component, add Size component
```

## Initializing components
You can initialize the value of a component in two ways. The first way lets you
set the value of a component for a single entity, with the `ecs_set` operation.
It can be used like this:

```c
ecs_set(world, e, Position, {.x = 10, .y = 20});
```
This method is flexible, but does come at a performance penalty since the
`ecs_set` operation has to iterate over the components of the entity to find the
`Position` component. For entities with small numbers of components the penalty
is probably not significant.

Alternatively, you can assign all entities with a given set of components with
`OnInit` systems. This is much faster than calling `ecs_set` on every single
entity. You can create an `OnInit` system just like normal systems, but instead
of specifying `EcsOnPeriodic`, you specify `EcsOnAdd`:

```c
ECS_SYSTEM(world, InitPosition, EcsOnAdd, Position);
```
This system will be invoked whenever a Position component is added to an entity.
This code is an example of a system action that simply initializes the values of
the Position component to zero:

```c
void InitPosition(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *position = ecs_column(rows, row, 0);
        position->x = 0;
        position->y = 0;
    }
}
```
`OnInit` systems are invoked when components are committed to memory, either by
`ecs_new` or `ecs_commit`. Note that `OnInit` systems are only invoked when
*all* components in the component list have been added in a single commit. If we
would have specified `Position` and `Velocity` in the system definition, but
would only have added `Position` to the entity, the system would not have been
invoked- not even if `Velocity` was introduced in a subsequent commit.

Similar to `OnInit` systems, there are also `OnDeinit` systems which are invoked
when components are removed from an entity. These systems can be specified
simply by specifying `EcsOnRemove`:

```c
ECS_SYSTEM(world, DeinitPosition, EcsOnRemove, Position);
```

Just like `OnInit` systems, `OnDeinit` systems are invoked only invoked when all
components in the system list have been removed. Note that `OnDeinit` systems
can only be invoked by an `ecs_commit`, and never by an `ecs_new`.

## Tags
In reflecs you can create tags. A tag is a component that does not have any
data. In the API, tags are treated no different than components. The following
convenience macro can be used to create a tag:

```c
ECS_TAG(world, MyTag);
```

Note that underneath, this macro uses the same function as `ECS_COMPONENT`
(`ecs_new_component`). You can thus use this tag just as you would use a
component.

Tags can be part of families:

```c
ECS_FAMILY(world, MyFamily, Position, MyTag);
```

You can add/remove tags from entities just like you would components:

```c
EcsHandle e = ecs_new(world, 0);
ecs_add(world, e, MyTag);
ecs_commit(world, e);
```

Systems can use tags in their signature, again just like components:

```c
ECS_SYSTEM(world, MySystem, EcsPeriodic, Position, MyTag);
```

Tags are a very efficient way of filtering data. Because tags are components,
two entities with the same data but different tags are still stored in different
tables. This allows reflecs to prematch systems to those tables, and ensures
that a system never has to iterate over entities that do not match.

Tags are slightly less efficient when you have a small number of entities and
a comparatively large number of tags to subdivide them. Since each combination
of components creates a new table, having many tags can result in many tables. A
table has a much higher memory footprint than an entity, so if you have a small
number of entities, it is probably more efficient to filter in the system code.

## Entities as tags
In reflecs, tags and components are actually entities that have a special
builtin component (`EcsComponent_h`). As a result of that, you can turn regular
entities into tags. A useful application of this is when you have a group of
entities of which subsets belong to another entity. Imagine a multiplayer game
with number of player entities. Each player has their own set of
units. By making the player entities tags and adding them to the units, you can
get a quick overview of which units belong to which player. The following
example illustrates how you can achieve this:

```c
ECS_COMPONENT(world, Player);
ECS_COMPONENT(world, Unit);

EcsHandle my_player = ecs_new(world, Player_h);
ecs_add(world, my_player, EcsComponent_h);
ecs_commit(world, my_player);

EcsHandle unit = ecs_new(world, Unit_h);
ecs_add(world, unit, my_player);
ecs_commit(world, unit);
```

By adding the `EcsComponent` to the `my_player` entity, you can now use this
entity as a component in `ecs_add` as is shown for the `unit` entity.

The next step being able to access the `Player` entity when iterating over the
units. Instead of making complicated (and expensive!) calls using `ecs_get_ptr` and
reflection, there is a feature that lets you specify the `Player` component as
an argument in your system signature, like this:

```c
ECS_SYSTEM(world, WalkPlayerUnits, EcsPeriodic, COMPONENT.Player, Unit);
```
The `COMPONENT.Player` adds a column to our system that contains a reference to
the `Player` component. `COMPONENT` indicates to reflecs that the `Player`
component should not be resolved on the unit, but on the *components
of the unit*. In the system callback, you can now simply do:

```c
void WalkPlayerUnits(EcsRows *rows) {
    void* row;
    for (row = rows->first, row < rows->last; row = ecs_next(rows, row)) {
        Player *p = ecs_column(rows, row, 0);
        Unit *u = ecs_column(rows, row, 1);
    }
}
```

## Preallocating memory
To get the best performance out of Reflecs, it is recommended to preallocate
memory wherever possible. Reflecs automatically allocates memory when needed,
and because memory is not automatically freed, eventually your memory usage
should stabilize. If you know in advance how much memory your application is
going to need however, preallocating it will make your code perform more
predictably.

Reflecs offers fine grained control over how much memory you preallocate, by
letting you specify amounts per component family. There are two calls used for
preallocating memory:

```c
ecs_dim(world, N); // N being the number of entities
```

With `ecs_dim` you can specify the total number of entities you expect your
application will need. This allocates space in the entity lookup table, but does
not actually allocate memory for components. To preallocate memory for
components, you use this function:

```c
ECS_FAMILY(world, MyFamily, Component1, Component2);
ecs_dim_family(world, MyFamily_h, N); // N being the number of entities
```

With `ecs_dim_family` you specify how many entities you expect per family. This
will create the table for `MyFamily` in advance, and will also preallocate the
space for the number of rows specified by the call.

## Reclaiming memory
Reflecs does not automatically reclaim memory. By default, all memory is cleaned
up when the world is deleted, with `ecs_fini`. However, sometimes you may want
to cleanup memory sooner, for example when something changes in your application
that causes it to use a lot less memory. Instead of recreating the world with
all its state, calling `ecs_dim` and `ecs_dim_family` with a number lower than
the currently allocated number of entities will clean up the memory, provided
that it is not in use.

One simple way to reclaim as much memory as possible is to provide `0` for the
number of entities, like this:

```c
ecs_dim(world, 0);
ecs_dim_family(world, MyFamily_h, 0);
```

This will cause Reflecs to free up as much memory as possible. It won't actually
free all memory, but only as much as possible given the amount of memory
currently in use.

## Multithreading
Code written for Reflecs can be easily turned into a multi threaded application
by calling this function:

```c
ecs_set_threads(world, N); // N being the number of worker threads
```
This will, under the right conditions, make your code run N times as fast! If
your speedup is less than that, one of the following things could be true:

- You don't have enough (available) cores to run your code on
- You don't have enough work to overcome the overhead of the job scheduler (which is pretty low)
- You have massive amounts of data, which makes RAM bandwidth your bottleneck, instead of CPU
- Synchronization code in your application is slowing stuff down

Even if your scenario isn't perfect, you will likely still see some speedup. The
Reflecs job scheduler minimizes the amount of context switches in an iteration,
and the scheduler packs as much processing (from multiple systems) in each job
sequence as possible. For most serious applications, you should see a decent
speedup.

## Components, tags, systems and families are entities
In reflecs, components, entities and families are also entities. These entities
have special builtin components (EcsComponent, EcsSystem, EcsFamily) which gives
them special meaning in the reflecs framework. This means you can do some cool
things, like adding components to systems, or even other components, like this
example shows:

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, MySystemData);
ECS_SYSTEM(world, MySystem, Position);

ecs_add(world, MySystem_h, MySystemData_h);
ecs_commit(world, MySystem_h);

/* Store handle to component in a custom WorldContext type (not shown here!) so
 * we can retrieve it from the MySystem callback */
WorldContext ctx = {.system_data = MySystemData_h};
ecs_world_set_context(world, &ctx);
```

To access this data, in the system callback you can then do:

```c
void MySystem(EcsRows *rows) {
    WorldContext *ctx = ecs_world_get_context(rows->world);
    MySystemData *data = ecs_get_ptr(rows->world, rows->system, ctx->system_data);
}
```

Note that you use the same API (`ecs_add`, `ecs_commit`, `ecs_get_ptr`) to add and
get components from the system as you would use for regular entities. This goes
for components and families as well.
