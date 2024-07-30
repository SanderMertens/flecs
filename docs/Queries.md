# Queries

## Introduction
Queries enable games to quickly find entities that match a list of conditions, and are at the core of many Flecs features like systems, observers, tooling and serialization.

Flecs queries can do anything from returning entities that match a simple list of components, to matching complex patterns against entity graphs.

This manual contains a full overview of the query features available in Flecs. Some of the features of Flecs queries are:

- Queries can be cached, uncached or a mix of both, which lets games pick the ideal balance between iteration performance, query creation performance and administration overhead.

- Queries have advanced features for matching patterns against entity graphs which can be used to build immersive game play without having to build and maintain complex custom data structures ([blog](https://ajmmertens.medium.com/building-games-in-ecs-with-entity-relationships-657275ba2c6c)).

- Queries support `and`, `or`, `not` and `optional` [operators](#operator-overview).

- Queries can combine components from multiple [sources](#source), like matching entities with a `Transform` component and a parent that have a `Transform` component.

- Queries can be created at runtime and match components that are created at runtime.

- Queries can be created with the Flecs APIs or by using the Flecs Query Language, which enables query creation in [tools like the explorer](https://www.flecs.dev/explorer/).

- The Flecs REST API has a query endpoint which can be used to build remote game servers.

## Definitions

| Name         | Description |
|--------------|-------------|
| Id           | An id that can be matched, added and removed |
| Component    | Id with a single element (same as an entity id) |
| Pair         | Id with two elements |
| Tag          | Component or pair not associated with data |
| Relationship | Used to refer to first element of pair |
| Target       | Used to refer to second element of pair |
| Source       | Entity on which a term is matched |
| Iterator     | Object used to iterate a query |
| Term         | An element of a query that expresses a condition to match |
| Field        | A single value or array of values made available by an iterator. An iterator usually provides a field per query term. |

## Examples
Make sure to check out the query code examples in the repository:

 - [queries (C)](https://github.com/SanderMertens/flecs/tree/master/examples/c/queries)
 - [queries (C++)](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/queries)
 - [queries (Rust)](https://github.com/Indra-db/Flecs-Rust/tree/main/flecs_ecs/examples/flecs/queries)

## Performance and Caching
Understanding the basic architecture of queries helps to make the right tradeoffs when using queries in games. The biggest impact on query performance is whether a query is cached or not. This section goes over what caching is, how it can be used and when it makes sense to use it.

### Caching: what is it?
Flecs is an archetype ECS, which means that entities with exactly the same components are grouped together in an "archetype" (also called a "table"). Archetypes are created on the fly whenever a new component _combination_ is created in the ECS. For example:

```cpp
flecs::entity e1 = world.entity();
e1.set(Position{10, 20}); // create archetype [Position]
e1.set(Velocity{1, 2});   // create archetype [Position, Velocity]

flecs::entity e2 = world.entity();
e2.set(Position{10, 20}); // archetype [Position] already exists
e2.set(Velocity{1, 2});   // archetype [Position, Velocity] already exists
e2.set(Mass{100});        // create archetype [Position, Velocity, Mass]

// e1 is now in archetype [Position, Velocity]
// e2 is now in archetype [Position, Velocity, Mass]
```

Archetypes are important for queries. Since all entities in an archetype have the same components, and a query matches entities with specific components, a query can often match entire archetypes instead of individual entities. This is one of the main reasons why queries in an archetype ECS are fast.

The second reason that queries in an archetype ECS are fast is that they are cheap to cache. While an archetype is created for each unique component combination, games typically only use a finite set of component combinations which are created quickly after game assets are loaded.

This means that instead of searching for archetypes each time a query is evaluated, a query can instead cache the list of matching archetypes. This is a cheap cache to maintain: even though entities can move in and out of archetypes, the archetypes themselves are often stable.

If none of that made sense, the main thing to remember is that a cached query does not actually have to search for entities. Iterating a cached query just means iterating a list of prematched results, and this is really, really fast.

### Tradeoffs
Flecs has both cached and uncached queries. If cached queries are so fast, why even bother with uncached queries? There are four main reasons:

- Cached queries are really fast to iterate, but take more time to create because the cache must be initialized first.

- Cached queries have a higher RAM utilization, whereas uncached queries have very little overhead and are stateless.

- Cached queries add overhead to archetype creation/deletion, as these changes have to get propagated to caches.

- While caching archetypes is fast, some query features require matching individual entities, which are not efficient to cache (and aren't cached).

As a rule of thumb, if you have a query that is evaluated each frame (as is typically the case with systems), they will benefit from being cached. If you need to create a query ad-hoc, an uncached query makes more sense.

Ad-hoc queries are often necessary when a game needs to find entities that match a condition that is only known at runtime, for example to find all child entities for a specific parent.

### Cache kinds
Queries can be created with a "cache kind", which specifies the caching behavior for a query. Flecs has four different caching kinds:

| Kind    | C | C++ | Description |
|---------|---|-----|-------------|
| Default | `EcsQueryCacheDefault` | `flecs::QueryCacheDefault` | Behavior determined by query creation context |
| Auto    | `EcsQueryCacheAuto`    | `flecs::QueryCacheAuto`    | Cache query terms that are cacheable |
| All     | `EcsQueryCacheAll`     | `flecs::QueryCacheAll`     | Require that all query terms are cached |
| None    | `EcsQueryCacheNone`    | `flecs::QueryCacheNone`    | No caching |

The following sections describe each of the kinds.

#### Default
When no cache kind is specified, queries will be created with the Default caching kind. A query with the Default kind will be created as cached (using the Auto kind, see next section) if a query is associated with an entity, and uncached if it isn't.

What does it mean for a query to be associated with an entity? When a query is created, an application can provide an entity to associate with the query. This binds the lifecycle of the query with that of the entity, and makes it possible to lookup the query (by name) in tools like the explorer.

The most common case of this is systems. Flecs systems have entity handles, which are associated with system queries. This means that all system queries by default are cached, unless specified otherwise.

The rationale behind this is that if a query is associated with an entity, it will likely be reused and outlive the scope in which the query is created. Queries that are reused many times across frames are good candidates for caching.

#### Auto
When the Auto kind is specified, all query terms that can be matched will be matched. Query features that rely on matching entire archetypes can typically be cached, whereas features that return individual entities cannot be cached. The following query features are cacheable:

- Components, Tags and Pairs
- Terms with a `$this` source (default behavior, see variables)
- Wildcards
- Operators
- Query traversal

There are scenarios that are cacheable in theory but aren't cached yet by the current implementation. Over time the query engine will be extended to cache terms with variables, as long as they match entire archetypes.

Queries with the Auto kind can mix terms that are cached and uncached.

#### All
Queries with the All kind require all terms to be cacheable. This forces a query to only use features that can be cached. If a query with kind All uses features that cannot be cached, query creation will fail.

#### None
Queries with the None kind will not use any caching.

### Performance tips & tricks

#### Rematching
Queries that use traversal (either `up` or `cascade`) can trigger query "rematching". This is a process that ensures that a query cache that matches components on an entity reached through traversal stays up to date.

A typical example of this is a query that matched a `Transform` component on a parent entity. If the `Transform` component is removed from the parent it invalidates the cache, and rematching will happen.

Rematching can be an expensive process, especially in games with lots of archetypes. To learn if an application is slowed down by rematching, connect the explorer to it with the `flecs::stats` module imported (see the REST API manual), and inspect the world statistics page.

If rematching is taking up a significant amount of time, consider changing cached queries with traversal to uncached. This will increase query evaluation time, but should get rid of the query rematching cost.

Rematching is a temporary solution to a complex problem that will eventually be solved with a much cheaper mechanism. For now however, rematching is something that needs to be monitored for queries that use query traversal features.

#### Empty archetype optimization
Cached queries have an optimization where they store empty archetypes in a separate list from non-empty archetypes. This generally improves query iteration speed, as games can have large numbers of empty archetypes that could waste time when iterated by queries.

However, to keep empty archetypes and non-empty archetypes in separate lists, events have to be emitted from archetypes to queries whenever their state changes. When emitting these events becomes too expensive, games can opt out of empty archetype optimization, and instead periodically cleanup empty archetypes.

To do this, a query should be created with the `EcsQueryMatchEmptyTables` flag, and the `ecs_delete_empty_tables` function should be called periodically. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create Position, Velocity query that matches empty archetypes.
ecs_query(world, {
    .terms = { { ecs_id(Position) }, { ecs_id(Velocity) } },
    .cache_kind = EcsQueryCacheAuto,
    .flags = EcsQueryMatchEmptyTables
});

// Delete empty archetypes that have been empty for 10 calls to this function.
ecs_delete_empty_tables(world, 0, 0, 10, 0, 0);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Create Position, Velocity query that matches empty archetypes.
flecs::query<Position, Velocity> q = world.query_builder<Position, Velocity>()
   .cached()
   .query_flags(EcsQueryMatchEmptyTables)
   .build();

// Delete empty archetypes that have been empty for 10 calls to this function.
ecs_delete_empty_tables(world, 0, 0, 10, 0, 0);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Create Position, Velocity query that matches empty archetypes.
let q = world
    .query::<(&mut Position, &Velocity)>()
    .set_cached()
    .query_flags(QueryFlags::MatchEmptyTables)
    .build();

// Delete empty archetypes that have been empty for 10 calls to this function.
world.delete_empty_tables(0, 0, 10, 0, 0.0);
```

</li>
</ul>
</div>

This will cause queries to return empty archetypes (iterators with count set to 0) which is something the application code will have to handle correctly.

## Creating queries
This section explains how to create queries in the different language bindings and the flecs Flecs Query Language.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

Query descriptors are the C API for creating queries. An example:

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }, { ecs_id(Velocity) },
  }
});
```

The example shows the short notation, which looks like this when expanded:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
  .terms = {
    { ecs_id(Position) }, { ecs_id(Velocity) },
  }
});
```

Note how component types are added with the `ecs_id()` macro. This translates the component type to the component id that queries require. Tags and pairs do not require the `ecs_id()` macro:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
  .terms = {
    { Npc }, { ecs_pair(Likes, Bob) },
  }
});
```

Query descriptors can also be used by the C++ API. However because C++ does not support taking the address of a temporary, and not all language revisions support designated initializers, query descriptors in C++ should be used like this:

```c
ecs_query_desc_t desc = {}; // Zero-initialize the struct
desc.terms[0].id = ecs_id(Position);
desc.terms[1].id = ecs_id(Velocity);
ecs_query_t *q = ecs_query_init(world, &desc);
```

Queries have to be deleted with the `ecs_query_fini` function, except when a query is associated with an entity. An example:

```c
ecs_entity_t query_entity = ecs_new(world);
ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
  .entity = query_entity,
  .terms = {
    { ecs_id(Position) }, { ecs_id(Velocity) },
  }
});

ecs_delete(world, query_entity); // Also deletes query
```

</li>
<li><b class="tab-title">C++</b>

Query builders are the C++ API for creating queries. The builder API is built on top of the descriptor API, and adds a layer of convenience and type safety that matches modern idiomatic C++. An example of a simple query:

```cpp
flecs::query<Position, const Velocity> q = 
    world.query<Position, const Velocity>();
```

Queries created with template arguments provide a type safe way to iterate components:

```cpp
q.each([](Position& p, const Velocity& v) {
    p.x += v.x;
    p.y += v.y;
});
```

The builder API allows for incrementally constructing queries:

```cpp
flecs::query<Position> q = world.query_builder<Position>();
q.with<const Velocity>();

if (add_npc) {
    q.with<Npc>(); // Conditionally add
}

q.build(); // Create query
```

</li>
<li><b class="tab-title">Rust</b>

The query builder API is built on top of the term builder API, and adds a layer of convenience and type safety that matches modern idiomatic Rust. An example of a simple query:

```rust
// new_query is a convenience function that creates a query with the default builder
let q = world.new_query::<(&mut Position, &Velocity)>();
```

Queries created with generic arguments provide a type safe way to iterate components:

```rust
q.each(|(p, v)| {
    p.x += v.x;
    p.y += v.y;
});
```

The builder API allows for incrementally constructing queries, but also gives you access to more advanced features (see later sections):

```rust
let mut q = world.query::<(&mut Position, &Velocity)>();
q.with::<&Velocity>();

if add_npc {
    q.with::<&Foo>(); // Conditionally add
}

q.build(); // Create query
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

The Flecs Query Language is a string format that can be parsed into a query. The format is used by convenience macros in the C API like `ECS_SYSTEM` and `ECS_OBSERVER`, and makes it easier to create queries at runtime for tools like https://www.flecs.dev/explorer/. An example of a simple query in the DSL:

```
Position, [in] Velocity
```

An example of how the DSL is used with the `ECS_SYSTEM` convenience macro:

```c
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, [in] Velocity);
```

Queries can be created from expressions with both the descriptor and builder APIs:

```c
ecs_query_t *q = ecs_query(world, {
  .expr = "Position, [in] Velocity"
});
```
```cpp
flecs::query<> q = world.query_builder()
  .expr("Position, [in] Velocity")
  .build();
```

For more details on the syntax, see the Flecs Query Language manual.

</li>
</ul>
</div>

## Iteration
This section describes the different ways queries can be iterated.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

In the C API an iterator object of type `ecs_iter_t` can be created for each of the query kinds, using the `ecs_query_iter`, `ecs_query_iter` and `ecs_query_iter` functions. This iterator can then be iterated with the respective `next` functions: `ecs_query_next`, `ecs_query_next` and `ecs_query_next`.

An iterator can also be iterated with the `ecs_iter_next` function which is slightly slower, but does not require knowledge about the source the iterator was created for.

An example:

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }, { ecs_id(Velocity) },
  }
});

ecs_iter_t it = ecs_query_iter(world, q);

// Outer loop: matching tables
while (ecs_query_next(&it)) {
  Position *p = ecs_field(&it, Position, 0); // 1st term
  Velocity *v = ecs_field(&it, Velocity, 1); // 2nd term

  // Inner loop: entities in table
  for (int i = 0; i < it.count; i ++) {
    p[i].x += v[i].x;
    p[i].y += v[i].y;
  }
}
```

Iteration is split up into two loops: the outer loop which iterates tables, and the inner loop which iterates the entities in that table. This approach provides direct access to component arrays, which allows compilers to do performance optimizations like auto-vectorization.

The indices provided to the `ecs_field()` function must correspond with the order in which terms have been specified in the query. This index starts counting from `0`.

</li>
<li><b class="tab-title">C++</b>

C++ has two iteration functions, `each` and `run`. The `each` function is the default and often fastest approach for iterating a query in C++. An example:

```cpp
auto q = world.query<Position, const Velocity>();

q.each([](Position& p, const Velocity& v) {
    p.x += v.x;
    p.y += v.y;
});
```

A `flecs::entity` can be added as first argument:

```cpp
auto q = world.query<Position>();

q.each([](flecs::entity e, Position& p) {
    std::cout << e.name() << ": " 
              << p.x << ", " << p.y 
              << std::endl;
});
```

A `flecs::iter` and `size_t` argument can be added as first arguments. This variant of `each` provides access to the `flecs::iter` object, which contains more information about the object being iterated. The `size_t` argument contains the index of the entity being iterated, which can be used to obtain entity-specific data from the `flecs::iter` object. An example:

```cpp
auto q = world.query_builder<Position>()
  .with(Likes, flecs::Wildcard)
  .build();

q.each([](flecs::iter& it, size_t index, Position& p) {
    flecs::entity e = it.entity(index);
    std::cout << e.name() << ": " 
              << it.id(1).str() // prints pair
              << std::endl;
});
```

When a query contains a template argument that is an empty type (a struct without any members), it should be passed by value instead of by reference:

```cpp
struct Tag { };

auto q = world.query<Tag>();

q.each([](flecs::entity e, Tag) {
    std::cout << e.name() << std::endl;
});
```

Alternatively an empty type can be specified outside of the query type, which removes it from the signature of `each`:

```cpp
struct Tag { };

auto q = world.query_builder()
  .with<Tag>()
  .build();

q.each([](flecs::entity e) {
    std::cout << e.name() << std::endl;
});
```

The `run` function provides an initialized iterator to a callback, and leaves iteration up to the callback implementation. Similar to C query iteration, the run callback has an outer and an inner loop.

An example:

```cpp
auto q = world.query<Position, const Velocity>();

q.run([](flecs::iter& it) {
    // Outer loop
    while (it.next()) {
      auto p = it.field<Position>(0);
      auto v = it.field<const Velocity>(1);

      // Inner loop
      for (auto i : it) {
          p[i].x += v[i].x;
          p[i].y += v[i].y;
          std::cout << it.entity(i).name() << ": " 
              << p.x << ", " << p.y 
              << std::endl;
      }
    }
});
```

Entities can be moved between tables when components are added or removed. This can cause unwanted side effects while iterating a table, like iterating an entity twice, or missing an entity. To prevent this from happening, a table is locked by the C++ `each` and `run` functions, meaning no entities can be moved from or to it.

When an application attempts to add or remove components to an entity in a table being iterated over, this can throw a runtime assert. An example:

```cpp
auto q = world.query<Position>();

q.each([](flecs::entity e, Position&) {
    e.add<Velocity>(); // throws locked table assert
});
```

This can be addressed by deferring operations while the query is being iterated:

```cpp
auto q = world.query<Position>();

world.defer([&]{
    q.each([](flecs::entity e, Position&) {
        e.add<Velocity>(); // OK
    });
}); // operations are executed here
```

An application can also use the `defer_begin` and `defer_end` functions which achieve the same goal:

```cpp
auto q = world.query<Position>();

world.defer_begin();

q.each([](flecs::entity e, Position&) {
    e.add<Velocity>(); // OK
});

world.defer_end(); // operations are executed here
```

Code ran by a system is deferred by default.

</li>
<li><b class="tab-title">Rust</b>

Rust has two main iteration functions, `each` and `run`. The `each` function is the default and often fastest approach for iterating a query in Rust. Both `each` and `run` have some variances that offer slightly different functionality.
These variances are `each_entity`, `each_iter` and `run_each` and `run_iter`.

An example:

```rust
let q = world.new_query::<(&mut Position, &Velocity)>();
q.each(|(p, v)| {
    p.x += v.x;
    p.y += v.y;
});
```

A `EntityView` can be added as by using `each_entity`:

```rust
let q = world.new_query::<(&mut Position, &Velocity)>();
q.each_entity(|e, (p, v)| {
    println!("Entity: {}", e.name());
    p.x += v.x;
    p.y += v.y;
});
```

Using `each_iter` a `TableIter` and `usize` argument are added as first arguments. This variant of `each` provides access to the `TableIter` object, which contains more information about the object being iterated. The `usize` argument contains the index of the entity being iterated, which can be used to obtain entity-specific data from the `TableIter` object. An example:

```rust
let q = world
    .query::<&Position>()
    .with::<(&Likes, &flecs::Wildcard)>()
    .build();

q.each_iter(|it, index, p| {
    println!("Entity: {}: {}", it.entity(index).name(), it.id(1).to_str());
});
```

A query can also contain generic arguments that is an empty type (a struct without any members).

```rust
#[derive(Component)]
struct Tag;

world.new_query::<&Tag>().each_entity(|e, tag| {
    /* */
});
```

Alternatively an empty type can be specified outside of the query type, which removes it from the signature of `each`:

```rust
#[derive(Component)]
struct Tag;

world
    .query::<()>()
    .with::<&Tag>()
    .build()
    .each_entity(|e, _| { /* */ });
```
 
The `run` function provides an initialized iterator to a callback, and leaves iteration up to the callback implementation. The run callback has an outer and an inner loop.

An example:

```rust
let q = world.new_query::<(&Position, &Velocity)>();

q.run(|mut it| {
    while it.next() {
        let mut p = it.field::<Position>(0).unwrap();
        let v = it.field::<Velocity>(1).unwrap();
        for i in it.iter() {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
            println!("Entity: {}", it.entity(i).name());
        }
    }
});
```

Entities can be moved between tables when components are added or removed. This can cause unwanted side effects while iterating a table, like iterating an entity twice, or missing an entity. To prevent this from happening, a table is locked in the Rust `each` and `run` functions, meaning no entities can be moved from or to it.

When an application attempts to add or remove components to an entity in a table being iterated over in not deferred context, this can throw a runtime assert. An example:

```rust
let q = world.new_query::<&Position>();

q.each_entity(|e, p| {
    e.add::<Velocity>(); // throws locked table assert
});
```

This can be addressed by deferring operations while the query is being iterated:

```rust
let q = world.new_query::<&Position>();
    
world.defer(|| {
    q.each_entity(|e, p| {
        e.add::<Velocity>(); // OK
    });
}); // operations are executed here
```

An application can also use the `defer_begin` and `defer_end` functions which achieve the same goal:

```rust
let q = world.new_query::<&Position>();

world.defer_begin();

q.each_entity(|e, p| {
    e.add::<Velocity>(); // OK
});

world.defer_end(); // operations are executed here
```

Code ran by a system is deferred by default.

</li>
</ul>
</div>

## Reference
This section goes over the different features of queries and how they can be expressed by the query descriptor API, query builder API and in the Flecs Query Language.

### Components
A component is any single id that can be added to an entity. This includes tags and regular entities, which are ids that are not associated with a datatype.

To match a query, an entity must have all the requested components. An example:

```cpp
flecs::entity e1 = world.entity()
  .add<Position>();

flecs::entity e2 = world.entity()
  .add<Position>()
  .add<Velocity>();

flecs::entity e3 = world.entity()
  .add<Position>()
  .add<Velocity>()
  .add<Mass>();
```

Only entities `e2` and `e3` match the query `Position, Velocity`.

The following sections describe how to create queries for components in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

To query for a component in C, the `id` field of a term can be set:

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { .id = ecs_id(Position) }, 
    { .id = ecs_id(Velocity) }
  }
});
```

The `id` field is guaranteed to be the first member of a term, which allows the previous code to be rewritten in this shorter form:

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity) }
  }
});
```

The `ecs_id` macro converts the component typename into the variable name that holds the component identifier. This macro is required for components created with `ECS_COMPONENT`, but not when querying for regular tags/entities:

```c
ECS_TAG(world, Npc);
ecs_entity_t Platoon_01 = ecs_new(world);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { Npc }, 
    { Platoon_01 }
  }
});
```

Components can also be queried for by name by setting the `.first.name` member in a term:

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { .first.name = "Position" }, 
    { .first.name = "Velocity" }
  }
});
```

</li>
<li><b class="tab-title">C++</b>

An easy way to query for components in C++ is to pass them as template arguments to the query factory function:

```cpp
flecs::query<Position, const Velocity> q = 
    world.query<Position, const Velocity>();
```

This changes the returned query type, which determines the type of the function used to iterate the query:

```cpp
q.each([](Position& p, const Velocity& v) { });
```

The builder API makes it possible to add components to a query without modifying the query type:

```cpp
flecs::query<Position> q = 
  world.query_builder<Position>()
    .with<const Velocity>()
    .build();
```

When template arguments are mixed with the builder API, the components added by the `term` function will be placed after the components provided as template arguments.

The builder API makes it possible to query for regular entity ids created at runtime:

```cpp
flecs::entity Npc = world.entity();
flecs::entity Platoon_01 = world.entity();

flecs::query<> q = world.query_builder()
  .with(Npc)
  .with(Platoon_01)
  .build();
```

Components can also be queried for by name. To query for component types by name, they have to be used or registered first.

```cpp
// Register component type so we can look it up by name
world.component<Position>();

// Create entity with name so we can look it up
flecs::entity Npc = world.entity("Npc");

flecs::query<> q = world.query_builder()
  .with("Position")
  .with("Npc")
  .build();
```

</li>
<li><b class="tab-title">Rust</b>

An easy way to query for components in Rust is to pass them as generic arguments to the query `new` function:

```rust
let q = world.new_query::<(&mut Position, &Velocity)>();
```

This changes the returned query type, which determines the type of the function used to iterate the query:

```rust
q.each(|(p, v)| { /* */ });
```

The builder API makes it possible to add components to a query without modifying the query type:

```rust
let q = world.query::<&mut Position>().with::<&Velocity>().build();
```

When generic arguments are mixed with the builder API, the components added by the `term` function will be placed after the components provided as generic arguments.

The builder API makes it possible to query for regular entity ids created at runtime:

```rust
let npc = world.entity();
let platoon_01 = world.entity();

let q = world
    .query::<(&mut Position, &Velocity)>()
    .with_id(npc)
    .with_id(platoon_01)
    .build();
```

Components can also be queried for by name. To query for component types by name, they have to be used or registered first.

```rust
// Create entity with name so we can look it up
let npc = world.entity_named("npc");

let q = world
    .query::<(&Position, &Npc)>()
    .with_name("npc")
    .with_name("Position")
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

To query for a components in the Flecs Query Language they can be specified in a comma separated list of identifiers. The rules for resolving identifiers are the same as the `ecs_lookup` / `world.lookup` functions. An example:

```
Position, Velocity
```

Any named entity can be specified this way. Consider:

```cpp
struct Npc { };

// Register component type so we can look it up by name
world.component<Npc>();

// Create entity with name so we can look it up
flecs::entity Platoon_01 = world.entity("Platoon_01");

flecs::entity e = world.entity()
  .add<Npc>()
  .add(Platoon_01);
```

The entity `e` from this example will be matched by this query:
```cpp
Npc, Platoon_01
```

When an identifier in the Flecs Query Language consists purely out of numeric characters it is converted to an entity id. If in the previous example `Npc` has id `100` and `Platoon_01` has id `101`, the following query string would be equivalent:

```
100, 101
```

The `,` symbol in the Flecs Query Language is referred to as the `and` operator, as an entity must have all comma-separated components in order to match the query.

</li>
</ul>
</div>

### Wildcards
Wildcards allow a single query term to match with more than one (component) ids. Flecs supports two kinds of wildcards:

| Name  | DSL Symbol | C identifier  | C++ identifier | Description |
|----------|-----|---------------|----------------|-------------|
| Wildcard | `*` | `EcsWildcard` | `flecs::Wildcard` | Match all |
| Any      | `_` | `EcsAny` | `flecs::Any` | Match at most one |

The `Wildcard` wildcard returns an individual result for anything that it matches. The query in the following example will return twice for entity `e`, once for component `Position` and once for component `Velocity`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);

ecs_entity_t e = ecs_new(world);
ecs_add(world, e, Position);
ecs_add(world, e, Velocity);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { EcsWildcard }
  }
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.entity()
    .add<Position>()
    .add<Velocity>();

flecs::query<> q = world.query_builder()
    .with(flecs::Wildcard)
    .build();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world
    .entity()
    .add::<Position>()
    .add::<Velocity>();

let q = world
    .query::<()>()
    .with::<flecs::Wildcard>()
    .build();
```
</li>
</ul>
</div>

The `Any` wildcard returns a single result for the first component that it matches. The query in the following example will return once for entity `e`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);

ecs_entity_t e = ecs_new(world);
ecs_add(world, e, Position);
ecs_add(world, e, Velocity);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { EcsAny }
  }
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.entity()
    .add<Position>()
    .add<Velocity>();

flecs::query<> q = world.query_builder()
    .with(flecs::Any)
    .build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world
    .entity()
    .add::<Position>().add::<Velocity>();

let q = world
    .query::<()>()
    .with::<flecs::Any>()
    .build();
```

</li>
</ul>
</div>

When using the `Any` wildcard it is undefined which component will be matched, as this can be influenced by other parts of the query. It is guaranteed that iterating the same query twice on the same dataset will produce the same result.

Wildcards are particularly useful when used in combination with pairs (next section).

### Pairs
A pair is an id that encodes two elements. Pairs, like components, can be added to entities and are the foundation for [Relationships](Relationships.md).

The elements of a pair are allowed to be wildcards. When a query pair contains the `Wildcard` wildcard, a query returns a result for each matching pair on an entity. When a query pair returns an `Any` wildcard, the query returns at most a single matching pair on an entity.

The following sections describe how to create queries for pairs in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

To query for a pair in C, the `id` field of a term can be set to a pair using the `ecs_pair` macro:

```c
ecs_entity_t Likes = ecs_new(world);
ecs_entity_t Bob = ecs_new(world);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { .id = ecs_pair(Likes, Bob) }
  }
});
```

The `id` field is guaranteed to be the first member of a term, which allows the previous code to be rewritten in this shorter form:

```c
ecs_entity_t Likes = ecs_new(world);
ecs_entity_t Bob = ecs_new(world);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_pair(Likes, Bob) }
  }
});
```

When an element of the pair is a component type, use the `ecs_id` macro to obtain the identifier to the id variable of the component type:

```c
ECS_COMPONENT(world, Eats);
ecs_entity_t Apples = ecs_new(world);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_pair(ecs_id(Eats), Apples) }
  }
});
```

The `ecs_isa`, `ecs_childof` and `ecs_dependson` convenience macros can be used to create pairs for builtin relationships. The two queries in the next example are equivalent:

```c
ecs_query_t *q1 = ecs_query(world, {
  .terms = {
    { ecs_pair(EcsChildOf, parent) }
  }
});

ecs_query_t *q2 = ecs_query(world, {
  .terms = {
    { ecs_childof(parent) }
  }
});
```

Pair queries can be created by setting their individual elements in the `first.id` and `second.id` members of a term:

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { .first.id = Eats, .second.id = Apples }
  }
});
```

Alternatively, one or both elements of a pair can be resolved by name. The two queries in the next example are equivalent:

```c
// ECS_TAG creates named entities
ECS_TAG(world, Eats);
ECS_TAG(world, Apples);

ecs_query_t *q1 = ecs_query(world, {
  .terms = {
    { .first.name = "Eats", .second.id = Apples }
  }
});

ecs_query_t *q2 = ecs_query(world, {
  .terms = {
    { .first.name = "Eats", .second.name = "Apples" }
  }
});
```

When a query pair contains a wildcard, the `ecs_field_id` function can be used to determine the id of the pair element that matched the query:

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_pair(Likes, EcsWildcard) }
  }
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
    ecs_id_t id = ecs_field_id(&it, 0);
    ecs_entity_t second = ecs_pair_second(world, id);

    for (int i = 0; i < it.count; i ++) {
        printf("entity %s likes %s\n",
            ecs_get_name(world, it.entities[i]),
            ecs_get_name(world, second));
    }
}
```

</li>
<li><b class="tab-title">C++</b>

When both parts of a pair are types, a `flecs::pair` template can be used. Pair templates can be made part of the query type, which makes them part of the argument list of the iterator functions. An example:

```cpp
struct Eats { float value; };
struct Apples { };
// Alias to save typing
using EatsApples = flecs::pair<Eats, Apples>;

flecs::query<EatsApples> q = 
    world.query<EatsApples>();

// Do not use reference argument for pair
q.each([](EatsApples v) {
    // Use -> operator to access value of pair
    v->value ++;
});
```

Pairs can also be added to queries using the builder API. This allows for the pair to be composed out of both types and regular entities. The three queries in the following example are equivalent:

```cpp
struct Eats { float value; };
struct Apples { };

flecs::entity eats = world.component<Eats>();
flecs::entity apples = world.component<Apples>();

flecs::query<> q1 = world.query_builder()
    .with<Eats, Apples>()
    .build();

flecs::query<> q2 = world.query_builder()
    .with<Eats>(apples)
    .build();

flecs::query<> q_3 = world.query_builder()
    .with(eats, apples)
    .build();
```

Individual elements of a pair can be specified with the `first` and `second` methods. The methods apply to the last added term. An example:

```cpp
flecs::query<> q = world.query_builder()
    .with().first<Eats>().second(apples)
    .build();
```

Individual elements of a pair can be resolved by name by using the `first` and `second` methods:

```cpp
flecs::query<> q = world.query_builder()
    .with().first("Eats").second("Apples")
    .build();
```

When a query pair contains a wildcard, the `flecs::iter::pair` method can be used to determine the id of the pair element that matched the query:

```cpp
flecs::query<> q = world.query_builder()
    .with<Eats>(flecs::Wildcard)
    .build();

q.each([](flecs::iter& it, size_t index) {
    flecs::entity second = it.pair(0).second();
    flecs::entity e = it.entity(index);
    
    std::cout << "entity " << e.name() 
              << " likes " second.name()
              << std::endl;
});
```

</li>
<li><b class="tab-title">Rust</b>

When both parts of a pair are types, a tuple can be used. tuple pairs can be made part of the query type, which makes them part of the argument list of the iterator functions. An example:

```rust
 #[derive(Component)]
 struct Eats {
     value: f32,
 }

 #[derive(Component)]
 struct Apples;

 let q = world.new_query::<&mut (Eats, Apples)>();

 q.each(|eats| {
     eats.value += 1.0;
 });
```

Tuple pairs can also be added to queries using the builder API. This allows for the pair to be composed out of both types and regular entities. The three queries in the following example are equivalent:

```rust
#[derive(Component)]
struct Eats {
    value: f32,
}

#[derive(Component)]
struct Apples;

let eats = world.component::<Eats>();
let apples = world.component::<Apples>();
let q1 = world.query::<()>().with::<(Eats, Apples)>().build(); // tuple types
let q2 = world.query::<()>().with_first::<Eats>(apples).build();
let q3 = world.query::<()>().with_id((eats, apples)).build(); // tuple ids
```

Individual elements of a tuple pair can be specified with the `first` and `second` methods. The methods apply to the last added term. An example:

```rust
let q = world
    .query::<()>()
    .term()
    .set_first::<Eats>()
    .set_second_id(apples)
    .build();
```

Individual elements of a pair can be resolved by name by using the `first` and `second` methods:

```rust
let q = world
    .query::<()>()
    .term()
    .set_first_name("Eats")
    .set_second_name("Apples")
    .build();
```

When a query pair contains a wildcard, the `TableIter::pair` method can be used to determine the id of the pair element that matched the query:

```rust
let q = world
    .query::<()>()
    .with::<(Eats, flecs::Wildcard)>()
    .build();

q.each_iter(|it, index, _| {
    let pair = it.pair(0).unwrap();
    let second = pair.second_id();
    let e = it.entity(index);
    println!("Entity {} likes {}", e.name(), second.name());
});
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

To query for a pair in the Flecs Query Language, the elements of a pair are a comma separated list surrounded by parentheses. An example:

```
(Likes, Apples)
```

A query may contain multiple pairs:

```
(Likes, Apples), (Likes, Pairs)
```
 
Queries for pairs that contain wildcards should use the symbols for either the `Wildcard` or `Any` wildcards:

```
(Likes, *)
```
```
(Likes, _)
```

A pair may contain two wildcards:

```
(*, *)
```

</li>
</ul>
</div>

### Access modifiers
Access modifiers specify which components of a query can be read and/or written. The different access modifiers are:

| Name  | DSL identifier | C identifier | C++ identifier | Description |
|-------|----------------|--------------|----------------|-------------|
| In    | `in`           | `EcsIn`      | `flecs::In`    | Component is readonly |
| Out   | `out`          | `EcsOut`     | `flecs::Out`   | Component is write only |
| InOut | `inout`        | `EcsInOut`   | `flecs::InOut` | Component can be read/written |
| None  | `none`         | `EcsInOutNone` | `flecs::InOutNone` | Component is neither read nor written |
| Default | n/a          | `EcsInOutDefault` | `flecs::InOutDefault` | Default modifier is selected for term |

Access modifiers can be used by API functions to ensure a component cannot be written, for example by requiring a component to be accessed with a `const` modifier. APIs may also infer access modifiers where possible, for example by using the `In` modifier for a query term with a type that has a `const` modifier.

When using pipelines, the scheduler may use access modifiers to determine where sync points are inserted. This typically happens when a system access modifier indicates a system writing to a component not matched by the query (for example, by using `set`), and is followed by a system that reads that component.

Access modifiers may also be used by serializers that serialize the output of an iterator (for example: `ecs_iter_to_json`). A serializer may for example decide to not serialize component values that have the `Out` or `None` modifiers.

When no access modifier is specified, `Default` is assumed. This selects `InOut` for components owned by the matched entity, and `In` for components that are from entities other than the one matched by the query. 

When a query term can either match a component from the matched entity or another entity (for example: when a component is inherited from a prefab) the `Default` access modifier only provides write access for the results where the component is owned by the matched entity. This prevents accidentally writing to a shared component. This behavior can be overridden by explicitly specifying an access modifier.

When a query term matches a tag (a component not associated with data) with a `Default` modifier, the `None` modifier is selected.

The following sections show how to use access modifiers in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

Access modifiers can be set using the `inout` member:

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity), .inout = EcsIn }
  }
});
```

</li>
<li><b class="tab-title">C++</b>

Access modifiers can be set using the `inout` method:

```cpp
// The following two queries are the same:
flecs::query<> q = world.query_builder()
    .with<Position>()
    .with<Velocity>().inout(flecs::In)
    .build();

flecs::query<> q = world.query_builder()
    .with<Position>()
    .with<Velocity>().in() // shorthand for .inout(flecs::In)
    .build();
```

When the `const` modifier is added to a type, the `flecs::In` modifier is automatically set:

```c
// Velocity term will be added with flecs::In modifier
flecs::query<Position, const Velocity> q = 
    world.query<Position, const Velocity>();
```

This also applies to types added with `term`:

```c
flecs::query<> q = world.query_builder()
    .with<Position>()
    .with<const Velocity>() // uses flecs::In modifier
    .build();
```

When a component is added by the `term` method and retrieved from a `flecs::iter` object during iteration, it must meet the constraints of the access modifiers. If the constraints are not met, a runtime assert may be thrown:

```cpp
flecs::query<> q = world.query_builder()
    .with<Position>()
    .with<Velocity>().inout(flecs::In)
    .build();

q.run([](flecs::iter& it) {
    while (it.next()) {
      auto p = it.field<Position>(0);       // OK
      auto p = it.field<const Position>(0); // OK
      auto v = it.field<const Velocity>(1); // OK
      auto v = it.field<Velocity>(1);       // Throws assert
    }
});
```

The builder API has `in()`, `inout()`, `out()` and `inout_none()` convenience methods:

```cpp
flecs::query<> q = world.query_builder()
    .with<Position>().inout()
    .with<Velocity>().in()
    .build();
```

</li>
<li><b class="tab-title">Rust</b>

Access modifiers can be set using the `set_inout_kind` method:

```rust
// The following two queries are the same:
let q = world
    .query::<()>()
    .with::<Position>()
    .with::<Velocity>()
    .set_inout_kind(InOutKind::In)
    .build();

let q = world
    .query::<()>()
    .with::<Position>()
    .with::<Velocity>()
    .set_in() // shorthand for .set_inout_kind(InOutKind::In)
    .build();
```

When the `const` / `immutable reference` modifier is added to a type, the `InOutKind::In` modifier is automatically set:

```rust
// Velocity term will be added with InOutKind::In modifier due to `&`
let q = world.new_query::<(&mut Position, &Velocity)>();
```

This also applies to types added with `term` / `with`:

```rust
let q = world
    .query::<()>()
    .with::<&mut Position>()
    .with::<&Velocity>() // uses InOutKind::In modifier
    .build();
```

When a component is added by the `term` method or generic terms with the `run` method, you can retrieve it from a `TableIter` object during iteration with the `field` method.

```rust
let q = world
    .query::<()>()
    .with::<&mut Position>()
    .with::<&Velocity>()
    .build();

q.run(|mut it| {
    while it.next() {
        let p = it.field::<Position>(0).unwrap();
        let v = it.field::<Velocity>(1).unwrap();
    }
});
```

The builder API has `set_in()`, `set_inout()`, `set_out()` and `set_inout_none()` convenience methods:

```rust
let q = world
    .query::<()>()
    .with::<Position>().set_inout()
    .with::<Velocity>().set_in()
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

Access modifiers in the Flecs Query Language can be specified inside of angular brackets before the component identifier:

```
Position, [in] Velocity
```

</li>
</ul>
</div>

### Operator Overview
The following operators are supported by queries:

| Name       | DSL operator   | C identifier  | C++ identifier    | Description |
|-----------|----------------|---------------|-------------------|-------------|
| And       | `,`            | `EcsAnd`      | `flecs::And`      | Match at least once with term |
| Or        | `\|\|`         | `EcsOr`       | `flecs::Or`       | Match at least once with one of the OR terms |
| Not       | `!`            | `EcsNot`      | `flecs::Not`      | Must not match with term |
| Optional  | `?`            | `EcsOptional` | `flecs::Optional` | May match with term |
| Equal     | `==`           | `EcsPredEq`   | `flecs::PredEq`   | Equals entity/entity name |
| Match     | `~=`           | `EcsPredMatch`| `flecs::PredMatch`| Match entity name with substring |
| AndFrom   | `and \|`       | `EcsAndFrom`  | `flecs::AndFrom`  | Match all components from id at least once |
| OrFrom    | `or \|`        | `EcsOrFrom`   | `flecs::OrFrom`   | Match at least one component from id at least once |
| NotFrom   | `not \|`       | `EcsNotFrom`  | `flecs::NotFrom`  | Don't match any components from id |

### And Operator
The `And` operator is used when no other operators are specified. The following sections show how to use the `And` operator in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

When no operator is specified, `And` is assumed. The following two queries are equivalent:

```c
ecs_query_t *q1 = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity) }
  }
});

ecs_query_t *q2 = ecs_query(world, {
  .terms = {
    { ecs_id(Position), .oper = EcsAnd }, 
    { ecs_id(Velocity), .oper = EcsAnd }
  }
});
```

</li>
<li><b class="tab-title">C++</b>

When no operator is specified, `And` is assumed. The following two queries are equivalent:

```cpp
flecs::query<Position, Velocity> q1 = world.query<Position, Velocity>();

flecs::query<> q2 = world.query_builder()
    .with<Position>()
    .with<Velocity>()
    .build();

flecs::query<> q2 = world.query_builder()
    .with<Position>().oper(flecs::And)
    .with<Velocity>().oper(flecs::And)
    .build();
```

The builder API has a `and_` convenience method:

```cpp
flecs::query<> q = world.query_builder()
    .with<Position>().and_(); // note escaping, 'and' is a C++ keyword
    .with<Velocity>().and_();
    .build();
```

</li>
<li><b class="tab-title">Rust</b>

When no operator is specified, `And` is assumed. The following two queries are equivalent:

```rust
let q = world.new_query::<(&mut Position, &Velocity)>();

let q2 = world
    .query::<()>()
    .with::<Position>()
    .with::<Velocity>()
    .build();

let q3 = world
    .query::<()>()
    .with::<Position>()
    .set_oper(OperKind::And)
    .with::<Velocity>()
    .set_oper(OperKind::And)
    .build();
```

The builder API has a `and` convenience method:

```rust
let q = world
    .query::<()>()
    .with::<Position>()
    .and()
    .with::<Velocity>()
    .and()
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

Query expressions with comma separated lists use the `And` operator:

```
Position, Velocity
```

</li>
</ul>
</div>

### Or Operator
The `Or` operator allows for matching a single component from a list. Using the `Or` operator means that a single term can return results of multiple types. When the value of a component is used while iterating the results of an `Or` operator, an application has to make sure that it is working with the expected type.

When using the `Or` operator, the terms participating in the `Or` expression are made available as a single field. Field indices obtained from an iterator need to account for this. Consider the following query:

```
Position, Velocity || Speed, Mass
```

This query has 4 terms, while an iterator for the query returns results with 3 fields. This is important to consider when retrieving the field for a term, as its index has to be adjusted. In this example, `Position` has index 1, `Velocity || Speed` has index 2, and `Mass` has index 3.

The following sections show how to use the `Or` operator in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

To create a query with `Or` terms, set `oper` to `EcsOr`:

```c
// Position, Velocity || Speed, Mass
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity), .oper = EcsOr },
    { ecs_id(Speed) },
    { ecs_id(Mass) }
  }
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
  Position *p = ecs_field(&it, Position, 0);
  Mass *m = ecs_field(&it, Mass, 2); // not 3, because of the Or expression

  ecs_id_t vs_id = ecs_field_id(&it, 1);
  if (vs_id == ecs_id(Velocity)) {
    // We can only use ecs_field if the field type is the same for all results,
    // but we can get the table column directly.
    Velocity *v = ecs_table_get_id(world, it.table, ecs_id(Velocity), it.offset);
    // iterate as usual
  } else if (vs_id == ecs_id(Speed)) {
    Speed *s = ecs_table_get_id(world, it.table, ecs_id(Speed), it.offset);
    // iterate as usual
  }
}
```

</li>
<li><b class="tab-title">C++</b>

To create a query with `Or` terms, use the `oper` method with `flecs::Or`:

```cpp
// Position, Velocity || Speed, Mass
flecs::query<> q = world.query_builder()
    .with<Position>()
    .with<Velocity>().oper(flecs::Or)
    .with<Speed>()
    .with<Mass>()
    .build();

q.run([&](flecs::iter& it) {
  while (it.next()) {
    auto p = it.field<Position>(0);
    auto v = it.field<Mass>(2); // not 4, because of the Or expression
    
    flecs::id vs_id = it.id(1);
    if (vs_id == world.id<Velocity>()) {
      // We can only use ecs_field if the field type is the same for all results,
      // but we can use range() to get the table column directly.
      auto v = it.range().get<Velocity>();
      // iterate as usual
    } else if (vs_id == world.id<Speed>()) {
      auto s = it.range().get<Speed>();
      // iterate as usual
    }
  }
});
```

The builder API has a `or_` convenience method:

```cpp
flecs::query<> q = world.query_builder()
    .with<Position>() 
    .with<Velocity>().or_(); // note escaping, 'or' is a C++ keyword
    .with<Speed>()
    .with<Mass>()
    .build();
```


</li>
<li><b class="tab-title">Rust</b>

To create a query with `Or` terms, use the `oper` method with enum `OperKind::Or`:

```rust
// Position, Velocity || Speed, Mass
let q = world
    .query::<()>()
    .with::<Position>()
    .with::<Velocity>()
    .set_oper(OperKind::Or)
    .with::<Speed>()
    .with::<Mass>()
    .build();

q.run(|mut it| {
    while it.next() {
        let p = it.field::<Position>(0).unwrap();
        let v = it.field::<Mass>(2).unwrap(); // not 4, because of the Or expression
        let vs_id = it.id(1);
        if vs_id == world.component_id::<Velocity>() {
            // We can only use ecs_field if the field type is the same for all results,
            // but we can use range() to get the table column directly.
            let v = it.range().unwrap().get_mut::<Velocity>();
            // iterate as usual
        } else if vs_id == world.component_id::<Speed>() {
            let s = it.range().unwrap().get_mut::<Speed>();
            // iterate as usual
        }
    }
});
```

The builder API has a `or` convenience method:

```rust
let q = world
    .query::<()>()
    .with::<Position>()
    .with::<Velocity>()
    .or()
    .with::<Speed>()
    .with::<Mass>()
    .build();
```


</li>
<li><b class="tab-title">Flecs Query Language</b>

To create a query with `Or` terms, use the `||` symbol:

```
Position, Velocity || Speed, Mass
```

</li>
</ul>
</div>

### Not Operator
The `Not` operator makes it possible to exclude entities with a specified component. Fields for terms that uses the `Not` operator will never provide data. 

A note on performance: `Not` terms are efficient to evaluate when combined with other terms, but queries that only have `Not` terms (or [`Optional`](#optional-operator)) can be expensive. This is because the storage only maintains indices for tables that _have_ a component, not for tables that do _not have_ a component.

The following sections show how to use the `Not` operator in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

To create a query with `Not` terms, set `oper` to `EcsNot`:

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity), .oper = EcsNot }
  }
});
```

</li>
<li><b class="tab-title">C++</b>

To create a query with `Not` terms, use the `oper` method with `flecs::Not`:

```cpp
flecs::query<> q = world.query_builder()
    .with<Position>()
    .with<Velocity>().oper(flecs::Not)
    .build();
```

The builder API has a `not_` convenience method:

```cpp
flecs::query<> q = world.query_builder()
    .with<Position>() 
    .with<Velocity>().not_(); // note escaping, 'not' is a C++ keyword
    .build();
```

An application can also use the `without` method:

```cpp
flecs::query<> q = world.query_builder()
    .with<Position>() 
    .without<Velocity>();
    .build();
```

</li>
<li><b class="tab-title">Rust</b>

To create a query with `Not` terms, use the `oper` method with enum `OperKind::Not`:

```rust
let q = world
    .query::<()>()
    .with::<Position>()
    .with::<Velocity>()
    .set_oper(OperKind::Not)
    .build();
```

The builder API has a `not_` convenience method:

```rust
let q = world
    .query::<()>()
    .with::<Position>()
    .with::<Velocity>()
    .not()
    .build();
```

An application can also use the `without` method:

```rust
let q = world
    .query::<()>()
    .with::<Position>()
    .without::<Velocity>()
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

To create a query with `Not` terms, use the `!` symbol:

```
Position, !Velocity
```

</li>
</ul>
</div>


### Optional Operator
The `Optional` operator optionally matches with a component. While this operator does not affect the entities that are matched by a query, it can provide more efficient access to a component when compared to conditionally getting the component in user code. Before accessing the value provided by an optional term, code must first check if the term was set.

A note on performance: just like the `Not` operator `Optional` terms are efficient to evaluate when combined with other terms, but queries that only have `Optional` terms can be expensive. Because the `Optional` operator does not restrict query results, a query that only has `Optional` terms will match all entities.

When an optional operator is used in a query, and a variable written by the optional term is read by a subsequent term, the subsequent term becomes a _dependent term_. This means that if the optional term does not match, the dependent term will be ignored. For example:

```
SpaceShip, ?(DockedTo, $planet), Planet($planet)
```

Because the second term is optional, the variable `$planet` may or may not be set depending on whether the term was matched. As a result the third term becomes dependent: if `$planet` was not set, the term will be ignored.

The following sections show how to use the `Optional` operator in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

To create a query with `Optional` terms, set `oper` to `EcsOptional`:

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity), .oper = EcsOptional }
  }
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
  Position *p = ecs_field(&it, Position, 0);
  if (ecs_field_is_set(&it, 1)) {
    Velocity *v = ecs_field(&it, Velocity, 1);
    // iterate as usual
  } else {
    // iterate as usual
  }
}
```

</li>
<li><b class="tab-title">C++</b>

To create a query with `Optional` terms, a component can be specified as a pointer type:

```cpp
flecs::query<> q = world.query<Position, Velocity*>()

q.each([](Position& p,  Velocity* v) {
  if (v) {
    // ...
  }
})
```

Alternatively, an application can call the `oper` method with `flecs::Optional`:

```cpp
flecs::query<> q = world.query_builder()
    .with<Position>()
    .with<Velocity>().oper(flecs::Optional)
    .build();

q.run([&](flecs::iter& it) {
  while (it.next()) {
    auto p = it.field<Position>(0);
    
    if (it.is_set(1)) {
      auto v = it.field<Velocity>(1);
      // iterate as usual
    }
  }
});
```

The builder API has an `optional` convenience method:

```cpp
flecs::query<> q = world.query_builder()
    .with<Position>() 
    .with<Velocity>().optional();
    .build();
```

</li>
<li><b class="tab-title">Rust</b>

To create a query with `Optional` terms, a component can be specified as an Option type:

```rust
let q = world.new_query::<(&Position, Option<&Velocity>)>();

q.each(|(p, v)| {
    if let Some(v) = v {
        // ...
    }
});
```

Alternatively, an application can call the `oper` method with enum `OperKind::Optional`:

```rust
let q = world
    .query::<()>()
    .with::<Position>()
    .with::<Velocity>()
    .set_oper(OperKind::Optional)
    .build();

q.run(|mut it| {
    while it.next() {
        let p = it.field::<Position>(0).unwrap();
        if let Some(v) = it.field::<Velocity>(1) {
            // iterate as usual
        }
    }
});
```

The builder API has an `optional` convenience method:

```rust
let q = world
    .query::<()>()
    .with::<Position>()
    .with::<Velocity>()
    .optional()
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

To create a query with `Optional` terms, use the `?` symbol:

```
Position, ?Velocity
```

</li>
</ul>
</div>

### Equality operators
Equality operators (`==`, `!=`, `~=`) allow a query to ensure that a variable equals a specific entity, that the entity it stores has a specific name, or that the entity name partially matches a substring.

The left hand side of an equality operator must be a variable. The right hand side of an operator can be an entity identifier or a string for the `==` and `!=` operators, and must be a string in case of the `~=` operator. For example:

Test if variable `$this` equals `Foo` (`Foo` must exist at query creation time):

```js
$this == Foo
```

Test if variable `$this` equals entity with name `Foo` (`Foo` does not need to exist at query creation time):

```js
$this == "Foo"
```

Test if variable `$this` stores an entity with a name that has substring `Fo`:

```js
$this ~= "Fo"
```

When the equals operator (`==`) is used with a variable that has not yet been initialized, the right-hand side of the operator will be assigned to the variable. If the right hand side is a string, it will be used to lookup an entity by name. If the lookup fails, the term will not match.

Other than regular operators, equality operators are set as `first`, with the left hand being `src` and the right hand being `second`. Equality operators can be combined with `And`, `Not` and `Or` terms.

Terms with equality operators return no data.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    // $this == Foo
    { .first.id = EcsPredEq, .second.id = Foo }, 
    // $this != Bar
    { .first.id = EcsPredEq, .second.id = Bar, .oper = EcsNot },
    // $this == "Foo"
    { .first.id = EcsPredEq, .second = { .name = "Foo", .id = EcsIsName }}, 
    // $this ~= "Fo"
    { .first.id = EcsPredMatch, .second = { .name = "Fo", .id = EcsIsName }}, 
  }
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.query_builder()
  // $this == Foo
  .with(flecs::PredEq, Foo)
  // $this != Foo
  .without(flecs::PredEq, Bar)
  // $this == "Foo"
  .with(flecs::PredEq).second("Foo").flags(EcsIsName)
  // $this ~= "Fo"
  .with(flecs::PredMatch).second("Fo").flags(EcsIsName)
  .build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .query::<()>()
    // $this == Foo
    .with::<(flecs::PredEq, Foo)>()
    // $this != Foo
    .without::<(flecs::PredEq, Bar)>()
    // $this == "Foo"
    .with::<flecs::PredEq>()
    .set_second_name("Foo")
    .flags(sys::EcsIsName)
    // $this ~= "Fo"
    .with::<flecs::PredMatch>()
    .set_second_name("Fo")
    .flags(sys::EcsIsName)
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

```js
$this == Foo
$this != Foo
$this == "Foo"
$this != "Fo"
```

</li>
</ul>
</div>

### AndFrom, OrFrom, NotFrom Operators
The `AndFrom`, `OrFrom` and `NotFrom` operators make it possible to match a list of components that is defined outside of the query. Instead of matching the id provided in the term, the operators match the list of components _of_ the provided id as if they were provided as a list of terms with `And`, `Or` or `Not` operators. For example, if entity `e` has components `Position, Velocity` and is combined in a query with the `AndFrom` operator, entities matching the query must have both `Position` and `Velocity`.

The `AndFrom`, `OrFrom` and `NotFrom` operators are especially useful when combined with prefab entities, which by default are not matched with queries themselves. Components that have the `(OnInstantiate, DontInherit)` property are ignored while matching the operators, which means that using a prefab in combination with `AndFrom`, `OrFrom` and `NotFrom` will not cause components like `Prefab` or `ChildOf` to be considered.

Component lists can be organized recursively by adding an id to an entity with the `AND` and `OR` id flags.

Fields for terms that use the `AndFrom`, `OrFrom` or `NotFrom` operators never provide data. Access modifiers for these operators default to `InOutNone`. When a `AndFrom`, `OrFrom` or `NotFrom` operator is combined with an access modifier other than `InOutDefault` or `InOutNone` query creation will fail.

The following sections show how to use the operators in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

To use the `AndFrom`, `OrFrom` and `NotFrom` operators, set `oper` to `EcsAndFrom`, `EcsOrFrom` or `EcsNotFrom`

```c
ecs_entity_t type_list = ecs_new_w_id(world, EcsPrefab);
ecs_add(world, type_list, Position);
ecs_add(world, type_list, Velocity);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { type_list, .oper = EcsAndFrom }, // match Position, Velocity
    { type_list, .oper = EcsOrFrom },  // match Position || Velocity
    { type_list, .oper = EcsNotFrom }  // match !Position, !Velocity
  }
});
```

</li>
<li><b class="tab-title">C++</b>

To use the `AndFrom`, `OrFrom` and `NotFrom` operators, call the `oper` method with `flecs::AndFrom`, `flecs::OrFrom` or `flecs::NotFrom`.

```cpp
flecs::entity type_list = world.prefab()
  .add<Position>()
  .add<Velocity>();

flecs::query<> q = world.query_builder()
    .with(type_list).oper(flecs::AndFrom) // match Position, Velocity
    .with(type_list).oper(flecs::OrFrom)  // match Position || Velocity
    .with(type_list).oper(flecs::NotFrom) // match !Position, !Velocity
    .build();
```

The builder API has the `and_from`, `or_from` and `not_from` convenience methods:

```cpp
flecs::query<> q = world.query_builder()
    .with(type_list).and_from()
    .with(type_list).or_from()
    .with(type_list).not_from()
    .build();
```

</li>
<li><b class="tab-title">Rust</b>

To use the `AndFrom`, `OrFrom` and `NotFrom` operators, call the `oper` method with enum `OperKind::AndFrom`, `OperKind::OrFrom` or `flecs::NotFrom`.

```rust
let q = world
    .query::<()>()
    .with_id(type_list)
    .set_oper(OperKind::AndFrom) // match Position, Velocity
    .with_id(type_list)
    .set_oper(OperKind::OrFrom) // match Position || Velocity
    .with_id(type_list)
    .set_oper(OperKind::NotFrom) // match !Position, !Velocity
    .build();
```

The builder API has the `and_from`, `or_from` and `not_from` convenience methods:

```rust
let q = world
    .query::<()>()
    .with_id(type_list)
    .and_from()
    .with_id(type_list)
    .or_from()
    .with_id(type_list)
    .not_from()
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

To create a query with the `AndFrom`, `OrFrom` and `NotFrom` operators in the C API, use `and`, `or` and `not` in combination with the bitwise OR operator (`|`):

```
and | type_list, or | type_list, not | type_list
```

</li>
</ul>
</div>

### Query scopes
Query scopes are a mechanism that allows for treating the output of a number of terms as a single condition. For example, the following query has two terms with an `Or` operator that are negated by a `Not` operator:

```c
// Match entities with Position that don't have Velocity or Speed
Position, !{ Velocity || Speed }
```

A query scope can contain any number of terms and operators. The following query has a scope with mixed operators:

```c
Position, !{ Mass, Velocity || Speed, !Rotation }
```

Query scopes allow for the creation of complex queries when combined with variables and relationships. The following query finds all entities that have no children with `Position`:

```c
Position($this), !{ ChildOf($child, $this), Position($child) }
```

Note how this is different from this query, which finds all children that don't have `Position`:

```c
Position($this), ChildOf($child, $this), !Position($child)
```

Whereas the first query only returns parents without children with `Position`, the second query returns parents that have _at least one child_ that doesn't have `Position`.

When a scope is evaluated, the entire result set of the scope is treated as a single term. This has as side effect that any variables first declared inside the scope are not available outside of the scope. For example, in the following query the value for variable `$child` is undefined, as it is first used inside a scope:

```c
Position($this), !{ ChildOf($child, $this), Position($child) }
```

Scopes currently have the following limitations:
 - scopes can only be combined with Not operators (e.g. `!{ ... }`). Future versions of flecs will add support for combining scopes with Or operators (e.g. `{ ... } || { ... }`).
 - scopes cannot appear as first terms in a query.

The following examples show how to use scopes in the different language bindings:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    // Position, !{ Velocity || Speed }
    { .id = ecs_id(Position) },
    { .id = EcsScopeOpen, .src.id = EcsIsEntity, .oper = EcsNot },
    { .id = ecs_id(Velocity), .oper = EcsOr },
    { .id = ecs_id(Speed) },
    { .id = EcsScopeClose, .src.id = EcsIsEntity }
  }
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.query_builder()
  // Position, !{ Velocity || Speed }
  .with<Position>()
  .scope_open().not_()
    .with<Velocity>().or_()
    .with<Speed>()
  .scope_close()
  .build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .query::<()>()
    // Position, !{ Velocity || Speed }
    .with::<Position>()
    .scope_open()
    .not()
    .with::<Velocity>()
    .or()
    .with::<Speed>()
    .scope_close()
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

```
Position, !{ Velocity || Speed }
```

</li>
</ul>
</div>

### Source
Source is a property of a term that specifies the entity on which the term should be matched. Queries support two kinds of sources: static and variable. A static source is known when the query is created (for example: match `SimTime` on entity `Game`), whereas a variable source is resolved while the query is evaluated. When no explicit source is specified, a default variable source called `$this` is used (see [Variables](#variables)). 

When a query only has terms with fixed sources, iterating the query will return a result at least once when it matches, and at most once if the query terms do not match wildcards. If a query has one or more terms with a fixed source that do not match the entity, the query will return no results. A source does not need to match the query when the query is created.

When a term has a fixed source and the [access modifiers](#access-modifiers) are not explicitly set, the access modifier defaults to `In`, instead of `InOut`. The rationale behind this is that it encourages code to only makes local changes (changes to components owned by the matched entity) which is easier to maintain and multithread. This default can be overridden by explicitly setting access modifiers.

The following sections show how to use variable and fixed sources with the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

To specify a fixed source, set the `src.id` member to the entity to match. The following example shows how to set a source, and how to access the value provided by a term with a fixed source:

```c
ecs_entity_t Game = ecs_new(world);
ecs_add(world, Game, SimTime);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }, // normal term, uses $this source
    { ecs_id(Velocity) },  // normal term, also uses $this source
    { ecs_id(SimTime), .src.id = Game } // fixed source, match SimTime on Game
  }
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
  Position *p = ecs_field(&it, Position, 0);
  Velocity *v = ecs_field(&it, Velocity, 1);
  SimTime *st = ecs_field(&it, SimTime, 2);
  
  for (int i = 0; i < it.count; i ++) {
    p[i].x += v[i].x * st[i].value;
    p[i].y += v[i].y * st[i].value;
  }
}
```

Note how in this example all components can be accessed as arrays. When a query has mixed fields (fields with both arrays and single values), behavior defaults to entity-based iteration where entities are returned one at a time. As a result, `i` in the previous example will never be larger than `0`, which is why this code works even though there is only a single instance of the `SimTime` component.

Returning entities one at a time can negatively affect performance, especially for large tables. To learn more about why this behavior exists and how to ensure that mixed results use table-based iteration, see [Instancing](#instancing). 

A source may also be specified by name by setting the `src.name` member:

```c
ecs_entity_t Game = ecs_entity(world, { .name = "Game" });
ecs_add(world, Game, SimTime);

ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(SimTime), .src.name = "Game" }
  }
});
```

This examples shows how to access the entities matched by the default `$this` source and a fixed source:

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }, // normal term, uses $this source
    { ecs_id(SimTime), .src.id = Game } // fixed source, match SimTime on Game
  }
});

while (ecs_query_next(&it)) {
  ecs_entity_t src_1 = ecs_field_src(&it, 0); // Returns 0, meaning entity is stored in it.entities
  ecs_entity_t src_2 = ecs_field_src(&it, 1); // Returns Game

  for (int i = 0; i < it.count; i ++) {
    printf("$this = %s, src_2 = %s\n", 
      ecs_get_name(world, it.entities[i]),
      ecs_get_name(world, src_2));
  }
}
```

The `entities` and `count` member are solely populated by the number of entities matched by the default `$this` source. If a query only contains fixed sources, `count` will be set to 0. This is important to keep in mind, as the inner for loop from the last example would never be iterated for a query that only has fixed sources.

</li>
<li><b class="tab-title">C++</b>

To specify a fixed source, call the `src` method to the entity to match. The following example shows how to set a source, and how to access the value provided by a term with a fixed source:

```cpp
flecs::entity Game = world.entity()
  .add<SimTime>();

flecs::query<> q = world.query_builder()
  .with<Position>()  // normal term, uses $this source
  .with<Velocity>()  // normal term, also uses $this source
  .with<SimTime>().src(Game) // fixed source, match SimTime on Game
  .build();

q.run([](flecs::iter& it) {
  while (it.next()) {
    auto p = it.field<Position>(0);
    auto v = it.field<Velocity>(1);
    auto st = it.field<SimTime>(2);
    
    for (auto i : it) {
      p[i].x += v[i].x * st[i].value;
      p[i].y += v[i].y * st[i].value;
    }
  }
});
```

Note how in this example all components can be accessed as arrays. When a query has mixed fields (fields with both arrays and single values), behavior defaults to entity-based iteration where entities are returned one at a time. As a result, `i` in the previous example will never be larger than `0`, which is why this code works even though there is only a single instance of the `SimTime` component.

Returning entities one at a time can negatively affect performance, especially for large tables. To learn more about why this behavior exists and how to ensure that mixed results use table-based iteration, see [Instancing](#instancing). 

The next example shows how queries with mixed `$this` and fixed sources can be iterated with `each`. The `each` function does not have the performance drawback of the last `run` example, as it uses [instancing](#instancing) by default.

```cpp
flecs::query<Position, Velocity, SimTime> q = 
  world.query_builder<Position, Velocity, SimTime>()
    .term_at(2).src(Game) // set fixed source for 3rd template argument (SimTime)
    .build();

// Because all components are now part of the query type, we can use each
q.each([](flecs::entity e, Position& p, Velocity& v, SimTime& st) {
  p.x += v.x * st.value;
  p.y += v.y * st.value;
});
```

When a query has no terms for the `$this` source, it must be iterated with the `run` function or with a variant of `each` that does not have a signature with `flecs::entity` as first argument:

```cpp
flecs::query<SimConfig, SimTime> q = 
  world.query_builder<SimConfig, SimTime>()
    .term_at(0).src(Cfg)
    .term_at(1).src(Game)
    .build();

// Ok (note that it.count() will be 0)
q.run([](flecs::iter& it) {
  while (it.next()) {
    auto sc = it.field<SimConfig>(0);
    auto st = it.field<SimTime>(1);
    st->value += sc->sim_speed;
  }
});

// Ok
q.each([](SimConfig& sc, SimTime& st) {
  st.value += sc.sim_speed;
});

// Ok
q.each([](flecs::iter& it, size_t index, SimConfig& sc, SimTime& st) {
  st.value += sc.sim_speed;
});

// Not ok: there is no entity to pass to first argument
q.each([](flecs::entity e, SimConfig& sc, SimTime& st) { 
  st.value += sc.sim_speed;
});
```

A source may also be specified by name:

```cpp
flecs::query<SimConfig, SimTime> q = 
  world.query_builder<SimConfig, SimTime>()
    .term_at(0).src("Cfg")
    .term_at(1).src("Game")
    .build();
```

</li>
<li><b class="tab-title">Rust</b>

To specify a fixed source, call the `set_src_id` method to the entity to match. The following example shows how to set a source, and how to access the value provided by a term with a fixed source:

```rust
let q = world
    .query::<()>()
    .with::<Position>() // normal term, uses $this source
    .with::<Velocity>() // normal term, uses $this source
    .with::<SimTime>()
    .set_src_id(game) // fixed source, match SimTime on Game
    .build();

q.run(|mut it| {
    while it.next() {
        let mut p = it.field::<Position>(0).unwrap();
        let v = it.field::<Velocity>(1).unwrap();
        let st = it.field::<SimTime>(2).unwrap();
        for i in it.iter() {
            p[i].x += v[i].x * st[i].value;
            p[i].y += v[i].y * st[i].value;
        }
    }
});
```

Note how in this example all components can be accessed as arrays. When a query has mixed fields (fields with both arrays and single values), behavior defaults to entity-based iteration where entities are returned one at a time. As a result, `i` in the previous example will never be larger than `0`, which is why this code works even though there is only a single instance of the `SimTime` component.

Returning entities one at a time can negatively affect performance, especially for large tables. To learn more about why this behavior exists and how to ensure that mixed results use table-based iteration, see [Instancing](#instancing). 

The next example shows how queries with mixed `$this` and fixed sources can be iterated with `each`. The `each` function does not have the performance drawback of the last `run` example, as it uses [instancing](#instancing) by default.

```rust
let q = world
    .query::<(&mut Position, &Velocity, &SimTime)>()
    .term_at(2)
    .set_src_id(game) // fixed source for 3rd template argument (SimTime)
    .build();

// Because all components are now part of the query type, we can use each
q.each_entity(|e, (p, v, st)| {
    p.x += v.x * st.value;
    p.y += v.y * st.value;
});
```

When a query has no terms for the `$this` source, it must be iterated with the `run` function or with a variant of `each` that does not have a signature with `flecs::entity` as first argument:

```rust
let q = world
    .query::<(&SimConfig, &mut SimTime)>()
    .term_at(0)
    .set_src_id(cfg)
    .term_at(1)
    .set_src_id(game)
    .build();

// Ok (note that it.count() will be 0)
q.run(|mut it| {
    while it.next() {
        let sc = it.field::<SimConfig>(0).unwrap();
        let mut st = it.field::<SimTime>(1).unwrap();
        st[0].value += sc[0].sim_speed; // 0 because it's a single source element
    }
});

// Ok
q.each(|(sc, st)| {
    st.value += sc.sim_speed;
});

// Ok
q.each_iter(|it, index, (sc, st)| {
    st.value += sc.sim_speed;
});

// Not ok: there is no entity to pass to first argument
q.each_entity(|e, (sc, st)| {
    st.value += sc.sim_speed;
});
```

A source may also be specified by name:

```rust
let q = world
    .query::<(&SimConfig, &SimTime)>()
    .term_at(0)
    .set_src_name("Cfg")
    .term_at(1)
    .set_src_name("Game")
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

To specify a source in the DSL, use parenthesis after the component identifier. The following example uses the default `$this` source for `Position` and `Velocity`, and `Game` as source for `SimTime`.

```
Position, Velocity, SimTime(Game)
```

In the previous example the source for `Position` and `Velocity` is implicit. The following example shows the same query with explicit sources for all terms:

```
Position($this), Velocity($this), SimTime(Game)
```

To specify a source for a pair, the second element of the pair is placed inside the parenthesis after the source. The following query uses the default `$this` source for the `(Color, Diffuse)` pair, and `Game` as source for the  `(Color, Sky)` pair.

```
(Color, Diffuse), Color(Game, Sky)
```

In the previous example the source for `(Color, Diffuse)` is implicit. The following example shows the same query with explicit sources for all terms:

```
Color($this, Diffuse), Color(Game, Sky)
```

</li>
</ul>
</div>

### Singletons
Singletons are components that are added to themselves, which can be matched by providing the component id as [source](#source). 

The following sections show how to use singletons in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

A singleton query is created by specifying the same id as component and source:

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { Player },
    { ecs_id(Position) },
    { ecs_id(Input), .src = ecs_id(Input) } // match Input on itself
  }
});
```

The singleton component data is accessed in the same way a component from a static [source](#source) is accessed.

</li>
<li><b class="tab-title">C++</b>

A singleton query can be created by specifying the same id as component and source:

```cpp
flecs::query<Player, Position> q = world.query_builder<Player, Position>()
  .with<Input>().src<Input>() // match Input on itself
  .build();
```

The builder API provides a `singleton` convenience function:

```cpp
flecs::query<Player, Position> q = world.query_builder<Player, Position>()
  .with<Input>().singleton() // match Input on itself
  .build();
```

The singleton component data is accessed in the same way a component from a static [source](#source) is accessed.

</li>
<li><b class="tab-title">Rust</b>

A singleton query can be created by specifying the same id as component and source:

```rust
let q = world
    .query::<(&Player, &Position)>()
    .with::<Input>()
    .set_src::<Input>() // match Input on itself
    .build();
```

The builder API provides a `singleton` convenience function:

```rust
let q = world
    .query::<(&Player, &Position)>()
    .with::<Input>()
    .singleton() // match Input on itself
    .build();
```

The singleton component data is accessed in the same way a component from a static [source](#source) is accessed.

The builder API provides a `term_at` function which allows specifying characteristics of a term at a specific index in the generic terms.

```rust
let q = world
    .query::<(&Player, &Position, &Input)>()
    .term_at(2)
    .singleton() // match Input on itself
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

A singleton query can be created by specifying the same id as component and source:

```
Player, Position, Input(Input)
```

For convenience the `$` character may be used as source, which resolves to the component id:

```
Player, Position, Input($)
```

</li>
</ul>
</div>

### Relationship Traversal
Relationship traversal enables a query to search for a component by traversing a relationship. One of the most common examples of where this is useful is a Transform system, which matches `Position` on an entity and the entity's parent. To find the `Position` component on a parent entity, a query traverses the `ChildOf` relationship upwards:

![filter diagram](img/relationship_traversal.png)

The arrows in this diagram indicate the direction in which the query is traversing the `ChildOf` relationship to find the component. A query will continue traversing until it has found an entity with the component, or until a root (an entity without the relationship) has been found. The traversal is depth-first. If an entity has multiple instances of a relationship a query will first traverse the first instance until its root entity before continuing with the second instance.

Using the relationship traversal feature will in most cases provide better performance than doing the traversal in user code. This is especially true for cached queries, where the results of traversal are cached. Relationship traversal can in some edge cases cause performance degradation, especially in applications with large numbers of cached queries and deep hierarchies. See the section on performance & rematching for more details.

Any relationship used for traversal must have the [Traversable](Relationships.md#traversable-property) property. Attempting to create a query that traverses a relationship that does not have the `Traversable` property will cause query creation to fail. This safeguards against creating queries that could end up in an infinite traversal loop when a cyclic relationship is encountered.

Relationship traversal works for both variable and fixed [sources](#source).

#### Traversal Flags
Traversal behavior can be customized with the following bit flags, in addition to the relationship being traversed:

| Name     | DSL identifier | C identifier  | C++ identifier    | Description |
|----------|----------------|---------------|-------------------|-------------|
| Self     | `self`         | `EcsSelf`     | `flecs::Self`     | Match self |
| Up       | `up`           | `EcsUp`       | `flecs::Up`       | Match by traversing upwards |
| Cascade  | `cascade`      | `EcsCascade`  | `flecs::Cascade`  | Same as Up, but iterate in breadth-first order |
| Desc     | `desc`         | `EcsDesc`     | `flecs::Desc`     | Combine with Cascade to iterate hierarchy bottom to top |

If just `Self` is set a query will only match components on the matched entity (no traversal). If just `Up` is set, a query will only match components that can be reached by following the relationship and ignore components from the matched entity. If both `Self` and `Up` are set, the query will first look on the matched entity, and if it does not have the component the query will continue searching by traverse the relationship.

When an `Up` traversal flag is set, but no traversal relationship is provided, the traversal relationship defaults to `ChildOf`. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// These two queries are the same:
ecs_query_t *q1 = ecs_query(world, {
  .terms = { ecs_id(Mass), .src.id = EcsUp, .trav = EcsChildOf }
});

ecs_query_t *q2 = ecs_query(world, {
  .terms = { ecs_id(Mass), .src.id = EcsUp } // defaults to .trav = EcsChildOf
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// These three queries are the same:
flecs::query<> q1 = world.query_builder()
  .with<Mass>().up(flecs::ChildOf)
  .build();

flecs::query<> q2 = world.query_builder()
  .with<Mass>().up() // defaults to .up(flecs::ChildOf)
  .build();

flecs::query<> q3 = world.query_builder()
  .with<Mass>().parent() // shortcut for .up(flecs::ChildOf)
  .build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// These three queries are the same:
let q1 = world
    .query::<()>()
    .with::<Mass>()
    .up_type::<flecs::ChildOf>()
    .build();

let q2 = world
    .query::<()>()
    .with::<Mass>()
    .up() // defaults to .up(flecs::ChildOf)
    .build();

let q3 = world
    .query::<()>()
    .with::<Mass>()
    .parent() // shortcut for .up(flecs::ChildOf)
    .build();
```

</li>
</ul>
</div>

Query terms default to `Self`, except for components that have the `(OnInstantiate, Inherit)` trait. In that case, terms default to `Self|Up` with the `IsA` relationship, which matches components inherited from `IsA` targets (typically prefabs). An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Register an inheritable component 'Mass'
ECS_COMPONENT(world, Mass);
ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

// These two queries are the same:
ecs_query_t *q1 = ecs_query(world, {
  .terms = { ecs_id(Mass), .src.id = EcsSelf|EcsUp, .trav = EcsIsA }
});

ecs_query_t *q2 = ecs_query(world, {
  .terms = { ecs_id(Mass) } // defaults to , .src.id = EcsSelf|EcsUp, .trav = EcsIsA
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Register an inheritable component 'Mass'
world.component<Mass>().add(flecs::OnInstantiate, flecs::Inherit);

// These two queries are the same:
flecs::query<> q1 = world.query_builder()
  .with<Mass>().self().up(flecs::IsA)
  .build();

flecs::query<> q2 = world.query_builder()
  .with<Mass>() // defaults to .self().up(flecs::IsA)
  .build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
 // Register an inheritable component 'Mass'
 world
     .component::<Mass>()
     .add_trait::<(flecs::OnInstantiate, flecs::Inherit)>();

 // These two queries are the same:
 let q1 = world
     .query::<()>()
     .with::<Mass>()
     .self_()
     .up_type::<flecs::IsA>()
     .build();

 let q2 = world
     .query::<()>()
     .with::<Mass>() // defaults to .self().up(flecs::IsA)
     .build();
```

</li>
</ul>
</div>

When a relationship that is not `IsA` is traversed, the entities visited while traversing will still be tested for inherited components. This means that an entity with a parent that inherits the `Mass` component from a prefab will match a query that traverses the `ChildOf` relationship to match the `Mass` component. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Register an inheritable component 'Mass'
ECS_COMPONENT(world, Mass);
ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

// Create a prefab, prefab instance and a child
ecs_entity_t prefab = ecs_insert(world, { EcsPrefab }, ecs_value(Mass, 100));
ecs_entity_t parent = ecs_insert(world, { ecs_isa(prefab) });  // inherits Mass
ecs_entity_t child = ecs_entity(world, { .parent = parent });

// Matches 'child', because parent inherits Mass from prefab
ecs_query_t *q = ecs_query(world, {
  .terms = { ecs_id(Mass), .src.id = EcsUp } // traverses ChildOf upwards
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Register an inheritable component 'Mass'
world.component<Mass>().add(flecs::OnInstantiate, flecs::Inherit);

flecs::entity base = world.entity()
  .add<Mass>();

flecs::entity parent = world.entity()
  .add(flecs::IsA, base); // inherits Mass

flecs::entity child = world.entity()
  .add(flecs::ChildOf, parent);

// Matches 'child', because parent inherits Mass from prefab
flecs::query<> q = world.query_builder()
  .with<Mass>().up() // traverses ChildOf upwards
  .build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Register an inheritable component 'Mass'
world
    .component::<Mass>()
    .add_trait::<(flecs::OnInstantiate, flecs::Inherit)>();

let base = world.entity().add::<Mass>();

let parent = world.entity().is_a_id(base); // inherits Mass

let child = world.entity().child_of_id(parent);

// Matches 'child', because parent inherits Mass from prefab
let q = world
    .query::<()>()
    .with::<Mass>()
    .up() // traverses ChildOf upwards
    .build();
```

</li>
</ul>
</div>

When a component is matched through traversal and its [access modifier](#access-modifiers) is not explicitly set, it defaults to `flecs::In`. This behavior is consistent with terms that have a fixed [source](#source).

#### Iteration
When a component is matched through traversal, the behavior is the same as though the component was matched through a fixed [source](#source): iteration will switch from table-based to entity-based. This happens on a per-result basis: if all terms are matched on the matched entity the entire table will be returned by the iterator. If one of the terms was matched through traversal, entities are returned one by one.

While returning entities one by one is measurably slower than iterating entire tables, this default behavior enables matching inherited components by default without requiring the user code to be explicitly aware of the difference between a regular component and an inherited component. An advantage of this approach is that applications that use inherited components can interoperate with third party systems that do not explicitly handle them.

To ensure fast table-based iteration an application can enable [instancing](#instancing). Instanced iteration is as fast as, and often faster than regular iteration. Using inherited components that are shared across many entities can improve cache efficiency as less data needs to be loaded from main RAM, and values are more likely to already be stored in the cache.

> Note: the C++ `each` API always uses [instancing](#instancing), which guarantees fast table-based iteration.

#### Limitations
This list is an overview of current relationship traversal limitations:

- The `Cascade` and `Desc` flags require caching.
- Traversal flags can only be specified for the term source.
- Union relationship queries that are not wildcards are not supported for traversal.

The following sections show how to use traversal in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

The following example shows a query that matches an inherited component:

```c
// Register inheritable 'Position' component
ECS_COMPONENT(world, Position);
ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

ecs_entity_t base = ecs_new_w(world, Position);
ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base); // Inherits Position

// The following two queries are the same:
ecs_query_t *q1 = ecs_query(world, {
  .terms = {
    { ecs_id(Position) }
  }
});

ecs_query_t *q2 = ecs_query(world, {
  .terms = {
    { ecs_id(Position), .src.id = EcsSelf|EcsUp, .trav = EcsIsA }
  }
});
```

The following example shows a query that matches a component from a parent:

```c
ecs_entity_t parent = ecs_new_w(world, Position);
ecs_entity_t inst = ecs_new_w_pair(world, EcsChildOf, parent);

ecs_query_t *q1 = ecs_query(world, {
  .terms = {
    { ecs_id(Position), .self.id = EcsUp }
  }
});
```

The following example shows a query that traverses a custom relationship:

```c
// Create a new traversable relationship
ecs_entity_t ContainedBy = ecs_insert(world, { EcsTraversable });

ecs_entity_t parent = ecs_new_w(world, Position);
ecs_entity_t inst = ecs_new_w_pair(world, ContainedBy, parent);

ecs_query_t *q1 = ecs_query(world, {
  .terms = {
    { ecs_id(Position), .self.id = EcsUp, .trav = ContainedBy }
  }
});
```

</li>
<li><b class="tab-title">C++</b>

The following example shows a query that matches an inherited component:

```cpp
// Register inheritable 'Position' component
world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);

flecs::entity base = world.entity()
  .add<Position>();

flecs::entity inst = world.entity()
  .is_a(base); // short for .add(flecs::IsA, base)

// The following two queries are the same:
flecs::query<> q1 = world.query<Position>();

flecs::query<> q2 = world.query_builder<Position>()
  .term_at(0).self().up(flecs::IsA)
  .build();
```

The following example shows a query that matches a component from a parent:

```cpp
flecs::entity parent = world.entity()
  .add<Position>();

flecs::entity child = world.entity()
  .child_of(parent); // short for .add(flecs::ChildOf, base)

flecs::query<> q = world.query_builder<Position>()
  .term_at(0).up()
  .build();
```

The following example shows a query that traverses a custom relationship:

```cpp
// Create a new traversable relationship
flecs::entity ContainedBy = world.entity().add(flecs::Traversable);

flecs::entity parent = world.entity()
  .add<Position>();

flecs::entity child = world.entity()
  .add(ContainedBy, parent);

flecs::query<> q = world.query_builder<Position>()
  .term_at(0).up(ContainedBy)
  .build();
```

</li>
<li><b class="tab-title">Rust</b>

The following example shows a query that matches an inherited component:

```rust
 // Register inheritable 'Position' component
 world
     .component::<Position>()
     .add_trait::<(flecs::OnInstantiate, flecs::Inherit)>();

 let base = world.entity().add::<Position>();
 let inst = world.entity().is_a_id(base); // short for .add_id((flecs::IsA::ID, base));

 // The following two queries are the same:
 let q1 = world.new_query::<&Position>();

 let q2 = world
     .query::<&Position>()
     .term_at(0)
     .self_()
     .up_id(flecs::IsA::ID)
     .build();
```

The following example shows a query that matches a component from a parent:

```rust
let parent = world.entity().add::<Position>();
let child = world.entity().child_of_id(parent); // short for .add_id((flecs::ChildOf::ID, base));

let q = world
.query::<&Position>()
.term_at(0).up()
.build();
```

The following example shows a query that traverses a custom relationship:

```rust
// Create a new traversable relationship
let contained_by = world.entity().add::<flecs::Traversable>();

let parent = world.entity().add::<Position>();

let child = world.entity().add_id((contained_by, parent));

let q = world
    .query::<&Position>()
    .term_at(0)
    .up_id(contained_by)
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

The following example shows a query that matches a component with the `(OnInstantiate, Inherit)` trait:

```c
// The following two queries are the same:
Position
Position(self|up)
```

The following example shows a query that matches a component from a parent:

```c
Position(up)
```

The following example shows a query that traverses a custom relationship:

```c
Position(up ContainedBy)
```

</li>
</ul>
</div>

### Variables
Query variables represent the state of a query while it is being evaluated. The most common form of state is "the entity (or table) against which the query is evaluated". While a query is evaluating an entity or table, it has to store it somewhere. In flecs, that "somewhere" is a query variable.

Consider this query example, written down with explicit term [sources](#source):

```
Position($this), Velocity($this)
```

The first term to encounter a variable is usually the one to populate it with all candidates that could match that term. Subsequent terms then use the already populated variable to test if it matches. If the condition matches, the query moves on to the next term. If the condition fails, the query moves back to the previous term and, if necessary, populates the variable with the next candidate. These kinds of conditions are usually referred to as [predicates](https://en.wikipedia.org/wiki/Predicate_(mathematical_logic)), and this evaluation process is called [backtracking](https://en.wikipedia.org/wiki/Backtracking).

This process effectively _constrains_ the possible results that a term could yield. By itself, the `Velocity` term would return all entities with the `Velocity` component, but because `$this` has been assigned already with entities that have `Position`, the term only feeds forward entities that have both `Position` and `Velocity`.

While using variables as [source](#source) is the most common application for variables, variables can be used in any part of the term. Consider constructing a query for all spaceships that are docked to a planet. A first attempt could look like this:

```
SpaceShip, (DockedTo, *)
```

When rewritten with explicit sources, the query looks like this:

```
SpaceShip($this), DockedTo($this, *)
```

This returns all spaceships that are docked to _anything_, instead of docked to planets. To constrain the result of this query, the wildcard used as target for the `DockedTo` relationship can be replaced with a variable. An example:

```
SpaceShip($this), DockedTo($this, $Location)
```

When the second term is evaluated for the first time, `$Location` will not yet be populated. This causes the term to do two things:

1. Test if the entity/table populated in `$this` has `(DockedTo, *)`
2. If so, populate `$Location` with the id matched by `*`.

After evaluating the second term, the `$Location` variable is populated with the location the spaceship is docked to. We can now use this variable in a new term, that constrains the location to only entities that have `Planet`:

```
SpaceShip($this), DockedTo($this, $Location), Planet($Location)
```

This query returns the desired result ("return all spaceships docked to a planet").

Variables can also be used to constrain matched components. Consider the following example query:

```
Serializable($Component), $Component($this)
```

This query returns serializable components for all entities that have at least one.

#### Setting Variables
By default variables are assigned while the query is being iterated, but variables can be set before query iteration to constrain the results of a query. Consider the previous example:

```
SpaceShip($this), DockedTo($this, $Location)
```

An application can set the `$this` variable or `$Location` variables, or both, before starting iteration to constrain the results returned by the query. This makes it possible to reuse a single query for multiple purposes, which provides better performance when compared to creating multiple queries.

#### Lookup variables
Variables can be used as the starting point of a by-name lookup. This can be useful when matching hierarchies that have a well-defined structure. An example:

```c
// Match all spaceship entities where the cockpit has no power
SpaceShip($this), !Powered($this.cockpit)
```

This query will look for an child entity named `cockpit` in the scope of the matched entity for `$this`, and use that entity to match with `Powered`. If no entity with the name `cockpit` is found, the term will evaluate to false.

The following sections show how to use queries in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

Query variables can be specified by setting the `name` member in combination with setting the `EcsIsVariable` bit in the `flags` member:

```c
// SpaceShip, (DockedTo, $Location), Planet($Location)
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { .id = SpaceShip },
    {
      .first.id = DockedTo, 
      .second = {
        .name = "Location",
        .id = EcsIsVariable
      }
    },
    {
      .id = Planet,
      .src = {
        .name = "Location",
        .id = EcsIsVariable
      }
    }
  }
});
```

Alternatively an application can specify a name with the `$` prefix to indicate it is a variable:

```c
// SpaceShip, (DockedTo, $Location), Planet($Location)
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { .id = SpaceShip },
    {
      .first.id = DockedTo, 
      .second = {
        .name = "$Location"
      }
    },
    {
      .id = Planet,
      .src = {
        .name = "$Location"
      }
    }
  }
});
```

An application can constrain the results of the query by setting the variable before starting iteration:

```c
ecs_entity_t earth = ecs_new_w(world, Planet);

// Find index for Location variable
int32_t location_var = ecs_query_find_var(q, "Location");

// Constrain results of iterator to return spaceships docked to Earth
ecs_iter_t it = ecs_query_iter(world, q);
ecs_iter_set_var(&it, location_var, earth);

// Iterate as usual
```

</li>
<li><b class="tab-title">C++</b>

Query variables can be specified by specifying a name with a `$` prefix:

```cpp
auto q = world.query_builder()
  .with<SpaceShip>()
  .with<DockedTo>().second("$Location")
  .with<Planet>().src("$Location")
  .build();
```

Alternatively, variables can also be specified using the `var` method:

```cpp
auto q = world.query_builder()
  .with<SpaceShip>()
  .with<DockedTo>().second().var("Location")
  .with<Planet>().src().var("Location")
  .build();
```

An application can constrain the results of the query by setting the variable before starting iteration:

```cpp
flecs::entity earth = world.entity();

int32_t location_var = q.find_var("Location");
q.iter().set_var(location_var, earth).each([]{
  // iterate as usual
});
```

Alternatively the variable name can be provided to `set_var` directly:

```cpp
flecs::entity earth = world.entity();

q.iter().set_var("Location", earth).each([]{
  // iterate as usual
});
```

</li>
<li><b class="tab-title">Rust</b>

Query variables can be specified by specifying a name with a `$` prefix:

```rust
let q = world
    .query::<()>()
    .with::<SpaceShip>()
    .with::<DockedTo>()
    .set_second_name("$Location")
    .with::<Planet>()
    .set_src_name("$Location")
    .build();
```

Alternatively, variables can also be specified using the `var` method:

```rust
let q = world
    .query::<()>()
    .with::<SpaceShip>()
    .with::<DockedTo>()
    .second()
    .set_var("$Location")
    .with::<Planet>()
    .src()
    .set_var("$Location")
    .build();
```

An application can constrain the results of the query by setting the variable before starting iteration:

```rust
let earth = world.entity();
let location_var = q.find_var("$Location").unwrap();

q.iterable().set_var(location_var, earth).each(|it| {
    // iterate as usual
});
```

Alternatively the variable name can be provided to `set_var` directly:

```rust
let earth = world.entity();

q.iterable().set_var_expr("$Location", earth).each(|it| {
    // iterate as usual
});
```

</li>
</ul>
</div>

### Member Value Queries
Queries can match against the values of component members if they are of the `ecs_entity_t` type, and the component type is described with the reflection framework. Member value queries make it possible to query on values that can change rapidly without requiring structural changes in the ECS. The tradeoff is that other than with the regular, union and toggle storage options there are no acceleration structures to speed up query evaluation, which means that a query has to evaluate each instance of the component.

The following sections show how to use queries in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
typedef struct {
  ecs_entity_t value;
} Movement;

// Register 'Movement' component and reflection data
ECS_COMPONENT(world, Movement);

ecs_struct(world, {
    .entity = ecs_id(Movement),
    .members = {
        { "value", ecs_id(ecs_entity_t) }
    }
});

// Create two entities for the direction
ecs_entity_t Left = ecs_new(world);
ecs_entity_t Right = ecs_new(world);

// Create two entities with different directions
ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Left }));
ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Right }));

// Create query that only matches e1
ecs_query_t *q = ecs_query(world, {
  .terms = {{ .first.name = "Movement.value", .second.id = Left }}
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
struct Movement {
  flecs::entity_t value;
};

// Register 'Movement' component and reflection data
world.component<Movement>()
  .member("value", &Movement::value);

// Create two entities for the direction
flecs::entity Left = world.entity();
flecs::entity Right = world.entity();

// Create two entities with different directions
flecs::entity e1 = world.entity().set(Movement{ Left });
flecs::entity e2 = world.entity().set(Movement{ Right });

// Create query that only matches e1
flecs::query<> q = world.query()
  .with("Movement.value", Left)
  .build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Rust API does not support member value queries until reflection is implemented. This is the Meta addon.
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

```
Movement.direction($this, Left)
```

</li>
</ul>
</div>

Member value queries can be used in combination with wildcards:

```
Movement.value($this, *)
```

Member value queries can be used in combination with variables:

```
Movement.value($this, $direction), $direction != Left
```

### Change Detection
Change detection makes it possible for applications to know whether data matching a query has changed. Changes are tracked at the table level, for each component in the table. While this is less granular than per entity tracking, the mechanism has minimal overhead, and can be used to skip entities in bulk.

Change detection works by storing a list of counters on tracked tables, where each counter tracks changes for a component in the table. When a component in the table changes, the corresponding counter is increased. An additional counter is stored for changes that add or remove entities to the table. Queries with change detection store a copy of the list of counters for each table in the cache, and compare counters to detect changes. To reduce overhead, counters are only tracked for tables matched with queries that use change detection.

The change detection feature cannot detect all changes. The following scenarios are detected by change detection:

- Anything that causes adding or removing a component, tag or pair
- Deleting an entity
- Setting a component value with `set`
- Calling `modified` for an entity/component
- Iterating the table _without skipping it_ with a query that has `inout` or `out` terms
- A change in tables matched by the query

The following scenarios are not detected by change detection:

- Modifying a component obtained by `ensure` without calling `modified`
- Modifying the value of a ref (`ecs_ref_t` or `flecs::ref`) without calling `modified`

A query with change detection enabled will only report a change for the components it matched with, or when an entity got added/removed to a matched table. A change to a component in a matched table that is not matched by the query will not be reported by the query.

By default queries do not use change detection. Change detection is automatically enabled when a function that requires change detection is called on the query, for example if an application calls `changed()` on the query. Once change detection is enabled it will stay enabled for both the query and the tables the query is matched with.

When a change occurred in a table matching a query, the query state for that table will remain changed until the table is iterated by the query.

When a query iterates a table for which changes are tracked and the query has `inout` or `out` terms that are matched with components of that table, the counter for those components will be increased by default. An application can indicate that no components were modified by skipping the table (see code examples).

> When a query uses change detection and has `out` or `inout` terms, its state will always be changed as iterating the query increases the table counters. It is recommended to only use terms with the `in` access modifier in combination with change detection.

The following sections show how to use change detection in the different language bindings. The code examples use cached queries, which is the only kind of query for which change detection is supported.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

The following example shows how the change detection API is used in C:

```c
// Query used for change detection. Note that change detection is not enabled on
// the query itself, but by calling change detection functions for the query.
ecs_query_cache_t *q_read = ecs_query(world, {
    .terms = {{ .id = ecs_id(Position), .inout = EcsIn }}
});

// Query used to create changes
ecs_query_cache_t *q_write = ecs_query(world, {
    .terms = {{ .id = ecs_id(Position) }} // defaults to inout
});

// Test if changes have occurred for anything matching the query. If this is the
// first call to the function, it will enable change detection for the query.
bool changed = ecs_query_changed(q_read, NULL);

// Setting a component will update the changed state
ecs_entity_t e = ecs_new(world);
ecs_set(world, e, Position, {10, 20});

// Iterating a query with inout/out terms will update the change state
ecs_iter_t it = ecs_query_iter(world, q_write);
while (ecs_query_next(&it)) {
  if (dont_change) {
    // If no changes are made to the iterated table, the skip function can be
    // called to prevent marking the matched components as dirty.
    ecs_iter_skip(&it);
  } else {
    // Iterate as usual. It does not matter whether the code actually writes the
    // components or not: when a table is not skipped, components matched with
    // inout or out terms will be marked dirty by the iterator.
  }
}

// Iterating tables with q_read will reset the changed state
it = ecs_query_iter(world, q_read);
while (ecs_query_next(&it)) {
  if (ecs_query_changed(q_read, &it)) {
    // Check if the current table has changed. The change state will be reset 
    // after the table is iterated, so code can respond to changes in individual
    // tables.
  }
}
```

</li>
<li><b class="tab-title">C++</b>

The following example shows how the change detection API is used in C++:

```cpp
// Query used for change detection. Note that change detection is not enabled on
// the query itself, but by calling change detection functions for the query.
flecs::query<const Position> q_read = world.query<const Position>();

// Query used to create changes
flecs::query<Position> q_write = world.query<Position>(); // defaults to inout

// Test if changes have occurred for anything matching the query. If this is the
// first call to the function, it will enable change detection for the query.
bool changed = q_read.changed();

// Setting a component will update the changed state
flecs::entity e = world.entity()
  .set<Position>({10, 20});

q_write.run([](flecs::iter& it) {
  if (it.next()) {
    if !changed {
      // If no changes are made to the iterated table, the skip function can be
      // called to prevent marking the matched components as dirty.
      it.skip();
    } else {
      // Iterate as usual. It does not matter whether the code actually writes the
      // components or not: when a table is not skipped, components matched with
      // inout or out terms will be marked dirty by the iterator.
    }
  }
});

q_read.run([](flecs::iter& it) {
  if (it.next()) {
    if (it.changed()) {
      // Check if the current table has changed. The change state will be reset 
      // after the table is iterated, so code can respond to changes in individual
      // tables.
    }
  }
});
```

</li>
<li><b class="tab-title">Rust</b>

The following example shows how the change detection API is used in C++:

```rust
// Query used for change detection. Note that change detection is not enabled on
// the query itself, but by calling change detection functions for the query.
let q_read = world.new_query::<&Position>();

// Query used to create changes
let q_write = world.new_query::<&mut Position>(); // defaults to inout

// Test if changes have occurred for anything matching the query. If this is the
// first call to the function, it will enable change detection for the query.
let changed = q_read.is_changed();

// Setting a component will update the changed state
let e = world.entity().set(Position { x: 10.0, y: 20.0 });

q_write.run(|mut it| {
    while it.next() {
        if !changed {
            // If no changes are made to the iterated table, the skip function can be
            // called to prevent marking the matched components as dirty.
            it.skip();
        } else {
            // Iterate as usual. It does not matter whether the code actually writes the
            // components or not: when a table is not skipped, components matched with
            // inout or out terms will be marked dirty by the iterator.
        }
    }
});

q_read.run(|mut it| {
    while it.next() {
        if it.is_changed() {
            // Check if the current table has changed. The change state will be reset
            // after the table is iterated, so code can respond to changes in individual
            // tables.
        }
    }
});
```

</li>
</ul>
</div>

### Sorting
Sorted queries allow an application to specify a component that entities should be sorted on. Sorting is enabled by setting the `order_by` function in combination with the component to order on. Sorted queries sort the tables they match with when necessary. To determine whether a table needs to be sorted, sorted queries use [change detection](#change-detection). A query determines whether a sort operation is needed when an iterator is created for it.

> Because sorting relies on change detection, it has the same limitations with respect to detecting changes. When using sorted queries, make sure a query is able to detect the changes necessary for knowing when to (re)sort.

Query sorting works best for data that does not change often, as the sorting process can be expensive. This is especially true for queries that match with many tables, as one step of the [sorting algorithm](#sorting-algorithm) scans all matched tables repeatedly to find an ordered set of slices.

An application should also prevent having sorted queries with conflicting sorting requirements. This can cause scenarios in which both queries are invalidating each others ordering, which can result in a resort each time an iterator is created for one of the conflicting queries.

Sorted queries are encouraged to mark the component used for sorting as `In`. If a sorted query has write access to the sorted component, iterating the query will invalidate its own order which can lead to continuous resorting.

Components matched through [traversal](#relationship-traversal) can be used to sort entities. This often results in more efficient sorting as component values can be used to sort entire tables, and as a result tables themselves do not have to be sorted.

#### Sorting Algorithm
Sorted queries use a two-step process to return entities in a sorted order. The sort algorithm used in both steps is quicksort. The first step sorts contents of all tables matched by the query. The second step is to find a list of ordered slices across the tables matched by the query. This second step is necessary to support datasets where ordered results have entities interleaved from multiple tables. An example data set:

Entity  | Components (table) | Value used for sorting
--------|--------------------|-----------------------
E1      | Position           | 1
E2      | Position           | 3
E3      | Position           | 4
E4      | Position, Velocity | 5
E5      | Position, Velocity | 7
E6      | Position, Mass     | 8
E7      | Position           | 10
E8      | Position           | 11

To make sure a query iterates the entities in the right order, it will iterate entities in the ordered tables to determine the largest slice of ordered entities in each table, which the query will iterate in order. Slices are computed during the sorting step. For the above set of entities slices would look like this:

Table              | Slice
-------------------|-------
Position           | 0..2
Position, Velocity | 3..4
Position, Mass     | 5
Position           | 6..7

To minimize time spent on sorting, the results of a sort are cached. The performance overhead of iterating an already sorted query is comparable to iterating a regular query, though for degenerate scenarios where a sort produces many slices for comparatively few tables the performance overhead can be significant.

The following sections show how to use sorting in the different language bindings. The code examples use cached queries, which is the only kind of query for which change detection is supported.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

The following example shows how to use sorted queries in C:

```c
ecs_query_t *q = ecs_query(world, {
    .terms = {
      // Use readonly term for component used for sorting
      { ecs_id(Depth), .inout = EcsIn }
    },
    .order_by = ecs_id(Position), // The component to use for sorting
    .order_by_callback = compare_position,
});
```

The function signature of the `order_by` function should look like the following example:

```c
int compare_depth(ecs_entity_t e1, const void *v1, ecs_entity_t e2, const void *v2) {
  const Depth *d1 = v1;
  const Depth *d2 = v2;
  return (d1->value > d2->value) - (d1->value < d2->value);
}
```

A query may only use entity identifiers to sort by not setting the `order_by` member:

```c
ecs_query_t *q = ecs_query(world, {
    .terms = {
      { ecs_id(Position) },
    },
    .order_by_callback = compare_entity,
});
```

The following example shows a function that sorts by entity id:

```c
int compare_entity(ecs_entity_t e1, const void *v1, ecs_entity_t e2, const void *v2) {
  return (e1 > e2) - (e1 < e2);
}
```

</li>
<li><b class="tab-title">C++</b>

The following example shows how to use sorted queries in C++:

```cpp
// Use readonly term for component used for sorting
auto q = world.query_builder<const Depth, Position>()
  .order_by<Depth>([](flecs::entity_t e1, const Depth *d1, flecs::entity_t e2, const Depth *d2) {
    return (d1->value > d2->value) - (d1->value < d2->value);
  })
  .build();
```

Queries may specify a component id if the component is not known at compile time:

```cpp
flecs::entity depth_id = world.component<Depth>();

auto q = world.query_builder<Position>()
  .with(depth_id).in()
  .order_by(depth_id, [](flecs::entity_t e1, const void *d1, flecs::entity_t e2, const void *d2) {
    // Generic sort code ...
  })
  .build();
```

Queries may specify zero for component id to sort on entity ids:

```cpp
auto q = world.query_builder<Position>()
  .order_by(0, [](flecs::entity_t e1, const void *d1, flecs::entity_t e2, const void *d2) {
    return (e1 > e2) - (e1 < e2);
  })
  .build();
```

</li>
<li><b class="tab-title">Rust</b>

The following example shows how to use sorted queries in Rust:

```rust
// Use readonly term for component used for sorting
let q = world
    .query::<(&Depth, &Position)>()
    .order_by::<Depth>(|e1, d1: &Depth, e2, d2: &Depth| {
        (d1.value > d2.value) as i32 - (d1.value < d2.value) as i32
    })
    .build();
```

Queries may specify a component id if the component is not known at compile time:

```rust
let depth_id = world.component::<Depth>();

let q = world
    .query::<&Position>()
    .with_id(depth_id)
    .set_in()
    .order_by_id(depth_id, |e1, d1: *const c_void, e2, d2: *const c_void| {
        let d1 = unsafe { &*(d1 as *const Depth) };
        let d2 = unsafe { &*(d2 as *const Depth) };
        (d1.value > d2.value) as i32 - (d1.value < d2.value) as i32
    })
    .build();
```

Queries may specify zero for component id to sort on entity ids:

```rust
let q = world
    .query::<&Position>()
    .order_by_id(0, |e1, _d1: *const c_void, e2, _d2: *const c_void| {
        (e1 > e2) as i32 - (e1 < e2) as i32
    })
    .build();
```

</li>
</ul>
</div>

### Grouping
Grouping is the ability of queries to assign an id ("group id") to a set of tables. Grouped tables are iterated together, as they are stored together in the query cache. Additionally, groups in the query cache are sorted by group id, which guarantees that tables with a lower group id are iterated after tables with a higher group id. Grouping is only supported for cached queries.

Group ids are local to a query, and as a result queries with grouping do not modify the tables they match with.

Grouping is the least granular, most efficient mechanism to order results of a query. Grouping does not rely on sorting individual tables or entities. Instead, sorting only happens when a new group id is introduced to a query, which is rare in many scenarios. Queries maintain an index that maps group ids to the first and last elements in the cache belonging to that group. This allows for tables to be inserted into the cache in a fast constant time operation, while also providing ordered access to tables.

The grouping mechanism is used internally by the `cascade` feature. Queries that use `cascade` use a computed hierarchy depth as group id. Because lower group ids are iterated before higher group ids, this provides in a bread-first topological sort of tables and entities that is almost free to maintain.

Queries may be grouped and [sorted](#sorting) at the same time. When combined, grouping takes precedence over sorting. Tables are first assigned to their groups in the query cache, after which each group is sorted individually.

An example of an internal query that uses `cascade` grouping in combination with [sorting](#sorting) is the builtin pipeline query. The pipeline query first groups systems by their depth in the `DependsOn` relationship tree. Within the depth-based groups systems are ordered based on their entity id, which ensures systems are iterated in order of declaration.

#### Group Iterators
A group iterator iterates over a single group of a grouped query. This can be useful when an application may need to match different entities based on the context of the game, such as editor mode, day/night, inside/outside or location in the world.

One example is that of an open game which is divided up into world cells. Even though a world may contain many entities, only the entities in cells close to the player need to be processed. Instead of creating a cached query per world cell, apps can create a single query grouped by world cell, and use group iterators to only iterate the necessary cells.

Grouped iterators, when used in combination with a good group_by function are one of the fastest available mechanisms for finding entities in Flecs. The feature provides the iteration performance of having a cached query per group, but without the overhead of having to maintain multiple caches. Whether a group has ten or ten thousand tables does not matter, which makes the feature an enabler for games with large numbers of entities.

The following sections show how to use grouping in the different language bindings. The code examples use cached queries, which is the only kind of query for which change detection is supported.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

The following example shows how grouping can be used to group entities that are in the same game region.

```c
ecs_entity_t Region = ecs_new(world);
ecs_entity_t Unit = ecs_new(world);

ecs_entity_t Region_01 = ecs_new(world);
ecs_entity_t Region_02 = ecs_new(world);

// Example of entities created in different regions
ecs_entity_t unit_01 = ecs_new_w_id(world, Unit);
ecs_entity_t unit_02 = ecs_new_w_id(world, Unit);

ecs_add_pair(world, unit_01, Region, Region_01);
ecs_add_pair(world, unit_02, Region, Region_02);

// Create query that groups entities that are in the same region
ecs_query(world, {
  .terms = {{ Unit }},
  .group_by_callback = group_by_target, // function that groups by relationship target
  .group_by = Region // optional, passed to group_by function
});
```

The following example shows what the implementation of `group_by_target` could look like:

```c
uint64_t group_by_target(
  ecs_world_t *world, ecs_table_t *table, ecs_id_t id, void *ctx)
{
  // Use id (Region) to find target for relationship
  ecs_id_t pair = 0;
  if (ecs_search(world, table, ecs_pair(id, EcsWildcard), &pair) != -1) {
    return ecs_pair_second(world, pair); // Return second element (target)
  } else {
    return 0; // Table does not have relationship
  }
}
```

When no value for `group_by_callback` is provided, it will default to an internal function with the same behavior as `group_by_target`. An example:

```c
// Create query that groups entities that are in the same region
ecs_query(world, {
  .terms = {{ Unit }},
  .group_by = Region
});
```

To iterate entities in a single group, use the `ecs_iter_set_group` function:

```c
ecs_iter_t it = ecs_query_iter(world, q);
ecs_iter_set_group(&it, Region_01);

while (ecs_query_next(&it)) {
  // iterate as usual
}
```

</li>
<li><b class="tab-title">C++</b>

The following example shows how grouping can be used to group entities that are in the same game region.

```cpp
struct Region { };
struct Unit { };

flecs::entity Region_01 = world.entity();
flecs::entity Region_02 = world.entity();

// Example of entities created in different regions
flecs::entity unit_01 = world.entity()
  .add<Unit>()
  .add<Region>(Region_01);

flecs::entity unit_02 = world.entity()
  .add<Unit>()
  .add<Region>(Region_02);

// Create query that groups entities that are in the same region
flecs::query<> q = world.query_builder()
  .with<Unit>()
  .group_by<Region>([](
      flecs::world_t *world,
      flecs::table_t *table,
      flecs::id_t id, 
      void *ctx) 
    {
      flecs::entity_t result = 0;
      ecs_id_t pair = 0;

      // Use id (Region) to find target for relationship
      if (ecs_search(world, table, ecs_pair(id, EcsWildcard), &pair) != -1) {
        result = ecs_pair_second(world, pair); // Return second element (target)
      }

      return result;
    })
  .build();
```

When no `group_by` functions, it will default to an internal function with the same behavior as the previous example. An example:

```cpp
// Create query that groups entities that are in the same region
flecs::query<> q = world.query_builder()
  .with<Unit>()
  .group_by<Region>()
  .build();
```

To iterate entities in a single group, use the `set_group` function:

```cpp
q.set_group(Region_01).each([](flecs::entity e) {
  // iterate as usual
});
```

</li>
<li><b class="tab-title">Rust</b>

This section for Rust is unfinished. For code examples, see the group_by examples in the `examples/flecs/queries` folder. Note, this still partially uses the C API. Until the Rust API is updated, this section will remain unfinished.

The following example shows how grouping can be used to group entities that are in the same game region.

```rust
// see example in examples folder under query/group_by
```

When no `group_by` functions, it will default to an internal function with the same behavior as the previous example. An example:

```rust
// see example in examples folder under query/group_by
```

To iterate entities in a single group, use the `set_group` function:

```rust
// see example in examples folder under query/group_by
```

</li>
</ul>
</div>

### Component Inheritance
Component inheritance allows for a query to match entities with a component and all subsets of that component, as defined by the `IsA` relationship. Component inheritance is enabled for all queries by default, for components where it applies.

It is possible to prevent inheriting from a component from by adding the [Final](Relationships.md#final-property) property. Queries for components with the `Final` property will not attempt to resolve component inheritance.

Inheritance relationships can, but are not required to mirror inheritance of the types used as long as it does not impact the layout of the type. Component inheritance is most often used with tags.

The following sections show how to use component inheritance in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

The following example shows a query that uses component inheritance to match entities:

```c
ecs_entity_t Unit = ecs_new(world);
ecs_entity_t MeleeUnit = ecs_new_w_pair(world, EcsIsA, Unit);
ecs_entity_t RangedUnit = ecs_new_w_pair(world, EcsIsA, Unit);

ecs_entity_t unit_01 = ecs_new_w_id(world, MeleeUnit);
ecs_entity_t unit_02 = ecs_new_w_id(world, RangedUnit);

// Matches entities with Unit, MeleeUnit and RangedUnit
ecs_query_t *q = ecs_query(world, {
  .terms = {{ Unit }}
});

// Iterate as usual
```

</li>
<li><b class="tab-title">C++</b>

The following example shows a query that uses component inheritance to match entities:

```cpp
flecs::entity Unit = ecs_new(world);
flecs::entity MeleeUnit = world.entity().is_a(Unit);
flecs::entity RangedUnit = world.entity().is_a(Unit);

flecs::entity unit_01 = world.entity().add(MeleeUnit);
flecs::entity unit_02 = world.entity().add(RangedUnit);

// Matches entities with Unit, MeleeUnit and RangedUnit
flecs::query<Unit> q = world.query<Unit>();

// Iterate as usual
```

</li>
<li><b class="tab-title">Rust</b>

The following example shows a query that uses component inheritance to match entities:

```rust
#[derive(Component)]
struct Unit;

let unit = world.component::<Unit>();

let melee_unit = world.entity().is_a::<Unit>();
let ranged_unit = world.entity().is_a::<Unit>();

let unit_01 = world.entity().add_id(melee_unit);
let unit_02 = world.entity().add_id(ranged_unit);

// Matches entities with Unit, MeleeUnit and RangedUnit
let q = world.query::<&Unit>();

// Iterate as usual
```

</li>
</ul>
</div>

### Transitive Relationships
When a [transitive relationship](Relationships.md#transitive-relationships) is used by a query, a query will automatically look up or test against pairs that satisfy the transitive property. Transitivity is usually defined as:

> If R(X, Y) and R(Y, Z) then R(X, Z)

In this example, `R` is the transitive relationship and `X`, `Y` and `Z` are entities that are used both as source and second element. A typical example of a transitive relationship is `LocatedIn`. If Bob (X) is located in Manhattan (Y) and Manhattan (Y) is located in New York (Z), then Bob (X) is also located in New York (Z). Therefore "located in" is transitive.

A relationship can be made transitive by adding the [transitive](Relationships.md#transitive-property) property. This would ensure that both `(LocatedIn, ManHattan)` and `(LocatedIn, NewYork)` queries would match `Bob`. When the location is replaced with a variable, the variable will assume all values encountered while traversing the transitive relationship. For example, `(LocatedIn, $Place)` would return results with `Place = [ManHattan, NewYork]` for `Bob`, and a result with `Place = [NewYork]` for `ManHattan`.

An example of a builtin relationship that has the `Transitive` property is the `IsA` relationship.

Transitive traversal can be disabled for a term with a transitive relationship by adding the `Self` flag to the second element of the pair.

The following sections show how to use transitive relationships in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

The following example shows a query that uses transitivity to match entities that are located in New York:

```c
// Create LocatedIn relationship with transitive property
ecs_entity_t LocatedIn = ecs_new_w_id(world, EcsTransitive);

ecs_entity_t NewYork = ecs_new(world);
ecs_entity_t Manhattan = ecs_new_w_pair(world, LocatedIn, NewYork);
ecs_entity_t CentralPark = ecs_new_w_pair(world, LocatedIn, Manhattan);
ecs_entity_t Bob = ecs_new_w_pair(world, LocatedIn, CentralPark);

// Matches ManHattan, CentralPark, Bob
ecs_query_t *q = ecs_query(world, {
  .terms = {{ ecs_pair(LocatedIn, NewYork) }}
});

// Iterate as usual
```

Queries for transitive relationships can be compared with variables. This query returns all locations an entity is in:

```c
// Matches:
//  - ManHattan (Place = NewYork)
//  - CentralPark (Place = ManHattan, NewYork)
//  - Bob (Place = CentralPark, ManHattan, NewYork)
ecs_query_t *q = ecs_query(world, {
  .terms = {{ .first.id = LocatedIn, .second.name = "$Place" }}
});
```

Variables can be used to constrain the results of a transitive query. The following query returns locations an entity is in that are a city:

```c
// Add City property to NewYork
ecs_entity_t City = ecs_new(world);
ecs_add_id(world, NewYork, City);

// Matches:
//  - ManHattan (Place = NewYork)
//  - CentralPark (Place = NewYork)
//  - Bob (Place = NewYork)
ecs_query_t *q = ecs_query(world, {
  .terms = {
    { .first.id = LocatedIn, .second.name = "$Place" },
    { .id = City, .src.name = "$Place" }
  }
});
```

</li>
<li><b class="tab-title">C++</b>

The following example shows a query that uses transitivity to match entities that are located in New York:

```cpp
// Create LocatedIn relationship with transitive property
struct LocatedIn { };

world.component<LocatedIn>().add(flecs::Transitive);

flecs::entity NewYork = world.entity();
flecs::entity Manhattan = world.entity().add<LocatedIn>(NewYork);
flecs::entity CentralPark = world.entity().add<LocatedIn>(Manhattan);
flecs::entity Bob = world.entity().add<LocatedIn>(CentralPark);

// Matches ManHattan, CentralPark, Bob
flecs::query<> q = world.query_builder()
  .with<LocatedIn>(NewYork)
  .build();

// Iterate as usual
```

Queries for transitive relationships can be compared with variables. This query returns all locations an entity is in:

```cpp
// Matches:
//  - ManHattan (Place = NewYork)
//  - CentralPark (Place = ManHattan, NewYork)
//  - Bob (Place = CentralPark, ManHattan, NewYork)
flecs::query<> q = world.query_builder()
  .with<LocatedIn>().second("$Place")
  .build();
```

Variables can be used to constrain the results of a transitive query. The following query returns locations an entity is in that are a city:

```cpp
struct City { };

// Add City property to NewYork
NewYork.add(City);

// Matches:
//  - ManHattan (Place = NewYork)
//  - CentralPark (Place = NewYork)
//  - Bob (Place = NewYork)
flecs::query<> q = world.query_builder()
  .with<LocatedIn>().second("$Place")
  .with<City>().src("$Place")
  .build();
```

</li>

<li><b class="tab-title">Rust</b>

The following example shows a query that uses transitivity to match entities that are located in New York:

```rust
// Create LocatedIn relationship with transitive property
#[derive(Component)]
struct LocatedIn;

world.component::<LocatedIn>().add::<flecs::Transitive>();

let new_york = world.entity();
let manhattan = world.entity().add_first::<LocatedIn>(new_york);
let central_park = world.entity().add_first::<LocatedIn>(manhattan);
let bob = world.entity().add_first::<LocatedIn>(central_park);

// Matches ManHattan, CentralPark, Bob
let q = world
    .query::<()>()
    .with_first::<LocatedIn>(new_york)
    .build();

// Iterate as usual
```

Queries for transitive relationships can be compared with variables. This query returns all locations an entity is in:

```rust
// Matches:
//  - ManHattan (Place = NewYork)
//  - CentralPark (Place = ManHattan, NewYork)
//  - Bob (Place = CentralPark, ManHattan, NewYork)
let q = world
    .query::<()>()
    .with::<LocatedIn>()
    .set_second_name("$Place")
    .build();
```

Variables can be used to constrain the results of a transitive query. The following query returns locations an entity is in that are a city:

```rust
#[derive(Component)]
struct City;

// Add City property to NewYork
new_york.add::<City>();

// Matches:
//  - ManHattan (Place = NewYork)
//  - CentralPark (Place = NewYork)
//  - Bob (Place = NewYork)

let q = world
    .query::<()>()
    .with::<LocatedIn>()
    .set_second_name("$Place")
    .with::<City>()
    .set_src_name("$Place")
    .build();
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

Transitivity in a query is enabled by adding the `Transitive` property to a relationship. As a result, a query for a transitive relationship looks the same as a query for a non-transitive relationship. The following examples show the queries used in the C/C++ examples:

Match all entities located in New York:

```
(LocatedIn, NewYork)
```

Return all locations an entity is in:

```
(LocatedIn, $Place)
```

Return the city entities are in:

```
(LocatedIn, $Place), City($Place)
```

</li>
</ul>
</div>

### Reflexive Relationships
When a query matches a [reflexive](Relationships.md#reflexive-property) relationship, a query term will evaluate to true if the source and target are equal. Reflexivity can be defined as:

> R(X, X)

For example, if relationship IsA (R) is reflexive, then a Tree (X) is a Tree (X). An example of a builtin relationship that has the `Reflexive` property is the `IsA` relationship.

The following sections show how to use transitive relationships in the different language bindings.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

The following example shows a query that uses the `IsA` reflexive relationship:

```c
ecs_entity_t Tree = ecs_new(world);
ecs_entity_t Oak = ecs_new_w_pair(world, EcsIsA, Tree);

// Matches Tree, Oak
ecs_query_t *q = ecs_query(world, {
  .terms = {{ ecs_pair(EcsIsA, Tree )}}
});
```

</li>
<li><b class="tab-title">C++</b>

The following example shows a query that uses the `IsA` reflexive relationship:

```cpp
flecs::entity Tree = world.entity();
flecs::entity Oak = world.entity().is_a(Tree);

// Matches Tree, Oak
flecs::query<> q = world.query_builder()
  .with(flecs::IsA, Tree)
  .build();
```

</li>
<li><b class="tab-title">Rust</b>

The following example shows a query that uses the `IsA` reflexive relationship:

```rust
let tree = world.entity();
let oak = world.entity().is_a_id(tree);

// Matches Tree, Oak
let q = world
.query::<()>()
.with_first::<flecs::IsA>(tree)
.build();

// Iterate as usual
```

</li>
<li><b class="tab-title">Flecs Query Language</b>

Reflexivity in a query is enabled by adding the `Reflexive` property to a relationship. As a result, a query for a reflexive relationship looks the same as a query for a non-reflexive relationship. The following example shows the query used in the C/C++ examples:

```
(IsA, Tree)
```

</li>
</ul>
</div>
