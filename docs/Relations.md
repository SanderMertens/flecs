# Relations
The relationships feature allows for the creation of entity graphs, by specifying which relations entities have to each other. Relations are similar to regular components and tags, in that they can contain data or no data, and can be added and removed. The following example demonstrates a simple example that uses relations:

```c
ecs_entity_t Likes = ecs_new_id(world);
ecs_entity_t Bob = ecs_new_id(world);
ecs_entity_t Alice = ecs_new_id(world);

// Bob Likes Alice
ecs_add_pair(world, Bob, Likes, Alice);

// Bob Likes Alice no more
ecs_remove_pair(world, Bob, Likes, Alice);
```
```cpp
auto Likes = world.entity();
auto Bob = world.entity();
auto Alice = world.entity();

// Bob Likes Alice
Bob.add(Likes, Alice);

// Bob Likes Alice no more
Bob.remove(Likes, Alice);
```

In this example, we would refer to `Bob` as the "subject", `Likes` as the "relation" and `Alice` as the "object". A relation when combined with an object is called a "relation pair". A pair combined with a subject is called a "relation triple".

The same relation can be added multiple times to an entity, as long as its object is different:

```c
ecs_entity_t Bob = ecs_new_id(world);
ecs_entity_t Eats = ecs_new_id(world);
ecs_entity_t Apples = ecs_new_id(world);
ecs_entity_t Pears = ecs_new_id(world);

ecs_add_pair(world, Bob, Eats, Apples);
ecs_add_pair(world, Bob, Eats, Pears);

ecs_has_pair(world, Bob, Eats, Apples); // true
ecs_has_pair(world, Bob, Eats, Pears); // true
```
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

An application can query for relationships with the `(Relation, Object)` notation:

```c
// Find all entities that eat apples
ecs_query_t *q = ecs_query_new(world, "(Eats, Apples)");

// Find all entities that eat anything
ecs_query_t *q = ecs_query_new(world, "(Eats, *)");

// Or with the ecs_query_init function:
ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
    .filter.terms = {{ecs_pair(Eats, Apples)}}
});
```
```cpp
// Find all entities that eat apples
auto q = world.query("(Eats, Apples)");

// Find all entities that eat anything
auto q = world.query("(Eats, *)");

// Or with the query builder API:
auto q = world.query_builder<>()
  .term(Eats, Apples)
  .build();
```

This example just shows a simple relation query. Relation queries are much more powerful than this as they provide the ability to match against entity graphs of arbitrary size. For more information on relation queries see the [Query manual](Queries.md).

## Relation components
So far we've just seen relations with regular entities. When used in combination with components, relations can be associated with data:

```c
typedef struct {
    float amount;
} Requires;

ECS_COMPONENT(world, Requires);
ECS_TAG(world, GigaWatts);

ecs_entity_t delorean = ecs_new_id(world);

ecs_set_pair(world, delorean, Requires, GigaWatts, {1.21});

const Requires *r = ecs_get_pair(world, delorean, Requires, GigaWatts);
printf("%f gigawatts!\n", r->amount);
```
```cpp
struct Requires {
  float amount;
};

struct Gigawatts { };

auto delorean = world.entity()
  .set<Requires, Gigawatts>({1.21});

auto r = delorean.get<Requires, Gigawatts>();
cout << r->value << " gigawatts!" << endl;
```

Relations composed out of types can use the `flecs::pair` template in the C++ API:

```cpp
using RequiresGigawatts = flecs::pair<Requires, Gigawatts>;

auto delorean = world.entity()
  .set<RequiresGigawatts>({1.21});

auto r = delorean.get<RequiresGigaWatts>();
cout << r->value << " gigawatts!" << endl;
```

An advantage of the `flecs::pair` template is that it can be used with function-style get/set operations:

```cpp
// rvalue required as API converts value from 'Requires' to 'RequiresGigawatts'
e.set([](RequiresGigawatts&& r) {
  r->value = 1.21; // Overloaded '->' operator to get the pair value 
})

e.get([](const RequiresGigawatts& r) {
  cout << r->value << " gigawatts!" << endl;
});
```

In the above examples we added data to the "relation" part of the pair. If the relation part is a tag, we can also attach data to the object:

```c
typedef struct {
  const char *value;
} Event;

ECS_TAG(world, Begin);
ECS_TAG(world, End);
ECS_COMPONENT(world, Event);

ecs_entity_t universe = ecs_new_id(world);

// Set data on the Event object, instead of the Begin, End relations:
ecs_set_pair_object(world, universe, Begin, Event, {"Big Bang"});
ecs_set_pair_object(world, universe, End, Event, {"Heat Death"});

const Event *e = ecs_get_pair_object(world, universe, Begin, Event);
printf("In the beginning there was the %s\n", e->value);
```
```cpp
struct Event {
  const char *value;
};

struct Begin { };
struct End { };

using BeginEvent = flecs::pair<Begin, Event>;
using EndEvent = flecs::pair<End, Event>;

// Set data on the Event object, instead of the Begin, End relations:
auto universe = world.entity();
  .set_object<Begin, Event>({"Big Bang"})
  .set_object<End, Event>({"Heat Death"});

universe.get([](const BeginEvent& e) {
  cout << "In the beginning there was the " << e.value << endl;
});
```

### Using relations to add components multiple times
A limitation of components is that they can only be added once to an entity. Relations make it possible to get around this limitation, as a component _can_ be added multiple times, as long as the pair is unique. Pairs can be constructed on the fly from new entity identifiers, which means this is possible:

```c
typedef struct {
  float x;
  float y;
} Position;

ecs_entity_t e = ecs_new_id(world);

ecs_entity_t first = ecs_new_id(world);
ecs_entity_t second = ecs_new_id(world);
ecs_entity_t third = ecs_new_id(world);

// Add component position 3 times, for 3 different objects
ecs_add_pair(world, e, Position, first, {1, 2});
ecs_add_pair(world, e, Position, second, {3, 4});
ecs_add_pair(world, e, Position, third, {5, 6});
```
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

## Relation wildcards
When querying for relation pairs, it is often useful to be able to find all instances for a given relation or object. To accomplish this, an application can use wildcard expressions. Consider the following example, that queries for all entities with a `Likes` relation:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
  .filter.terms = {
    {ecs_pair(Likes, EcsWildcard)}
  }
});

ecs_iter_t it = ecs_query_iter(q);

while (ecs_query_next(&it)) {
  ecs_id_t id = ecs_term_id(&it, 1); // Obtain pair id

  // Get relation & object
  ecs_entity_t rel = ecs_pair_relation(world, id);
  ecs_entity_t obj = ecs_pair_object(world, id);

  for (int i = 0; i < it.count; it++) {
    printf("entity %d has relation %s, %s\n", 
      it.entities[i],
      ecs_get_name(world, rel), 
      ecs_get_name(world, obj));
  }
}
```
```cpp
auto q = world.query_builder()
  .term(Likes, flecs::Wildcard)
  .build();

q.iter([]flecs::iter& it) {
  auto id = it.term_id(1);

  for (auto i : it) {
    cout << "entity " << it.entity(i) << " has relation "
      << id.relation().name() << ", "
      << id.object().name() << endl;
  }
});
```

Wildcards may appear in query expressions, using the `*` character:

```c
ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
  .filter.expr = "(Likes, *)"
});
```
```cpp
auto q = world.query("(Likes, *)");
```

Wildcards may used for the relation or object part of a pair, or both:

```cpp
"(Likes, *)" // Matches all Likes relations
"(*, Alice)" // Matches all relations with Alice as object
"(*, *)"     // Matches all relations
```

## Inspecting relations
An application can use pair wildcard expressions to find all instances of a relationship for an entity. The following example shows how to find all `Eats` relationships for an entity:

```c
// Bob eats apples and pears
ecs_entity_t Eats = ecs_new_id(world);
ecs_entity_t Apples = ecs_new_id(world);
ecs_entity_t Pears = ecs_new_id(world);

ecs_entity_t Bob = ecs_new_id(world);
ecs_add_pair(world, Bob, Eats, Apples);
ecs_add_pair(world, Bob, Eats, Pears);

// Find all (Eats, *) relations in Bob's type
ecs_type_t bob_type = ecs_get_type(world, Bob);
ecs_id_t wildcard = ecs_pair(Eats, EcsWildcard);
ecs_id_t *ids = ecs_vector_first(bob_type);
int32_t cur = 0;

while (-1 != (cur = ecs_type_match(type, cur, wildcard))) {
  ecs_entity_t obj = ecs_pair_object(ids[cur]);
  printf("Bob eats %s\n", ecs_get_name(world, obj));
}
```
```cpp
// Bob eats apples and pears
auto Bob = world.entity();
auto Eats = world.entity();
auto Apples = world.entity();
auto Pears = world.entity();

Bob.add(Eats, Apples);
Bob.add(Eats, Pears);

// Find all (Eats, *) relations in Bob's type
bob.match(world.pair(Eats, flecs::Wildcard), [](flecs::id id) {
  cout << "Bob eats " << id.object().name() << endl;
});

// For object wildcard pairs, each() can be used:
bob.each(Eats, [](flecs::entity obj) {
  cout << "Bob eats " << obj.name() << endl;
})
```

## Relation cleanup
When either the relation or object entity of a relation is deleted, by default all of the instances of the relation are removed from the store. Consider the following example:

```c
ecs_entity_t Likes = ecs_new_id(world);
ecs_entity_t Bob = ecs_new_id(world);
ecs_entity_t Alice = ecs_new_id(world);

ecs_add_pair(world, Bob, Likes, Alice);

// This removes (Likes, Alice) from Bob, and all other entities that had a 
// relation with Alice
ecs_delete(world, Alice);
```
```cpp
auto Likes = world.entity();
auto Bob = world.entity();
auto Alice = world.entity();

Bob.add(Likes, Alice);

// This removes (Likes, Alice) from Bob, and all other entities that had a 
// relation with Alice
Alice.destruct();
```

This behavior can be customized with cleanup policies as the above behavior is not always what you want. A typical example is the builtin `ChildOf` relation, where child entities should be deleted when the parent is deleted:

```c
ecs_entity_t Spaceship = ecs_new_id(world);
ecs_entity_t Cockpit = ecs_new_id(world);

ecs_add_pair(world, Cockpit, EcsChildOf, Spaceship);

// This deletes both the spaceship and the cockpit entity
ecs_delete(world, Spaceship);
```
```cpp
auto Spaceship = world.entity();
auto Cockpit = world.entity();

Cockpit.child_of(Spaceship);

// This deletes both the spaceship and the cockpit entity
Spaceship.destruct();
```

To customize this behavior, an application can add the `OnDeleteObject` policy to the relationship. The following examples show how:

```c
ecs_entity_t Likes = ecs_new_id(world);
ecs_entity_t Bob = ecs_new_id(world);
ecs_entity_t Alice = ecs_new_id(world);

ecs_add_pair(world, Bob, Likes, Alice);

// When Alice is deleted, remove (Likes, Alice) from Bob
ecs_add_pair(world, Likes, EcsOnDeleteObject, EcsRemove);

// When Alice is deleted, delete Bob 
ecs_add_pair(world, Likes, EcsOnDeleteObject, EcsDelete);

// When Alice is deleted, throw an error (assert)
ecs_add_pair(world, Likes, EcsOnDeleteObject, EcsThrow);
```
```cpp
auto Likes = world.entity();
auto Bob = world.entity();
auto Alice = world.entity();

Bob.add(Likes, Alice);

// When Alice is deleted, remove (Likes, Alice) from Bob
Likes.add(flecs::OnDeleteObject, flecs::Remove)

// When Alice is deleted, delete Bob 
Likes.add(flecs::OnDeleteObject, flecs::Delete);

// When Alice is deleted, throw an error (assert)
Likes.add(flecs::OnDeleteObject, flecs::Throw);
```

An application may also specify what cleanup action should be performed if the relation itself is deleted with the `OnDelete` policy:

```c
ecs_entity_t Likes = ecs_new_id(world);
ecs_entity_t Bob = ecs_new_id(world);
ecs_entity_t Alice = ecs_new_id(world);

ecs_add_pair(world, Bob, Likes, Alice);

// When Likes is deleted, remove (Likes, Alice) from Bob
ecs_add_pair(world, Likes, EcsOnDelete, EcsRemove);

// When Likes is deleted, delete Bob 
ecs_add_pair(world, Likes, EcsOnDelete, EcsDelete);

// When Likes is deleted, throw an error (assert)
ecs_add_pair(world, Likes, EcsOnDelete, EcsThrow);
```
```cpp
auto Likes = world.entity();
auto Bob = world.entity();
auto Alice = world.entity();

Bob.add(Likes, Alice);

// When Likes is deleted, remove (Likes, Alice) from Bob
Likes.add(flecs::OnDelete, flecs::Remove)

// When Likes is deleted, delete Bob 
Likes.add(flecs::OnDelete, flecs::Delete);

// When Likes is deleted, throw an error (assert)
Likes.add(flecs::OnDelete, flecs::Throw);
```

By default, components are created with the `(OnDelete, Throw)` policy.

## Relation performance
A relation that does not have any data has the same performance as a regular tag. A relation that does have data has the same performance as a component.

To understand how this works consider that on the storage level a component or tag is represented by a unique id and a size. The id identifies the component, whereas the size is the size of the component data. If an id represents a tag, the size is 0. An entity can only have a single instance of each unique id, which means that you can only add component `Position` once to the same entity.

On the storage level, relations are nothing more than unique ids, which are generated from a (relation, object) pair by the API. Each unique (relation, object) combination produces a unique id that can then be added or removed from an entity in the storage. For the most part the storage is unaware of whether an id represents a relation, component or tag which means their performance is equivalent.

The following code example shows how the API generates a unique id from a pair before passing it down to the storage:

```c
// This function call
const Requires *r = ecs_get_pair(world, e, Requires, Gigawatts);

// Is equivalent to this code
ecs_id_t pair_id = ecs_pair(ecs_id(Requires), Gigawatts);
const Required *r = ecs_get_id(world, e, pair_id); // Regular get

// The pair_id is generated by storing the relation id in the upper 32 bits of
// the identifier and the object in the lower 32 bits:
ecs_id_t pair_id = Requires_id << 32 | Gigawatts;
```
