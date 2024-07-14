# Component Traits
Component traits are tags and pairs that can be added to components to modify their behavior. This manual contains an overview of all component traits supported by Flecs.

## Cleanup traits
When entities that are used as tags, components, relationships or relationship targets are deleted, cleanup traits ensure that the store does not contain any dangling references. Any cleanup policy provides this guarantee, so while they are configurable, applications cannot configure traits that allows for dangling references.

**Note**: this only applies to entities (like tags, components, relationships) that are added _to_ other entities. It does not apply to components that store an entity value, so:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```cpp
struct MyComponent {
  ecs_entity_t e; // Not covered by cleanup traits
}

ecs_add_pair(world, e, EcsChildOf, parent); // covered by cleanup traits
```

</li>
<li><b class="tab-title">C++</b>

```cpp
struct MyComponent {
  flecs::entity e; // Not covered by cleanup traits
}

e.add(ChildOf, parent); // Covered by cleanup traits
```

</li>
<li><b class="tab-title">C#</b>

```cs
public struct MyComponent
{
    public Entity e; // Not covered by cleanup traits
}

e.Add(Ecs.ChildOf, parent); // Covered by cleanup traits
```

</li>
<li><b class="tab-title">Rust</b>

```rust
#[derive(Component)]
struct MyComponent {
    e: Entity, // Not covered by cleanup traits
}

e.child_of_id(parent); // Covered by cleanup traits
```

</li>
</ul>
</div>

The default policy is that any references to the entity will be **removed**. For example, when the tag `Archer` is deleted, it will be removed from all entities that  have it, which is similar to invoking the `remove_all` operation:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_remove_all(world, Archer);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.remove_all(Archer);
```

</li>
<li><b class="tab-title">C#</b>

```cs
world.RemoveAll(archer);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world.remove_all_id(archer);
```

</li>
</ul>
</div>

Since entities can be used in relationship pairs, just calling `remove_all` on just the entity itself does not guarantee that no dangling references are left. A more comprehensive description of what happens is:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_remove_all(world, Archer);
ecs_remove_all(world, ecs_pair(Archer, EcsWildcard));
ecs_remove_all(world, ecs_pair(EcsWildcard, Archer));
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.remove_all(Archer);
world.remove_all(Archer, flecs::Wildcard);
world.remove_all(flecs::Wildcard, Archer);
```

</li>
<li><b class="tab-title">C#</b>

```cs
world.RemoveAll(archer);
world.RemoveAll(archer, Ecs.Wildcard);
world.RemoveAll(Ecs.Wildcard, archer);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world.remove_all_id(archer);
world.remove_all_id((archer, flecs::Wildcard::ID));
world.remove_all_id((flecs::Wildcard::ID, archer));
```

</li>
</ul>
</div>

This succeeds in removing all possible references to `Archer`. Sometimes this behavior is not what we want however. Consider a parent-child hierarchy, where we want to delete the child entities when the parent is deleted. Instead of removing `(ChildOf, parent)` from all children, we need to _delete_ the children.

We also want to specify this per relationship. If an entity has `(Likes, parent)` we may not want to delete that entity, meaning the cleanup we want to perform for `Likes` and `ChildOf` may not be the same.

This is what cleanup traits are for: to specify which action needs to be executed under which condition. They are applied _to_ entities that have a reference to the entity being deleted: if I delete the `Archer` tag I remove the tag _from_ all entities that have it.

To configure a cleanup policy for an entity, a `(Condition, Action)` pair can be added to it. If no policy is specified, the default cleanup action (`Remove`) is performed.

There are three cleanup actions:

- `Remove`: as if doing `remove_all(entity)` (default)
- `Delete`: as if doing `delete_with(entity)`
- `Panic`: throw a fatal error (default for components)

There are two cleanup conditions:

- `OnDelete`: the component, tag or relationship is deleted
- `OnDeleteTarget`: a target used with the relationship is deleted

Policies apply to both regular and pair instances, so to all entities with `T` as well as `(T, *)`. 

### Examples
The following examples show how to use cleanup traits

#### (OnDelete, Remove)

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Remove Archer from entities when Archer is deleted
ECS_TAG(world, Archer);
ecs_add_pair(world, Archer, EcsOnDelete, EcsRemove);

ecs_entity_t e = ecs_new_w_id(world, Archer);

// This will remove Archer from e
ecs_delete(world, Archer);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Remove Archer from entities when Archer is deleted
world.component<Archer>()
  .add(flecs::OnDelete, flecs::Remove);

auto e = world.entity().add<Archer>();

// This will remove Archer from e
world.component<Archer>().destruct();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Remove Archer from entities when Archer is deleted
world.Component<Archer>().Entity
    .Add(Ecs.OnDelete, Ecs.Remove);

Entity e = world.Entity().Add<Archer>();

// This will remove Archer from e
world.Component<Archer>().Entity.Destruct();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Remove Archer from entities when Archer is deleted
world
    .component::<Archer>()
    .add_trait::<(flecs::OnDelete, flecs::Remove)>();

let e = world.entity().add::<Archer>();
```
</ul>
</div>

#### (OnDelete, Delete)

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Delete entities with Archer when Archer is deleted
ECS_TAG(world, Archer);
ecs_add_pair(world, Archer, EcsOnDelete, EcsDelete);

ecs_entity_t e = ecs_new_w_id(world, Archer);

// This will delete e
ecs_delete(world, Archer);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Delete entities with Archer when Archer is deleted
world.component<Archer>()
  .add(flecs::OnDelete, flecs::Delete);

auto e = world.entity().add<Archer>();

// This will delete e
world.component<Archer>().destruct();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Delete entities with Archer when Archer is deleted
world.Component<Archer>()
    .Add(Ecs.OnDelete, Ecs.Delete);

Entity e = world.Entity().Add<Archer>();

// This will delete e
world.Component<Archer>().Entity.Destruct();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Remove Archer from entities when Archer is deleted
world
    .component::<Archer>()
    .add_trait::<(flecs::OnDelete, flecs::Remove)>();

let e = world.entity().add::<Archer>();

// This will remove Archer from e
world.component::<Archer>().destruct();
```

</li>
</ul>
</div>

#### (OnDeleteTarget, Delete)

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Delete children when deleting parent
ECS_TAG(world, ChildOf);
ecs_add_pair(world, ChildOf, EcsOnDeleteTarget, EcsDelete);

ecs_entity_t p = ecs_new(world);
ecs_entity_t e = ecs_new_w_pair(world, ChildOf, p);

// This will delete both p and e
ecs_delete(world, p);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Delete children when deleting parent
world.component<ChildOf>()
  .add(flecs::OnDeleteTarget, flecs::Delete);

auto p = world.entity();
auto e = world.entity().add<ChildOf>(p);

// This will delete both p and e
p.destruct();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Delete children when deleting parent
world.Component<ChildOf>().Entity
    .Add(Ecs.OnDeleteTarget, Ecs.Delete);

Entity p = world.Entity();
Entity e = world.Entity().Add<ChildOf>(p);

// This will delete both p and e
p.Destruct();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Delete children when deleting parent
world
    .component::<flecs::ChildOf>()
    .add_trait::<(flecs::OnDeleteTarget, flecs::Delete)>();

let p = world.entity();
let e = world.entity().add_first::<flecs::ChildOf>(p);

// This will delete both p and e
p.destruct();
```

</li>
</ul>
</div>

### Cleanup order
While cleanup actions allow for specifying what needs to happen when a particular entity is deleted, or when an entity used with a particular relationship is deleted, they do not enforce a strict cleanup _order_. The reason for this is that there can be many orderings that satisfy the cleanup traits.

This is important to consider especially when writing `OnRemove` triggers or hooks, as the order in which they are invoked highly depends on the order in which entities are cleaned up.

Take an example with a parent and a child that both have the `Node` tag:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_observer(world, {
  .query.terms = {{ Node }},
  .events = { EcsOnRemove },
  .callback = ...
});

ecs_entity_t p = ecs_new_w(world, Node);
ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.observer<Node>()
  .event(flecs::OnRemove)
  .each([](flecs::entity e) { });

flecs::entity p = world.entity().add<Node>();
flecs::entity c = world.entity().add<Node>().child_of(p);
```

</li>
<li><b class="tab-title">C#</b>

```cs
world.Observer<Node>()
    .Event(Ecs.OnRemove)
    .Each((Entity e) => { });

Entity p = world.Entity().Add<Node>();
Entity c = world.Entity().Add<Node>().ChildOf(p);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .observer::<flecs::OnRemove, &Node>()
    .each_entity(|e, node| {
// This observer will be invoked when a Node is removed
});

let p = world.entity().add::<Node>();
let c = world.entity().add::<Node>().child_of_id(p);
```

</li>
</ul>
</div>

In this example, when calling `p.destruct()` the observer is first invoked for the child, and then for the parent, which is to be expected as the child is deleted before the parent. Cleanup traits do not however guarantee that this is always the case.

An application could also call `world.component<Node>().destruct()` which would delete the `Node` component and all of its instances. In this scenario the cleanup traits for the `ChildOf` relationship are not considered, and therefore the ordering is undefined. Another typical scenario in which ordering is undefined is when an application has cyclical relationships with a `Delete` cleanup action.

#### Cleanup order during world teardown
Cleanup issues often show up during world teardown as the ordering in which entities are deleted is controlled by the application. While world teardown respects cleanup traits, there can be many entity delete orderings that are valid according to the cleanup traits, but not all of them are equally useful. There are ways to organize entities that helps world cleanup to do the right thing. These are:

**Organize components, triggers, observers and systems in modules.**
Storing these entities in modules ensures that they stay alive for as long as possible. This leads to more predictable cleanup ordering as components will be deleted as their entities are, vs. when the component is deleted. It also ensures that triggers and observers are not deleted while matching events are still being generated.

**Avoid organizing components, triggers, observers and systems under entities that are not modules**. If a non-module entity with children is stored in the root, it will get cleaned up along with other regular entities. If you have entities such as these organized in a non-module scope, consider adding the `EcsModule`/`flecs::Module`/`Ecs.Module` tag to the root of that scope.

The next section goes into more detail on why this improves cleanup behavior and what happens during world teardown.

#### World teardown sequence
To understand why some ways to organize entities work better than others, having an overview of what happens during world teardown is useful. Here is a list of the steps that happen when a world is deleted:

1. **Find all root entities**
World teardown starts by finding all root entities, which are entities that do not have the builtin `ChildOf` relationship. Note that empty entities (entities without any components) are not found during this step.

2. **Query out modules, components, observers and systems**
This ensures that components are not cleaned up before the entities that use them, and triggers, observers and systems are not cleaned up while there are still conditions under which they could be invoked.

3. **Query out entities that have no children**
If entities have no children they cannot cause complex cleanup logic. This also decreases the likelihood of initiating cleanup actions that could impact other entities.

4. **Delete root entities**
The root entities that were not filtered out will be deleted.

5. **Delete everything else**
The last step will delete all remaining entities. At this point cleanup traits are no longer considered and cleanup order is undefined.

## Trait trait
The trait trait marks an entity as a trait, which is any tag that is added to another tag/component/relationship to modify its behavior. All traits in this manual are marked as trait. It is not required to mark a trait as a trait before adding it to another tag/component/relationship. The main reason for the trait trait is to ease some of the constraints on relationships (see the Relationship trait).

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_TAG(world, Serializable);

ecs_add_id(world, Serializable, EcsTrait);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
struct Serializable { };

world.component<Serializable>().add(flecs::Trait);
```

</li>
<li><b class="tab-title">C#</b>

```cs
public struct Serializable { }

world.Component<Serializable>().Entity.Add(Ecs.Trait);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
#[derive(Component)]
struct Serializable;

world
    .component::<Serializable>()
    .add_trait::<flecs::Trait>();
```
}
</li>
</ul>
</div>

## Relationship trait
The relationship trait enforces that an entity can only be used as relationship. Consider the following example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_TAG(world, Likes);
ECS_TAG(world, Apples);

ecs_add_id(world, Likes, EcsRelationship);

ecs_entity_t e = ecs_new_id(world);
ecs_add(world, Likes);              // Panic, 'Likes' is not used as relationship
ecs_add_pair(world, Apples, Likes); // Panic, 'Likes' is not used as relationship
ecs_add_pair(world, Likes, Apples); // OK
```

</li>
<li><b class="tab-title">C++</b>

```cpp
struct Likes { };
struct Apples { };

world.component<Likes>().add(flecs::Relationship);

flecs::entity e = world.entity()
  .add<Likes>()          // Panic, 'Likes' is not used as relationship
  .add<Apples, Likes>()  // Panic, 'Likes' is not used as relationship
  .add<Likes, Apples>(); // OK
```

</li>
<li><b class="tab-title">C#</b>

```cs
public struct Likes { }
public struct Apples { }

world.Component<Likes>().Entity.Add(Ecs.Relationship);

Entity e = ecs.Entity()
    .Add<Likes>()          // Panic, 'Likes' is not used as relationship
    .Add<Apples, Likes>()  // Panic, 'Likes' is not used as relationship
    .add<Likes, Apples>(); // OK
```

</li>
<li><b class="tab-title">Rust</b>

```rust
#[derive(Component)]
struct Likes;

#[derive(Component)]
struct Apples;

world
    .component::<Likes>()
    .add_trait::<flecs::Relationship>();

let e = world
    .entity()
    .add::<Likes>() // Panic, 'Likes' is not used as relationship
    .add::<(Apples, Likes)>() // Panic, 'Likes' is not used as relationship, but as target
    .add::<(Likes, Apples)>(); // OK
```

</li>
</ul>
</div>

Entities marked with `Relationship` may still be used as target if the relationship part of the pair has the `Trait` trait. This ensures the relationship can still be used to configure the behavior of other entities. Consider the following code example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_TAG(world, Likes);
ECS_TAG(world, Loves);

ecs_add_id(world, Likes, EcsRelationship);

// Even though Likes is marked as relationship and used as target here, this 
// won't panic as With is marked as trait.
ecs_add_pair(world, Loves, EcsWith, Likes);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
struct Likes { };
struct Loves { };

world.component<Likes>().add(flecs::Relationship);

// Even though Likes is marked as relationship and used as target here, this 
// won't panic as With is marked as trait.
world.component<Loves>().add(flecs::With, world.component<Likes>());
```

</li>
<li><b class="tab-title">C#</b>

```cs
public struct Likes { }
public struct Loves { }

world.Component<Likes>().Entity.Add(Ecs.Relationship);

world.Component<Loves>().Entity.Add(Ecs.With, world.Component<Likes>());
```

</li>
<li><b class="tab-title">Rust</b>

```rust
#[derive(Component)]
struct Likes;

#[derive(Component)]
struct Loves;

world
    .component::<Likes>()
    .add_trait::<flecs::Relationship>();

// Even though Likes is marked as relationship and used as target here, this
// won't panic as With is marked as trait.
world
    .component::<Loves>()
    .add_trait::<(flecs::With, Likes)>();
```

</li>
</ul>
</div>

## Target trait
The target trait enforces that an entity can only be used as relationship target. Consider the following example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_TAG(world, Likes);
ECS_TAG(world, Apples);

ecs_add_id(world, Apples, EcsTarget);

ecs_entity_t e = ecs_new_id(world);
ecs_add(world, Apples);             // Panic, 'Apples' is not used as target
ecs_add_pair(world, Apples, Likes); // Panic, 'Apples' is not used as target
ecs_add_pair(world, Likes, Apples); // OK
```

</li>
<li><b class="tab-title">C++</b>

```cpp
struct Likes { };
struct Apples { };

world.component<Apples>().add(flecs::Target);

flecs::entity e = world.entity()
  .add<Apples>()         // Panic, 'Apples' is not used as target
  .add<Apples, Likes>()  // Panic, 'Apples' is not used as target
  .add<Likes, Apples>(); // OK
```

</li>
<li><b class="tab-title">C#</b>

```cs
public struct Likes { }
public struct Apples { }

world.Component<Apples>().Entity.Add(Ecs.Target);

Entity e = ecs.Entity()
    .Add<Apples>()         // Panic, 'Apples' is not used as target
    .Add<Apples, Likes>()  // Panic, 'Apples' is not used as target
    .Add<Likes, Apples>(); // OK
```

</li>
<li><b class="tab-title">Rust</b>

```rust
#[derive(Component)]
struct Likes;

#[derive(Component)]
struct Apples;

world.component::<Apples>().add_trait::<flecs::Target>();

let e = world
    .entity()
    .add::<Apples>() // Panic, 'Apples' is not used as target
    .add::<(Apples, Likes)>() // Panic, 'Apples' is not used as target, but as relationship
    .add::<(Likes, Apples)>(); // OK
```

</li>
</ul>
</div>

## PairIsTag trait
A relationship can be marked with PairIsTag in which case a pair with the relationship will never contain data. By default the data associated with a pair is determined by whether either the relationship or target are components. For some relationships however, even if the target is a component, no data should be added to the relationship. Consider the following example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
typedef struct {
  float x;
  float y;
} Position;

ECS_TAG(world, Serializable);
ECS_COMPONENT(world, Position);

ecs_entity_t e = ecs_new(world);
ecs_set(world, e, Position, {10, 20});
ecs_add_pair(world, e, Serializable, ecs_id(Position));

// Gets value from Position component
const Position *p = ecs_get(world, e, Position);

// Gets (unintended) value from (Serializable, Position) pair
const Position *p = ecs_get_pair_second(world, e, Serializable, Position);
```

</li>
<li><b class="tab-title">C++</b>

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

</li>
<li><b class="tab-title">C#</b>

```cs
public struct Serializable { } // Tag, contains no data

public record struct Position(float X, float Y);

Entity e = ecs.Entity()
    .Set<Position>(new(10, 20))
    .Add<Serializable, Position>(); // Because Serializable is a tag, the pair
                                    // has a value of type Position

// Gets value from Position component
ref readonly Position p = ref e.Get<Position>();

// Gets (unintended) value from (Serializable, Position) pair
ref readonly Position p = ref e.GetSecond<Serializable, Position>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
#[derive(Component)]
struct Serializable; // Tag, contains no data

impl flecs::FlecsTrait for Serializable {}

#[derive(Component)]
struct Position {
    x: f32,
    y: f32,
}

let e = world
    .entity()
    .set(Position { x: 10.0, y: 20.9 })
    .add_trait::<(Serializable, Position)>(); // Because Serializable is a tag, the pair
// has a value of type Position

// Gets value from Position component
e.get::<&Position>(|pos| {
    println!("Position: ({}, {})", pos.x, pos.y);
});
// Gets (unintended) value from (Serializable, Position) pair
e.get::<&(Serializable, Position)>(|pos| {
    println!("Serializable Position: ({}, {})", pos.x, pos.y);
});
```

</li>
</ul>
</div>

To prevent data from being associated with pairs that can apply to components, the `Tag` trait can be added to relationships:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Ensure that Serializable never contains data
ecs_add_id(world, Serializable, EcsPairIsTag);

// Because Serializable is marked as a Tag, no data is added for the pair
// even though Position is a component
ecs_add_pair(world, e, Serializable, ecs_id(Position));

// This is still OK
const Position *p = ecs_get(world, e, Position);

// This no longer works, the pair has no data
const Position *p = ecs_get_pair_second(world, e, Serializable, Position);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Ensure that Serializable never contains data
ecs.component<Serializable>()
  .add<flecs::PairIsTag>();

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

</li>
<li><b class="tab-title">C#</b>

```cs
// Ensure that Serializable never contains data
ecs.Component<Serializable>().Entity
    .Add(Ecs.PairIsTag);

Entity e = ecs.Entity()
    .Set<Position>(new(10, 20))
    .Add<Serializable, Position>(); // Because Serializable marked as a Tag, no
                                    // data is added for the pair even though
                                    // Position is a component

// Gets value from Position component
ref readonly Position p = ref e.Get<Position>();

// This no longer works, the pair has no data
ref readonly Position p = ref e.GetSecond<Serializable, Position>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// This is currently not supported in Rust due to safety concerns.
```

</li>
</ul>
</div>

The `Tag` trait is only interpreted when it is added to the relationship part of a pair.

## Final trait
Entities can be annotated with the `Final` trait, which prevents using them with `IsA` relationship. This is similar to the concept of a final class as something that cannot be extended. The following example shows how use `Final`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_new(world);
ecs_add_id(world, e, EcsFinal);

ecs_entity_t i = ecs_new(world);
ecs_add_pair(world, e, i, EcsIsA, e); // not allowed
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto e = ecs.entity()
  .add(flecs::Final);

auto i = ecs.entity()
  .is_a(e); // not allowed
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity e = ecs.Entity()
    .Add(Ecs.Final);

Entity i = ecs.Entity()
    .IsA(e); // not allowed
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.entity().add_trait::<flecs::Final>();

let i = world.entity().is_a_id(e); // not allowed
```

</li>
</ul>
</div>

Queries may use the final trait to optimize, as they do not have to explore subsets of a final entity. For more information on how queries interpret final, see the [Query manual](Queries.md). By default, all components are created as final.

## OnInstantiate trait
The `OnInstantiate` trait configures the behavior of components when an entity is instantiated from another entity (usually a prefab). Instantiation happens when an `IsA` pair is added to an entity.

By default, when an entity is instantiated, the components from the base entity (the `IsA` target) are copied to the instance. This behavior can be modified with the `OnInstantiate` trait, which can be used as pair in combination with three targets:

| Target      | C                | C++                  | C#                | Description |
|-------------|------------------|----------------------|-------------------|-------------|
| Override    | `EcsOverride`    | `flecs::Override`    | `Ecs.Override`    | Copy component from base to instance (default) |
| Inherit     | `EcsInherit`     | `flecs::Inherit`     | `Ecs.Inherit`     | Inherit component from base |
| DontInherit | `EcsDontInherit` | `flecs::DontInherit` | `Ecs.DontInherit` | Don't inherit (and don't copy) component from base |

### Override
The default behavior for `OnInstantiate` is `Override`, which means that the component is copied to the instance. This means that after instantiation, the instance has an owned copy for the component that masks the base component (the "override").

Note that for an override to work correctly, a component has to be copyable.

The following example shows how to use the `Override` trait:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(ecs, Mass);

// Set trait on Mass. Optional, since this is the default behavior
ecs_add_pair(ecs, ecs_id(Mass), EcsOnInstantiate, EcsOverride);

ecs_entity_t base = ecs_insert(ecs, ecs_value(Mass, { 100 }));
ecs_entity_t inst = ecs_insert(ecs, { ecs_isa(base) }); // Mass is copied to inst

assert(ecs_owns(ecs, inst, Mass));
assert(ecs_get(ecs, base, Mass) != ecs_get(ecs, inst, Mass));
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Register component with trait. Optional, since this is the default behavior.
ecs.component<Mass>().add(flecs::OnInstantiate, flecs::Override);

ecs_entity_t base = ecs.entity().set(Mass, { 100 });
ecs_entity_t inst = ecs.entity().is_a(base); // Mass is copied to inst

assert(inst.owns<Mass>());
assert(base.get<Mass>() != inst.get<Mass>());
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Register component with trait. Optional, since this is the default behavior.
ecs.Component<Mass>().Entity
    .Add(Ecs.OnInstantiate, Ecs.Override);

Entity base = ecs.Entity()
    .Set<Mass>(new(100));

Entity inst = ecs.Entity()
    .IsA(base); // Mass is copied to inst

Debug.Assert(inst.Owns<Mass>());
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Register component with trait. Optional, since this is the default behavior.
world
.component::<Mass>()
.add_trait::<(flecs::OnInstantiate, flecs::Override)>();

let base = world.entity().set(Mass { value: 100.0 });
let inst = world.entity().is_a_id(base); // Mass is copied to inst

assert!(inst.owns::<Mass>());
assert!(base.cloned::<&Mass>() != inst.cloned::<&Mass>());
```

</li>
</ul>
</div>

### Inherit
Components with the `Inherit` trait are inherited from a base entity (the `IsA` target) on instantiation. Inherited components are not copied to the instance, and are only stored once in memory. Operations such as `get` and `has`, and queries will automatically lookup inheritable components by following the `IsA` relationship.

Inheritable components can be overridden manually by adding the component to the instance. This results in the same behavior as the `Override` trait, where the component is copied from the base entity.

The following example shows how to use the `Inherit` trait:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(ecs, Mass);

// Set trait on Mass
ecs_add_pair(ecs, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

ecs_entity_t base = ecs_insert(ecs, ecs_value(Mass, { 100 }));
ecs_entity_t inst = ecs_insert(ecs, { ecs_isa(base) }); // Mass is copied to inst

assert(ecs_has(ecs, inst, Mass));
assert(!ecs_owns(ecs, inst, Mass));
assert(ecs_get(ecs, base, Mass) != ecs_get(ecs, inst, Mass));
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Register component with trait
ecs.component<Mass>().add(flecs::OnInstantiate, flecs::Inherit);

ecs_entity_t base = ecs.entity().set(Mass, { 100 });
ecs_entity_t inst = ecs.entity().is_a(base);

assert(inst.has<Mass>());
assert(!inst.owns<Mass>());
assert(base.get<Mass>() != inst.get<Mass>());
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Register component with trait
ecs.Component<Mass>().Entity
    .Add(Ecs.OnInstantiate, Ecs.Inherit);

Entity base = ecs.Entity()
    .Set<Mass>(new(100));

Entity inst = ecs.Entity()
    .IsA(base);

Debug.Assert(inst.Has<Mass>());
Debug.Assert(!inst.Owns<Mass>());
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Register component with trait
world
.component::<Mass>()
.add_trait::<(flecs::OnInstantiate, flecs::Inherit)>();

let base = world.entity().set(Mass { value: 100.0 });
let inst = world.entity().is_a_id(base);

assert!(inst.has::<Mass>());
assert!(!inst.owns::<Mass>());
assert!(base.cloned::<&Mass>() != inst.cloned::<&Mass>());
```

</li>
</ul>
</div>

### DontInherit
Components with the `DontInherit` trait are not inherited from a base entity (the `IsA` target) on instantiation, and are not copied to the instance. Operations such as `has` and `get` will not find the component, and queries will not match it.

Components with the `DontInherit` cannot be overridden manually. When a component is added to an instance and the base also has the component, the base component is ignored and its value is not copied to the instance.

The following example shows how to use the `DontInherit` trait:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(ecs, Mass);

// Set trait on Mass
ecs_add_pair(ecs, ecs_id(Mass), EcsOnInstantiate, EcsDontInherit);

ecs_entity_t base = ecs_insert(ecs, ecs_value(Mass, { 100 }));
ecs_entity_t inst = ecs_insert(ecs, { ecs_isa(base) }); // Mass is copied to inst

assert(!ecs_has(ecs, inst, Mass));
assert(!ecs_owns(ecs, inst, Mass));
assert(ecs_get(ecs, inst, Mass) == NULL);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Register component with trait
ecs.component<Mass>().add(flecs::OnInstantiate, flecs::DontInherit);

ecs_entity_t base = ecs.entity().set(Mass, { 100 });
ecs_entity_t inst = ecs.entity().is_a(base);

assert(!inst.has<Mass>());
assert(!inst.owns<Mass>());
assert(inst.get<Mass>() == nullptr);
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Register component with trait
ecs.Component<Mass>().Entity
    .Add(Ecs.OnInstantiate, Ecs.DontInherit);

Entity base = ecs.Entity()
    .Set<Mass>(new(100));

Entity inst = ecs.Entity()
    .IsA(base);

Debug.Assert(!inst.Has<Mass>());
Debug.Assert(!inst.Owns<Mass>());
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Register component with trait
world
.component::<Mass>()
.add_trait::<(flecs::OnInstantiate, flecs::DontInherit)>();

let base = world.entity().set(Mass { value: 100.0 });
let inst = world.entity().is_a_id(base);

assert!(!inst.has::<Mass>());
assert!(!inst.owns::<Mass>());
assert!(!inst.try_get::<&Mass>(|mass| {}));
```

</li>
</ul>
</div>

## Transitive trait
Relationships can be marked as transitive. A formal-ish definition if transitivity in the context of relationships is:

```
If   Relationship(EntityA, EntityB) 
And  Relationship(EntityB, EntityC) 
Then Relationship(EntityA, EntityC)
```

What this means becomes more obvious when translated to a real-life example:

> If Manhattan is located in New York, and New York is located in the USA, then Manhattan is located in the USA.

In this example, `LocatedIn` is the relationship and `Manhattan`, `New York` and `USA` are entities `A`, `B` and `C`. Another common example of transitivity is found in OOP inheritance:

> If a Square is a Rectangle and a Rectangle is a Shape, then a Square is a Shape.

In this example `IsA` is the relationship and `Square`, `Rectangle` and `Shape` are the entities.

When relationships in Flecs are marked as transitive, queries can follow the transitive relationship to see if an entity matches. Consider this example dataset:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t LocatedIn = ecs_new(world);
ecs_entity_t Manhattan = ecs_new(world);
ecs_entity_t NewYork = ecs_new(world);
ecs_entity_t USA = ecs_new(world);

ecs_add_pair(world, Manhattan, LocatedIn, NewYork);
ecs_add_pair(world, NewYork, LocatedIn, USA);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto LocatedIn = world.entity();
auto Manhattan = world.entity();
auto NewYork = world.entity();
auto USA = world.entity();

Manhattan.add(LocatedIn, NewYork);
NewYork.add(LocatedIn, USA);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity locatedin = world.Entity();
Entity manhattan = world.Entity();
Entity newyork = world.Entity();
Entity usa = world.Entity();

Manhattan.Add(locatedin, newyork);
NewYork.Add(locatedin, usa);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let locatedin = world.entity();
let manhattan = world.entity();
let newyork = world.entity();
let usa = world.entity();

manhattan.add_id((locatedin, newyork));
newyork.add_id((locatedin, usa));
```

</li>
</ul>
</div>

If we were now to query for `(LocatedIn, USA)` we would only match `NewYork`, because we never added `(LocatedIn, USA)` to `Manhattan`. To make sure queries `Manhattan` as well we have to make the `LocatedIn` relationship transitive. We can simply do this by adding the transitive trait to the relationship entity:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_add_id(world, LocatedIn, Transitive);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
LocatedIn.add(flecs::Transitive);
```

</li>
<li><b class="tab-title">C#</b>

```cs
locatedIn.Add(Ecs.Transitive);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
locatedin.add_trait::<flecs::Transitive>();
```

</li>
</ul>
</div>

When now querying for `(LocatedIn, USA)`, the query will follow the `LocatedIn` relationship and return both `NewYork` and `Manhattan`. For more details on how queries use transitivity, see the [Transitive Relationships section in the query manual](Queries.md#transitive-relationships).

## Reflexive trait
A relationship can be marked reflexive which means that a query like `Relationship(Entity, Entity)` should evaluate to true. The utility of `Reflexive` becomes more obvious with an example:

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

However, this does not apply to all relationships. Consider a dataset with a `LocatedIn` relationship:

```
LocatedIn(SanFrancisco, UnitedStates)
```

we can now ask whether SanFrancisco is located in SanFrancisco, which it is not:
```
LocatedIn(SanFrancisco, SanFrancisco)
- No
```

In these examples, `IsA` is a reflexive relationship, whereas `LocatedIn` is not.

## Acyclic trait
A relationship can be marked with the `Acyclic` trait to indicate that it cannot contain cycles. Both the builtin `ChildOf` and `IsA` relationships are marked acyclic. Knowing whether a relationship is acyclic allows the storage to detect and throw errors when a cyclic relationship is introduced by accident.

Note that because cycle detection requires expensive algorithms, adding `Acyclic` to a relationship does not guarantee that an error will be thrown when a cycle is accidentally introduced. While detection may improve over time, an application that runs without errors is no guarantee that it does not contain acyclic relationships with cycles.

## Traversable trait
Traversable relationships are allowed to be traversed automatically by queries, for example using the `up` bit flag (upwards traversal, see [query traversal flags](Queries.md#traversal-flags)). Traversable relationships are also marked as `Acyclic`, which ensures a query won't accidentally attempt to traverse a relationship that contains cycles.

Events are propagated along the edges of traversable relationships. A typical example of this is when a component value is changed on a prefab. The event of this change will be propagated by traversing the `IsA` relationship downwards, for all instances of the prefab. Event propagation does not happen for relationships that are not marked with `Traversable`.

## Exclusive trait
The `Exclusive` trait enforces that an entity can have only a single instance of a relationship. When a second instance is added, it replaces the first instance. An example of a relationship with the `Exclusive` trait is the builtin `ChildOf` relationship:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_add_pair(world, child, EcsChildOf, parent_a);
ecs_add_pair(world, child, EcsChildOf, parent_b); // replaces (ChildOf, parent_a)
```

</li>
<li><b class="tab-title">C++</b>

```cpp
e.child_of(parent_a);
e.child_of(parent_b); // replaces (ChildOf, parent_a)
```

</li>
<li><b class="tab-title">C#</b>

```cs
e.ChildOf(parentA);
e.ChildOf(parentB); // replaces (ChildOf, parentA)
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let parent_a = world.entity();
let parent_b = world.entity();
e.child_of_id(parent_a);
e.child_of_id(parent_b); // replaces (ChildOf, parent_a)
```

</li>
</ul>
</div>

To create a custom exclusive relationship, add the `Exclusive` trait:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t MarriedTo = ecs_new(world);
ecs_add_id(world, MarriedTo, EcsExclusive);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity MarriedTo = world.entity()
  .add(flecs::Exclusive);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity marriedTo = world.Entity()
    .Add(Ecs.Exclusive);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let married_to = world.entity().add_trait::<flecs::Exclusive>();
```

</li>
</ul>
</div>

## CanToggle trait
The `CanToggle` trait allows a component to be toggled. Component toggling can (temporarily) disable a component, which excludes it from queries. Component toggling can be used as a cheaper alternative to adding/removing as toggling relies on setting a bitset, and doesn't require the entity to be moved between tables. Component toggling can also be used to restore a component with its old value.

Queries treat a disabled component as if the entity doesn't have it. `CanToggle` components add a small amount of overhead to query evaluation, even for entities that did not toggle their component.

The following example shows how to use the `CanToggle` trait:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(world, Position);
ecs_add_id(world, ecs_id(Position), EcsCanToggle);

ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

ecs_enable_component(world, e, Position, false); // Disable component
assert(!ecs_is_enabled(world, e, Position));

ecs_enable_component(world, e, Position, true); // Enable component
assert(ecs_is_enabled(world, e, Position));
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.component<Position>().add(flecs::CanToggle);

flecs::entity e = world.entity().set(Position{10, 20});

e.disable<Position>(); // Disable component
assert(!e.is_enabled<Position>());

e.enable<Position>(); // Enable component
assert(e.is_enabled<Position>());
```

</li>
<li><b class="tab-title">C#</b>

```cs
ecs.Component<Position>().Entity
    .Add(Ecs.CanToggle);

Entity e = world.Entity()
    .Set<Position>(new(10, 20));

e.Disable<Position>(); // Disable component
Debug.Assert(!e.IsEnabled<Position>());

e.Enable<Position>(); // Enable component
Debug.Assert(e.IsEnabled<Position>());
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world
.component::<Position>()
.add_trait::<flecs::CanToggle>();

let e = world.entity().set(Position { x: 10.0, y: 20.0 });

e.disable::<Position>(); // Disable component
assert!(!e.is_enabled::<Position>());

e.enable::<Position>(); // Enable component
assert!(e.is_enabled::<Position>());
```

</li>
</ul>
</div>

## Union trait
The `Union` is similar to `Exclusive` in that it enforces that an entity can have only a single instance of a relationship. The difference between `Exclusive` and `Union` is that `Union` combines different relationship targets in a single table. This reduces table fragmentation, and as a result speeds up add/remove operations. This increase in add/remove speed does come at a cost: iterating a query with union terms is more expensive than iterating a regular relationship.

The API for using the `Union` trait is similar to regular relationships, as this example shows:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t Movement = ecs_new(world);
ecs_add_id(world, Movement, EcsUnion);

ecs_entity_t Walking = ecs_new(world);
ecs_entity_t Running = ecs_new(world);

ecs_entity_t e = ecs_new(world);
ecs_add_pair(world, e, Movement, Running);
ecs_add_pair(world, e, Movement, Walking); // replaces (Movement, Running)
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity Movement = world.entity().add(flecs::Union);
flecs::entity Walking = world.entity();
flecs::entity Running = world.entity();

flecs::entity e = world.entity().add(Movement, Running);
e.add(Movement, Walking); // replaces (Movement, Running)
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity movement = world.Entity().Add(Ecs.Union);
Entity walking = world.Entity();
Entity running = world.Entity();

Entity e = world.Entity().Add(movement, running);
e.Add(movement, walking); // replaces (Movement, Running)
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let movement = world.entity().add_trait::<flecs::Union>();
let walking = world.entity();
let running = world.entity();

let e = world.entity().add_id((movement, running));
e.add_id((movement, walking)); // replaces (Movement, Running)
```

</li>
</ul>
</div>

When compared to regular relationships, union relationships have some differences and limitations:
- Relationship cleanup does not work yet for union relationships
- Removing a union relationship removes any target, even if the specified target is different
- Union relationships cannot have data

## Sparse trait
The `Sparse` trait configures a component to use sparse storage. Sparse components are stored outside of tables, which means they do not have to be moved. Sparse components are also guaranteed to have stable pointers, which means that a component pointer is not invalidated when an entity moves to a new table. ECS operations and queries work as expected with sparse components.

Sparse components trade in query speed for component add/remove speed. Adding and removing sparse components still requires an archetype change.

They also enable storage of non-movable components. Non-movable components in the C++ API are automatically marked as sparse.

The following code example shows how to mark a component as sparse:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(world, Position);
ecs_add_id(world, ecs_id(Position), EcsSparse);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.component<Position>().add(flecs::Sparse);
```

</li>
<li><b class="tab-title">C#</b>

```cs
ecs.Component<Position>().Entity
    .Add(Ecs.Sparse);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world.component::<Position>().add_trait::<flecs::Sparse>();
```

</li>
</ul>
</div>

## Symmetric trait
The `Symmetric` trait enforces that when a relationship `(R, Y)` is added to entity `X`, the relationship `(R, X)` will be added to entity `Y`. The reverse is also true, if relationship `(R, Y)` is removed from `X`, relationship `(R, X)` will be removed from `Y`.

The symmetric trait is useful for relationships that do not make sense unless they are bidirectional. Examples of such relationships are `AlliesWith`, `MarriedTo`, `TradingWith` and so on. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t MarriedTo = ecs_new_w_id(world, EcsSymmetric);
ecs_entity_t Bob = ecs_new(world);
ecs_entity_t Alice = ecs_new(world);
ecs_add_pair(world, Bob, MarriedTo, Alice); // Also adds (MarriedTo, Bob) to Alice
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto MarriedTo = world.entity().add(flecs::Symmetric);
auto Bob = ecs.entity();
auto Alice = ecs.entity();
Bob.add(MarriedTo, Alice); // Also adds (MarriedTo, Bob) to Alice
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity marriedTo = world.Entity().Add(Ecs.Symmetric);
Entity bob = ecs.Entity();
Entity alice = ecs.Entity();
Bob.Add(marriedTo, alice); // Also adds (MarriedTo, Bob) to Alice
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let married_to = world.entity().add_trait::<flecs::Symmetric>();
let bob = world.entity();
let alice = world.entity();
bob.add_id((married_to, alice)); // Also adds (MarriedTo, Bob) to Alice
```

</li>
</ul>
</div>

## With trait
The `With` relationship can be added to components to indicate that it must always come together with another component. The following example shows how `With` can be used with regular components/tags:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t Responsibility = ecs_new(world);
ecs_entity_t Power = ecs_new_w_pair(world, EcsWith, Responsibility);

// Create new entity that has both Power and Responsibility
ecs_entity_t e = ecs_new_w_id(world, Power);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto Responsibility = world.entity();
auto Power = world.entity().add(flecs::With, Responsibility);

// Create new entity that has both Power and Responsibility
auto e = world.entity().add(Power);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity responsibility = world.Entity();
Entity power = world.Entity().Add(Ecs.With, responsibility);

// Create new entity that has both Power and Responsibility
Entity e = world.Entity().Add(power);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let responsibility = world.entity();
let power = world.entity().add_first::<flecs::With>(responsibility);

// Create new entity that has both Power and Responsibility
let e = world.entity().add_id(power);
```

</li>
</ul>
</div>

When the `With` relationship is added to a relationship, the additional id added to the entity will be a relationship pair as well, with the same target as the original relationship:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t Likes = ecs_new(world);
ecs_entity_t Loves = ecs_new_w_pair(world, EcsWith, Likes);
ecs_entity_t Pears = ecs_new(world);

// Create new entity with both (Loves, Pears) and (Likes, Pears)
ecs_entity_t e = ecs_new_w_pair(world, Loves, Pears);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto Likes = world.entity();
auto Loves = world.entity().add(flecs::With, Likes);
auto Pears = world.entity();

// Create new entity with both (Loves, Pears) and (Likes, Pears)
auto e = world.entity().add(Loves, Pears);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity likes = world.Entity();
Entity loves = world.Entity().Add(Ecs.With, likes);
Entity pears = world.Entity();

// Create new entity with both (Loves, Pears) and (Likes, Pears)
Entity e = world.Entity().Add(loves, pears);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let likes = world.entity();
let loves = world.entity().add_trait::<(flecs::With, Likes)>();
let pears = world.entity();

// Create new entity with both (Loves, Pears) and (Likes, Pears)
let e = world.entity().add_id((loves, pears));
```

</li>
</ul>
</div>

## OneOf trait
The `OneOf` trait enforces that the target of the relationship is a child of a specified entity. `OneOf` can be used to indicate that the target needs to be either a child of the relationship (common for enum relationships), or of another entity.

The following example shows how to constrain the relationship target to a child of the relationship:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t Food = ecs_new(world);

// Enforce that target of relationship is child of Food
ecs_add_id(world, Food, EcsOneOf);

ecs_entity_t Apples = ecs_new_w_pair(world, EcsChildOf, Food);
ecs_entity_t Fork = ecs_new(world);

// This is ok, Apples is a child of Food
ecs_entity_t a = ecs_new_w_pair(world, Food, Apples);

// This is not ok, Fork is not a child of Food
ecs_entity_t b = ecs_new_w_pair(world, Food, Fork);
```

</li>
<li><b class="tab-title">C++</b>

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

</li>
<li><b class="tab-title">C#</b>

```cs
// Enforce that target of relationship is child of Food
Entity food = world.Entity().Add(Ecs.OneOf);
Entity apples = world.Entity().ChildOf(food);
Entity fork = world.Entity();

// This is ok, Apples is a child of Food
Entity a = world.Entity().Add(food, apples);

// This is not ok, Fork is not a child of Food
Entity b = world.Entity().Add(food, fork);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Enforce that target of relationship is child of Food
let food = world.entity().add_trait::<flecs::OneOf>();
let apples = world.entity().child_of_id(food);
let fork = world.entity();

// This is ok, Apples is a child of Food
let a = world.entity().add_id((food, apples));

// This is not ok, Fork is not a child of Food
let b = world.entity().add_id((food, fork));
```

</li>
</ul>
</div>

The following example shows how `OneOf` can be used to enforce that the relationship target is the child of an entity other than the relationship:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t Food = ecs_new(world);
ecs_entity_t Eats = ecs_new(world);

// Enforce that target of relationship is child of Food
ecs_add_pair(world, Eats, EcsOneOf, Food);

ecs_entity_t Apples = ecs_new_w_pair(world, EcsChildOf, Food);
ecs_entity_t Fork = ecs_new(world);

// This is ok, Apples is a child of Food
ecs_entity_t a = ecs_new_w_pair(world, Eats, Apples);

// This is not ok, Fork is not a child of Food
ecs_entity_t b = ecs_new_w_pair(world, Eats, Fork);
```

</li>
<li><b class="tab-title">C++</b>

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

</li>
<li><b class="tab-title">C#</b>

```cs
// Enforce that target of relationship is child of Food
Entity food = world.Entity();
Entity eats = world.Entity().Add(Ecs.OneOf, food);
Entity apples = world.Entity().ChildOf(food);
Entity fork = world.Entity();

// This is ok, Apples is a child of Food
Entity a = world.Entity().Add(eats, apples);

// This is not ok, Fork is not a child of Food
Entity b = world.Entity().Add(eats, fork);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Enforce that target of relationship is child of Food
let food = world.entity();
let eats = world.entity().add_first::<flecs::OneOf>(food);
let apples = world.entity().child_of_id(food);
let fork = world.entity();

// This is ok, Apples is a child of Food
let a = world.entity().add_id((eats, apples));

// This is not ok, Fork is not a child of Food
let b = world.entity().add_id((eats, fork));
```

</li>
</ul>
</div>
