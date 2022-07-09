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

This example just shows a simple relation query. Relation queries are much more powerful than this as they provide the ability to match against entity graphs of arbitrary size. For more information on relation queries see the [query manual](Queries.md).

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

An advantage of the `flecs::pair` template is that it can be used with function-style `get`/`set` operations:

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
  ecs_entity_t rel = ecs_pair_first(world, id);
  ecs_entity_t obj = ecs_pair_second(world, id);

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

q.iter([](flecs::iter& it) {
  auto id = it.pair(1);

  for (auto i : it) {
    cout << "entity " << it.entity(i) << " has relation "
      << id.first().name() << ", "
      << id.second().name() << endl;
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
ecs_entity_t Eats = ecs_new_entity(world, "Eats");
ecs_entity_t Apples = ecs_new_entity(world, "Apples");
ecs_entity_t Pears = ecs_new_entity(world, "Pears");

ecs_entity_t Bob = ecs_new_id(world);
ecs_add_pair(world, Bob, Eats, Apples);
ecs_add_pair(world, Bob, Eats, Pears);

// Find all (Eats, *) relations in Bob's type
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
  cout << "Bob eats " << id.second().name() << endl;
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

This specifies that `GrannySmith` is a subset of `Apple`. A key thing to note here is that because `Apple` is a subset of `Fruit`, `GrannySmith` is a subset of `Fruit` as well. This means that if an application were to query for `(IsA, Fruit)` it would both match `Apple` and `GrannySmith`. This property of the `IsA` relationship is called "transitivity" and it is a feature that can be applied to any relation. See the [section on Transitivity](#transitive-property) for more details.

#### Component sharing
An entity with an `IsA` relation to another entity is equivalent to the other entity. So far the examples showed how querying for an `IsA` relation will find the subsets of the thing that was queried for. In order for entities to be treated as true equivalents though, everything the superset contains (its components, tags, relations) must also be found on the subsets. Consider:

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

Scopes in C++ can also be used with the `scope` function on an entity, which accepts a (typically lambda) function:

```cpp
auto parent = world.entity().scope([&]{
  auto child_a = world.entity();
  auto child_b = world.entity();

  child_a.has(flecs::ChildOf, parent); // true
  child_b.has(flecs::ChildOf, parent); // true
});
```

Scopes are the mechanism that ensure contents of a module are created as children of the module, without having to explicitly add the module as a parent.

## Cleanup properties
When entities that are used as tags, components, relationships or relationship targets are deleted, cleanup policies ensure that the store does not contain any dangling references. Any cleanup policy provides this guarantee, so while they are configurable, applications cannot configure policies that allows for dangling references.

**Note**: this only applies to entities (like tags, components, relationships) that are added _to_ other entities. It does not apply to components that store an entity value, so:

```c
struct MyComponent {
  entity e; // not covered by cleanup policies
}
```
```c
e.add(ChildOf, parent); // covered by cleanup policies
```

The default policy is that any references to the entity will be **removed**. For example, when the tag `Archer` is deleted, it will be removed from all entities that  have it, which is similar to invoking the `remove_all` operation:

```c
ecs_remove_all(world, Archer);
```
```cpp
world.remove_all(Archer);
```

Since entities can be used in relationship pairs, just calling `remove_all` on just the entity itself does not guarantee that no dangling references are left. A more comprehensive description of what happens is:

```c
ecs_remove_all(world, Archer);
ecs_remove_all(world, ecs_pair(Archer, EcsWildcard));
ecs_remove_all(world, ecs_pair(EcsWildcard, Archer));
```
```cpp
world.remove_all(Archer);
world.remove_all(Archer, flecs::Wildcard);
world.remove_all(flecs::Wildcard, Archer);
```

This succeeds in removing all possible references to `Archer`. Sometimes this behavior is not what we want however. Consider a parent-child hierarchy, where we want to delete the child entities when the parent is deleted. Instead of removing `(ChildOf, parent)` from all children, we need to _delete_ the children.

We also want to specify this per relationship. If an entity has `(Likes, parent)` we may not want to delete that entity, meaning the cleanup we want to perform for `Likes` and `ChildOf` may not be the same.

This is what cleanup policies are for: to specify which action needs to be executed under which condition. They are applied _to_ entities that have a reference to the entity being deleted: if I delete the `Archer` tag I remove the tag _from_ all entities that have it.

To configure a cleanup policy for an entity, a `(Condition, Action)` pair can be added to it. If no policy is specified, the default cleanup action (`Remove`) is performed.

There are three cleanup actions:

- `Remove`: as if doing `remove_all(entity)` (default)
- `Delete`: as if doing `delete_with(entity)`
- `Panic`: throw a fatal error (default for components)

There are two cleanup conditions:

- `OnDelete`: the component, tag or relationship is deleted
- `OnDeleteObject`: a target used with the relationship is deleted

Policies apply to both regular and pair instances, so to all entities with `T` as well as `(T, *)`. 

### Examples
The following examples show how to use cleanup policies

**(OnDelete, Remove)**
```c
// Remove Archer from entities when Archer is deleted
ECS_TAG(world, Archer);
ecs_add_pair(world, EcsOnDelete, EcsRemove);

ecs_entity_t e = ecs_new_w_id(world, Archer);

// This will remove Archer from e
ecs_delete(world, Archer);
```
```cpp
// Delete entities with Archer when Archer is deleted
world.component<Archer>()
  .add(flecs::OnDelete, flecs::Remove);

auto e = world.entity().add<Archer>();

// This will remove Archer from e
world.component<Archer>().destruct();
```

**(OnDelete, Delete)**
```c
// Delete entities with Archer when Archer is deleted
ECS_TAG(world, Archer);
ecs_add_pair(world, EcsOnDelete, EcsDelete);

ecs_entity_t e = ecs_new_w_id(world, Archer);

// This will delete e
ecs_delete(world, Archer);
```
```cpp
// Delete entities with Archer when Archer is deleted
world.component<Archer>()
  .add(flecs::OnDelete, flecs::Delete);

auto e = world.entity().add<Archer>();

// This will delete e
world.component<Archer>().destruct();
```

**(OnDeleteObject, Delete)**
```c
// Delete children when deleting parent
ECS_TAG(world, ChildOf);
ecs_add_pair(world, EcsOnDeleteObject, EcsDelete);

ecs_entity_t p = ecs_new_id(world);
ecs_entity_t e = ecs_new_w_pair(world, ChildOf, p);

// This will delete both p and e
ecs_delete(world, p);
```
```cpp
// Delete children when deleting parent
world.component<ChildOf>()
  .add(flecs::OnDeleteObject, flecs::Delete);

auto p = world.entity();
auto e = world.entity().add<ChildOf>(p);

// This will delete both p and e
p.destruct();
```

### Cleanup order
While cleanup actions allow for specifying what needs to happen when a particular entity is deleted, or when an entity used with a particular relationship is deleted, they do not enforce a strict cleanup _order_. The reason for this is that there can be many orderings that satisfy the cleanup policies.

This is important to consider especially when writing `OnRemove` triggers or hooks, as the order in which they are invoked highly depends on the order in which entities are cleaned up.

Take an example with a parent and a child that both have the `Node` tag:

```cpp
world.observer<Node>()
  .event(flecs::OnRemove)
  .each([](flecs::entity e) { });

flecs::entity p = world.entity().add<Node>();
flecs::entity c = world.entity().add<Node>().child_of(p);
```

In this example, when calling `p.destruct()` the observer is first invoked for the child, and then for the parent, which is to be expected as the child is deleted before the parent. Cleanup policies do not however guarantee that this is always the case.

An application could also call `world.component<Node>().destruct()` which would delete the `Node` component and all of its instances. In this scenario the cleanup policies for the `ChildOf` relationship are not considered, and therefore the ordering is undefined. Another typical scenario in which ordering is undefined is when an application has cyclical relationships with a `Delete` cleanup action.

#### Cleanup order during world teardown
Cleanup issues often show up during world teardown as the ordering in which entities are deleted is controlled by the application. While world teardown respects cleanup policies, there can be many entity delete orderings that are valid according to the cleanup policies, but not all of them are equally useful. There are ways to organize entities that helps world cleanup to do the right thing. These are:

**Organize components, triggers, observers and systems in modules.**
Storing these entities in modules ensures that they stay alive for as long as possible. This leads to more predictable cleanup ordering as components will be deleted as their entities are, vs. when the component is deleted. It also ensures that triggers and observers are not deleted while matching events are still being generated.

**Avoid organizing components, triggers, observers and systems under entities that are not modules**. If a non-module entity with children is stored in the root, it will get cleaned up along with other regular entities. If you have entities such as these organized in a non-module scope, consider adding the `EcsModule`/`flecs::Module` tag to the root of that scope.

The next section goes into more detail on why this improves cleanup behavior and what happens during world teardown.

#### World teardown sequence
To understand why some ways to organize entities work better than others, having an overview of what happens during world teardown is useful. Here is a list of the steps that happen when a world is deleted:

1. **Find all root entities**
World teardown starts by finding all root entities, which are entities that do not have the builtin `ChildOf` relationship.

2. **Filter out modules, components, observers and systems**
This ensures that components are not cleaned up before the entities that use them, and triggers, observers and systems are not cleaned up while there are still conditions under which they could be invoked.

3. **Filter out entities that have no children**
If entities have no children they cannot cause complex cleanup logic. This also decreases the likelihood of initiating cleanup actions that could impact other entities.

4. **Delete root entities**
The root entities that were not filtered out will be deleted.

5. **Delete everything else**
The last step will delete all remaining entities. At this point cleanup policies are no longer considered and cleanup order is undefined.

## Relation properties
Relation properties are tags that can be added to relations to modify their behavior.

### Tag property
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

### Final property
Entities can be annotated with the `Final` property, which prevents using them with `IsA` relation. This is similar to the concept of a final class as something that cannot be extended. The following example shows how use `Final`:

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

Queries may use the final property to optimize, as they do not have to explore subsets of a final entity. For more information on how queries interpret final, see the [Query manual](Queries.md). By default, all components are created as final.

### DontInherit property
The `DontInherit` property prevents inheriting a component from a base entity (`IsA` object). Consider the following example:

```c
ecs_entity_t TagA = ecs_new_id(world);
ecs_entity_t TagB = ecs_new_id(world);
ecs_add_id(world, TagB, EcsDontInherit);

ecs_entity_t base = ecs_new_id(world);
ecs_add_id(world, base, TagA);
ecs_add_id(world, base, TagB);

ecs_entity_t inst = ecs_new_id(world);
ecs_has_id(world, inst, TagA); // true
ecs_has_id(world, inst, TagB); // false
```
```cpp
struct TagA = { };
struct TagB = { };

world.component<TagB>().add(flecs::DontInherit);

auto base = world.entity()
  .add<TagA>()
  .add<TagB>();

auto inst = world.entity().is_a(base);
inst.has<TagA>(); // true
inst.has<TagB>(); // false
```

The builtin `Prefab`, `Disabled`, `Identifier` and `ChildOf` tags/relations are marked as `DontInherit`.

### Transitive property
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

When now querying for `(LocatedIn, USA)`, the query will follow the `LocatedIn` relation and return both `NewYork` and `Manhattan`. For more details on how queries use transitivity, see the [Transitivity section in the query manual](Queries.md#Transitivity).

### Reflexive property
A relation can be marked reflexive which means that a query like `Relation(Entity, Entity)` should evaluate to true. The utility of `Reflexive` becomes more obvious with an example:

Given this dataset:
```
IsA(Oak, Tree)
```

we can ask whether an oak is a tree:
```
IsA(Oak, Tree)
- Yes, an Oak is a tree (Oak has (IsA, Tree))
```

We can also ask whether a tree is a tree, which it obviously is:
```
IsA(Tree, Tree)
- Yes, even though Tree does not have (IsA, Tree)
```

However, this does not apply to all relations. Consider a dataset with a `LocatedIn` relation:

```
LocatedIn(SanFrancisco, UnitedStates)
```

we can now ask whether SanFrancisco is located in SanFrancisco, which it is not:
```
LocatedIn(SanFrancisco, SanFrancisco)
- No
```

In these examples, `IsA` is a reflexive relation, whereas `LocatedIn` is not.

### Acyclic property
A relationship can be marked with the `Acyclic` property to indicate that it cannot contain cycles. Both the builtin `ChildOf` and `IsA` relationships are marked acyclic.

Knowing whether a relationship is acyclic allows the storage to detect and throw errors when a cyclic relationship is introduced by accident. A number of features are only available for acyclic relationships, such as event propagation and query substitution. For example, the following query is only valid if `LocatedIn` is acyclic:

```c
// Find Position by traversing LocatedIn relationship upwards
Position(superset(LocatedIn))
```

The same goes for observers/triggers that subscribe for events propagated through a relationship. A typical example of this is when a component value is changed on a prefab. The event of this change will be propagated by traversing the `IsA` relationship downwards, for all instances of the prefab. Event propagation does not happen for relationships that are not marked with `Acyclic`, as this could cause infinite loops.

Note that because cycle detection requires expensive algorithms, adding `Acyclic` to a relationship does not guarantee that an error will be thrown when a cycle is accidentally introduced. While detection may improve over time, an application that runs without errors is no guarantee that it does not contain acyclic relationships with cycles.

### Exclusive property
The `Exclusive` property enforces that an entity can only have a single instance of a relationship. When a second instance is added, it replaces the first instance. An example of a relation with the `Exclusive` property is the builtin `ChildOf` relation:

```c
ecs_add_pair(world, child, EcsChildOf, parent_a);
ecs_add_pair(world, child, EcsChildOf, parent_b); // replaces (ChildOf, parent_a)
```
```cpp
e.child_of(parent_a);
e.child_of(parent_b); // replaces (ChildOf, parent_a)
```

To create a custom exclusive relationship, add the `Exclusive` property:
```c
ecs_entity_t MarriedTo = ecs_new_id(world);
ecs_add_id(world, MarriedTo, EcsExclusive);
```
```cpp
flecs::entity MarriedTo = world.entity()
  .add(flecs::Exclusive);
```

### Union property
The `Union` is similar to `Exclusive` in that it enforces that an entity can only have a single instance of a relationship. The difference between `Exclusive` and `Union` is that `Union` combines different relationship targets in a single table. This reduces table fragmentation, and as a result speeds up add/remove operations. This increase in add/remove speed does come at a cost: iterating a query with union terms is more expensive than iterating a regular relationship.

The API for using the `Union` property is similar to regular relationships, as this example shows:

```c
ecs_entity_t Movement = ecs_new_id(world);
ecs_add_id(world, Movement, EcsUnion);

ecs_entity_t Walking = ecs_new_id(world);
ecs_entity_t Running = ecs_new_id(world);

ecs_entity_t e = ecs_new_id(world);
ecs_add_pair(world, e, Movement, Running);
ecs_add_pair(world, e, Movement, Walking); // replaces (Movement, Running)
```
```cpp
flecs::entity Movement = world.entity().add(flecs::Union);
flecs::entity Walking = world.entity();
flecs::entity Running = world.entity();

flecs::entity e = world.entity().add(Movement, Running);
e.add(Movement, Walking); // replaces (Movement, Running)
```

When compared to regular relationships, union relationships have some differences and limitations:
- Relationship cleanup does not work yet for union relations
- Removing a union relationship removes any target, even if the specified target is different
- Filters and rules do not support union relationships
- Union relationships cannot have data
- Union relationship query terms can only use the `And` operator
- Queries with a `(R, *)` term will return `(R, *)` as term id for each entity

### Symmetric property
The `Symmetric` property enforces that when a relation `(R, Y)` is added to entity `X`, the relation `(R, X)` will be added to entity `Y`. The reverse is also true, if relation `(R, Y)` is removed from `X`, relation `(R, X)` will be removed from `Y`.

The symmetric property is useful for relations that do not make sense unless they are bidirectional. Examples of such relations are `AlliesWith`, `MarriedTo`, `TradingWith` and so on. An example:

```c
ecs_entity_t MarriedTo = ecs_new_w_id(world, EcsSymmetric);
ecs_entity_t Bob = ecs_new_id(world);
ecs_entity_t Alice = ecs_new_id(world);
ecs_add_pair(world, Bob, MarriedTo, Alice); // Also adds (MarriedTo, Bob) to Alice
```
```cpp
auto MarriedTo = world.entity().add(flecs::Symmetric);
auto Bob = ecs.entity();
auto Alice = ecs.entity();
Bob.add(MarriedTo, Alice); // Also adds (MarriedTo, Bob) to Alice
```

### With property
The `With` relation can be added to components to indicate that it must always come together with another component. The following example shows how `With` can be used with regular components/tags:

```c
ecs_entity_t Responsibility = ecs_new_id(world);
ecs_entity_t Power = ecs_new_w_pair(world, EcsWith, Responsibility);

// Create new entity that has both Power and Responsibility
ecs_entity_t e = ecs_new_w_id(world, Power);
```
```cpp
auto Responsibility = world.entity();
auto Power = world.entity().add(flecs::With, Responsibility);

// Create new entity that has both Power and Responsibility
auto e = world.entity().add(Power);
```

When the `With` relation is added to a relation, the additional id added to the entity will be a relation pair as well, with the same object as the original relation:

```c
ecs_entity_t Likes = ecs_new_id(world);
ecs_entity_t Loves = ecs_new_w_pair(world, EcsWith, Likes);
ecs_entity_t Pears = ecs_new_id(world);

// Create new entity with both (Loves, Pears) and (Likes, Pears)
ecs_entity_t e = ecs_new_w_pair(world, Loves, Pears);
```
```cpp
auto Likes = world.entity();
auto Loves = world.entity().add(flecs::With, Likes);
auto Pears = world.entity();

// Create new entity with both (Loves, Pears) and (Likes, Pears)
auto e = world.entity().add(Loves, Pears);
```

### OneOf property
The `OneOf` property enforces that the target of the relationship is a child of a specified entity. `OneOf` can be used to either indicate that the target needs to be a child of the relation (common for enum relationships), or of another entity.

The following example shows how to constrain the relationship target to a child of the relation:

```c
ecs_entity_t Food = ecs_new_id(world);

// Enforce that target of relationship is child of Food
ecs_add_id(world, Food, EcsOneOf);

ecs_entity_t Apples = ecs_new_w_pair(world, EcsChildOf, Food);
ecs_entity_t Fork = ecs_new_id(world);

// This is ok, Apples is a child of Food
ecs_entity_t a = ecs_new_w_pair(world, Food, Apples);

// This is not ok, Fork is not a child of Food
ecs_entity_t b = ecs_new_w_pair(world, Food, Fork);
```
```cpp
// Enforce that target of relationship is child of Food
auto Food = world.entity().add(flecs::OneOf);
auto Apples = world.entity().child_of(Food);
auto Fork = world.entity();

// This is ok, Apples is a child of Food
auto a = world.entity().add(Food, Apples);

// This is not ok, Fork is not a child of Food
auto b = world.entity().add(Food, Fork);
```

The following example shows how `OneOf` can be used to enforce that the relationship target is the child of an entity other than the relation:

```c
ecs_entity_t Food = ecs_new_id(world);
ecs_entity_t Eats = ecs_new_id(world);

// Enforce that target of relationship is child of Food
ecs_add_pair(world, Eats, EcsOneOf, Food);

ecs_entity_t Apples = ecs_new_w_pair(world, EcsChildOf, Food);
ecs_entity_t Fork = ecs_new_id(world);

// This is ok, Apples is a child of Food
ecs_entity_t a = ecs_new_w_pair(world, Eats, Apples);

// This is not ok, Fork is not a child of Food
ecs_entity_t b = ecs_new_w_pair(world, Eats, Fork);
```
```cpp
// Enforce that target of relationship is child of Food
auto Food = world.entity();
auto Eats = world.entity().add(flecs::OneOf, Food);
auto Apples = world.entity().child_of(Food);
auto Fork = world.entity();

// This is ok, Apples is a child of Food
auto a = world.entity().add(Eats, Apples);

// This is not ok, Fork is not a child of Food
auto b = world.entity().add(Eats, Fork);
```

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
