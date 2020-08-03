# Migration guide

## Highlights
Version 2 is a major upgrade from v1, with a complete revamp of internal data structures resulting in better and more predictable performance. Many new features and usability improvements are introduced. This document provides a summary of all changes to make the migration path from v1 applications to v2 easier.

Note that this guide describes the differences between v1 and v2, not between intermediate branches (like `bleeding_edge`).

Here are the highlights of v2:

### Internal changes
- A new table lookup data structure (the table graph) resulting in much better performance for most entity operations (new, add, remove, delete)
- A new sparse-set based entity index which improves entity lookup speed
- A faster and more user-friendly staging design that reduces unnecessary merging and allows for bulk merging of entities
- A new threading design that is easier to use
- A much faster implementation for reactive systems (now called triggers)
- A new and improved prefab hierarchy design that allows for faster instancing of hierarchies
- A unified design for disabling systems and entities that now both use `ecs_enable`

### New features
- A new pipeline architecture that enables fully customizable phases
- Queries provide an interface to iterate over entities with the same benefits as systems (prematched) but without requiring a callback
- Sorting
- Change tracking
- Component lifecycle callbacks allow an application to specify behavior when a component is constructed, destucted, copied or moved
- Component traits allow applications to implement generic behavior once for multiple components
- Many new functions to support working with hierarchies, including a new API to iterate over a hierarchy depth-first
- Modules are now namespaced, which ensures that component & system identifiers won't clash between modules
- Monitors are a new kind of system that is executed once when the system condition becomes true
- `OnSet` systems are now faster and more capable than in v1, and make it much easier to create declarative, component-based APIs
- More ways to introspect internals, including a new, component-based API for creation and introspection of systems
- Time-based and rate-based filters that can be shared across systems
- Type constraints enable adding restrictions on what components can (not) be added to an entity
- A modular core that makes it easy to remove features that are not needed
- Many more smaller usability and performance improvements

### Bugfixes
- A number of issues in multithreading have been addressed that could cause crashes
- Creating / deleting a world would leak memory in v1. v2 is leak free.

## Behavior changes

### Entity ids
In v2 the first `ECS_HI_COMPONENT_ID` ids (defined in `include/flecs/util/api_support.h`) are reserved for components. Ids returned by `ecs_new` will be `ECS_HI_COMPONENT_ID` or higher.

Another change is that entity ids will be recycled when using `ecs_delete`. In v2 an application should never use `ecs_delete` without `ecs_new`. Id recycling relies on a stack with unused entity ids, and if `ecs_delete` is invoked but `ecs_new` is not, this stack will grow unbounded. Bulk operations (`ecs_bulk_new`) do recycle ids.

It is still possible to do operations on bare entity ids like in this example:

```c
ecs_add(world, 42, Position);
```

### Staging
In v2, staging has been completely revised, and this causes some changes in behavior. In v1, only the components that were added, set or removed were staged. This meant that if a system did this in v1:

```c
ecs_add(world, e, Position);
```

only `Position` would be staged. If the entity had other components, they would still only be stored in the main stage. In v2 this is no longer the case, and the entire entity is staged:

```c
// Add a component in a system, causes the entity to be staged
ecs_add(world, e, Position);

// This component is resolved from the stage
Velocity *v = ecs_get(world, e, Velocity);
```

This can cause problems if a v1 application used the pointer returned from `ecs_get_ptr` to set a component. In v2 this function (renamed to `ecs_get`) returns a `const` pointer. If an application wants to obtain a modifiable pointer, it needs to use the `ecs_get_mut` function. This function will guarantees that the returned pointer is staged.

### Merging
In v1 merging would happen after each phase. This caused merging, and in multithreaded applications, synchronizing, even when this was not required. In v2, flecs automatically determines when merging is needed.

It does this by looking at the system in & outputs. If a system writes to the stage, and a subsequent system reads from the main stage, a merge is automatically inserted, as is illustrated by this example:

```c
// System that writes Velocity to the stage
ECS_SYSTEM(world, SetV, EcsOnUpdate, [in] Position, [out] :Velocity);

// System that reads Velocity from the main stage
ECS_SYSTEM(world, Move, EcsOnUpdate, [out] Position, [in] Velocity);
```

Note that the colon (`:`) character in v1 was the dot (`.`). The way `Velocity` is passed into `SetV` is how component handles are ordinarily passed into a system, however here the handle is annotated with `[out]`. This tells Flecs that the system intends to write the component.

The subsequent system reads `Velocity` from the main stage, meaning that the staged data from the previous system needs to be merged. Flecs will automatically insert a merge between these systems.

Note that the `[in]` and `[out]` annotations are recommended, but the example would have worked without them, as Flecs assumes `[inout]` by default.

### Threading
Threading works mostly the same as in v1 as an application can still use the `ecs_set_threads` function to specify on how many threads the systems should run. However, there is one big difference that will likely not affect existing applications, but may make it easier for new applications to use the feature.

In v1 the entities were distributed across threads by looking at the total number of entities that was matched with a system, and evenly distributing these entities by splicing up the range. This meant that it was possible for one thread to completely process one table and only part of the next table, after which the next thread would take it over.

In v2 entities are distributed by splitting up the entities in each table across threads. This means that each thread will visit all the tables, and will only process the number of entities divided by the number of threads.

The new design guarantees that within a frame, an entity will only ever be processed by a single thread, which makes it much easier to reason about potential race conflicts (there will be far less).

### Reactive systems & staging
Reactive systems, triggers in v2, would enable staging, which meant that if an application added a component which triggered an OnAdd system, any changes made to entities in the OnAdd system would be staged, and a merge would occur after the OnAdd system finished.

This could cause issues with recursive OnAdd systems. If an OnAdd system added a component that triggered another OnAdd system, changes could occur that changed the component arrays while iterating. This made it difficult to write a good OnAdd system, as it was hard to predict in isolation whether the system could rely on staging or not.

In v2 this has changed. If an OnAdd system adds a component that would trigger another OnAd system, adding that component will be deferred until after the first system has finished. In practice this means that only one operation can be active at a time, which makes reasoning about the behavior much simpler.

This does however mean that the following code no longer works when ran inside an OnAdd system (trigger), whereas in v1 it did (most of the time):

```c
void OnAddSystem(ecs_iter_t *it) {
    // ...
    ecs_add(world, e, Position);
    ecs_has(world, e, Position); // Returned true in v1, false in v2
}
```

In this example the `Position` component will be added after the first add operation has finished.

## API changes
While there have been many changes to the API in v2, in general the operations remained the same, and moving to v2 should be mostly a copy+paste exercise. This section lists all the changes to the API:

- `ecs_rows_t` is now `ecs_iter_t`
- `ecs_has_owned` is now `ecs_owns`
- `ecs_is_shared` is now `!ecs_is_owned`
- Function names with underscores (`_ecs_new`) have been replaced with normal names (`ecs_new_w_type`).
- `ecs_adopt`, `ecs_orphan`, `ecs_inherit` and `ecs_disinherit` are gone. An application should now use the following:

Old                      | New
-------------------------|------
`ecs_adopt(world, e, p)` | `ecs_add_entity(world, e, ECS_CHILDOF \| p)`
`ecs_orphan(world, e, p)` | `ecs_remove_entity(world, e, ECS_CHILDOF \| p)`
`ecs_inherit(world, e, b)` | `ecs_add_entity(world, e, ECS_INSTANCEOF \| b)`
`ecs_disinherit(world, e, b)` | `ecs_remove_entity(world, e, ECS_INSTANCEOF \| b)`

- `ecs_get_id` is now `ecs_get_name`
- `EcsId` is now `EcsName`
- `ecs_lookup` will no longer return entities that have a parent / are not in the root scope, enabling proper namespacing. Use `ecs_lookup_child` or `ecs_lookup_path` to find entities that are not in the root scope.
- `EcsTypeComponent` is now `EcsType` and is a public component
- `EcsOnAdd` and `EcsOnRemove` systems are now triggers and can only be registered for a single component:

```c
ECS_TRIGGER(world, AddP, EcsOnAdd, Position);
ECS_TRIGGER(world, RemoveP, EcsOnRemove, Position);
```

- `EcsOnRemove` systems (triggers) are no longer executed at merge time, but directly after the remove occurs. As a result `OnRemove` systems should no longer be used for cleaning up resources. Instead, use component lifecycle callbacks.
- `EcsOnSet` systems now trigger when an entity has both components, and one is set:

```c
// Invoked for entities that have both Position and Velocity, when either 
// Position or Velocity is set
ECS_SYSTEM(world, SetP, EcsOnSet, Position, Velocity);
```

- Manual systems are now simply systems for which no phase is specified. These systems can still be ran with `ecs_run`, with as only difference that enabling / disabling the system no longer has an effect. This shows an example of a manual system in v2: 

```c
// A system that provides 0 for the phase will not be ran automatically
ECS_SYSTEM(world, MySystem, 0, Position, Velocity);
```

- `ecs_enable` can now be used on any entity (not just on systems), and adds `EcsDisabled`
- Prefab children are now specified with the `ECS_CHILDOF` flag like regular children. Here is an example of v1 code and what it looks like in v2:

```c
// v1 code
ECS_PREFAB(world, Parent);
    ECS_PREFAB(world, ChildPrefab);
        ecs_set(world, ChildPrefab, EcsPrefab, {.parent = Parent});

// v2 code
ECS_PREFAB(world, Parent);
    ECS_PREFAB(world, ChildPrefab, CHILDOF | Parent);
```

- Prefab children are no longer instantiated. Instead, their contents are copied over to children of the instance. In practice this means that the instance children will have exactly the same type as the prefab child. The following example shows the differences:

```c
ECS_PREFAB(world, Parent);
    ECS_PREFAB(world, ChildPrefab, Position, CHILDOF | Parent);

// Instantiate Parent
ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Parent);

// In v1 'e' would have a child with type [ECS_INSTANCEOF | ChildPrefab]. In 
// v2 'e' the child has type [Position, CHILDOF | e].

// To achieve the same behavior as in v1, use the following definition:
ECS_PREFAB(world, Parent);
    ECS_PREFAB(world, Base, Position);
    ECS_PREFAB(world, ChildPrefab, INSTANCEOF | Base, CHILDOF | Parent);

// When this prefab is instantiated, the child will have type
// [INSTANCEOF | Base, CHILDOF | e]
```

- `ecs_set_w_data` is removed, as it caused more trouble than what it was worth. Instead use `ecs_bulk_new_w_type` (where possible).
- `ecs_get_ptr` is now `ecs_get` and returns a const pointer. If a mutable pointer is required, the application should use `ecs_get_mut`.
- The OR operator in system signatures has changed from `|` to `||`
- The source operator in system signatures has changed from `.` to `:`
- The `CONTAINER` keyword has changed to `PARENT`
- Cyclic `INSTANCEOF` relationships are no longer supported
- Task OnRemove systems (systems that are not matched with any entities) are no longer supported.
- Modules no longer take a `flags` argument, but now only accept a world.
- Module components are no longer stored on the singleton entity but on their own entity, which also acts as the component handle:

```c
MyModule *m_ptr = ecs_get(world, ecs_entity(MyModule), MyModule);
```
