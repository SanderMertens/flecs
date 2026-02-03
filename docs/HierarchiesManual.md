# Hierarchies

## Introduction
Entities can be arranged in parent/child hierarchies using the built-in hierarchy implementation. Hierarchies are useful for:

- Organizing entities into scenes
- Organizing components/systems/observers/prefabs into modules
- Representing asset hierarchies (see also the [Prefabs manual](PrefabsManual.md))
- Representing UI widgets
- Representing hierarchically organized data (like a file system)

The following example shows how to create a simple hierarchy:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t spaceship = ecs_new(world);
ecs_entity_t cockpit = ecs_new_w_pair(world, EcsChildOf, spaceship);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity spaceship = world.entity();
flecs::entity cockpit = world.entity().child_of(spaceship);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity spaceship = world.Entity();
Entity cockpit = world.Entity()
    .ChildOf(spaceship);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let spaceship = world.entity();
let cockpit = world.entity().child_of_id(spaceship);
```
</li>
</ul>
</div>

The following sections cover the different features of Flecs hierarchies.

## Get parent and children
Applications can get the parent and children of an entity using the following APIs:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Get parent for entity
ecs_entity_t parent = ecs_get_parent(world, entity);

// Iterate children for entity
ecs_iter_t it = ecs_children(world, parent);
while (ecs_children_next(&it)) {
    for (int i = 0; i < it.count; i ++) {
        ecs_entity_t child = it.entities[i];
        // ...
    }
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Get parent for entity
flecs::entity parent = entity.parent();

// Iterate children for entity
parent.children([](flecs::entity child) {
    // ...
});
```

</li>
<li><b class="tab-title">C#</b>

```cs
// TODO
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// TODO
```
</li>
</ul>
</div>

## Recursive cleanup
When a parent is deleted, all of its children will also be deleted in depth-last order (children will be deleted before their parents). An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t spaceship = ecs_new(world);
ecs_entity_t cockpit = ecs_new_w_pair(world, EcsChildOf, spaceship);
ecs_entity_t pilot = ecs_new_w_pair(world, EcsChildOf, cockpit);

ecs_delete(world, spaceship);
ecs_is_alive(world, spaceship); // false
ecs_is_alive(world, cockpit); // false
ecs_is_alive(world, pilot); // false
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity spaceship = world.entity();
flecs::entity cockpit = world.entity().child_of(spaceship);
flecs::entity pilot = world.entity().child_of(cockpit);

spaceship.destruct();
spaceship.is_alive(); // false
cockpit.is_alive(); // false
pilot.is_alive(); // false
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity spaceship = world.Entity();
Entity cockpit = world.Entity()
    .ChildOf(spaceship);
Entity pilot = world.Entity()
    .ChildOf(cockpit);

spaceship.Destruct();
spaceship.IsAlive(); // false
cockpit.IsAlive(); // false
pilot.IsAlive(); // false
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let spaceship = world.entity();
let cockpit = world.entity().child_of_id(spaceship);
let pilot = world.entity().child_of_id(cockpit);

spaceship.destruct();
spaceship.is_alive(); // false
cockpit.is_alive(); // false
pilot.is_alive(); // false
```
</li>
</ul>
</div>

## Depth-first traversal
Hierarchies can be traversed depth-first by iterating the children of a parent recursively. For an example, see:

- C: https://github.com/SanderMertens/flecs/blob/master/examples/c/entities/hierarchy/src/main.c
- C++: https://github.com/SanderMertens/flecs/blob/master/examples/cpp/entities/hierarchy/src/main.cpp

## Breadth-first traversal
Hierarchies can be traversed breadth-first by using the `cascade` feature of [queries](https://www.flecs.dev/flecs/md_docs_2Queries.html#relationship-traversal). For an example, see:

- C: https://github.com/SanderMertens/flecs/blob/master/examples/c/queries/hierarchies/src/main.c
- C++: https://github.com/SanderMertens/flecs/blob/master/examples/cpp/queries/hierarchies/src/main.cpp

## Namespacing
Entities are named, which allows them to be looked up in the world by name. Names are namespaced according to the entity hierarchy. For example, an entity named "Cockpit" with a parent called "SpaceShip" will have to be looked up as "SpaceShip.Cockpit" (C) or "SpaceShip::Cockpit" (C++).

For more details, see the [names section](https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#names) in the Entities and Components manual.

## OrderedChildren
The `OrderedChildren` trait can be added to entities to indicate that creation order or a custom order should be preserved. 

When this trait is added to a parent, the entity ids returned by the `ecs_children` / `entity::children` operations will be in creation or custom order. Children of a parent with the `OrderedChildren` trait are guaranteed to be returned in a single result.

The trait does not affect the order in which entities are returned by queries.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t parent = ecs_new(world);
ecs_add_id(world, parent, EcsOrderedChildren);

ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent);
ecs_entity_t child_2 = ecs_new_w_pair(world, EcsChildOf, parent);
ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent);

// Adding/removing components usually changes the order in which children are
// iterated, but with the OrderedChildren trait order is preserved.
ecs_set(world, child_2, Position, {10, 20});

ecs_iter_t it = ecs_children(world, parent);
while (ecs_children_next(&it)) {
    // it.table will be set to NULL when iterating ordered children.
    for (int i = 0; i < it.count; i ++) {
        ecs_entity_t e = it.entities[i];
        // i == 0: child_1
        // i == 1: child_2
        // i == 2: child_3
    }
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity parent = world.entity().add(flecs::OrderedChildren);

flecs::entity child_1 = world.entity().child_of(parent);
flecs::entity child_2 = world.entity().child_of(parent);
flecs::entity child_3 = world.entity().child_of(parent);

// Adding/removing components usually changes the order in which children are
// iterated, but with the OrderedChildren trait order is preserved.
child_2.set(Position{10, 20});

parent.children([](flecs::entity child) {
    // 1st result: child_1
    // 2nd result: child_2
    // 3rd result: child_3
});
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity parent = world.Entity().Add(Ecs.OrderedChildren);

Entity child_1 = world.Entity().ChildOf(parent);
Entity child_2 = world.Entity().ChildOf(parent);
Entity child_3 = world.Entity().ChildOf(parent);

// Adding/removing components usually changes the order in which children are
// iterated, but with the OrderedChildren trait order is preserved.
child_2.Set<Position>(new(10, 20));

parent.Children((Entity child) => {
    // 1st result: child_1
    // 2nd result: child_2
    // 3rd result: child_3
});
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let parent = world.entity().add_trait::<flecs::OrderedChildren>();

let child_1 = world.entity().child_of_id(parent);
let child_2 = world.entity().child_of_id(parent);
let child_3 = world.entity().child_of_id(parent);

// Adding/removing components usually changes the order in which children are
// iterated, but with the OrderedChildren trait order is preserved.
child_2.set(Position{10, 20});

parent.each_child(|child| {
    // 1st result: child_1
    // 2nd result: child_2
    // 3rd result: child_3
});
```

</li>
</ul>
</div>

The stored order can be modified by an application with the `ecs_set_child_order` / `entity::set_child_order` operation.

## Hierarchy storage
Flecs implements two hierarchy storages that are optimized for different use cases: `ChildOf` hierarchies and `Parent` hierarchies. The following sections go over the differences between the two storages, and when to use which one.

### Overview
`ChildOf` hierarchies are optimized for large, "unstructured" hierarchies. An example of a large unstructured hierarchy is a scene. A scene root can have many (think thousands of) children that are dynamically created and deleted while the game is running. Which entities exist at any point in time is often not known beforehand, which is why this is an unstructured hierarchy.

`Parent` hierarchies are optimized for small structured hierarchies. An example of a small structured hierarchy is a [prefab](PrefabsManual.md). A prefab typically has a small number (think dozens) of children that are created when a prefab is instantiated, and deleted when an instance is deleted. At any point in the game it is usually known which entities exist.

Use `ChildOf` hierarchies when:

- A parent has many children
- The number and/or kind of children is not known beforehand

Use `Parent` hierarchies when:

- A parent has a small number (dozens) of children
- The number and kind of children is known beforehand
- The hierarchy is deeply nested

`ChildOf` hierarchies are created by adding a `(ChildOf, parent)` relationship pair to an entity. `Parent` hierarchies are created by setting a `Parent` component on an entity. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t spaceship = ecs_new(world);

// Create child with ChildOf storage
ecs_entity_t cockpit = ecs_new_w_pair(world, EcsChildOf, spaceship);

// Create child with Parent storage
ecs_entity_t cockpit = ecs_new_w_parent(world, spaceship, NULL /* optional name */);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity spaceship = world.entity();

// Create child with ChildOf storage
flecs::entity cockpit = world.entity(spaceship, nullptr /* optional name */);

// Create child with Parent storage
flecs::entity cockpit = world.entity(flecs::Parent{spaceship}, nullptr /* optional name */);
```

</li>
<li><b class="tab-title">C#</b>

```cs
// TODO
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// TODO
```
</li>
</ul>
</div>

A single parent can contain children that use both `ChildOf` and `Parent` hierarchies. A single entity cannot both have a `ChildOf` pair and a `Parent` component. For example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t spaceship = ecs_new(world);

// OK: single parent with children using mixed storages
ecs_entity_t cockpit = ecs_new_w_pair(world, EcsChildOf, spaceship);
ecs_entity_t engine = ecs_new_w_parent(world, spaceship, NULL);

// Not OK: single entity with both storages
ecs_entity_t engineering = ecs_new_w_parent(world, spaceship, NULL);
ecs_add_pair(world, engineering, EcsChildOf, spoaceship);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity spaceship = world.entity();

// OK: single parent with children using mixed storages
flecs::entity cockpit = world.entity(spaceship, nullptr);
flecs::entity engine = world.entity(flecs::Parent{spaceship}, nullptr);

// Not OK: single entity with both storages
flecs::entity engineering = world.entity(flecs::Parent{spaceship}, nullptr);
engineering.child_of(spaceship); // will remove Parent component
```

</li>
<li><b class="tab-title">C#</b>

```cs
// TODO
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// TODO
```
</li>
</ul>
</div>

The behavior of `ChildOf` hierarchies and `Parent` hierarchies is mostly the same:

- Children are recursively deleted when their parent is deleted.
- Queries with `ChildOf` terms work with both storages.
- Relationship traversal works for both storages (`Position(up)` or `Position(cascade)`).
- Name lookups work with both storages (`parent.lookup("child_name")`).
- `ecs_get_parent` / `e.parent()` can be used to obtain a parent with both storages.
- `ecs_children` / `e.children()` can be used to iterate children with both storages.
- [Prefab instantiation](https://www.flecs.dev/flecs/md_docs_2PrefabsManual.html#prefab-hierarchies) works with both storages.
- [JSON serialization](https://www.flecs.dev/flecs/md_docs_2FlecsRemoteApi.html#json-serialization) works for both storages.

There are some differences between `ChildOf` and `Parent` hierarchies:

- When using `Parent` hierarchies, the parent will not show up in the entity's type (`ecs_get_type()` / `e.type()`)
- `Parent` hierarchies are built on top of the `OrderedChildren` feature, which means that children are stored in well defined order
- When instantiating a prefab with a `Parent` hierarchy, instance children will inherit from the prefab children. Instantiating a prefab with a `ChildOf` hierarchy copies the prefab child components to the instance child.
- When instantiating a prefab with a `Parent` hierarchy, the names of the prefab children are not copied over to the instance children.
- The `Or` and `Not` [query operators](https://www.flecs.dev/flecs/md_docs_2Queries.html#operator-overview) do not yet work with `Parent` hierarchies.

The following two sections describe in more detail how the different hierarchy storages are implemented.

### ChildOf storage details
`ChildOf` hierarchies are implemented as regular "fragmenting" Flecs relationships, where each unique `(ChildOf, parent)` pair is conceptually similar to a component. As a result the performance of `ChildOf` hierarchies is predictable, as almost all component operations have O(1) time complexity. Additionally, it also means that querying for components of children for a specific parent is fast, as components can be accessed as contiguous plain arrays. For example:

```cpp
ecs_query_t *q = ecs_query(world, {
    .expr = "(ChildOf, parent), Position"
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
    Position *p = ecs_field(&it, Position, 0);
    for (int i = 0; i < it.count; i ++) {
        p[i].x ++; // Position can be accessed efficiently as array
    }
}
```

These benefits are most noticeable if a parent has many children, or when an application only needs to access children of a specific parent. For example, an application may load multiple scenes into memory, where only one scene is rendered and actively updated. With `ChildOf` hierarchies, children for different parents are stored separately from each other, which means that filtering out entities for a different scene can be a near zero cost operation.

When an application has both many different parents _and_ a need to access children of different parents simultaneously however, `ChildOf` hierarchies can become expensive. There are several factors contributing to that cost:

- Children for different parents will be stored in different tables (archetypes), which fragments memory access. The performance difference between a system that iterates over 1 vs. 1000 tables can be over an order of magnitude.
- Each table consumes memory, which scales linearly with the number of components in the table.
- Cached queries store matching tables. If an application has many tables, the memory footprint of queries will grow as well.
- Dynamic hierarchies cause table creation & deletion which is expensive relative to the cost of creating & deleting entities. In addition, tables may have to get matched/unmatched with queries, which further increases the cost of dynamic hierarchies.
- Relationship traversal (a query feature) under certain conditions requires rematching, which rebuilds the cache. If a query matches many tables, rematching can be costly and responsible for frame time spikes.

These downsides are most noticeable when an application has many parents where each parent only has a small number of children. In the degenerate case, an application ends up with a single child per table, which can happen if each child of a parent has a different set of components.

When deciding whether `ChildOf` hierarchies are a good fit, consider:

- Is the application only accessing children for a specific parent?
- Is the number of parents reasonable (not higher than thousands)?
- Is the memory overhead acceptable (use the statistics API or explorer dashboard to see detailed memory statistics)

When the answer to one or more is "no", consider using `Parent` hierarchies.

### Parent storage details
`Parent` hierarchies are enabled by setting a `Parent` component with as value the parent. Unlike `ChildOf` hierarchies, children for multiple parents can be stored in the same table when using `Parent` hierarchies.. Children in a `Parent` hierarchy are stored in a `vector<ecs_entity_t>` on the component index record for `(ChildOf, parent)`. This is the same vector that is used for the `OrderedChildren` storage.

When a `Parent` component is set, a `(ParentDepth, depth)` pair is automatically added to the child. When inspecting the type of a child, this will show up as:

```
Parent, Position, Velocity, (ParentDepth, @3)
```

This means that the child is stored 3 levels deep in the hierarchy. Hierarchy depth is used by queries to efficiently implement the `cascade`/`group_by` features for `Parent` hierarchies. The `@` indicates that this is a value pair, as opposed to 3 specifying an entity id. User defined queries can also take advantage of `ParentDepth`. For example, the following query iterates entities breadth-first:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
    .terms = {
        { ecs_id(Position) }
    },
    .group_by = EcsParentDepth
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto q = world.query_builder<Position>()
    .group_by(flecs::ParentDepth)
    .build();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// TODO
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// TODO
```
</li>
</ul>
</div>

#### Advantages vs. ChildOf
The main benefit of `Parent` hierarchies is that they do not significantly change the memory layout of components in the ECS storage. A query that does not interact with the hierarchy will perform the same whether entities are organized in a hierarchy or not. This can be a significant benefit over `ChildOf` hierarchies, especially if the application contains many small (prefab) hierarchies. Switching to `Parent` hierarchies can improve performance and reduce memory footprint by over an order of magnitude.

In addition to not fragmenting the storage, `Parent` hierarchies have additional performance benefits when used in combination with prefabs. This is because:

- Prefab hierarchy instantiation does not cause table creation when using `Parent` hierarchies.
- Because no new tables are created, no tables have to be matched with queries. The more queries an application has, the larger the performance improvement will be.
- Prefabs that use `Parent` hierarchies build a "TreeSpawner", which caches the structure and tables of the prefab hierarchy. This is not possible for `ChildOf` hierarchies, because tables are not the same across instances.
- Instance children can inherit components from prefab children, which reduces memory footprint and copy overhead.

#### Disadvantages vs. ChildOf
In most cases `Parent` hierarchies will outperform `ChildOf` hierarchies, but there are a few exceptions:

- Children in a `Parent` hierarchy are stored in a vector, which means that deleting a child is an O(n) operation. If a parent has many (think thousands) children,this can be an expensive operation. If this is the case, consider using a `ChildOf` hierarchy.
- Queries with multiple terms where one is a `ChildOf` term may perform worse. This is largely due to children of multiple parents being stored in the same table, which means a query needs to do more work for `ChildOf` terms to filter out matching entities.

The O(n) cost of deleting children does not apply when deleting the parent. In that case children are deleted in bulk, which does not require scanning the children vector.

#### Query performance
How a query will perform with the new hierarchy storage highly depends on the kind of query. This section provides an overview of the different kinds of queries, and how their performance compares to the `ChildOf` hierarchy.

---
```
Position, Velocity
```
Queries such as these that do not interact with the hierarchy will almost always perform better with `Parent` hierarchies. `ChildOf` hierarchies result in more tables, which decreases query performance. When using `Parent` hierarchies it is much more likely that components are densely packed in memory.

---
```
(ChildOf, my_parent)
```
Queries in this form will typically perform the same or better for `Parent` hierarchies. The reason for this is that the query returns the children vector of a parent directly. This means that it never has to iterate multiple tables, which is something that can occur for `ChildOf` hierarchies.

---
```
(ChildOf, my_parent), Position
```
This query will perform _worse_ for `Parent` hierarchies. The reason is that this query cannot be evaluated at the table level. Instead the query has to individually check whether each child of `my_parent` has `Position`.

---
```
(ChildOf, *), Position
```
This query will perform _worse_ for `Parent` hierarchies. The reason for this is that this query will first find all tables with either a `ChildOf` pair or a `Parent` component, and then check if the resulting table has `Position`. For tables with the `Parent` component, the query then has to return each individual entity, because it needs to return the actual pair matched by the wildcard. This is to support code that uses `ecs_field_id`, `it.id()` or `it.pair()`:

```cpp
auto q = world.query_builder<Position>()
    .with(flecs::ChildOf, flecs::Wildcard)
    .each([](flecs::iter& it, size_t, Position&) {
        flecs::entity parent = it.pair().second() // value is set by query
    });
```

Both storages will perform better if the terms are reversed. The first term is likely to match significantly more tables than the second term, which increases the cost of uncached queries, and the cost of populating a query cache.

---
```
Position, (ChildOf, my_parent)
```
This query will perform _worse_ for `Parent` hierarchies. This query will first find all tables with `Position`, and then has to check for each entity in that table whether it is a child of `my_parent`. If the table contains many entities, this can be a very expensive operation.

If the table only contains a single child for `my_parent`, the query uses a faster path that does not require scanning each entity, but the query will still perform worse than with `ChildOf` hierarchies.

---
```
Position, (ChildOf, *)
```
This query will perform _worse_ for `Parent` hierarchies. This query will first find all tables with `Position`, and then check if the table has the `Parent` component. If so, the query now knows that each entity in the table matches the query. However, it has to return entities one by one because the matched parent needs to be communicated to the application (see above).

For a more efficient way to do this with `Parent` hierarchies, see the next query:

---
```
Position, (ChildOf, _)
```
This query will perform the same for `Parent` hierarchies. This query will first find all tables with `Position`, and then check if the table has the `Parent` component. If so, the query now knows that each entity in the table matches the query. Because the any (`_`) wildcard does not require communicating the parent to the application, the entire table can be returned.

To also find the entities' parent, an application can add the `flecs::Parent` component to the query:

```cpp
auto q = world.query_builder<Position, const flecs::Parent>()
    .with(flecs::ChildOf, flecs::Wildcard)
    .each([](flecs::iter& it, size_t, Position&, const flecs::Parent& p) {
        flecs::entity parent = it.world().entity(p.value);
    });
```

---
```
Position, Position(up)
```
This query will perform _worse_ for `Parent` queries. This query will first find all tables with `Position`. If it finds a table with a `Parent` component (as opposed to one with a `ChildOf` pair) the query will have to iterate each entity, and traverse the hierarchy upwards for that entity. The result of this cannot be cached, which further hurts performance.

The reason up traversal is supported for `Parent` hierarchies is mostly to provide an easier migration path from `ChildOf` hierarchies. Performance critical queries should generally not use up traversal in combination with `Parent` hierarchies. See below on how to migrate to alternative queries.

---
```
Position(up), Position
```
This query will perform _worse_ for `Parent` queries. Though not as bad as the previous query, this query will also perform worse with `Parent` hierarchies than for `ChildOf` hierarchies, because the results that use the `Parent` hierarchy cannot be cached.

#### Optimizing Relationship Traversal
As outlined in the previous section, queries that use relationship traversal can be slower for entities with `Parent` hierarchies. To get around this, an application can split up a query that uses relationship traversal into two queries, one for `ChildOf` hierarchies, and one for `Parent` hierarchies. Consider the following query:

```cpp
Position, Position(up)
```

This can be split up into two queries like so:

```cpp
// Query for ChildOf hierarchies: uses relationship traversal
Position, Position(up), !Parent

// Query for Parent hierarchies: does not use relationship traversal
Position, Parent
```

Then in the iteration logic, manually fetch the `Position` component from the parent. For example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
    .terms = {
        { ecs_id(Position) },
        { ecs_id(EcsParent) }
    },
    .cache_kind = EcsQueryCacheAuto
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
    Position *p = ecs_field(&it, Position, 0);
    EcsParent *parents = ecs_field(&it, EcsParent, 1);

    for (int i = 0; i < it.count; i ++) {
        const Position *p_parent = ecs_get(world, parents[i].value, Position);
        // logic as usual
    }
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto q = world.query<Position, const flecs::Parent>();

q.each([](Position& p, const flecs::Parent& parent) {
    const Position& p_parent = parent.get<Position>();
    // logic as usual
});
```

</li>
<li><b class="tab-title">C#</b>

```cs
// TODO
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// TODO
```
</li>
</ul>
</div>

For queries that use `cascade`, applications can use `group_by(flecs::ParentDepth)` instead, which will iterate entities ordered by hierarchy depth. The following code shows how to do breadth-first traversal with a query per storage kind. Note how the implementation iterates each depth individually; this prevents deeper `ChildOf` levels from being processed before higher-level `Parent` entities.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q_childof = ecs_query(world, {
    .terms = {
        { ecs_id(Position) },
        { ecs_id(Position), .src.id = EcsCascade },
        { ecs_id(EcsParent), .oper = EcsNot }
    },
    .cache_kind = EcsQueryCacheAuto
});

ecs_query_t *q_parent = ecs_query(world, {
    .terms = {
        { ecs_id(Position) },
        { ecs_id(EcsParent) }
    },
    .group_by = EcsParentDepth,
    .cache_kind = EcsQueryCacheAuto
});

// Replace 16 with max hierarchy depth in application
for (int depth = 0; depth < 16; depth ++) {
    // Iterate ChildOf hierarchy at current depth
    {
        ecs_iter_t it = ecs_query_iter(world, q_childof);
        ecs_iter_set_group(&it, depth);
        while (ecs_query_next(&it)) {
            Position *p = ecs_field(&it, Position, 0);
            Position *p_parent = ecs_field(&it, Position, 1);
            for (int i = 0; i < it.count; i ++) {
                p[i].x += p_parent->x;
                // ...
            }
        }
    }

    // Iterate Parent hierarchy at current depth
    {
        ecs_iter_t it = ecs_query_iter(world, q_parent);
        ecs_iter_set_group(&it, depth);
        while (ecs_query_next(&it)) {
            Position *p = ecs_field(&it, Position, 0);
            EcsParent *parents = ecs_field(&it, EcsParent, 1);
            for (int i = 0; i < it.count; i ++) {
                const Position *p_parent = ecs_get(
                    world, parents[i].value, Position);
                p[i].x += p_parent->x;
                // ...
            }
        }
    }
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto q_childof = world.query_builder<Position, const Position>()
    .term_at(1).cascade()
    .without<flecs::Parent>()
    .build();

auto q_parent = world.query_builder<Position, const flecs::Parent>()
    .group_by(flecs::ParentDepth)
    .build();

// Replace 16 with max hierarchy depth in application
for (int depth = 0; depth < 16; depth ++) {
    q_childof.set_group(depth)
        .each([](Position& p, const Position& p_parent) {
            p.x += p_parent.x;
        });

    q_parent.set_group(depth)
        .each([](Position& p, const flecs::Parent& parent) {
            const Position& p_parent = parent.get<Position>();
            p.x += p_parent.x;
        });
}
```

</li>
<li><b class="tab-title">C#</b>

```cs
// TODO
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// TODO
```
</li>
</ul>
</div>
