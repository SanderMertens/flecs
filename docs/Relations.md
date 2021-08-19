# Relations
The relations feature allows for the creation of entity graphs, by specifying which relations entities have to each other. Relations are similar to regular components and tags, in that they can contain data or no data, and can be added and removed. The following code is a simple example that uses relations:

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

An application can query for relations with the `(Relation, Object)` notation:

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

// With the query builder API:
auto q = world.query_builder<>()
  .term(Eats, Apples)
  .build();
  
// Or when using pair types, when both relation & object are compile time types:
auto q = world.query<flecs::pair<Eats, Apples>>();  
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
  .set<Begin, Event>({"Big Bang"})
  .set<End, Event>({"Heat Death"});

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

ecs_iter_t it = ecs_query_iter(world, q);

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
An application can use pair wildcard expressions to find all instances of a relation for an entity. The following example shows how to find all `Eats` relations for an entity:

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
int32_t cur = -1;

while (-1 != (cur = ecs_type_index_of(type, cur + 1, wildcard))) {
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

## Builtin relations
Flecs comes with a few builtin relations that have special meaning within the framework. While they are implemented as regular relations and therefore obey the same rules as any custom relation, they are used to enhance the features of different parts of the framework. The following two sections describe the builtin relations of Flecs.

### The IsA relation
The `IsA` relation is a builtin relation that allows applications to express that one entity is equivalent to another. This relation is at the core of component sharing and plays a large role in queries. The `IsA` relation can be used like any other relation, as is shown here:

```c
ecs_entity_t Apple = ecs_new_id(world);
ecs_entity_t Fruit = ecs_new_id(world);
ecs_add_pair(world, Apple, EcsIsA, Fruit);
```
```cpp
auto Apple = world.entity();
auto Fruit = world.entity();
Apple.add(flecs::IsA, Fruit);
```

In C++, adding an `IsA` relation has a shortcut:

```cpp
Apple.is_a(Fruit);
```

This indicates to Flecs that an `Apple` is equivalent to a `Fruit` and should be treated as such. This equivalence is one-way, as a `Fruit` is not equivalent to an `Apple`. Another way to think about this is that `IsA` allows an application to express subsets and supersets. An `Apple` is a subset of `Fruit`. `Fruit` is a superset of `Apple`.

We can also add `IsA` relations to `Apple`:

```c
ecs_entity_t GrannySmith = ecs_new_id(world);
ecs_add_pair(world, GrannySmith, EcsIsA, Apple);
```
```cpp
auto GrannySmith = world.entity();
GrannySmith.add(flecs::IsA, Apple);
```

This specifies that `GrannySmith` is a subset of `Apple`. A key thing to note here is that because `Apple` is a subset of `Fruit`, `GrannySmith` is a subset of `Fruit` as well. This means that if an application were to query for `(IsA, Fruit)` it would both match `Apple` and `GrannySmith`. This property of the `IsA` relationhip is called "transitivity" and it is a feature that can be applied to any relation. See the [section on Transitivity](#transitive-relations) for more details.

#### Component sharing
An entity with an `IsA` relation to another entity is equivalent to the other entity. So far the examples showed how querying for an `IsA` relation will find the subsets of the thing that was queried for. In order for entities to be treated as true equivalents though, everything the supserset contains (its components, tags, relations) must also be found on the subsets. Consider:

```c
ecs_entity_t Spaceship = ecs_new_id(world);
ecs_set(world, Spaceship, MaxSpeed, {100});
ecs_set(world, SpaceShip, Defense, {50});

ecs_entity_t Frigate = ecs_new_id(world);
ecs_add(world, Frigate, EcsIsA, Spaceship);
ecs_set(world, Frigate, Defense, {100});
```
```cpp
auto Spaceship = world.entity()
  .set<MaxSpeed>({100})
  .set<Defense>({50});

auto Frigate = world.entity()
  .is_a(SpaceShip) // shorthand for .add(flecs::IsA, Spaceship)
  .set<Defense>({75});
```

Here, the `Frigate` "inherits" the contents of `SpaceShip`. Even though `MaxSpeed` was never added directly to `Frigate`, an application can do this:

```c
// Obtain the inherited component from Spaceship
const MaxSpeed *v = ecs_get(world, Frigate, MaxSpeed);
v->value == 100; // true
```
```cpp
// Obtain the inherited component from Spaceship
const MaxSpeed *v = Frigate.get<MaxSpeed>();
v->value == 100; // true
```

While the `Frigate` entity also inherited the `Defense` component, it overrode this with its own value, so that the following example works:

```c
// Obtain the overridden component from Frigate
const Defense *v = ecs_get(world, Frigate, Defense);
v->value == 75; // true
```
```cpp
// Obtain the overridden component from Frigate
const Defense *v = Frigate.get<Defense>();
v->value == 75; // true
```

The ability to share components is also applied transitively, so `Frigate` could be specialized further into a `FastFrigate`:

```c
ecs_entity_t FastFrigate = ecs_new_id(world);
ecs_add(world, FastFrigate, EcsIsA, Frigate);
ecs_set(world, FastFrigate, MaxSpeed, {200});

// Obtain the overridden component from FastFrigate
const MaxSpeed *s = ecs_get(world, Frigate, MaxSpeed);
s->value == 200; // true

// Obtain the inherited component from Frigate
const Defense *d = Frigate.get<Defense>();
d->value == 75; // true
```
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

This ability to inherit and override components is one of the key enabling features of Flecs prefabs, and is further explained in the [Inheritance section](Manual.md#Inheritance) of the manual.

#### Final entities
Entities can be annotated with the `Final` property, which prevents using them with the `IsA` relation. This is similar to the concept of a final class as something that cannot be extended. The following example shows how to add final to an entity:

```c
ecs_entity_t e = ecs_new_id(world);
ecs_add_id(world, e, EcsFinal);

ecs_entity_t i = ecs_new_id(world);
ecs_add_pair(world, e, i, EcsIsA, e); // not allowed
```
```cpp
auto e = ecs.entity()
  .add(flecs::Final);

auto i = ecs.entity()
  .is_a(e); // not allowed
```

Queries may use the final property to optimize, as they do not have to explore subsets of a final entity. For more information on how queries interpret final, see the [Query manual](Queries.md). By default, all components are created final.

### The ChildOf relation
The `ChildOf` relation is the builtin relation that allows for the creation of entity hierarchies. The following example shows how hierarchies can be created with `ChildOf`:

```c
ecs_entity_t Spaceship = ecs_new_id(world);
ecs_entity_t Cockpit = ecs_new_id(world);

ecs_add_pair(world, Cockpit, EcsChildOf, Spaceship);
```
```cpp
auto Spaceship = world.entity();
auto Cockpit = world.entity();

Cockpit.add(flecs::ChildOf, Spaceship);
```

In C++, adding a `ChildOf` relation has a shortcut:

```cpp
Cockpit.child_of(Spaceship);
```

The `ChildOf` relation is defined so that when a parent is deleted, its children are also deleted. For more information on specifying cleanup behavior for relations, see the [Relation cleanup properties](#relation-cleanup-properties) section.

The `ChildOf` relation is defined as a regular relation in Flecs. There are however a number of features that interact with `ChildOf`. The following sections describe these features.

#### Namespacing
Entities in flecs can have names, and name lookups can be relative to a parent. Relative name lookups can be used as a namespacing mechanism to prevent clashes between entity names. This example shows a few examples of name lookups in combination with hierarchies:

```c
// Create two entities with a parent/child name
ecs_entity_t parent = ecs_entity_init(world, &(ecs_entity_desc_t){
  .name = "Parent"
});

ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
  .name = "Child"
});

// Create the hierarchy
ecs_add_pair(world, child, EcsChildOf, parent);

child = ecs_lookup_fullpath(world, "Parent::Child"); // true
child = ecs_lookup_path(world, parent, "Child"); // true
```
```cpp
auto parent = world.entity("Parent");
auto child = world.entity("Child")
  .child_of(parent);

child == world.lookup("Parent::Child"); // true
child == parent.lookup("Child"); // true
```

#### Scoping
In some scenarios a number of entities all need to be created with the same parent. Rather than adding the relation to each entity, it is possible to configure the parent as a scope, which ensures that all entities created afterwards are created in the scope. The following example shows how:

```c
ecs_entity_t parent = ecs_new_id(world);
ecs_entity_t prev = ecs_set_scope(world, parent);

// Note that we're not using the ecs_new_id function for the children. This 
// function only generates a new id, and does not add the scope to the entity. 
ecs_entity_t child_a = ecs_new(world, 0);
ecs_entity_t child_b = ecs_new(world, 0);

// Restore the previous scope
ecs_set_scope(world, prev);

ecs_has_pair(world, child_a, EcsChildOf, parent); // true
ecs_has_pair(world, child_b, EcsChildOf, parent); // true
```
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

Scopes in C++ can also be used with the `scope` function on an entity, which accepts a (typcially lambda) function:

```cpp
auto parent = world.entity().scope([&]{
  auto child_a = world.entity();
  auto child_b = world.entity();

  child_a.has(flecs::ChildOf, parent); // true
  child_b.has(flecs::ChildOf, parent); // true
});
```

Scopes are the mechanism that ensure contents of a module are created as children of the module, without having to explicitly add the module as a parent.

## Relation properties
Relations can be assigned properties that change the way they are treated by flecs. Properties are modelled as components, tags and relations that you can add to the relation entity.

### Relation cleanup properties
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

This behavior can be customized with cleanup properties as the above behavior is not always what you want. A typical example is the builtin `ChildOf` relation, where child entities should be deleted when the parent is deleted:

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

To customize this behavior, an application can add the `OnDeleteObject` policy to the relation. The following examples show how:

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

### Transitive relations
Relations can be marked as transitive. A formal-ish definition if transitivity in the context of relations is:

```
If Relation(EntityA, EntityB) And Relation(EntityB, EntityC) Then Relation(EntityA, EntityC)
```

What this means becomes more obvious when translated to a real-life example:

```
If Manhattan is located in New York, and New York is located in the USA, then Manhattan is located in the USA.
```

In this example, `LocatedIn` is the relation and `Manhattan`, `New York` and `USA` are entities `A`, `B` and `C`. Another common example of transitivity is found in OOP inheritance:

```
If a Square is a Rectangle and a Rectangle is a Shape, then a Square is a Shape.
```

In this example `IsA` is the relation and `Square`, `Rectangle` and `Shape` are the entities.

When relations in Flecs are marked as transitive, queries can follow the transitive relation to see if an entity matches. Consider this example dataset:

```c
ecs_entity_t LocatedIn = ecs_new_id(world);
ecs_entity_t Manhattan = ecs_new_id(world);
ecs_entity_t NewYork = ecs_new_id(world);
ecs_entity_t USA = ecs_new_id(world);

ecs_add_pair(world, Manhattan, LocatedIn, NewYork);
ecs_add_pair(world, NewYork, LocatedIn, USA);
```
```cpp
auto LocatedIn = world.entity();
auto Manhattan = world.entity();
auto NewYork = world.entity();
auto USA = world.entity();

ManHattan.add(LocatedIn, NewYork);
NewYork.add(LocatedIn, USA);
```

If we were now to query for `(LocatedIn, USA)` we would only match `NewYork`, because we never added `(LocatedIn, USA)` to `Manhattan`. To make sure queries `Manhattan` as well we have to make the `LocatedIn` relation transitive. We can simply do this by adding the transitive property to the relation entity:

```c
ecs_add_id(world, LocatedIn, Transitive);
```
```cpp
LocatedIn.add(flecs::Transitive);
```

When now querying for `(LocatedIn, USA)`, the query will follow the `LocatedIn` relation and return both `NewYork` and `Manhattan`. For more details on how queries use transitivity, see the section in the query manual on transitivity: [Query transitivity](Queries.md#Transitivity).

### Tag relations
A relation can be marked as a tag in which case it will never contain data. By default the data associated with a pair is determined by whether either the relation or object are components. For some relations however, even if the object is a component, no data should be added to the relation. Consider the following example:

```c
typedef struct {
  float x;
  float y;
} Position;

ECS_TAG(world, Serializable);
ECS_COMPONENT(world, Position);

ecs_entity_t e = ecs_new_id(world);
ecs_set(world, e, Position, {10, 20});
ecs_add_pair(world, e, Serializable, ecs_id(Position));

// Gets value from Position component
const Position *p = ecs_get(world, e, Position);

// Gets (unintended) value from (Serializable, Position) pair
const Position *p = ecs_get_pair_object(world, e, Serializable, Position);
```
```cpp
struct Serializable { }; // Tag, contains no data

struct Position {
  float x, y;
};

auto e = ecs.entity()
  .set<Position>({10, 20})
  .add<Serializable, Position>(); // Because Serializable is a tag, the pair 
                                  // has a value of type Position

// Gets value from Position component
const Position *p = e.get<Position>();

// Gets (unintended) value from (Serializable, Position) pair
const Position *p = e.get<Serializable, Position>();
```

To prevent data from being associated with pairs that can apply to components, the `Tag` property can be added to relations:

```c
// Ensure that Serializable never contains data
ecs_add_id(world, Serializable, EcsTag);

// Because Serializable is marked as a Tag, no data is added for the pair
// even though Position is a component
ecs_add_pair(world, e, Serializable, ecs_id(Position));

// This is still OK
const Position *p = ecs_get(world, e, Position);

// This no longer works, the pair has no data
const Position *p = ecs_get_pair_object(world, e, Serializable, Position);
```
```cpp
// Ensure that Serializable never contains data
ecs.component<Serializable>()
  .add<flecs::Tag>();

auto e = ecs.entity()
  .set<Position>({10, 20})
  .add<Serializable, Position>(); // Because Serializable marked as a Tag, no
                                  // data is added for the pair even though
                                  // Position is a component

// Gets value from Position component
const Position *p = e.get<Position>();

// This no longer works, the pair has no data
const Position *p = e.get<Serializable, Position>();
```

The `Tag` property is only interpreted when it is added to the relation part of a pair.

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
