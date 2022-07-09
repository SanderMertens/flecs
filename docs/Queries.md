Queries are the mechanism that allow applications to get the entities that match with a certain set of conditions. Queries can range from simple lists of components to complex expressions that efficiently traverse an entity graph. This manual explains the ins & outs of how to use them.

**NOTE**: this manual describes queries as they are intended to work. The actual implementation may not have support for certain combinations of features. When an application attempts to use a feature that is not yet supported, an `UNSUPPORTED` error will be thrown.

**NOTE**: features that rely on query variables or automatic substitution of components or objects (like when relying on component inheritance or transitive queries) require the rule query engine, included in the `FLECS_RULES` addon.

## Query kinds
Flecs has two different kinds of queriers: cached and uncached. The differences are described here. Note that when "query" is mentioned in the other parts of the manual it always refers to all query kinds, unless explicitly mentioned otherwise.

### Filter (uncached)
A filter is an uncached query that is cheap to create and finds matching entities as it is being iterated. The performance of a filter can be roughly thought of as having to do a hashmap lookup per filter term. The first hashmap lookup retrieves a list of archetypes that matches with the first term. Then the filter does a hashmap lookup for the second term. If it succeeds, the archetype that is being evaluated matches the second term, and we move on to the next term. If the hashmap lookup does not succeed, we go back to our initial list of archetypes and move on to the next one.

In pseudo code, filter evaluation roughly looks like this:

```python
Archetype archetypes[] = filter.get_archetypes_for_first_term();
for archetype in archetypes:
  bool match = true;
  for each term in filter.range(1, filter.length):
    if !archetype.match(term):
      match = false;
      break;
  if match:
    yield archetype;
```

An application then iterates the archetype, which can contain 1...N entities.

### Query (cached)
A query is a data structure that caches its results. Queries are heavier to create since they need to build their cache, but are very fast to iterate. Queries cannot be created ad-hoc as they mutate the state of the world, but they can be iterated simultaneously from multiple threads.

The performance of iterating a query is to simply iterate its cached list of archetypes. The actual matching is performed when the query is created, and afterwards, when archetypes are created or deleted.

In pseudo code, query evaluation roughly looks like this:

```python
for archetype in query.cache.archetypes:
  yield archetype;
```

Just like with filters, an application then iterates the archetype, which can contain 1...N entities.

Queries are almost a strict superset of filters, in that they provide the same functionality, but cached. There are a few exceptions to this rule, as there are some filters that cannot be cached.

## Query creation
Queries can be created in different ways in flecs, which are described here.

### Query DSL
The query DSL (domain specific language) is a flecs-specific string-based format that can be parsed at runtime into a valid query object. The query DSL is useful when an application cannot know in advance what a query will be, such as when exposing a query interface through a REST endpoint.

This is an example of a simple query in the query DSL:
```
Position, Velocity, NPC, (Likes, Apples)
```

For identifiers to be used in the query DSL they must be registered as named entities.

Another feature of the query DSL is that it can be used in reverse, as a data definition format. This shows how the query DSL can be used to define an entity called "MyEntity" with components Position and Velocity:

```
Position(MyEntity)
Velocity(MyEntity)
```

The DSL relies on the optional parser addon. If the parser addon is not enabled, DSL strings cannot be parsed.

### Simple queries (C++)
Queries that are just simple lists of components can be constructed by simply providing a list of types to the query factory function:

```c
// q will be of type flecs::query<Position, const Velocity>
auto q = world.query<Position, const Velocity>();
```

### Query Builder (C++)
The query builder is a C++ API to construct queries that follows the fluent API pattern. The query builder can be used to create either fully or partially constructed queries.

This is an example of a simple query in the builder API:

```cpp
// q will be of type flecs::query<Position, const Velocity>
auto q = world.query_builder<Position, const Velocity>()
  .term<NPC>()
  .term(Likes, Apples)
  .build();
```

The builder API allows for partially constructing (and then finalizing) queries:

```cpp
auto qb = world.query_builder<Position, const Velocity>()
  .term<NPC>();

if (add_fruit) {
  qb.term(Likes, Apples);
}

auto q = qb.build();
```

The builder API has support for adding terms using the DSL:

```cpp
auto q = world.query_builder()
  .expr("Position, [in] Velocity")
  .term("NPC")
  .term("(Likes, Apples)")
  .build();
```

### Query Descriptors (C)
The query descriptor is a C API to construct queries that follows the struct initialization pattern. Query descriptors can be used to create either fully or partially constructed queries.

This is an example of the query descriptor API:

```c
ecs_filter_t *f = ecs_filter_init(world, &(ecs_filter_desc_t){
  .terms = {
    {ecs_id(Position)},
    {ecs_id(Velocity)},
    {NPC},
    {ecs_pair(Likes, Apples)}
  }
});
```

The descriptor API has support for using the DSL:

```c
ecs_filter_t *f = ecs_filter_init(world, &(ecs_filter_desc_t){
  .terms = {
    {ecs_id(Position)},
    {ecs_id(Velocity)}
  },
  .expr = "NPC, (Likes, Apples)"
});
```

Filters and queries can both be created with the descriptor API, and use the same descriptor structs. When creating a query, the `ecs_filter_desc_t` type is embedded by the `filter` member:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
  .filter.terms = {
    {ecs_id(Position)},
    {ecs_id(Velocity)},
    {NPC},
    {ecs_pair(Likes, Apples)}
  }
});
```

## Query Iteration
This section describes the different ways in which a query can be iterated.

### Each (C++)
The each function is a simple way to linearly iterate entities in a query. It is used by passing a (lambda) function to the `each` function.

This is a simple example of using `each`:

```cpp
// q will be of type flecs::query<Position, const Velocity>
auto q = world.query_builder<Position, const Velocity>()
  .term<NPC>()
  .build();

q.each([](flecs::entity e, Position& p, const Velocity& v) {
  p.x += v.x;
  p.y += v.y;
});
```

Note that the arguments passed to each are derived from the query type. Terms added to the query by the query builder do not affect the query type, and do not appear in the argument list of the `each` function.

### Iter (C++)
The iter function is a more advanced way to iterate entities in a query. It provides an application with more freedom to iterate a query, such as allowing for iterating the set of entities multiple times, or in different ordering.

The iter callback is invoked per archetype, which means that it may be invoked multiple times per iter call. All the entities within a single call will be of the same type, meaning they have the exact same components.

This is a simple example of using `iter`:

```cpp
auto q = world.query_builder<Position, const Velocity>()
  .term<NPC>()
  .build();

q.iter([](flecs::iter& it, Position *p, const Velocity *v) {
  for (auto i : it) {
    p[i].x += v[i].x;
    p[i].y += v[i].y;
  }
});
```

Note how the iter function provides direct access to the component arrays.

The iter callback can obtain access to components that are not part of the query type:

```cpp
auto q = world.query_builder<Position, const Velocity>()
  .term<Mass>()
  .build();

q.iter([](flecs::iter& it, Position *p, const Velocity *v) {
  auto mass = it.term<Mass>(3); // 3rd term of the query

  for (auto i : it) {
    p[i].x += v[i].x / mass[i].value;
    p[i].y += v[i].y / mass[i].value;
  }
});
```

Note how the `iter::term` function accepts a number. This number corresponds to the index of the term in the query, starting from 1. When a query is constructed with both template arguments and builder functions, the template arguments appear first in the query. If the type provided to the `it.term` function does not match with the component, the function will throw a runtime error.

The iter callback an introspect the results of a query. When the type of a query term is not defined at creation time, which happens when using Or queries or wildcards, the type can be obtained in the iter callback:

```cpp
auto q = world.query_builder<>()
  .term(Likes, flecs::Wildcard)
  .build();

q.iter([](flecs::iter& it) {
  // Get the type id for the first term.
  auto likes = it.id(1);

  // Extract the object from the pair
  std::cout << "Entities like "
    << likes.second().name()
    << std::endl;
});
```

Alternatively, the `iter` call can be written in such a way that it is fully generic, and just prints its inputs:

```cpp
q.iter([](flecs::iter& it) {
  for (int t = 0; t < it.term_count(); t++) {
    auto id = it.id(t);
    auto data = it.term(t);

    // Use id & data, for example for reflection
    for (auto i : it) {
      void *ptr = data[i];
      // ...
    }
  }
});
```

### Iter (C)
The `ecs_query_iter` function is how C applications can iterate queries, and provides functionality similar to the C++ `iter` function. This is a simple example of using `ecs_query_iter`:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
  .filter.terms = {
    {ecs_id(Position)},
    {ecs_id(Velocity)}
  }
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
  Position *p = ecs_term(&it, Position, 1);
  Velocity *v = ecs_term(&it, Velocity, 2);

  for (int i = 0; i < it.count; i++) {
    p[i].x += v[i].x;
    p[i].y += v[i].y;
  }
}
```

Unlike C++, queries are not typed in C, and as such all components are obtained using the `ecs_term` function. Note how the number provided to `ecs_term` corresponds with the location of the component in the query, offset by 1. If a type is provided to `ecs_term` that does not match the term type, the function may throw a runtime error.

Similar to the `term_id` function in C++, the C API has the `ecs_term_id` function:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
  .filter.terms = {
    {ecs_pair(Likes, EcsWildcard)}
  }
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
  ecs_id_t id = ecs_term_id(&it, 1);
  ecs_entity_t obj = ecs_pair_second(it->world, id);
  printf("Entities like %s\n", ecs_get_name(world, object));
}
```

## Sorting
Applications are able to access entities in order, by using sorted queries. Sorted queries allow an application to specify a component that entities should be sorted on. Sorting is enabled by setting the order_by function:

```c
ecs_query_t q = ecs_query_init(world, &(ecs_query_desc_t){
    .filter.terms = {{ ecs_id(Position) }},
    .order_by_component = ecs_id(Position),
    .order_by = compare_position,
});
```

This will sort the query by the `Position` component. The function also accepts a compare function, which looks like this:

```c
int compare_position(ecs_entity_t e1, Position *p1, ecs_entity_t e2, Position *p2) {
    return p1->x - p2->x;
}
```

Once sorting is enabled for a query, the data will remain sorted, even after the underlying data changes. The query keeps track of any changes that have happened to the data, and if changes could have invalidated the ordering, data will be resorted. Resorting does not happen when the data is modified, which means that sorting will not decrease performance of regular operations. Instead, the sort will be applied when the application obtains an iterator to the query:

```c
ecs_entity_t e = ecs_new(world, Position); // Does not reorder
ecs_set(world, e, Position, {10, 20}); // Does not reorder
ecs_iter_t it = ecs_query_iter(world, q); // Reordering happens here
```

The following operations mark data dirty can can cause a reordering:
- Creating a new entity with the ordered component
- Deleting an entity with the ordered component
- Adding the ordered component to an entity
- Removing the ordered component from an entity
- Setting the ordered component
- Running a system that writes the ordered component (through an [out] column)

Applications iterate a sorted query in the same way they would iterate a regular query:

```c
while (ecs_query_next(&it)) {
    Position *p = ecs_term(&it, Position, 1);

    for (int i = 0; i < it.count; i++) {
        printf("{%f, %f}\n", p[i].x, p[i].y); // Values printed will be in order
    }
}
```

### Sorting algorithm
The algorithm used for the sort is a quicksort. Each table that is matched with the query will be sorted using a quicksort. As a result, sorting one query affects the order of entities in another query. However, just sorting tables is not enough, as the list of ordered entities may have to jump between tables. For example:

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

To make sure a query iterates the entities in the right order, it will iterate entities in the ordered tables to determine the largest slice of ordered entities in each table, which the query will iterate in order. Slices are precomputed during the sorting step, which means that the performance of query iteration is similar to a regular iteration. For the above set of entities, these slices would look like this:

Table              | Slice
-------------------|-------
Position           | 0..2
Position, Velocity | 3..4
Position, Mass     | 5
Position           | 6..7

This process is transparent for applications, except that the slicing will result in smaller contiguous arrays being iterated by the application.

### Sorting by entity id
Instead of sorting by a component value, applications can sort by entity id by not specifying order_by_component

```c
ecs_query_t q = ecs_query_init(world, &(ecs_query_desc_t){
    .filter.terms = {{ ecs_id(Position) }},
    .order_by = compare_entity,
});
```

The compare function would look like this:

```c
int compare_entity(ecs_entity_t e1, void *p1, ecs_entity_t e2, void *p2) {
    return e1 - e2;
}
```

When no component is provided, no reordering will happen as a result of setting components or running a system with `[out]` columns.

## Query Concepts
Now that we have the basics under our belt, lets look a bit more in-depth at the different concepts from which queries are composed.

### Expression
An expression refers to all the terms of the query. For a query that finds all entities with components Position & Velocity, the expression is `Position, Velocity`. An expression can be written down in any of the query description formats (DSL, builder or descriptor). However, when the term expression is used in the API, it typically indicates a DSL string.

### Term
A term is a single element of a query expression. The query expression `Position, Velocity` has two terms, `Position` and `Velocity`. For a query to match an entity (or archetype), all terms in the expression must match.

In the DSL, each term in an expression is usually separated by a comma, also called the `And` operator.

### Identifier
A query term may contain one or more string-based identifiers that refer to the different entities in the term, such as `"Position"` and `"Velocity"`. These identifiers are used to lookup their corresponding entities while the query is constructed. Identifiers can be hierarchical, such as `flecs.components.transform.Position`.

### Operator
An operator specifies how the term should be applied to the query. For example, if the result of a term is `true`, but it has the `Not` operator, the result of the term in the query will be `false`. Queries support the following operators:

#### And operator
This is the default operator. Here is a simple example of a query with an And operator:

```c
Position, Velocity // Position And Velocity
```

Terms added by the builder or descriptor by default use the `And` operator.

#### Not operator
The Not operator instructs a query to reverse the result of a term. In the query DSL, the Not operator is specified after the And operator:

```c
Position, !Velocity // Position And Not Velocity
```

This example shows how to add a `Not` term with the query builder:

```cpp
auto qb = world.query_builder<Position, const Velocity>()
  .term<NPC>().oper(flecs::Not);
```

This example shows how to add a `Not` term with the query descriptor:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_id(Position)},
    {NPC, .oper = EcsNot}
  }
});
```

#### Optional operator
The optional operator instructs a query to return the result of the term as `true`, even if the entity/archetype did not match the term. In the query DSL the optional operator is provided after the `And` operator:

```c
Position, ?Velocity // Position And Optionally Velocity
```

Optional arguments, while they do not impact query matching, are useful as they provide a quicker way to access the optional component than using `e.get<T>()`. Optional components are faster because of three reasons:

1) Queries iterate archetypes, and if an archetype does not have the optional component, none of the entities in the archetype do.

2) When an archetype does have the optional component, the query can access it as an array, just like a regular component.

3) Cached queries store the archetype location of an optional component  in their cache, which prevents the lookup that is needed by `get<T>()`.

This example shows how to add a `Optional` term with the query builder:

```cpp
auto qb = world.query_builder<Position, const Velocity>()
  .term<NPC>().oper(flecs::Optional);
```

This example shows how to add a `Optional` term with the query descriptor:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_id(Position)},
    {NPC, .oper = EcsOptional}
  }
});
```

#### Or operator
The `Or` operator instructs a query to match _at least_ one term out of a list of terms that are chained together by the operator. Here is a simple example of a query with an `Or` operator:

```c
Position || Velocity // Position Or Velocity
```

When a query contains multiple `Or` chains separated by `And` operators, the entity must at least match one component from each chain:

```c
// (Position Or Mass) And (Speed Or Velocity)
Position || Mass, Speed || Velocity
```

This example shows how to add a `Or` term with the query builder:

```cpp
auto qb = world.query_builder<Position, const Velocity>()
  .term<NPC>().oper(flecs::Or)
  .term<Enemy>().oper(flecs::Or);
```

This example shows how to add a `Or` term with the query descriptor:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_id(Position)},
    {NPC, .oper = EcsOr},
    {Enemy, .oper = EcsOr}
  }
});
```

Note that both in the builder and descriptor APIs, each term that participates in an `Or` chain must specify the `Or` operator.

#### AndFrom, OrFrom, NotFrom operators
The `*From` operators allow a query to match against an external list of components with the `And`, `Or` or `Not` operator. This list of components is specified as a type entity. For example, if an application has the following type entity:

```cpp
auto Ingredients = world.type("Ingredients")
  .add(Bacon)
  .add(Eggs)
  .add(Tomato)
  .add(Lettuce);
```

A term can select one of the ids in the type with the `OrFrom` operator, which in the query DSL looks like this:

```c
OR | Ingredients // Match with at least one of Ingredients
```

Similarly, `AND`, and `NOT` can be used in combination with type entities.

This example shows how to add an `AndFrom` term with the query builder:

```cpp
auto qb = world.query_builder<Position, const Velocity>()
  .term(Ingredients).oper(flecs::AndFrom);
```

This example shows how to add a `AndFrom` term with the query descriptor:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_id(Position)},
    {Ingredients, .oper = EcsAndFrom}
  }
});
```

### Read/write access
A query term can specify whether the query intends to read or write data from the term. This is an example of a query term that specifies readonly access:

```
Position, [in] Velocity
```

This example shows how to specify read/write access in the builder API:

```cpp
auto qb = world.query_builder<Position>()
  .term<Velocity>().inout(flecs::In);
```

Alternatively a query may also use `const` to indicate readonly access, which is equivalent to specifying `flecs::In`. Using `const` however should be preferred, as the qualifier is propagated to the `each` and `iter` callbacks:

```cpp
auto q = world.query<Position, const Velocity>();

q.each([](flecs::entity e, Position& p, const Velocity& v){
  p.x += v.x;
  p.y += v.y;
});
```

This example shows how to specify read/write access in the query descriptor:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_id(Position)},
    {ecs_id(Position), .inout = EcsIn}
  }
});
```

The default read/write access is `InOut` for terms that have `This` as the subject. Terms that have an entity other than `This` have `In` as default. Either defaults can be overridden.

### Predicate
Each term has exactly one predicate. Conceptually, a predicate is a function that returns true when its inputs match, and false when its inputs do not match. When translating this to simple flecs query, a component represents a predicate that returns true when its input has the component:

```c
Position(Bob) // true if entity Bob has Position
```

For a term to match, its predicate has to evaluate to true.

### Subject
A subject is the argument passed to a predicate. In most cases this is the entity (or archetype) that is being matched with the query, but not necessarily. Take for example this term:

```
Position(Bob)
```

Here `Position` is the predicate, and `Bob` is the subject. If `Bob` has `Position`, the predicate will return true and the term will match. Otherwise the term will return false.

The term "subject" is borrowed from English grammar. In the sentence "Bob has component Position", "Bob" is the subject.

### This
`This` is the placeholder for an entity (or archetype) being evaluated by a query. When a query is looking for all matching results, it will iterate the set of entities that represent a potential match, and pass each entity (or archetype) as "This" to each term. By default `This` is used as the subject for each predicate. Thus a simple query like this:

```
Position, Velocity
```

actually looks like this when written out with explicit subjects:

```
Position(This), Velocity(This)
```

### Object
An object is an optional second argument that can be passed to a predicate. Objects are used when querying for relationships. For example, entity `Bob` may have the relationship `Likes` with `Alice`. In this case, `Bob` is the subject, `Likes` is the predicate and `Alice` is the object. When querying for whether `Bob` likes `Alice`, we do:

```c
Likes(Bob, Alice) // True when Bob Likes Alice
```

When we are querying for a relationship on `This`, we can use this shorthand notation:

```c
(Likes, Alice)
```

which is the same as:

```c
Likes(This, Alice)
```

The term "object" is borrowed from English grammar and has nothing to do with the OOP meaning of object (as in an instance of a class). In the sentence "Bob likes Alice", "Bob" is the subject, and "Alice" is the object.

### Component
When a term refers to a component, it appears as a predicate with a single argument (subject). Components are typically referred to by their language types in APIs. While the query DSL is agnostic to this, the bindings provide dedicated ways for setting a component for a term.

In the builder API components can be specified using template arguments:

```cpp
// Position & Velocity are C++ types
auto qb = world.query_builder<Position, const Velocity>()
  .term<Mass>(); // Mass is a C++ type
```

In the descriptor API components can be specified with the `ecs_id` macro. This macro requires that the component id is known in the current scope (see the manual for how to register components/declare component identifiers):

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_id(Position)},
    {ecs_id(Velocity)},
    {ecs_id(Mass)}
  }
});
```

### Tag
A tag is similar to a component when used in a query, in that it appears as a predicate with a single argument (subject). Tags are regular entities that are not associated with a datatype.

In the builder API tags can be added like this:

```cpp
// NPC
auto NPC = world.entity();
auto qb = world.query_builder<>()
  .term(NPC);
```

In the query descriptor API tags can be added like this:

```c
// NPC
ecs_entity_t NPC = ecs_new_id(world);
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {NPC}
  }
});
```

Note that in C++ empty types can be used as tags. These can be specified in the same way as a regular component, by their type.

### Relation
A relation in a term is a predicate with two arguments, a subject and an object. Relations and objects can be specified as either entities or types in the builder API, depending on how the application defines them:

```cpp
// (Likes, Apples)
auto qb = world.query_builder<>()
  .term<Likes, Apples>();

auto qb = world.query_builder<>()
  .term(Likes, Apples);

auto qb = world.query_builder<>()
  .term<Likes>(Apples);
```

In the query descriptor API relations can be specified as pairs:

```c
// (Likes, Apples)
ecs_entity_t NPC = ecs_new_id(world);
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_pair(Likes, Apples)}
  }
});
```

Just like in C++, pairs can contain either types or entities:

```c
// (Likes, Apples)
ecs_entity_t NPC = ecs_new_id(world);
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_pair(ecs_id(Likes), ecs_id(Apples))}
  }
});
```

### Singleton
Singletons are a feature that make it easy to work with components for which there is only a single instance in the world. A typical example is a `Game` component, which could contain information that is global to a game, like simulation speed.

In the query language, a singleton can be queried for by providing the `$` symbol as subject:

```
Game($)
```

In Flecs singletons are implemented as components that are added to themselves. The above query is short for:

```
Game(Game)
```

This example shows how to add a singleton with the query builder:

```cpp
// $Game
auto qb = world.query_builder<Position, const Velocity>()
  .term<Game>().singleton();
```

This example shows how to add a singleton with the query descriptor. Note that the descriptor does not have a dedicated API for singletons:

```c
// Game(Game)
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    { ecs_id(Game), .subj.entity = ecs_id(Game) }
  }
});
```

### Assigning Identifiers with the API
The query builder and descriptor APIs allow for setting the individual predicate, subject and object of a term. These are also referred to as the "term identifiers".

This example shows how to explicitly set identifiers with the builder API:

```cpp
// Position(This), (Likes, Alice)
auto qb = world.query_builder<>()
  .term<Position>().subj(flecs::This)
  .term(Likes).object(Alice);
```

This example shows how to explicitly set identifiers with the query descriptor:

```c
// Position(This), (Likes, Alice)
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {.pred.entity = ecs_id(Position), .subj.entity = EcsThis},
    {.pred.entity = Likes, .obj.entity = Alice}
  }
});
```

Note how in the descriptor API, the subject and object are assigned as elements in an arguments array for the predicate.

In practice it is often enough to just assign the predicate, and an optional object for terms that query for a relation, as by default the subject is set to `EcsThis`. The predicate and object can both be set in a single `id` field. In the descriptor API, the `id` field is the first member of the struct, so it is possible to do this:

```c
// Position
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_id(Position)}
  }
});
```

which is equivalent to

```c
// Position(This)
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {.pred.entity = ecs_id(Position), .subj.entity = EcsThis}
  }
});
```

The id can also contain an additional object:

```c
// (Likes, Alice)
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_pair(Likes, Alice)}
  }
});
```

which is equivalent to

```c
// Likes(This, Alice)
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {
      .pred.entity = Likes,
      .subj.entity = EcsThis,
      .obj.entity = Alice
    }
  }
});
```

### Wildcard
We can use the wildcard to match with any entity. Wildcards are typically used in combination with relationships. This is a simple example of a term that returns all entities (objects) that `This` likes.

```
(Likes, *)
```

A single entity may have multiple `Likes` relationships. When this is the case, the above term will cause the same entity to be returned multiple times, once for each object of the `Likes` relationship.

Wildcards can also be used as predicate. This term returns all entities that have a relationship with Alice:

```
(*, Alice)
```

It is also possible to query for all relationships for an entity:

```
(*, *)
```

Wildcards are not limited to relationships. The following query is also valid, and requests all components from the iterated over entity:

```
*
```

Note that `(*, *)` only matches relations, and `*` only matches components. To create a query that matches everything from an entity, the two wildcard expressions have to be combined with an `Or` expression:

```
(*, *) || *
```

This example shows how to use wildcards in the C++ API:

```cpp
// (Likes, *)
auto qb = world.query_builder<>()
  .term(Likes, flecs::Wildcard);
```

This example shows how to use wildcards in the query descriptor:

```c
// (Likes, *)
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_pair(Likes, EcsWildcard)}
  }
});
```

### Variables
A variable is a wildcard that must match with the same entity across terms. To illustrate where this is useful, let's first start with a query that finds all the `Likes` relationships for entities that have them:

```
(Likes, *)
```

Now suppose we want to know whether the object matched by the wildcard (`*`) is also a colleague of the subject. We could do this:

```
(Likes, *), (Colleague, *)
```

But that does not work, as this query would return all Likes relationships * all Colleague relationships. To constrain the query to only return `Likes` relationships for objects that are also colleagues, we can use a variable:

```
(Likes, $X), (Colleague, $X)
```

A variable is an identifier that is local to the query and does not need to be defined in advance. In the query DSL identifiers that start with a `$` are interpreted as a variable.

Variables can occur in multiple places. For example, this query returns all the relationships the `This` entity has with each object it likes as `R`:

```
(Likes, $X), ($R, $X)
```

A useful application for variables is ensuring that an entity has a component referenced by a relationship. Consider an application that has an `ExpiryTimer` relationship that removes a component after a certain time has expired. This logic only needs to be executed when the entity actually has the component to remove.

With variables this can be ensured:

```
(ExpiryTimer, $C), $C
```

Variables allow queries to arbitrarily traverse the entity relationship graph. For example, the following query tests whether there exist entities that like objects that like their enemies:

```
(Likes, $Friend), Likes($Friend, $Enemy), (Enemy, $Enemy)
```

This example shows how to use variables in the C++ API:

```cpp
// (Likes, X), (Colleague, X)
auto qb = world.query_builder<>()
  .term(Likes).obj().var("X")
  .term(Colleague).obj().var("X");
```

This example shows how to use wildcards in the query descriptor:

```c
// (Likes, X), (Colleague, X)
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {.pred.entity = Likes, .obj.name = "X", .var = EcsVarIsVariable }
    {.pred.entity = Colleague, .obj.name = "X", .var = EcsVarIsVariable }
  }
});
```

**NOTE**: Variables are not yet universally supported yet in cached queries, as not all expressions yield cacheable archetypes. Near future versions of Flecs will have basic variable support for cached queries where variables cannot be used as subject, but are valid in other places. Full caching support for any expression with variables will likely not arrive before the next major release.

### Set Substitution
The different parts of a query term (predicates, subjects, objects) can be automatically substituted by following one of their relationships. While this may sound complicated, it is actually pretty common, and quite useful.

A typical example of a use case that requires substitution is, "find component `Position` for a parent entity". To achieve this, we need to tell the query term which relationship to follow to reach the parent. In this case that is the builtin `ChildOf` relationship.

We also need to tell the query in which direction to follow the relationship. We can either follow the relationship upwards, which gives us parents, or downwards, which gives us children of the substituted entity.

The following term shows how to write the above example down in the DSL:

```
Position(super(ChildOf))
```

Let's unpack what is happening here. First of all the term has a regular `Position` predicate. The subject of this term is `super(ChildOf)`. What this does is, it instructs the term to search upwards (`superset`) for the `ChildOf` relationship.

As a result, this term will follow the `ChildOf` relation of the `This` subject until it has found an object with `Position`. Here is the behavior in pseudo code:

```python
def find_object_w_component(This, Component):
  for pair in This.each(ChildOf, *):
    if pair.object.has(Component):
      yield pair.object
    else
      find_object_w_component(pair.object, Component)
  return 0 '// No match
```

### Parent components
Queries may use the `parent` shortcut to select components from a parent entity which is short for `super(ChildOf)` /`This` query:

```
Position(parent)
```

is equivalent to

```
Position(super(ChildOf))
```

#### Self Substitution
Substitution can do more than just searching supersets. It is for example possible to start the search on `This` itself, and when the component is not found on `This`, keep searching by following the `ChildOf` relation:

```c
Position(self|super(ChildOf))
```

A substitution that has both `self` and `superset` or `subset` is also referred to as an "inclusive" substitution.

This behavior is equivalent to this almost identical pseudo code:

```python
def find_object_w_component(This, Component):
  if This.has(Component):
    yield This

  for pair in This.each(ChildOf, *):
    find_object_w_component(pair.object, Component)

  return 0 '// No match
```

#### Substitution Depth
Queries can specify how deep the query should search. For example, the following term specifies to search the `ChildOf` relation, but no more than 3 levels deep:

```c
Position(super(ChildOf, 3))
```

Additionally, it is also possible to specify a minimum search depth:

```c
// Start at depth 2, search until at most depth 4
Position(super(ChildOf, 2, 4))
```

#### Cascade Ordering
Substitution expressions may contain the `cascade` modifier, which ensures that results of the query are iterated in breadth-first order, where depth is defined by the relation used int the substitution.

A useful application of `cascade` is transform systems, where parents need to be transformed before their children. The term in the following example finds the `Transform` component from both `This` and its parent, while ordering the results of the query breadth-first:

```
Transform, Transform(cascade|super(ChildOf))
```

In an actual transform system we would also want to match the root, which can be achieved by making the second term optional:

```
Transform, ?Transform(cascade|super(ChildOf))
```

#### Substitute for All
The default behavior of a substitution term is to stop looking when an object with the required component has been found. The following example shows a term that specifies that the substitution needs to keep looking, so that the entire tree (upwards or downwards) for a subject is returned:

```
Transform(all|super(ChildOf))
```

#### Substitution on Identifiers
So far all the substitution terms have applied to a default (`This`) subject. Substitution can also be defined on entities other than `This`:

```c
// Get Position for a parent of Bob
Position(Bob[super(ChildOf)])
```

Additionally, substitution is not limited to the subject:

```c
// Does the entity like any of the parents of Alice?
(Likes, Alice[super(ChildOf)])
```

This example shows how to use substitution in the C++ API:

```cpp
// Position(super(ChildOf))
auto qb = world.query_builder<>()
  .term<Position>().super(ChildOf);

// Position(self|super(ChildOf, 3))
auto qb = world.query_builder<>()
  .term<Position>()
    .set(flecs::Self | flecs::SuperSet, flecs::ChildOf)
      .max_depth(3);
```

This example shows how to use wildcards in the query descriptor:

```c
// Position(super(ChildOf))
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_id(Position), .subj.set = {
      .mask = EcsSuperSet,
      .relation = EcsChildOf
    }}
  }
});

// Position(self|super(ChildOf, 3))
ecs_query_t *q = ecs_query_init(world, &(ecs_query_decs_t){
  .filter.terms = {
    {ecs_id(Position), .subj.set = {
      .mask = EcsSelf | EcsSuperSet,
      .relation = EcsChildOf,
      .max_depth = 3
    }}
  }
});
```

**NOTE**: While substitution can require recursive algorithms, cached queries do not perform this logic when iterating results. Matching for cached queries is only performed when queries are created, and afterwards, when new archetypes are created. As a result, cached queries can have complex expressions without visible impact on iteration performance.

**NOTE**: Subset substitution rules are not yet supported for cached queries. Terms with `all` are not supported yet in cached queries.

## Transitivity
Queries can work with transitive relations. A relation `R` is transitive if the following is true:

```basic
if R(X, Y) and R(Y, Z) then R(X, Z)
```

A common example of transitivity is inheritance. Consider, if `GrannySmith` is an `Apple`, and an `Apple` is `Fruit`, then a `GrannySmith` is also `Fruit`.

Suppose an application keeps track of the location of entities. Entities can be located in a neighborhood, which are part of a city, which are part of a country. We could create a `LocatedIn` relation with a few locations like this:

```cpp
// Create the LocatedIn relation
auto LocatedIn = world.entity();

// Create locations & their relations to each other
auto US = world.entity();
auto SanFrancisco = world.entity().add(LocatedIn, US);
auto Mission = world.entity().add(LocatedIn, SanFrancisco);
auto SOMA = world.entity().add(LocatedIn, SanFrancisco);

// An entity located in the Mission
auto e = world.entity().add(LocatedIn, Mission);
```

We could now create the following query:
```c
(LocatedIn, SanFrancisco)
```

At the moment this query will not match with the entity, because the entity itself does not have `(LocatedIn, SanFrancisco)`. We can however change this behavior by making the `LocatedIn` relation transitive:

```cpp
auto LocatedIn = world.entity().add(flecs::Transitive);
```

This tells the query that it should treat an entity that has `(LocatedIn, Mission)` as an entity that has `(LocatedIn, SanFrancisco)`, because `Mission` has `(LocatedIn, SanFrancisco)`. We can now also run this query:

```
(LocatedIn, US)
```

which will also return the entity, as it has `(LocatedIn, Mission)`, `Mission` has `(LocatedIn, SanFrancisco)`, and `SanFrancisco` has `(LocatedIn, US)`.

### Transitivity and Substitution
To understand how transitivity is implemented, we need to look at how queries interpret a transitive relation. When a query encounters a transitive relation, it inserts a substitution term for that relation. Consider the previous example with a transitive `LocatedIn` relation:

```
(LocatedIn, SanFrancisco)
```

To find whether the subject should match this term, it should not just consider `(LocatedIn, SanFrancisco)`, but also `(LocatedIn, Mission)` and `(LocatedIn, SOMA)`. To achieve this, a query will insert an implicit substitution on the object, when the relation is transitive:

```
(LocatedIn, SanFrancisco[self|sub(LocatedIn)])
```

Note that the substitution includes `self`, as we also should match entities that are in `SanFrancisco` itself.

### Transitivity and Cycles
Transitive relations are not allows to have cycles. While neither queries nor the storage check for cycles (doing so would be too expensive), adding a cycle with a transitive relation can cause infinite recursion when evaluating the relation with a query.

## The IsA relation
The `IsA` relation is a builtin flecs relation that allows applications to define that an object is a subset of another object. Consider an application that categorizes artworks. It could create the following `IsA` hierarchy:

```cpp
auto Artwork = world.entity();
auto Painting = world.entity().add(flecs::IsA, Artwork);
auto Portrait = world.entity().add(flecs::IsA, Painting);
auto SelfPortrait = world.entity().add(flecs::IsA, Portrait);
```

Alternatively, it could use the shorthand `is_a`: function

```cpp
auto Artwork = world.entity();
auto Painting = world.entity().is_a(Artwork);
auto Portrait = world.entity().is_a(Painting);
auto SelfPortrait = world.entity().is_a(Portrait);
```

We could now instantiate artworks with these "classes":

```cpp
auto MonaLisa = world.entity().add(Portrait);
```

The `IsA` relation is transitive. This means that adding `Portrait` to `MonaLisa`, also means that the entity should be treated _as if_ it also had `Artwork` and `Painting`.

### The IsA relation and Queries
Queries do automatic `IsA` substitution on the predicate, subject and object. To see how this works exactly, let's use the previous example. A query term could ask for all entities that are an artwork:

```
Artwork
```

Ordinarily this would only match entities that have `Artwork`, but because there are entities with an `IsA` relation to `Artwork`, this query should expand to:

```
Artwork
Painting
Portrait
SelfPortrait
```

To achieve this, a query implicitly substitutes terms with their `IsA` subsets. When written out in full, this looks like:

```
Artwork[self|all|sub(IsA)]
```

The default relation for set substitution is `IsA`, so we can rewrite this as a slightly shorter term:

```
Artwork[self|all|subset]
```

Note the `all` modifier. We need to consider all subsets, not just the subsets until a result has been found, to find all entities that are artworks.

The predicate is not the only part of a query for which implicit `IsA` substitution happens. Imagine we use this query to find all things that are an artwork:

```
(IsA, Artwork)
```

Instead of storing the actual artworks (the `MonaLisa`) this query returns all the things that _are_ an artwork: `Painting`, `Portrait` and `SelfPortrait`. To achieve this, the query needs to substitute the object. When written out in full, this looks like:

```
(IsA, Artwork[self|all|subset])
```

This effectively expands the query to:

```
(IsA, Artwork)
(IsA, Painting)
(IsA, Portrait)
(IsA, SelfPortrait)
```

Finally, the subject itself can also be automatically substituted. This is quite common for applications that use the `IsA` relation to construct prefabs. Consider the following code that creates a spaceship prefab:

```cpp
auto SpaceShip = world.prefab()
  .set<Attack>({100})
  .set<Defense>({50})
  .set<MaxSpeed>({75});
```

We can now use this prefab to create an entity that shares its components with the `IsA` relation:

```cpp
auto my_spaceship = world.entity()
  .is_a(SpaceShip)
  .set<Position>({10, 20});
  .set<Velocity>({5, 5});
```

Suppose we now want to find all entities that exceed their maximum speed. We could use the following query:

```
MaxSpeed, Velocity
```

However, `my_spaceship` only has `MaxSpeed` through `SpaceShip`, which this query will not find unless it substitutes the subject. When written out, this substitution looks like this:

```
MaxSpeed(self|superset), Velocity(self|superset)
```

When all implicit substitution is written out in full, a single component query looks like this:

```
Component[self|all|subset] (self|superset)
```

**NOTE**: cached queries currently do not support implicit substitution on predicates and objects. Implicit `IsA` substitution on subjects needs to be enabled explicitly, by specifying `subset` or `self|subset` as the subject.

This behavior may change in future versions, as more efficient methods become available to find the set of `IsA` subsets or supersets for an entity.

### Final Entities
An application can mark an entity as `Final`, which means that it cannot have  `IsA` subsets. This is similar to the meaning of `Final` in class-based inheritance, where a final class cannot be derived from.

A query will not do implicit `IsA` substitution for predicates if they are final. When a component is registered, it is made `Final` by default, which means that for regular component queries a query will never attempt to substitute the predicate. Additionally, the algorithm that searches for `IsA` subsets may run more efficient when a `Final` subset is found.

To mark an entity as final, an application can add the `Final` tag:

```cpp
// Don't try to substitute "Likes" in queries
auto Likes = world.entity().add(flecs::Final);
```
