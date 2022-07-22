# Queries
At the core of an Entity Component System are queries, which make it possible to find entities matching a list of conditions in realtime, for example:

```
Position, Velocity
```

This query returns all entities that at least have the `Position` and `Velocity` components. Queries provide direct access to cache efficient storages of matched components, which gives applications the ability to process large numbers (think millions) of entities each frame.

## Highlights
Here are some of the highlights of the Flecs query implementation:

- Queries can cache cache results, which removes search overhead from time critical game loops

- Queries can efficiently traverse [entity relationship](#Relations) graphs, reducing the need for building custom hard to maintain data structures ([blog](https://ajmmertens.medium.com/building-games-in-ecs-with-entity-relationships-657275ba2c6c)).

- Queries can be created at runtime, and do not need compile time knowledge of the components they query for.

- Queries can match components that are created at runtime.

- Queries support `and`, `or`, `not` and `optional` operators.

- Queries can combine components from from multiple entities, like for a transform system that uses `Position` component from an entity and its parent.

- Queries can be observed, allowing applications to get notified when entities start or stop matching a query.

- Query results are self describing, which means iterators can be passed to generic code which can then do things like [serializing it to JSON](https://github.com/SanderMertens/flecs/blob/master/include/flecs/addons/json.h).

- Queries can be created with the regular API or by parsing a query string, making it possible to [create tools that create queries at runtime](https://www.flecs.dev/explorer/).

- Queries support [component inheritance](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/rules/component_inheritance).

## Definitions

| Name        | Description |
|--------------|-------------|
| Query        | A list of conditions used to match entities |
| Term         | A single element of a query |
| Id           | Something that can be matched, added and removed |
| Component    | Id with a single element (same as an entity id) |
| Pair         | Id with two elements |
| Tag          | Component or pair not associated with data |
| Relationship | Used to refer to first element of pair |
| Target       | Used to refer to second element of pair |
| Source       | Entity on which a term is matched |
| Iterator     | Object used to iterate a query |
| Field        | A single value or array of values made available by an iterator. An iterator usually provides a field per query term. |

## Query Types
Flecs has different query types, which are optimized for different kinds of use cases. This section provides a brief overview of each kind:

 ### Filters
 Cheap to create, low overhead, reasonably efficient to iterate. Filters are good for ad-hoc queries with runtime-defined conditions. An example:
 
 ```c
 ecs_filter_t *f = ecs_filter(world, {
    .terms = {
        { ecs_id(Position) }, { ecs_id(Velocity) }
    }
});

ecs_iter_t it = ecs_filter_iter(world, f);
while (ecs_filter_next(&it)) {
    Position *p = ecs_term(&it, Position, 1);
    Velocity *v = ecs_term(&it, Velocity, 2);

    for (int i = 0; i < it.count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}
```
```cpp
flecs::filter<Position, Velocity> f = 
    world.filter<Position, Velocity>();

f.each([](Position& p, Velocity& v) {
    p.x += v.x;
    p.y += v.y;
});
```

### Cached Queries
Caches the output of a filter. Cached queries are more expensive to create, have higher overhead, and are the fastest to iterate. Cached queries are the default for systems. An example:

```c
ecs_query_t *q = ecs_query(world, {
    .filter.terms = {
        { ecs_id(Position) }, { ecs_id(Velocity) }
    }
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
    Position *p = ecs_term(&it, Position, 1);
    Velocity *v = ecs_term(&it, Velocity, 2);

    for (int i = 0; i < it.count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}
```
```cpp
flecs::query<Position, Velocity> q = 
    world.query<Position, Velocity>();

q.each([](Position& p, Velocity& v) {
    p.x += v.x;
    p.y += v.y;
});
```

### Rules
Constraint-based query engine capable of traversing graphs. Rules are more expensive to create than filters, have low overhead, and their iteration performance depends on query complexity. An example:

```c
ecs_rule_t *r = ecs_rule(world, {
    .terms = {
        { ecs_id(Position) }, { ecs_id(Velocity) }
    }
});

ecs_iter_t it = ecs_rule_iter(world, r);
while (ecs_rule_next(&it)) {
    Position *p = ecs_term(&it, Position, 1);
    Velocity *v = ecs_term(&it, Velocity, 2);

    for (int i = 0; i < it.count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}
```
```cpp
flecs::rule<Position, Velocity> r = 
    world.rule<Position, Velocity>();

r.each([](Position& p, Velocity& v) {
    p.x += v.x;
    p.y += v.y;
});
```

For more information on thow each implementation performs, see [Query Performance](#query-performance).

## Query Creation
This section explains how to create queries in the different language bindings and the flecs query DSL.

### Query Descriptors (C)
Query descriptors are the C API for creating queries. The API uses a type called `ecs_filter_desc_t`, to describe the structure of a query. This type is used to create all query kinds (`ecs_filter_t`, `ecs_query_t`, `ecs_rule_t`). An example:

```c
ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_id(Position) }, { ecs_id(Velocity) },
  }
});
```

The example shows the short notation, which looks like this when expanded:

```c
ecs_filter_t *f = ecs_filter_init(world, &(ecs_filter_desc_t){
  .terms = {
    { ecs_id(Position) }, { ecs_id(Velocity) },
  }
});
```

Query descriptors can also be used by the C++ API. However because C++ does not support taking the address of a temporary, and not all language revisions support designated initializers, query descriptors in C++ should be used like this:

```c
ecs_filter_desc_t desc = {}; // Zero-initialize the struct
desc.terms[0].id = ecs_id(Position);
desc.terms[1].id = ecs_id(Velocity);
ecs_filter_t *f = ecs_filter_init(world, &desc);
```

Cached queries are created using the `ecs_query_init` function and `ecs_query_desc_t` type. The `ecs_query_desc_t` type embeds the `ecs_filter_desc_t` type in a `filter` member. Rules are created using the `ecs_rule_init` function and `ecs_filter_desc_t` type. Filters, cached queries and rules are deleted respectively with the `ecs_filter_fini`, `ecs_query_fini` and `ecs_rule_fini` functions.

### Query Builder (C++)
Query builders are the C++ API for creating queries. The builder API is built on top of the descriptor API, and adds a layer of convenience and type safety that matches modern idiomatic C++. The builder API is implemented for all query kinds (filters, cached queries, rules). An example of a simple query:

```cpp
flecs::filter<Position, const Velocity> f = 
    world.filter<Position, const Velocity>();
```

Queries created with template arguments provide a type safe way to iterate components:

```cpp
f.each([](Position& p, const Velocity& v) {
    p.x += v.x;
    p.y += v.y;
});
```

The builder API allows for incrementally constructing queries:

```cpp
flecs::filter<Position> q = world.filter_builder<Position>();
f.term<const Velocity>();

if (add_npc) {
    f.term<Npc>(); // Conditionally add
}

f.build(); // Create query
```

### Query DSL
The query DSL (domain specific language) is a string format that can represent a string. The query DSL is used by some convenience macros in the C API like `ECS_SYSTEM` and `ECS_OBSERVER`, and makes it easier to create queries at runtime for tools like https://www.flecs.dev/explorer/. The query DSL requires the `FLECS_PARSER` addon to be included in a build. An example of a simple query in the DSL:

```
Position, [in] Velocity
```

An example of how the DSL is used with the `ECS_SYSTEM` convenience macro:

```c
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, [in] Velocity);
```

Queries can be created from expressions with both the descriptor and builder APIs:

```c
ecs_filter_t *f = ecs_filter(world, {
  .expr = "Position, [in] Velocity"
});
```
```cpp
flecs::filter<> f = world.filter_builder()
  .expr("Position, [in] Velocity")
  .build();
```

## Query Iteration
This section describes the different ways a query can be iterated, and where they make sense.

### Iterators (C)
In the C API an iterator object of type `ecs_iter_t` can be created for each of the query kinds, using the `ecs_filter_iter`, `ecs_query_iter` and `ecs_rule_iter` functions. This iterator can then be iterated with the respective `next` functions: `ecs_filter_next`, `ecs_query_next` and `ecs_rule_next`.

An iterator can also be iterated with the `ecs_iter_next` function which is slightly slower, but does not require knowledge about the source the iterator was created for.

An example:

```c
ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_id(Position) }, { ecs_id(Velocity) },
  }
});

ecs_iter_t it = ecs_filter_iter(world, f);

// Outer loop: matching tables
while (ecs_filter_next(&it)) {
  Position *p = ecs_term(&it, Position, 1); // 1st term
  Velocity *v = ecs_term(&it, Velocity, 2); // 2nd term

  // Inner loop: entities in table
  for (int i = 0; i < it.count; i ++) {
    p[i].x += v[i].x;
    p[i].y += v[i].y;
  }
}
```

Iteration is split up into two loops: the outer loop which iterates tables, and the inner loop which iterates entities. This approach provides direct access to component arrats, which allows for optimizations like auto-vectorization.

The indices provided to the `ecs_term` function must correspond with the order in which terms have been specified in the query. This index starts counting from `1`, with index `0` reserved for the array containing entity ids.

### Each (C++)
The `each` function is the default and often fastest approach for iterating a query in C++. `each` can be called directly on a `flecs::filter`, `flecs::query` and `flecs::rule`. An example:

```cpp
auto f = world.filter<Position, const Velocity>();

f.each([](Position& p, const Velocity& v) {
    p.x += v.x;
    p.y += v.y;
});
```

A `flecs::entity` can be added as first argument:

```cpp
auto f = world.filter<Position>();

f.each([](flecs::entity e, Position& p) {
    std::cout << e.name() << ": " 
              << p.x << ", " << p.y 
              << std::endl;
});
```

A `flecs::iter` and `size_t` argument can be added:

```cpp
auto f = world.filter_builder<Position>()
  .term(Likes, flecs::Wildcard)
  .build();

f.each([](flecs::iter& it, size_t index, Position& p) {
    flecs::entity e = it.entity(index);
    std::cout << e.name() << ": " 
              << it.id(2).str() // prints pair
              << std::endl;
});
```

When a query contains a template argument that is an empty type (a struct without any members), it should be passed by value instead of by reference:

```cpp
struct Tag { };

auto f = world.filter<Tag>();

f.each([](flecs::entity e, Tag) {
    std::cout << e.name() << std::endl;
});
```

The `flecs::iter` object provides introspection into the current result, and makes it possible to obtain components outside of the query type.

### Iter (C++)
The `iter` function has an outer and inner loop (similar to C iterators) which provide more control over how to iterate entities in a table. The function for example makes it possible to iterate entities backwards, or multiple times per table.

An example:

```cpp
auto f = world.filter<Position, const Velocity>();

f.iter([](flecs::iter& it, Position *p, Velocity *v) {
    // Inner loop
    for (auto i : it) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        std::cout << it.entity(i).name() << ": " 
            << p.x << ", " << p.y 
            << std::endl;
    }
});
```

The component arguments may be ommitted, and can be obtained from the iterator object. This makes it possible to pass functions to `iter` that need no knowledge of the query type:

```cpp
auto f = world.filter<Position, const Velocity>();

f.iter([](flecs::iter& it) {
    auto p = it.term<Position>(1);
    auto v = it.term<const Velocity>(2);

    for (auto i : it) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
});
```

### Iteration safety
Entities can be moved between tables when components are added or removed. This can cause unwanted side effects while iterating a table, like iterating an entity twice, or missing an entity. To prevent this from happening, a table is locked by the C++ `each` and `iter` functions, meaning no entities can be moved from or to it.

When an application attempts to add or remove components to an entity in a table being iterated over, this can throw a runtime assert. An example:

```cpp
auto f = world.filter<Position>();

f.each([](flecs::entity e, Position&) {
    e.add<Velocity>(); // throws locked table assert
});
```

This can be addressed by deferring operations while the query is being iterated:

```cpp
auto f = world.filter<Position>();

world.defer([&]{
    f.each([](flecs::entity e, Position&) {
        e.add<Velocity>(); // OK
    });
}); // operations are executed here
```

An application can also use the `defer_begin` and `defer_end` functions which achieve the same goal:

```cpp
auto f = world.filter<Position>();

world.defer_begin();

f.each([](flecs::entity e, Position&) {
    e.add<Velocity>(); // OK
});

world.defer_end(); // operations are executed here
```

Code ran as part of a system defers operations by default.

## Query Reference
This section goes over the different features of queries and how they can be expressed by the query descriptor API, query builder API and in the query DSL.

### Components
> *Supported by: filters, cached queries, rules*

A component is any single _entity_ id that can be added to an entity. This includes components and tags, as their handles are entities. By default an entity needs to have at least all components of a query. Consider:

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

#### Query Decsriptor (C)
To query for a component in C, the `id` field of a term can be set:

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);

ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { .id = ecs_id(Position) }, 
    { .id = ecs_id(Velocity) }
  }
});
```

The `id` field is guaranteed to be the first member of a term, which allows the previous code to be rewritten in this shorter form:

```c
ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity) }
  }
});
```

The `ecs_id` macro converts the component typename into the variable name that holds the component identifier. This macro is required for components created with `ECS_COMPONENT`, but not when querying for regular tags/entities:

```c
ECS_TAG(world, Npc);
ecs_entity_t Platoon_01 = ecs_new_id(world);

ecs_filter_t *f = ecs_filter(world, {
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

ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { .first.name = "Position" }, 
    { .first.name = "Velocity" }
  }
});
```

#### Query Builder (C++)
The easiest way to query for components in C++ is to pass them as template arguments to the query factory function:

```cpp
flecs::filter<Position, const Velocity> f = 
    world.filter<Position, const Velocity>();
```

This changes the returned query type, which determines the type of the function used to iterate the query:

```cpp
f.each([](Position& p, const Velocity& v) { });
```

The builder API makes it possible to add components to a query without modifying the query type:

```cpp
flecs::filter<Position> f = 
  world.filter_builder<Position>()
    .term<const Velocity>()
    .build();
```

When template arguments are mixed with the builder API, the components added by the `term` function will be placed after the components provided as template arguments.

The builder API makes it possible to query for regular entity ids created at runtime:

```cpp
flecs::entity Npc = world.entity();
flecs::entity Platoon_01 = world.entity();

flecs::filter<> f = world.filter_builder()
  .term(Npc)
  .term(Platoon_01)
  .build();
```

Components, can also be queried for by name. To query for component types by name, they have to be used or registered first.

```cpp
// Register component type so we can look it up by name
world.component<Position>();

// Create entity with name so we can look it up
flecs::entity Npc = world.entity("Npc");

flecs::filter<> f = world.filter_builder()
  .term("Position")
  .term("Npc")
  .build();
```

#### Query DSL
To query for a components in the query DSL they can be specified in a comma separated list of identifiers. The rules for resolving identifiers are the same as the `ecs_lookup_fullpath` / `world.lookup` functions. An example:

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

When an identifier in the query DSL consists purely out of numeric characters it is converted to an entity id. If in the previous example `Npc` has id `100` and `Platoon_01` has id `101`, the following query string would be equivalent:

```
100, 101
```

The `,` symbol in the query DSL is referred to as the `and` operator, as an entity must have all comma-separated components in order to match the query.

### Wildcards
> *Supported by: filters, cached queries, rules*

Wildcards allow a single query term to match with more than one (component) ids. Flecs supports two kinds of wildcards:

| Name  | DSL Symbol | C identifier  | C++ identifier | Description |
|----------|-----|---------------|----------------|-------------|
| Wildcard | `*` | `EcsWildcard` | `flecs::Wildcard` | Match all |
| Any      | `_` | `EcsAny` | `flecs::Any` | Match at most one |

The `Wildcard` wildcard will return an individual result for anything that it matches. The query in the following example will return twice for entity `e`, once for component `Position` and once for component `Velocity`:

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);

ecs_entity_t e = ecs_new_id(world);
ecs_add(world, e, Position);
ecs_add(world, e, Velocity);

ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { EcsWildcard }
  }
});
```
```cpp
flecs::entity e = world.entity()
    .add<Position>()
    .add<Velocity>();

flecs::filter<> f = world.filter_builder()
    .term(flecs::Wildcard)
    .build();
```

The `Any` wildcard returns a single result for the first component that it matches. The query in the following example will return once for entity `e`:

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);

ecs_entity_t e = ecs_new_id(world);
ecs_add(world, e, Position);
ecs_add(world, e, Velocity);

ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { EcsAny }
  }
});
```
```cpp
flecs::entity e = world.entity()
    .add<Position>()
    .add<Velocity>();

flecs::filter<> f = world.filter_builder()
    .term(flecs::Any)
    .build();
```

When using the `Any` wildcard it is undefined which component will be matched, as this can be influenced by other parts of the query. It is guaranteed that iterating the same query twice on the same dataset will produce the same result.

Wildcards are particularly useful when used in combination with pairs (next section).

### Pairs
> *Supported by: filters, cached queries, rules*

A pair is just like a component in that it can be added to an entity, but instead of a single entity, a pair encodes _two_ entity ids.

The elements of a pair are allowed to be wildcards. When a query pair contains the `Wildcard` wildcard, a query returns a result for each matching pair on an entity. When a query pair returns an `Any` wildcard, the query returns at most a single matching pair on an entity.

The following sections describe how to create queries for pairs in the different language bindings.

#### Query Descriptor (C)
To query for a pair in C, the `id` field of a term can be set to a pair using the `ecs_pair` macro:

```c
ecs_entity_t Likes = ecs_new_id(world);
ecs_entity_t Bob = ecs_new_id(world);

ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { .id = ecs_pair(Likes, Bob) },
  }
});
```

The `id` field is guaranteed to be the first member of a term, which allows the previous code to be rewritten in this shorter form:

```c
ecs_entity_t Likes = ecs_new_id(world);
ecs_entity_t Bob = ecs_new_id(world);

ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_pair(Likes, Bob) },
  }
});
```

When an element of the pair is a component type, use the `ecs_id` macro to obtain the identifier to the id variable of the component type:

```c
ECS_COMPONENT(world, Eats);
ecs_entity_t Apples = ecs_new_id(world);

ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_pair(ecs_id(Eats), Apples) },
  }
});
```

The `ecs_isa`, `ecs_childof` and `ecs_dependson` convenience macros can be used to create pairs for builtin relationships. The two queries in the next example are equivalent:

```c
ecs_filter_t *f_1 = ecs_filter(world, {
  .terms = {
    { ecs_pair(EcsChildOf, parent) },
  }
});

ecs_filter_t *f_2 = ecs_filter(world, {
  .terms = {
    { ecs_childof(parent) },
  }
});
```

Pair queries can be created by setting their individual elements in the `first.id` and `second.id` members of a term:

```c
ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { .first.id = Eats, .second.id = Apples },
  }
});
```

Alternatively, one or both elements of a pair can be resolved by name. The two queries in the next example are equivalent:

```c
// ECS_TAG creates named entities
ECS_TAG(world, Eats);
ECS_TAG(world, Apples);

ecs_filter_t *f_1 = ecs_filter(world, {
  .terms = {
    { .first.name = "Eats", .second.id = Apples },
  }
});

ecs_filter_t *f_2 = ecs_filter(world, {
  .terms = {
    { .first.name = "Eats", .second.name = "Apples" },
  }
});
```

When a query pair contains a wildcard, the `ecs_term_id` function can be used to determine the id of the pair element that matched the query:

```c
ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_pair(Likes, EcsWildcard) },
  }
});

ecs_iter_t it = ecs_filter_iter(world, f);
while (ecs_filter_next(&it)) {
    ecs_id_t id = ecs_term_id(&it, 1);
    ecs_entity_t second = ecs_pair_second(world, id);

    for (int i = 0; i < it.count; i ++) {
        printf("entity %s likes %s\n",
            ecs_get_name(world, it.entities[i]),
            ecs_get_name(world, second));
    }
}
```

#### Query Builder (C++)
When both parts of a pair are types, a `flecs::pair` template can be used. Pair templates can be made part of the query type, which makes them part of the argument list of the iterator functions. An example:

```cpp
struct Eats { float value; };
struct Apples { };
// Alias to save typing
using EatsApples = flecs::pair<Eats, Apples>;

flecs::filter<EatsApples> f = 
    world.filter<EatsApples>();

// Do not use reference argument for pair
f.each([](EatsApples v) {
    // Use -> operator to access value of pair
    v->value ++;
});
```

When using the `iter` function to iterate a query with a pair template, the argument type assumes the type of the pair. This is required as the component array being passed directly to the `iter` function. An example:

```cpp
flecs::filter<EatsApples> f = 
    world.filter<EatsApples>();

f.iter([](flecs::iter& it, Eats *v) {
    for (auto i : it) {
        v[i].value ++;
    }
})
```

Pairs can also be added to queries using the builder API. This allows for the pair to be composed out of both types and regular entities. The three queries in the following example are equivalent:

```cpp
struct Eats { float value; };
struct Apples { };

flecs::entity eats = world.component<Eats>();
flecs::entity apples = world.component<Apples>();

flecs::filter<> f_1 = world.filter_builder()
    .term<Eats, Apples>()
    .build();

flecs::filter<> f_2 = world.filter_builder()
    .term<Eats>(apples)
    .build();

flecs::filter<> f_3 = world.filter_builder()
    .term(eats, apples)
    .build();
```

Individual elements of a pair can be specified with the `first` and `second` methods. The methods apply to the last added term. An example:

```cpp
flecs::filter<> f = world.filter_builder()
    .term().first<Eats>().second(apples)
    .build();
```

Individual elements of a pair can be resolved by name by using the `first` and `second` methods:

```cpp
flecs::filter<> f = world.filter_builder()
    .term().first("Eats").second("Apples")
    .build();
```

When a query pair contains a wildcard, the `flecs::iter::pair` method can be used to determine the id of the pair element that matched the query:

```cpp
flecs::filter<> f = world.filter_builder()
    .term<Eats>(flecs::Wildcard)
    .build();

f.each([](flecs::iter& it, size_t index) {
    flecs::entity second = it.pair(1).second();
    flecs::entity e = it.entity(index);
    
    std::cout << "entity " << e.name() 
              << " likes " second.name()
              << std::endl;
});
```

#### Query DSL
To query for a pair in the query DSL, the elements of a pair are a comma separated list surrounded by parentheses. An example:

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

### Access modifiers
> *Supported by: filters, cached queries, rules*

Access modifiers specify which components of a query can be read and/or written. The different access modifiers are:

| Name  | DSL identifier | C identifier | C++ identifier | Description |
|-------|----------------|--------------|----------------|-------------|
| In    | `in`           | `EcsIn`      | `flecs::In`    | Component is readonly |
| Out   | `out`          | `EcsOut`     | `flecs::Out`   | Component is write only |
| InOut | `inout`        | `EcsInOut`   | `flecs::InOut` | Component can be read/written |
| None  | `none`         | `EcsInOutNone` | `flecs::InOutNone` | Component is neither read nor written |
| Default | n/a          | `EcsInOutDefault` | `flecs::InOutDefault` | Default modifier is selected for term |

Access modifiers can be used by API functions to ensure a component cannot be written, for example by requiring a component to be accessed with a `const` modifier. APIs may also infer access modifiers where possible, for example by using the `In` modifier for a query term with a type that has a `const` modifier.

When using pipelines, the scheduler may use access modifiers to determine where sync points are inserted. This typically happens when a system acccess modifiers indicate a system writes a component not matched by the query (for example, by using `set`), and is followed by a system that reads the component.

Access modifiers may also be used by serializers that serialize the output of an iterator (for example: `ecs_iter_to_json`). A serializer may for example opt to not serialize component values that have the `Out` or `None` modifiers.

When no access modifier is specified, `Default` is assumed. This selects `InOut` for components owned by the matched entity, and `In` for components that are from entities other than the one matched by the query. 

When a query term can either match a component from the matched entity or another entity (for example: when a component is inherited from a prefab) the `Default` access modifier only provides write access for the results where the component is owned by the matched entity.

When a query term matches a tag (a component not associated with data) with a `Default` modifier, the `None` modifier is selected.

The following sections show how to use access modifiers in the different language bindings

#### Query Descriptors (C)
Access modifiers can be set using the `inout` member:

```c
ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity), .inout = EcsIn }
  }
});
```

#### Query Builder (C++)
Access modifiers can be set using the `inout` method:

```cpp
flecs::filter<> f = world.filter_builder()
    .term<Position>()
    .term<Velocity>().inout(flecs::In)
    .build();
```

When the `const` modifier is added to a type, the `flecs::In` modifier is automatically set:

```c
// Velocity term will be added with flecs::In modifier
flecs::filter<Position, const Velocity> f = 
    world.filter<Position, const Velocity>();
```

This also applies to types added with `term`:

```c
flecs::filter<> f = world.filter_builder()
    .term<Position>()
    .term<const Velocity>() // uses flecs::In modifier
    .build();
```

When a component is added by the `term` method and retrieved from a `flecs::iter` object during iteration, it must meet the constraints of the access modifiers. If the constraints are not met, a runtime assert may be thrown:

```cpp
flecs::filter<> f = world.filter_builder()
    .term<Position>()
    .term<Velocity>().inout(flecs::In)
    .build();

f.iter([](flecs::iter& it) {
    auto p = it.term<Position>(1);       // OK
    auto p = it.term<const Position>(1); // OK
    auto v = it.term<const Velocity>(2); // OK
    auto v = it.term<Velocity>(2);       // Throws assert
});
```

#### Query DSL
Access modifiers in the query DSL can be specified inside of angular brackets before the component identifier:

```
Position, [in] Velocity
```

### Operator Overview
> *Supported by: filters, cached queries, rules*

The following operators are supported by queries:

| Name     | DSL operator   | C identifier  | C++ identifier    | Description |
|----------|----------------|---------------|-------------------|-------------|
| And      | `,`            | `EcsAnd`      | `flecs::And`      | Match at least once with term |
| Or       | `||`           | `EcsOr`       | `flecs::Or`       | Match at least once with one of the OR terms |
| Not      | `!`            | `EcsNot`      | `flecs::Not`      | Must not match with term |
| Optional | `?`            | `EcsOptional` | `flecs::Optional` | May match with term |
| AndFrom  | `AND |`        | `EcsAndFrom`  | `flecs::AndFrom`  | Match all components from id at least once |
| OrFrom   | `OR |`         | `EcsOrFrom`   | `flecs::OrFrom`   | Match at least one component from id at least once |
| NotFrom  | `NOT |`        | `EcsNotFrom`  | `flecs::NotFrom`  | Don't match any components from id |

### And Operator
> *Supported by: filters, cached queries, rules*

The `And` operator is the default operator that is used when no other operators are specified. The following sections show how to use the `And` operator in the different language bindings:

#### Query Descriptor (C)
When no operator is specified, `And` is assumed. The following two queries are equivalent:

```c
ecs_filter_t *f_1 = ecs_filter(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity) }
  }
});

ecs_filter_t *f_2 = ecs_filter(world, {
  .terms = {
    { ecs_id(Position), .oper = EcsAnd }, 
    { ecs_id(Velocity), .oper = EcsAnd }
  }
});
```

#### Query Builder (C++)
When no operator is specified, `And` is assumed. The following three queries are equivalent:

```cpp
flecs::filter<Position, Velocity> f_1 = world.filter<Position, Velocity>();

flecs::filter<> f_2 = world.filter_builder()
    .term<Position>()
    .term<Velocity>()
    .build();

flecs::filter<> f_2 = world.filter_builder()
    .term<Position>().oper(flecs::And)
    .term<Velocity>().oper(flecs::And)
    .build();
```

The builder API has a `add_` convenience method:

```cpp
flecs::filter<> f = world.filter_builder()
    .term<Position>().and_(); // note escaping, 'and' is a C++ keyword
    .term<Velocity>().and_();
    .build();
```

#### Query DSL
To use the `And` operator in the query DSL, create a comma-separated list of components:

```
Position, Velocity
```

### Or Operator
> *Supported by: filters, cached queries, rules*

The `Or` operator allows for matching a single component from a list. Using the `Or` operator means that a single term can return results of multiple types. When the value of a component is used while iterating the results of an `Or` operator, an application has to make sure that it is working with the expected type (examples show how).

Another thing to keep in mind is that when using the `Or` operator, all terms participating in the `Or` expression count as a single term, even though they are specified as multiple terms in the descriptor/builder APIs. Consider the following query:

```
Position, Velocity || Speed, Mass
```

This query has three terms, while an iterator for the query returns results with 2 fields. This is important to consider when retrieving the field for a term, as its index has to be adjusted. In this example, `Position` has index 1, `Velocity || Speed` has index 2, and `Mass` has index 3.

A limitation of the current query descriptors is that each term participating in an `Or` expression must be created with the `Or` operator. This currently makes it impossible to have a query with two `Or` expressions that come right after each other. This limitation will be addressed in future versions.

The following sections show how to use the `Or` operator in the different language bindings:

#### Query Descriptor (C)
To create a query with `Or` terms, set `oper` to `EcsOr`:

```c
ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity), .oper = EcsOr },
    { ecs_id(Speed), .oper = EcsOr },
    { ecs_id(Mass) }
  }
});

ecs_iter_t it = ecs_filter_iter(world, f);
while (ecs_filter_next(&it)) {
  Position *p = ecs_term(&it, Position, 1);
  Mass *m = ecs_term(&it, Mass, 3); // not 4, because of the Or expression

  ecs_id_t vs_id = ecs_term_id(&it, 2);
  if (vs_id == ecs_id(Velocity)) {
    Velocity *v = ecs_term(&it, Velocity, 2);
    // iterate as usual
  } else if (vs_id == ecs_id(Speed)) {
    Speed *s = ecs_term(&it, Speed, 2);
    // iterate as usual
  }
}
```

#### Query Builder (C++)
To create a query with `Or` terms, use the `oper` method with `flecs::Or`:

```cpp
flecs::filter<> f = world.filter_builder()
    .term<Position>()
    .term<Velocity>().oper(flecs::Or)
    .term<Speed>().oper(flecs::Or)
    .term<Mass>()
    .build();

f.iter([&](flecs::iter& it) {
  auto p = it.term<Position>(1);
  auto v = it.term<Mass>(3); // not 4, because of the Or expression
  
  flecs::id vs_id = it.id(2);
  if (vs_id == world.id<Velocity>()) {
    auto v = it.term<Velocity>(2);
    // iterate as usual
  } else if (vs_id == world.id<Speed>()) {
    auto s = it.term<Speed>(2);
    // iterate as usual
  }
});
```

The builder API has a `or_` convenience method:

```cpp
flecs::filter<> f = world.filter_builder()
    .term<Position>() 
    .term<Velocity>().or_(); // note escaping, 'or' is a C++ keyword
    .term<Speed>().or_();
    .term<Mass>()
    .build();
```

#### Query DSL
To create a query with `Or` terms, use the `||` symbol:

```
Position, Velocity || Speed, Mass
```

### Not Operator
> *Supported by: filters, cached queries, rules*

The `Not` operator makes it possible to exclude entities with a component. A term that uses the `Not` operator will never return data. 

A note on performance: `Not` terms are efficient to evaluate when combined with other terms, but queries that only have `Not` terms (or `Optional`, see next section) can be expensive. This is because the storage only maintains indices for tables that _have_ a component, not for tables that do _not have_ a component (this would be hard/costly).

The following sections show how to use the `Not` operator in the different language bindings:

#### Query Descriptor (C)
To create a query with `Not` terms, set `oper` to `EcsNot`:

```c
ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity), .oper = EcsNot }
  }
});
```

#### Query Builder (C++)
To create a query with `Not` terms, use the `oper` method with `flecs::Not`:

```cpp
flecs::filter<> f = world.filter_builder()
    .term<Position>()
    .term<Velocity>().oper(flecs::Not)
    .build();
```

The builder API has a `not_` convenience method:

```cpp
flecs::filter<> f = world.filter_builder()
    .term<Position>() 
    .term<Velocity>().not_(); // note escaping, 'not' is a C++ keyword
    .build();
```

#### Query DSL
To create a query with `Not` terms, use the `!` symbol:

```
Position, !Velocity
```

### Optional Operator
> *Supported by: filters, cached queries, rules*

The `Optional` operator optionally matches with a component. While this operator does not affect the entities that are matched by a query, it can provide more efficient access to a component when compared to conditionally getting the component in user code. Before accessing the value provided by an optional term, code must first check if the term was set.

A note on performance: just like the `Not` operator `Optional` terms are efficient to evaluate when combined with other terms, but queries that only have `Optional` terms can be expensive. Because the `Optional` operator does not restrict query results, a query that only has `Optional` terms will match all entities.

The following sections show how to use the `Not` operator in the different language bindings:

#### Query Descriptor (C)
To create a query with `Optional` terms, set `oper` to `EcsOptional`:

```c
ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_id(Position) }, 
    { ecs_id(Velocity), .oper = EcsOptional }
  }
});

ecs_iter_t it = ecs_filter_iter(world, f);
while (ecs_filter_next(&it)) {
  Position *p = ecs_term(&it, Position, 1);
  if (ecs_term_is_set(&it, 2)) {
    Velocity *v = ecs_term(&it, Velocity, 2);
    // iterate as usual
  } else {
    // iterate as usual
  }
}
```

#### Query Builder (C++)
To create a query with `Optional` terms, call the `oper` method with `flecs::Optional`:

```cpp
flecs::filter<> f = world.filter_builder()
    .term<Position>()
    .term<Velocity>().oper(flecs::Optional)
    .build();

f.iter([&](flecs::iter& it) {
  auto p = it.term<Position>(1);
  
  if (it.is_set(2)) {
    auto v = it.term<Velocity>(2);
    // iterate as usual
  } else if (vs_id == world.id<Speed>()) {
    // iterate as usual
  }
});
```

The builder API has an `optional` convenience method:

```cpp
flecs::filter<> f = world.filter_builder()
    .term<Position>() 
    .term<Velocity>().optional();
    .build();
```

#### Query DSL
To create a query with `Optional` terms, use the `?` symbol:

```
Position, ?Velocity
```

### AndFrom, OrFrom, NotFrom Operators
> *Supported by: filters, cached queries*

The `AndFrom`, `OrFrom` and `NotFrom` operators make it possible to match a list of components that is defined outside of the query. Instead of matching the id provided in the term, the operators match the list of components _of_ the provided id as if they were provided as a list of terms with `And`, `Or` or `Not` operators. For example, if entity `e` has components `Position, Velocity` and is combined in a query with the `AndFrom` operator, entities matching the query must have both `Position` and `Velocity`.

The `AndFrom`, `OrFrom` and `NotFrom` operators are especially useful when combined with prefab entities, which by default are not matched with queries. Components that have the `DontInherit` property are ignored while matching the operators, which means that using a prefab in combination with `AndFrom`, `OrFrom` and `NotFrom` will not cause components like `Prefab` to be considered.

Component lists can be organized recursively by adding an id to an entity with the `AND` and `OR` type flags.

A term that uses the `AndFrom`, `OrFrom` or `NotFrom` operators never returns data. Access modifiers for these operators default to `InOutNone`. When a the `AndFrom`, `OrFrom` or `NotFrom` operator is combined with an access modifier other than `InOutDefault` or `InOutNone` query creation will fail.

The following sections show how to use the operators in the different language bindings:

#### Query Descriptor (C)
To use the `AndFrom`, `OrFrom` and `NotFrom` operators, set `oper` to `EcsAndFrom`, `EcsOrFrom` or `EcsNotFrom`

```c
ecs_entity_t type_list = ecs_new_w_id(world, EcsPrefab);
ecs_add(world, type_list, Position);
ecs_add(world, type_list, Velocity);

ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { type_list, .oper = EcsAndFrom }, // match Position, Velocity
    { type_list, .oper = EcsOrFrom },  // match Position || Velocity
    { type_list, .oper = EcsNotFrom }, // match !Position, !Velocity
  }
});
```

#### Query Builder (C++)
To use the `AndFrom`, `OrFrom` and `NotFrom` operators, call the `oper` method with `flecs::AndFrom`, `flecs::OrFrom` or `flecs::NotFrom`.

```cpp
flecs::entity type_list = world.prefab()
  .add<Position>()
  .add<Velocity>();

flecs::filter<> f = world.filter_builder()
    .term(type_list).oper(flecs::AndFrom) // match Position, Velocity
    .term(type_list).oper(flecs::OrFrom)  // match Position || Velocity
    .term(type_list).oper(flecs::NotFrom) // match !Position, !Velocity
    .build();
```

The builder API has the `and_from`, `or_from` and `not_from` convenience methods:

```cpp
flecs::filter<> f = world.filter_builder()
    .term(type_list).and_from()
    .term(type_list).or_from()
    .term(type_list).not_from()
    .build();
```

#### Query DSL
To create a query with the `AndFrom`, `OrFrom` and `NotFrom` operators in the C API, use `AND`, `OR` and `NOT` in combination with the bitwise OR operator (`|`):

```
AND | type_list, OR | type_list, NOT | type_list
```

### Source
> *Supported by: filters, cached queries, rules*

Source is a property of a term that specifies the entity on which the term should be matched. Queries support two kinds of sources: static and variable. A static source is known when the query is created (for example: match `SimTime` on entity `Game`), whereas a variable source is resolved while the query is evaluated. When no explicit source is specified, a default variable source called `$This` is used (see [Variables](#variables)). 

When a query only has terms with fixed sources, iterating the query will return a result at least once, and at most once if the query terms do not match wildcards. If a query has one or more terms with a fixed source that do not match the entity, the query will return no results. A source does not need to match the query when the query is created.

The following sections show how to use variable and fixed sources with the different language bindings.

#### Query Descriptor (C)
To specify a fixed source, set the `src.id` member to the entity to match. The following example shows how to set a source, and how to access the value provided by a term with a fixed source:

```c
ecs_entity_t Game = ecs_new_id(world);
ecs_add(world, Game, SimTime);

ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_id(Position) }, // normal term, uses $This source
    { ecs_id(Velocity) },  // normal term, also uses $This source
    { ecs_id(SimTime), .src.id = Game } // fixed source, match SimTime on Game
  }
});

ecs_iter_t it = ecs_filter_iter(world, f);
while (ecs_filter_next(&it)) {
  Position *p = ecs_term(&it, Position, 1);
  Velocity *v = ecs_term(&it, Velocity, 2);
  SimTime *st = ecs_term(&it, SimTime, 3);
  
  for (int i = 0; i < it.count; i ++) {
    p[i].x += v[i].x * st->value; // p, v are accessed as array, st as pointer
    p[i].y += v[i].y * st->value;
  }
}
```

A source may also be specified by name by setting the `src.name` member:

```c
ecs_entity_t Game = ecs_entity(world, { .name = "Game" });
ecs_add(world, Game, SimTime);

ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_id(SimTime), .src.name = "Game" }
  }
});
```

This examples shows how to access the entities matched by the default `$This` source and a fixed source:

```c
ecs_filter_t *f = ecs_filter(world, {
  .terms = {
    { ecs_id(Position) }, // normal term, uses $This source
    { ecs_id(SimTime), .src.id = Game } // fixed source, match SimTime on Game
  }
});

while (ecs_filter_next(&it)) {
  ecs_entity_t src_1 = ecs_term_src(&it, 1); // Returns 0, meaning entity is stored in it.entities
  ecs_entity_t src_2 = ecs_term_src(&it, 2); // Returns Game

  for (int i = 0; i < it.count; i ++) {
    printf("$This = %s, src_2 = %s\n", 
      ecs_get_name(world, it.entities[i]),
      ecs_get_name(world, src_2));
  }
}
```

The `entities` and `count` member are solely populated by the number of entities matched by the default `$This` source. If a query only contains fixed sources, `count` will be set to 0. This is important to keep in mind, as the inner for loop from the last example would never be iterated for a query that only has fixed sources.

#### Query Builder (C++)
To specify a fixed source, call the `src` method to the entity to match. The following example shows how to set a source, and how to access the value provided by a term with a fixed source:

```cpp
flecs::entity Game = world.entity()
  .add<SimTime>();

flecs::filter<> f = world.filter_builder()
  .term<Position>()  // normal term, uses $This source
  .term<Velocity>()  // normal term, also uses $This source
  .term<SimTime>().src(Game) // fixed source, match SimTime on Game
  .build();

f.iter([](flecs::iter& it) {
  auto p = it.term<Position>(1);
  auto v = it.term<Velocity>(2);
  auto st = it.term<SimTime>(3);
  
  for (auto i : it) {
    p[i].x += v[i].x * st[i].value;
    p[i].y += v[i].y * st[i].value;
  }
});
```

Note how in this example all components can be accessed as arrays. When a query has mixed `$This` and fixed source terms, results are returned one entity at a time vs. one table at a time. As a result, `i` in the previous example will never be larger than `0`, which is why this code works even though there is only a single instance of the `SimTime` component.

Returning entities one at a time can negatively affect performance, especially for large tables. To learn more about why this behavior exists and how to change the code to table-based iteration, see [Instancing](#instancing). 

The next example shows how queries with mixed `$This` and fixed sources can be iterated with `each`. The `each` function does not have the performance drawback of the last `iter` example, as it uses [instancing](#instancing) by default.

```cpp
flecs::filter<Position, Velocity, SimTime> f = 
  world.filter_builder<Position, Velocity, SimTime>()
    .arg(3).src(Game) // set fixed source for 3rd template argument (SimTime)
    .build();

// Because all components are now part of the filter type, we can use each
f.each([](Position& p, Velocity& v, SimTime& st) {
  p.x += v.x * st.value;
  p.y += v.y * st.value;
});
```

One thing to note is that when a query only contains fixed source terms, `each` cannot be used. The `each` function is called for each entity matched by the `$This` source. 




### Singletons

### Instancing

### Traversal

### Cascade

### Variables

## Query Performance
This section describes performance characteristics for each query type.

### Filter
A filter is a data type that describes the structure of a query in Flecs. Filters are the cheapest to create, as creating one just requires initializing the value of the filter object.

Filters can serve different purposes, like iterating all matching entities, or testing if a specific entity matches with a filter. Filters can also be used to filter the output of other queries, including other filters.

While the exact way a filter is evaluated depends on what kind of filter it is, almost all filter evaluation follows steps similar to the ones in this diagram:

![filter diagram](img/filter_diagram.png)

Each node in the diagram represents a function that can return either true or false. When a node returns true, evaluation moves on to the next node. When it returns false, evaluation goes back one node. These kinds of functions are called [predicates](https://en.wikipedia.org/wiki/Predicate_(mathematical_logic)), and this evaluation process is called [backtracking](https://en.wikipedia.org/wiki/Backtracking).

Whether a node returns true or false depends on what its function does:

- The first node invokes the function `select(Position)`. This function finds all *tables* with `Position`, and will forward these tables to the next node. As long as `select` is able to find more tables with `Position`, the node will keep returning true. When no more tables can be found the node returns false.

- The second node invokes the function `with(Velocity)`. This function takes the table found by the previous node and returns true if the table has `Velocity`, and false if it does not.

Both functions are O(1) operations.

When a table reaches the `yield` node it means that it matched all parts of the filter, and it will be returned by the iterator doing the evaluation.

> A table groups all entities that have _exactly_ the same components. Thus if one entity in a table matches a node, all entities in the table match the node. This is one of the main reasons queries are fast: instead of checking each individual entity for components, we can eliminate a table with thousands of entities in a single operation.

Because filters are fast to create, have low overhead, and are reasonably efficient to iterate, they are the goto solution for when an application cannot know in advance what it needs to query for, like finding all children for a specific entity:

```c
ecs_filter_f fs = ECS_FILTER_INIT;
ecs_filter_t *f = ecs_filter(world, {
    .storage = &fs, // optional, but prevents allocation
    .terms = {{ ecs_childof(e) }}
});

ecs_iter_t child_it = ecs_filter_iter(f);
while (ecs_filter_next(&child_it)) {
    for (int c = 0; c < child_it.count; c ++) {
        printf("child %s\n", ecs_get_name(world, 
            child_it.entities[c]));
    }
}

ecs_filter_fini(f);
```
```cpp
auto f = world.filter_builder()
  .term(flecs::ChildOf, e)
  .build();

f.each([](flecs::entity child) {
    std::cout << child.path().str() << "\n";
});
```

While filter evaluation for a single component is very fast, each additional component adds some overhead. This is not just because of the time it takes to do an additional check, but because more tables will get discarded during evaluation. The cost of discarded tables adds up as they pass through one or more nodes before getting rejected, consuming resources while not contributing to the query result.

This cost can be particularly significant for filters that match a large (5-10) number of components in applications with many (>1000s) tables. While Flecs implements several strategies to limit this overhead, like storing empty tables separately from non-empty tables, this is something to keep in mind when using filters.

### Cached Queries
A cached query is an object that caches the output of a filter. Cached queries are significantly faster to iterate than a filter, as an iterator just needs to walk the list of cache entries and return each one to the application.

This also makes the performance of cached queries more predictable. The overhead per returned table for a filter depends on how many components it matches with, and how many tables it had to discard. For a cached query the cost per table is much lower, and is constant time.

> [A benchmark showing the difference between cached queries and filters](https://twitter.com/ajmmertens/status/1509473999205507080).

This predictable behavior is why cached queries are the default for Flecs systems. Once tables are created and caches are initialized, the (often time critical) main loop of an application does not have to spend  time matching entities with systems, which frees up resources for application logic.

Another benefit of cached queries is that they are compatible with good code practices. Cached queries add minimal overhead to systems, which means applications do not have to compromise on design where the cost of evaluating a query has to be weighed against the benefit of having many small decoupled systems.

The following example shows how cached queries are used:

```c
// Creates query, populates the cache with existing tables
ecs_query_t *q = ecs_query(world, {
    .filter.terms = {
        { ecs_id(Position) }, { ecs_id(Velocity) }
    }
});

// When a new table is created that matches the query, it is
// added to the cache
ecs_entity_t e = ecs_new_id(world);
ecs_add(world, e, Position);
ecs_add(world, e, Velocity);

// Iterate the tables in the cache
ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
    Position *p = ecs_term(&it, Position, 1);
    Velocity *v = ecs_term(&it, Velocity, 2);

    for (int i = 0; i < it.count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}
```
```cpp
// Creates query, populates the cache with existing tables
auto q = world.query<Position, Velocity>();

// When a new table is created that matches the query, it is
// added to the cache
auto e = world.entity()
    .add<Position>()
    .add<Velocity>();

// Iterate the tables in the cache
q.each([](Position& p, Velocity& v) {
    p.x += v.x;
    p.y += v.y;
});
```

The scenarios described so far are in the sweetspot of cached queries, where their performance is amongst the fastest of any ECS implementation. To build games that perform well however, it also helps to know when cached queries perform badly:

- Cached queries do not perform well when they are repeatedly created and destroyed, or when they are only used a handful of times. The overhead of initializing the query cache and the cost of keeping it up to date would be many times higher than using a filter.

- Large numbers of cached queries (>1000s) can become a bottleneck. When this happens in combination with large table turnover, matching each new table with all queries can become expensive.

- ECS operations can cause matched tables to no longer match. A simple example is that of a query matching `Position` on a parent entity, where the component is removed from the parent. This triggers cache revalidation, where a query reevaluates its filter to correct invalid entries. When this happens for a large number of queries and tables, this can be time consuming.

- Performance can degrade when a query needs to traverse a deep (>50 levels) hierarchy in order to determine if a table matches. If a hierarchy has deeply nested children and a parent component can only be found on the root, a query will have to traverse this tree likely for multiple tables, which is expensive.



