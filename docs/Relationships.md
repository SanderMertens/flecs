# Relationships
The relationships feature makes it possible to describe entity graphs natively in ECS. Graphs are created by adding and removing relationships from one entity to another entity. See this [blog](https://ajmmertens.medium.com/building-games-in-ecs-with-entity-relationships-657275ba2c6c) for an introduction to entity relationships.

Adding/removing relationships is similar to adding/removing regular components, with as difference that instead of a single component id, a relationship adds a pair of two things to an entity. In this pair, the first element represents the relationship (e.g. "Eats"), and the second element represents the relationship target (e.g. "Apples").

Relationships can be used to describe many things, from hierarchies to inventory systems to trade relationships between players in a game. The following sections go over how to use relationships, and what features they support.

## Definitions

| Name         | Description |
|--------------|-------------|
| Id           | An id that can be added and removed |
| Component    | Id with a single element (same as an entity id) |
| Pair         | Id with two elements |
| Tag          | Component or pair not associated with data |
| Relationship | Used to refer to first element of pair |
| Target       | Used to refer to second element of pair |
| Source       | Entity to which an id is added |

## Examples
Make sure to check out the code examples in the repository:

 - [relationships (C)](https://github.com/SanderMertens/flecs/tree/master/examples/c/relationships)
 - [relationships (C++)](https://github.com/SanderMertens/flecs/tree/master/examples/cpp/relationships)
 - [relationships (C#)](https://github.com/BeanCheeseBurrito/Flecs.NET/tree/main/src/Flecs.NET.Examples/Cpp/Relationships)

## Introduction
The following code is a simple example that uses relationships:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t Likes = ecs_new(world);
ecs_entity_t Bob = ecs_new(world);
ecs_entity_t Alice = ecs_new(world);

// Bob Likes Alice
ecs_add_pair(world, Bob, Likes, Alice);

// Bob Likes Alice no more
ecs_remove_pair(world, Bob, Likes, Alice);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto Likes = world.entity();
auto Bob = world.entity();
auto Alice = world.entity();

// Bob Likes Alice
Bob.add(Likes, Alice);

// Bob Likes Alice no more
Bob.remove(Likes, Alice);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity Likes = world.Entity();
Entity Bob = world.Entity();
Entity Alice = world.Entity();

// Bob Likes Alice
Bob.Add(Likes, Alice);

// Bob Likes Alice no more
Bob.Remove(Likes, Alice);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let likes = world.entity();
let bob = world.entity();
let alice = world.entity();

// bob likes alice
bob.add_id((likes, alice));

// bob likes alice no more
bob.remove_id((likes, alice));
```

</li>
</ul>
</div>

In this example, we refer to `Bob` as the "source", `Likes` as the "relationship" and `Alice` as the "target". A relationship when combined with an target is called a "relationship pair".

The same relationship can be added multiple times to an entity, as long as its target is different:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t Bob = ecs_new(world);
ecs_entity_t Eats = ecs_new(world);
ecs_entity_t Apples = ecs_new(world);
ecs_entity_t Pears = ecs_new(world);

ecs_add_pair(world, Bob, Eats, Apples);
ecs_add_pair(world, Bob, Eats, Pears);

ecs_has_pair(world, Bob, Eats, Apples); // true
ecs_has_pair(world, Bob, Eats, Pears); // true
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto Bob = world.entity();
auto Eats = world.entity();
auto Apples = world.entity();
auto Pears = world.entity();

Bob.add(Eats, Apples);
Bob.add(Eats, Pears);

Bob.has(Eats, Apples); // true
Bob.has(Eats, Pears); // true
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity Bob = world.Entity();
Entity Eats = world.Entity();
Entity Apples = world.Entity();
Entity Pears = world.Entity();

Bob.Add(Eats, Apples);
Bob.Add(Eats, Pears);

Bob.Has(Eats, Apples); // true
Bob.Has(Eats, Pears); // true
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let bob = world.entity();

let eats = world.entity();
let apples = world.entity();
let pears = world.entity();

bob.add_id((eats, apples));
bob.add_id((eats, pears));

bob.has_id((eats, apples)); // true
bob.has_id((eats, pears)); // true
```

</li>
</ul>
</div>

An application can query for relationships with the `(Relationship, Target)` notation:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Find all entities that eat apples
ecs_query_t *q = ecs_query(world, { .expr = "(Eats, Apples)" });

// Find all entities that eat anything
ecs_query_t *q = ecs_query(world, { .expr = "(Eats, *)" });

// Or with the ecs_query_cache_init function:
ecs_query_t *q = ecs_query(world, {
    .terms = {{ecs_pair(Eats, Apples)}}
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Find all entities that eat apples
auto q = world.query_builder<>()
  .expr("(Eats, Apples)")
  .build();

// Find all entities that eat anything
auto q = world.query_builder<>()
  .expr("(Eats, *)")
  .build();

// With the query builder API:
auto q = world.query_builder<>()
  .with(Eats, Apples)
  .build();

// Or when using pair types, when both relationship & target are compile time types:
auto q = world.query<flecs::pair<Eats, Apples>>();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Find all entities that eat apples
Query q = world.QueryBuilder()
    .Expr("(Eats, Apples)")
    .Build();

// Find all entities that eat anything
Query q = world.QueryBuilder()
    .Expr("(Eats, *)")
    .Build();

// With the query builder API:
Query q = world.QueryBuilder()
    .Expr(Eats, Apples)
    .Build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Find all entities that eat apples
let q = world.query::<()>().expr("(Eats, Apples)").build();

// Find all entities that eat anything
let q = world.query::<()>().expr("(Eats, *)").build();

// With the query builder API:
let q = world.query::<()>().with_id((eats, apples)).build();

// Or when using pair types, when both relationship & target are compile time types, they can be represented as a tuple:

let q = world.new_query::<&(Eats, Apples)>();
```

</li>
</ul>
</div>

This example just shows a simple relationship query. Relationship queries are much more powerful than this as they provide the ability to match against entity graphs of arbitrary size. For more information on relationship queries see the [query manual](Queries.md).

## Relationship queries
There are a number of ways an application can query for relationships. The following kinds of queries are available for all (unidirectional) relationships, and are all constant time:

### Test if entity has a relationship pair

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_has_pair(world, Bob, Eats, Apples);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
Bob.has(Eats, Apples);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Bob.Has(Eats, Apples);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
bob.has_id((eats, apples));
```

</li>
</ul>
</div>

### Test if entity has a relationship wildcard

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_has_pair(world, Bob, Eats, EcsWildcard);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
Bob.has(Eats, flecs::Wildcard);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Bob.Has(Eats, Ecs.Wildcard);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
bob.has_id((eats, flecs::Wildcard::ID));
```

</li>
</ul>
</div>

### Get parent for entity

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t parent = ecs_get_parent(world, Bob);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity parent = Bob.parent();
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity parent = Bob.Parent();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let parent = bob.parent();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let parent = bob.parent();
```

</li>
</ul>
</div>

### Find first target of a relationship for entity

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t food = ecs_get_target(world, Bob, Eats, 0);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity food = Bob.target(Eats);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity food = Bob.Target(Eats);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let food = bob.target_id(eats, 0); // first target
```

</li>
</ul>
</div>

### Find all targets of a relationship for entity

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
int32_t index = 0;
while ((food = ecs_get_target(world, Bob, Eats, index ++))) {
  // ...
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
int32_t index = 0;
while ((food = Bob.target(Eats, index ++))) {
  // ...
}
```

</li>
<li><b class="tab-title">C#</b>

```cs
int index = 0;
while ((food = Bob.Target(Eats, index++)) != 0)
{
  // ...
}
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let mut index = 0;
while bob.target_id(eats, index).is_some() {
    index += 1;
}
```

</li>
</ul>
</div>

### Find target of a relationship with component

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t parent = ecs_get_target_for(world, Bob, EcsChildOf, Position);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity parent = Bob.target_for<Position>(flecs::ChildOf);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity parent = Bob.TargetFor<Position>(Ecs.ChildOf);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let parent = bob.target_for::<Position>(flecs::ChildOf::ID);
```

</li>
</ul>
</div>

### Iterate all pairs for entity

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
const ecs_type_t *type = ecs_get_type(world, Bob);
for (int i = 0; i < type->count; i ++) {
  ecs_id_t id = type->array[i];
  if (ECS_IS_PAIR(id)) {
    ecs_entity_t first = ecs_pair_first(world, id);
    ecs_entity_t second = ecs_pair_second(world, id);
  }
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
Bob.each([](flecs::id id) {
  if (id.is_pair()) {
    flecs::entity first = id.first();
    flecs::entity second = id.second();
  }
});
```

</li>
<li><b class="tab-title">C#</b>

```cs
Bob.Each((Id id) =>
{
    if (id.IsPair())
    {
        Entity first = id.First();
        Entity second = id.Second();
    }
});
```

</li>
<li><b class="tab-title">Rust</b>

```rust
bob.each_component(|id| {
    if id.is_pair() {
        let first = id.first_id();
        let second = id.second_id();
    }
});
```

</li>
</ul>
</div>

### Find all entities with a pair

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *f = ecs_query(world, {
  .terms[0] = ecs_pair(Eats, Apples)
});

ecs_iter_t it = ecs_query_iter(world, f);
while (ecs_query_next(&it)) {
  for (int i = 0; i < it.count; i ++) {
    // Iterate as usual
  }
}

ecs_query_fini(f);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.query_builder()
  .with(Eats, Apples)
  .build()
  .each([](flecs::entity e) {
    // Iterate as usual
  });
```

</li>
<li><b class="tab-title">C#</b>

```cs
world.FilterBuilder()
    .With(Eats, Apples)
    .Build()
    .Each((Entity e) =>
    {
        // Iterate as usual
    });
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .query::<()>()
    .with_id((eats, apples))
    .build()
    .each_entity(|e, _| {
        // Iterate as usual
    });
```

</li>
</ul>
</div>

### Find all entities with a pair wildcard

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *f = ecs_query(world, {
  .terms[0] = ecs_pair(Eats, EcsWildcard)
});

ecs_iter_t it = ecs_query_iter(world, f);
while (ecs_query_next(&it)) {
  ecs_id_t pair_id = ecs_field_id(&it, 0);
  ecs_entity_t food = ecs_pair_second(world, pair_id); // Apples, ...
  for (int i = 0; i < it.count; i ++) {
    // Iterate as usual
  }
}
ecs_query_fini(f);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.query_builder()
  .with(Eats, flecs::Wildcard)
  .build()
  .each([](flecs::iter& it, size_t i) {
    flecs::entity food = it.pair(0).second(); // Apples, ...
    flecs::entity e = it.entity(i);
    // Iterate as usual
  });
```

</li>
<li><b class="tab-title">C#</b>

```cs
world.FilterBuilder()
    .With(Eats, Ecs.Wildcard)
    .Build()
    .Each((Iter it, int i) =>
    {
        Entity food = it.Pair(1).Second(); // Apples, ...
        Entity e = it.Entity(i);
        // Iterate as usual
    });
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .query::<()>()
    .with_id((eats, flecs::Wildcard::ID))
    .build()
    .each_iter(|it, i, _| {
        let food = it.pair(0).unwrap().second_id(); // Apples, ...
        let e = it.entity(i);
        // Iterate as usual
    });
```

</li>

</ul>
</div>

### Iterate all children for a parent

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
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
parent.children([](flecs::entity child) {
   // ...
});
```

</li>
<li><b class="tab-title">C#</b>

```cs
parent.Children((Entity child) =>
{
    // ...
});
```

</li>
<li><b class="tab-title">Rust</b>

```rust
parent.each_child(|child| {
    // ...
});
```

</li>
</ul>
</div>

More advanced queries are possible with Flecs queries. See the [Queries manual](Queries.md) for more details.

## Relationship components
Relationship pairs, just like regular component, can be associated with data. To associate data with a relationship pair, at least one of its elements needs to be a component. A pair can be associated with at most one type. To determine which type is associated with a relationship pair, the following rules are followed in order:

- If neither the first nor second elements are a type, the pair is a tag
- If the first element has the [tag](Relationships.md#tag-property) property, the pair is a tag
- If the first element is a type, the pair type is the first element
- If the second element is a type, the pair type is the second element

The following examples show how these rules can be used:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
typedef struct {
  float x, y;
} Position;

typedef struct {
  float amount;
} Eats;

// Components
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Eats);

// Tags
ecs_entity_t Likes = ecs_new(world);
ecs_entity_t Begin = ecs_new(world);
ecs_entity_t End = ecs_new(world);
ecs_entity_t Apples = ecs_new(world);

ecs_entity_t e = ecs_new(world);

// Both Likes and Apples are tags, so (Likes, Apples) is a tag
ecs_add_pair(world, e, Likes, Apples);

// Eats is a type and Apples is a tag, so (Eats, Apples) has type Eats
ecs_set_pair(world, e, Eats, Apples, { .amount = 1 });

// Begin is a tags and Position is a type, so (Begin, Position) has type Position
ecs_set_pair_second(world, e, Begin, Position, {0, 0});
ecs_set_pair_second(world, e, End, Position, {10, 20});  // same for End

// ChildOf has the Tag property, so even though Position is a type, the pair
// does not assume the Position type
ecs_add_pair(world, e, EcsChildOf, Position);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
struct Position {
  float x, y;
};

struct Eats {
  float amount;
};

// Empty types (types without members) are automatically interpreted as tags
struct Begin { };
struct End { };

// Tags
flecs::entity Likes = world.entity();
flecs::entity Apples = world.entity();

flecs::entity e = world.entity();

// Both Likes and Apples are tags, so (Likes, Apples) is a tag
e.add(Likes, Apples);

// Eats is a type and Apples is a tag, so (Eats, Apples) has type Eats
e.set<Eats>(Apples, { 1 });

// Begin is a tags and Position is a type, so (Begin, Position) has type Position
e.set<Begin, Position>({0, 0});
e.set<End, Position>({10, 20}); // Same for End

// ChildOf has the Tag property, so even though Position is a type, the pair
// does not assume the Position type
e.add(flecs::ChildOf, world.id<Position>());
```

</li>
<li><b class="tab-title">C#</b>

```cs
public record struct Position(float X, float Y);
public record struct Eats(float Amount);

// Empty types (types without members) are automatically interpreted as tags
public struct Begin { }
public struct End { }

// Tags
Entity Likes = world.Entity();
Entity Apples = world.Entity();

Entity e = world.Entity();

// Both Likes and Apples are tags, so (Likes, Apples) is a tag
e.Add(Likes, Apples);

// Eats is a type and Apples is a tag, so (Eats, Apples) has type Eats
e.Set<Eats>(Apples, new Eats(1));

// Begin is a tags and Position is a type, so (Begin, Position) has type Position
e.Set<Begin, Position>(new Position(0, 0));
e.Set<End, Position>(new Position(10, 20)); // Same for End

// ChildOf has the Tag property, so even though Position is a type, the pair
// does not assume the Position type
e.Add(Ecs.ChildOf, world.Id<Position>());
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Empty types (types without members) are letmatically interpreted as tags

#[derive(Component)]
struct Begin;

#[derive(Component)]
struct End;

// Tags
let likes = world.entity();
let apples = world.entity();

let e = world.entity();

// Both likes and Apples are tags, so (likes, Apples) is a tag
e.add_id((likes, apples));

// Eats is a type and Apples is a tag, so (Eats, Apples) has type Eats
e.set_pair::<Eats, Apples>(Eats { amount: 1 });

// Begin is a tags and Position is a type, so (Begin, Position) has type Position
e.set_pair::<Begin, Position>(Position { x: 10.0, y: 20.0 });
e.set_pair::<End, Position>(Position { x: 100.0, y: 20.0 }); // Same for End

// ChildOf has the Tag property, so even though Position is a type, the pair
// does not assume the Position type
e.add_id((flecs::ChildOf::ID, world.component_id::<Position>()));
e.add::<(flecs::ChildOf, Position)>();
```

</li>
</ul>
</div>

### Using relationships to add components multiple times
A limitation of components is that they can only be added once to an entity. Relationships make it possible to get around this limitation, as a component _can_ be added multiple times, as long as the pair is unique. Pairs can be constructed on the fly from new entity identifiers, which means this is possible:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
typedef struct {
  float x;
  float y;
} Position;

ecs_entity_t e = ecs_new(world);

ecs_entity_t first = ecs_new(world);
ecs_entity_t second = ecs_new(world);
ecs_entity_t third = ecs_new(world);

// Add component position 3 times, for 3 different objects
ecs_add_pair(world, e, Position, first, {1, 2});
ecs_add_pair(world, e, Position, second, {3, 4});
ecs_add_pair(world, e, Position, third, {5, 6});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
struct Position {
  float x;
  float y;
}

auto e = world.entity();

auto first = world.entity();
auto second = world.entity();
auto third = world.entity();

// Add component position 3 times, for 3 different objects
e.set<Position>(first, {1, 2});
e.set<Position>(second, {3, 4});
e.set<Position>(third, {5, 6});
```

</li>
<li><b class="tab-title">C#</b>

```cs
public record struct Position(float X, float Y);

Entity e = world.Entity();

Entity first = world.Entity();
Entity second = world.Entity();
Entity third = world.Entity();

// Add component position 3 times, for 3 different objects
e.Set<Position>(first, new(1, 2));
e.Set<Position>(second, new(3, 4));
e.Set<Position>(third, new(5, 6));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.entity();

let first = world.entity();
let second = world.entity();
let third = world.entity();

// Add component position 3 times, for 3 different objects
e.set_first::<Position>(Position { x: 1.0, y: 2.0 }, first);
e.set_first::<Position>(Position { x: 3.0, y: 4.0 }, second);
e.set_first::<Position>(Position { x: 5.0, y: 6.0 }, third);
```

</li>
</ul>
</div>

## Relationship wildcards
When querying for relationship pairs, it is often useful to be able to find all instances for a given relationship or target. To accomplish this, an application can use wildcard expressions. Consider the following example, that queries for all entities with a `Likes` relationship:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
  .terms = {
    {ecs_pair(Likes, EcsWildcard)}
  }
});

ecs_iter_t it = ecs_query_iter(world, q);

while (ecs_query_next(&it)) {
  ecs_id_t id = ecs_field_id(&it, 0); // Obtain pair id

  // Get relationship & target
  ecs_entity_t rel = ecs_pair_first(world, id);
  ecs_entity_t obj = ecs_pair_second(world, id);

  for (int i = 0; i < it.count; it++) {
    printf("entity %d has relationship %s, %s\n",
      it.entities[i],
      ecs_get_name(world, rel),
      ecs_get_name(world, obj));
  }
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto q = world.query_builder()
  .with(Likes, flecs::Wildcard)
  .build();

q.each([](flecs::iter& it, size_t i) {
  cout << "entity " << it.entity(i) << " has relationship "
    << it.pair(0).first().name() << ", "
    << it.pair(0).second().name() << endl;
});
```

</li>
<li><b class="tab-title">C#</b>

```cs
Query q = world.QueryBuilder()
    .With(Likes, Ecs.Wildcard)
    .Build();

q.Iter((Iter it) =>
{
    Id id = it.Pair(1);

    foreach (int i in it)
        Console.WriteLine($"entity {it.Entity(i)} has relationship {id.First()}, {id.Second()}");
});
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let q = world
    .query::<()>()
    .with_id((likes, flecs::Wildcard::ID))
    .build();

q.each_iter(|it, i, _| {
    println!(
        "entity {} has relationship {} {}",
        it.entity(i),
        it.pair(0).unwrap().first_id().name(),
        it.pair(0).unwrap().second_id().name()
    );
});
```

</li>
</ul>
</div>

Wildcards may appear in query expressions, using the `*` character:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
  .query.expr = "(Likes, *)"
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto q = world.query_builder<>().expr("(Likes, *)").build();
```

</li>
<li><b class="tab-title">C#</b>

```cs
Query q = world.QueryBuilder().Expr("(Likes, *)").Build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let q = world.query::<()>().expr("(likes, *)").build();
```

</li>
</ul>
</div>

Wildcards may used for the relationship or target part of a pair, or both:

```cpp
"(Likes, *)" // Matches all Likes relationships
"(*, Alice)" // Matches all relationships with Alice as target
"(*, *)"     // Matches all relationships
```

## Inspecting relationships
An application can use pair wildcard expressions to find all instances of a relationship for an entity. The following example shows how to find all `Eats` relationships for an entity:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Bob eats apples and pears
ecs_entity_t Eats = ecs_entity(world, { .name = "Eats" });
ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
ecs_entity_t Pears = ecs_entity(world, { .name = "Pears" });

ecs_entity_t Bob = ecs_new(world);
ecs_add_pair(world, Bob, Eats, Apples);
ecs_add_pair(world, Bob, Eats, Pears);

// Find all (Eats, *) relationships in Bob's type
ecs_table_t *bob_table = ecs_get_table(world, Bob);
ecs_type_t bob_type = ecs_get_type(world, Bob);
ecs_id_t wildcard = ecs_pair(Eats, EcsWildcard);
ecs_id_t *ids = ecs_vector_first(bob_type, ecs_id_t);
int32_t cur = -1;

while (-1 != (cur = ecs_search_offset(world, bob_table, cur + 1, wildcard, 0))){
  ecs_entity_t obj = ecs_pair_second(world, ids[cur]);
  printf("Bob eats %s\n", ecs_get_name(world, obj));
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Bob eats apples and pears
auto Bob = world.entity();
auto Eats = world.entity();
auto Apples = world.entity();
auto Pears = world.entity();

Bob.add(Eats, Apples);
Bob.add(Eats, Pears);

// Find all (Eats, *) relationships in Bob's type
bob.match(world.pair(Eats, flecs::Wildcard), [](flecs::id id) {
  cout << "Bob eats " << id.second().name() << endl;
});

// For target wildcard pairs, each() can be used:
bob.each(Eats, [](flecs::entity obj) {
  cout << "Bob eats " << obj.name() << endl;
})
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Bob eats apples and pears
Entity Bob = world.Entity();
Entity Eats = world.Entity();
Entity Apples = world.Entity();
Entity Pears = world.Entity();

Bob.Add(Eats, Apples);
Bob.Add(Eats, Pears);

// For target wildcard pairs, each() can be used:
bob.Each(Eats, (Entity obj) =>
{
    Console.WriteLine($"Bob eats {obj}");
})
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// bob eats apples and pears
let bob = world.entity();

let eats = world.entity();
let apples = world.entity();
let pears = world.entity();

bob.add_id((eats, apples));
bob.add_id((eats, pears));

// Find all (Eats, *) relationships in bob's type
bob.each_pair(eats, flecs::Wildcard::ID, |id| {
    println!("bob eats {}", id.second_id().name());
});

// For target wildcard pairs, each_target_id() can be used:
bob.each_target_id(eats, |entity| {
    println!("bob eats {}", entity.name());
});
```

</li>
</ul>
</div>

## Builtin relationships
Flecs comes with a few builtin relationships that have special meaning within the framework. While they are implemented as regular relationships and therefore obey the same rules as any custom relationship, they are used to enhance the features of different parts of the framework. The following two sections describe the builtin relationships of Flecs.

### The IsA relationship
The `IsA` relationship is a builtin relationship that allows applications to express that one entity is equivalent to another. This relationship is at the core of component sharing and plays a large role in queries. The `IsA` relationship can be used like any other relationship, as is shown here:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t Apple = ecs_new(world);
ecs_entity_t Fruit = ecs_new(world);
ecs_add_pair(world, Apple, EcsIsA, Fruit);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto Apple = world.entity();
auto Fruit = world.entity();
Apple.add(flecs::IsA, Fruit);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity Apple = world.Entity();
Entity Fruit = world.Entity();
Apple.Add(Ecs.IsA, Fruit);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let apple = world.entity();
let fruit = world.entity();

apple.add_id((flecs::IsA::ID, fruit));
```

</li>
</ul>
</div>

In C++ and C#, adding an `IsA` relationship has a shortcut:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C++</b>

```cpp
Apple.is_a(Fruit);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Apple.IsA(Fruit);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
apple.is_a_id(fruit);
```

</li>
</ul>
</div>

This indicates to Flecs that an `Apple` is equivalent to a `Fruit` and should be treated as such. This equivalence is one-way, as a `Fruit` is not equivalent to an `Apple`. Another way to think about this is that `IsA` allows an application to express subsets and supersets. An `Apple` is a subset of `Fruit`. `Fruit` is a superset of `Apple`.

We can also add `IsA` relationships to `Apple`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t GrannySmith = ecs_new(world);
ecs_add_pair(world, GrannySmith, EcsIsA, Apple);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto GrannySmith = world.entity();
GrannySmith.add(flecs::IsA, Apple);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity GrannySmith = world.Entity();
GrannySmith.Add(Ecs.IsA, Apple);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let granny_smith = world.entity();
granny_smith.add_id((flecs::IsA::ID, apple));
```

</li>
</ul>
</div>

This specifies that `GrannySmith` is a subset of `Apple`. A key thing to note here is that because `Apple` is a subset of `Fruit`, `GrannySmith` is a subset of `Fruit` as well. This means that if an application were to query for `(IsA, Fruit)` it would both match `Apple` and `GrannySmith`. This property of the `IsA` relationship is called "transitivity" and it is a feature that can be applied to any relationship. See the [section on Transitivity](#transitive-property) for more details.

#### Component sharing
An entity with an `IsA` relationship to another entity is equivalent to the other entity. So far the examples showed how querying for an `IsA` relationship will find the subsets of the thing that was queried for. In order for entities to be treated as true equivalents though, everything the superset contains (its components, tags, relationships) must also be found on the subsets. Consider:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t Spaceship = ecs_new(world);
ecs_set(world, Spaceship, MaxSpeed, {100});
ecs_set(world, SpaceShip, Defense, {50});

ecs_entity_t Frigate = ecs_new(world);
ecs_add(world, Frigate, EcsIsA, Spaceship);
ecs_set(world, Frigate, Defense, {100});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto Spaceship = world.entity()
  .set<MaxSpeed>({100})
  .set<Defense>({50});

auto Frigate = world.entity()
  .is_a(SpaceShip) // shorthand for .add(flecs::IsA, Spaceship)
  .set<Defense>({75});
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity Spaceship = world.Entity()
    .Set<MaxSpeed>(new(100))
    .Set<Defense>(new(50));

Entity Frigate = world.Entity()
    .IsA(SpaceShip) // shorthand for .Add(Ecs.IsA, Spaceship)
    .Set<Defense>(new(75));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let spaceship = world
    .entity()
    .set(MaxSpeed { value: 100 })
    .set(Defense { value: 50 });

let frigate = world
    .entity()
    .is_a_id(spaceship) // shorthand for .add(flecs::IsA, Spaceship)
    .set(Defense { value: 75 });
```

</li>
</ul>
</div>

Here, the `Frigate` "inherits" the contents of `SpaceShip`. Even though `MaxSpeed` was never added directly to `Frigate`, an application can do this:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Obtain the inherited component from Spaceship
const MaxSpeed *v = ecs_get(world, Frigate, MaxSpeed);
v->value == 100; // true
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Obtain the inherited component from Spaceship
const MaxSpeed *v = Frigate.get<MaxSpeed>();
v->value == 100; // true
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Obtain the inherited component from Spaceship
ref readonly MaxSpeed v = ref Frigate.Get<MaxSpeed>();
v.Value == 100; // true
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Obtain the inherited component from Spaceship
let is_100 = frigate.map::<&mut MaxSpeed, _>(|v| {
    v.value == 100 // True
});
```

</li>
</ul>
</div>

While the `Frigate` entity also inherited the `Defense` component, it overrode this with its own value, so that the following example works:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Obtain the overridden component from Frigate
const Defense *v = ecs_get(world, Frigate, Defense);
v->value == 75; // true
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Obtain the overridden component from Frigate
const Defense *v = Frigate.get<Defense>();
v->value == 75; // true
```

</li>
<li><b class="tab-title">C#</b>

```cs
ref readonly Defense v = ref Frigate.get<Defense>();
v.Value == 75; // true
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Obtain the overridden component from Frigate
let is_75 = frigate.map::<&mut Defense, _>(|v| {
    v.value == 75 // True
});
```

</li>
</ul>
</div>

The ability to share components is also applied transitively, so `Frigate` could be specialized further into a `FastFrigate`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t FastFrigate = ecs_new(world);
ecs_add(world, FastFrigate, EcsIsA, Frigate);
ecs_set(world, FastFrigate, MaxSpeed, {200});

// Obtain the overridden component from FastFrigate
const MaxSpeed *s = ecs_get(world, Frigate, MaxSpeed);
s->value == 200; // true

// Obtain the inherited component from Frigate
const Defense *d = Frigate.get<Defense>();
d->value == 75; // true
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto FastFrigate = world.entity()
  .is_a(Frigate)
  .set<MaxSpeed>({200});

// Obtain the overridden component from FastFrigate
const MaxSpeed *s = Frigate.get<MaxSpeed>();
s->value == 200; // true

// Obtain the inherited component from Frigate
const Defense *d = Frigate.get<Defense>();
d->value == 75; // true
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity FastFrigate = world.Entity()
    .IsA(Frigate)
    .Set<MaxSpeed>(new(200));

// Obtain the overridden component from FastFrigate
ref readonly MaxSpeed s = ref Frigate.Get<MaxSpeed>();
s.Value == 200; // true

// Obtain the inherited component from Frigate
ref readonly Defense d = ref Frigate.Get<Defense>();
d.Value == 75; // true
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let fast_frigate = world.entity().is_a_id(frigate).set(MaxSpeed { value: 200 });

// Obtain the overridden component from FastFrigate
let is_200 = fast_frigate.map::<&mut MaxSpeed, _>(|v| {
    v.value == 200 // True
});

// Obtain the inherited component from Frigate
let is_75 = fast_frigate.map::<&mut Defense, _>(|v| {
    v.value == 75 // True
});
```

</li>
</ul>
</div>

This ability to inherit and override components is one of the key enabling features of Flecs prefabs, and is further explained in the [Inheritance section](Manual.md#Inheritance) of the manual.

### The ChildOf relationship
The `ChildOf` relationship is the builtin relationship that allows for the creation of entity hierarchies. The following example shows how hierarchies can be created with `ChildOf`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t Spaceship = ecs_new(world);
ecs_entity_t Cockpit = ecs_new(world);

ecs_add_pair(world, Cockpit, EcsChildOf, Spaceship);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto Spaceship = world.entity();
auto Cockpit = world.entity();

Cockpit.add(flecs::ChildOf, Spaceship);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity Spaceship = world.Entity();
Entity Cockpit = world.Entity();

Cockpit.Add(Ecs.ChildOf, Spaceship);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let spaceship = world.entity();
let cockpit = world.entity();
cockpit.add_id((flecs::ChildOf::ID, spaceship));
```

</li>
</ul>
</div>

In C++, C# and Rust, adding a `ChildOf` relationship has a shortcut:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C++</b>

```cpp
Cockpit.child_of(Spaceship);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Cockpit.ChildOf(Spaceship);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
cockpit.child_of_id(spaceship);
```

</li>
</ul>
</div>

The `ChildOf` relationship is defined so that when a parent is deleted, its children are also deleted. For more information on specifying cleanup behavior for relationships, see the [Relationship cleanup properties](#cleanup-properties) section.

The `ChildOf` relationship is defined as a regular relationship in Flecs. There are however a number of features that interact with `ChildOf`. The following sections describe these features.

#### Namespacing
Entities in flecs can have names, and name lookups can be relative to a parent. Relative name lookups can be used as a namespacing mechanism to prevent clashes between entity names. This example shows a few examples of name lookups in combination with hierarchies:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create two entities with a parent/child name
ecs_entity_t parent = ecs_entity(world, {
  .name = "Parent"
});

ecs_entity_t child = ecs_entity(world, {
  .name = "Child"
});

// Create the hierarchy
ecs_add_pair(world, child, EcsChildOf, parent);

child = ecs_lookup(world, "Parent::Child"); // true
child = ecs_lookup_from(world, parent, "Child"); // true
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto parent = world.entity("Parent");
auto child = world.entity("Child")
  .child_of(parent);

child == world.lookup("Parent::Child"); // true
child == parent.lookup("Child"); // true
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity parent = world.Entity("Parent");
Entity child = world.Entity("Child")
    .ChildOf(parent);

child == world.Lookup("Parent.Child"); // true
child == parent.Lookup("Child"); // true
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let parent = world.entity_named("Parent");
let child = world.entity_named("Child").child_of_id(parent);

child == world.lookup("Parent::Child"); // true
child == parent.lookup("Child"); // true
```

</li>
</ul>
</div>

#### Scoping
In some scenarios a number of entities all need to be created with the same parent. Rather than adding the relationship to each entity, it is possible to configure the parent as a scope, which ensures that all entities created afterwards are created in the scope. The following example shows how:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t parent = ecs_new(world);
ecs_entity_t prev = ecs_set_scope(world, parent);

// Note that we're not using the ecs_new_id function for the children. This
// function only generates a new id, and does not add the scope to the entity.
ecs_entity_t child_a = ecs_new(world);
ecs_entity_t child_b = ecs_new(world);

// Restore the previous scope
ecs_set_scope(world, prev);

ecs_has_pair(world, child_a, EcsChildOf, parent); // true
ecs_has_pair(world, child_b, EcsChildOf, parent); // true
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto parent = world.entity();
auto prev = world.set_scope(parent);

auto child_a = world.entity();
auto child_b = world.entity();

// Restore the previous scope
world.set_scope(prev);

child_a.has(flecs::ChildOf, parent); // true
child_b.has(flecs::ChildOf, parent); // true
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity parent = world.Entity();
Entity prev = world.SetScope(parent);

Entity childA = world.Entity();
Entity childB = world.Entity();

// Restore the previous scope
world.SetScope(prev);

childA.Has(Ecs.ChildOf, parent); // true
childB.Has(Ecs.ChildOf, parent); // true
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let parent = world.entity();

let prev = world.set_scope_id(parent);

let child_a = world.entity();
let child_b = world.entity();

// Restore the previous scope
world.set_scope_id(prev);

child_a.has_id((flecs::ChildOf::ID, parent)); // true
child_b.has_id((flecs::ChildOf::ID, parent)); // true
```

</li>
</ul>
</div> 

Scopes in C++, C# and Rust can also be used with the `scope`/`Scope`/`run_in_scope` function on an entity, which accepts a (typically lambda) function:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C++</b>

```cpp
auto parent = world.entity().scope([&]{
  auto child_a = world.entity();
  auto child_b = world.entity();

  child_a.has(flecs::ChildOf, parent); // true
  child_b.has(flecs::ChildOf, parent); // true
});
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity parent = world.Entity().Scope(() =>
{
    Entity childA = world.Entity();
    Entity childB = world.Entity();

    childA.Has(Ecs.ChildOf, parent); // true
    childB.Has(Ecs.ChildOf, parent); // true
});
```

</li>
<<li><b class="tab-title">Rust</b>

```rust
let parent = world.entity().run_in_scope(|| {
    let child_a = world.entity();
    let child_b = world.entity();
    child_a.has_id((flecs::ChildOf::ID, parent)); // true
    child_b.has_id((flecs::ChildOf::ID, parent)); // true
});
```

</li>
</ul>
</div>

Scopes are the mechanism that ensure contents of a module are created as children of the module, without having to explicitly add the module as a parent.

## Relationship performance
This section goes over the performance implications of using relationships.

### Introduction
The ECS storage needs to know two things in order to store components for entities:

- Which ids are associated with an entity
- Which types are associated with those ids

Ids represent anything that can be added to an entity. An id that is not associated with a type is called a tag. An id associated with a type is a component. For regular components, the id is a regular entity that has the builtin `Component` component. This component contains the information needed by the storage to associate the entity with a type. If an entity does not have the `Component` component, it is a tag.

### Storing relationships
Relationships do not fundamentally change or extend the capabilities of the storage. Relationship pairs are two elements encoded into a single 64-bit id, which means that on the storage level they are treated the same way as regular component ids. What changes is the function that determines which type is associated with an id. For regular components this is simply a check on whether an entity has `Component`. To support relationships, [new rules](#relationship-components) are added to determine the type of an id.

Because of this, adding/removing relationships to entities has the same performance as adding/removing regular components. This becomes more obvious when looking more closely at a function that adds a relationship pair. The following example shows how the function that adds a regular component and the function that adds a pair actually map to the same functions:

```c
// Component
ECS_COMPONENT(world, Position);

// Tags
ecs_entity_t Likes = ecs_new(world);
ecs_entity_t Apples = ecs_new(world);
ecs_entity_t Npc = ecs_new(world);

ecs_entity_t e = ecs_new(world);

// The ecs_add_id function can be used to add one id to another
ecs_add_id(world, e, Npc);

// The ecs_add function is translated into an ecs_add_id function:
ecs_add(world, e, Position);
ecs_add_id(world, e, ecs_id(Position));

// The ecs_add_pair function is translated into an ecs_add_id function:
ecs_add_pair(world, e, Likes, Apples);
ecs_add_id(world, e, ecs_pair(Likes, Apples));

// ecs_pair() translates two ids into a single 64bit pair id.
```

This example also applies to C++, as the C++ API maps to the same C API functions.

While most of the storage uses the same code paths for regular components and relationships, there are a few properties of the storage that can impact performance when using relationships. These properties are not unique to relationships, but are more likely to be significant when using relationships.

### Id ranges
Flecs reserves entity ids under a threshold (`FLECS_HI_COMPONENT_ID`, default is 256) for components. This low id range is used by the storage to more efficiently encode graph edges between tables. Graph edges for components with low ids use direct array indexing, whereas graph edges for high ids use a hashmap. Graph edges are used to find the next archetype when adding/removing component ids, and are a contributing factor to the performance overhead of add/remove operations.

Because of the way pair ids are encoded, a pair will never be in the low id range. This means that adding/removing a pair id always uses a hashmap to find the next archetype. This introduces a small overhead, which is usually 5-10% of the total cost of an operation.

### Fragmentation
Fragmentation is a property of archetype-based ECS implementations where entities are spread out over more tables as the number of different component combinations increases. The overhead of fragmentation is visible in two areas:

- Table creation
- Queries (queries have to match & iterate more tables)

Applications that make extensive use of relationships might observe high levels of fragmentation, as relationships can introduce many different combinations of components. While the Flecs storage is optimized for supporting large amounts (hundreds of thousands) of tables, fragmentation is a factor to consider when using relationships.

Fragmentation can be reduced by using [union relationships](#union-property). There are additional storage improvements on the roadmap that will decrease the overhead of fragmentation introduced by relationships.

### Table Creation
When an id added to an entity is deleted, all references to that id are deleted from the storage (see [cleanup properties](#cleanup-properties)). For example, when the component `Position` is deleted it is removed from all entities, and all tables with the `Position` component are deleted. While not unique to relationships, it is more common for relationships to trigger cleanup actions, as relationship pairs contain regular entities.

The opposite is also true. Because relationship pairs can contain regular entities which can be created on the fly, table creation is more common than in applications that do not use relationships. While Flecs is optimized for fast table creation, creating and cleaning up tables is inherently more expensive than creating/deleting an entity. Therefore table creation is a factor to consider, especially for applications that make extensive use of relationships.

### Indexing
To improve the speed of evaluating queries, Flecs has indices that store all tables for a given component id. Whenever a new table is created, it is registered with the indices for the ids the table has, including ids for relationship pairs.

While registering a table for a relationship index is not more expensive than registering a table for a regular index, a table with relationships has to also register itself with the appropriate wildcard indices for its relationships. For example, a table with relationship `(Likes, Apples)` registers itself with the `(Likes, Apples)`, `(Likes, *)`, `(*, Apples)` and `(*, *)` indices. For this reason, creating new tables with relationships has a higher overhead than a table without relationships.

### Wildcard Queries
A wildcard query for a relationship pair, like `(Likes, *)` may return multiple results for each instance of the relationship. To find all instances of a relationship, the table index (see previous section) stores two additional pieces of information:

- The `column`: At which offset in the table type does the id first occur
- The `count`: How many occurrences of the id does the table have

If the id is not a wildcard id, the number of occurrences will always be one. When the id is a wildcard, a table type may have multiple occurrences of a relationship. For wildcard queries in the form of `(Likes, *)`, finding all occurrences is cheap, as a query can start at the `column` and iterate the next `count` members.

For wildcard queries in the form of `(*, Apples)`, however, the pair ids are not stored contiguously in a table type. This means that if a table has multiple instances that match `(*, Apples)`, a query will have to perform a linear search starting from `column`. Once the query has found `count` occurrences, it can stop searching.

The following example of a table type shows how relationships are ordered, and demonstrates why `(Likes, *)` wildcards are easier to resolve than `(*, Apples)` wildcards:

```
Npc, (Likes, Apples), (Likes, Pears), (Likes, Bananas), (Eats, Apples), (Eats, Pears)
```

The index for `(Likes, *)` will have `column=1, count=3`, whereas the index for `(*, Pears)` will have `column=2, count=2`. To find all occurrences of `(Likes, *)` a query can start iteration at index 1 and iterate 3 elements. To find all instances of `(*, Pears)` a query has to start at index 2 and scan until the second instance is found.
