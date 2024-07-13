# Prefabs

## Introduction
Prefabs are entities that can be used as templates for other entities. They can provide a convenient API for creating assets natively in the ECS. Prefabs have the following features:

- Prefab components can be shared across instances
- Inherited components can be overridden on a per-instance basis
- Inherited components can be auto-overridden on a per-prefab basis
- Prefab inheritance makes creating variations easy
- Prefabs can have children that are instantiated for instances
- Prefab children can be easily identified with prefab slots
- Prefabs are runtime accessible and modifiable
- Prefabs can be mapped to types for easy access in the C++ API
- Natively supported by Flecs Script & the JSON serializer

The following example shows how to instantiate a simple prefab:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(ecs, Defense);

// Create a SpaceShip prefab with a Defense component.
ecs_entity_t SpaceShip = ecs_entity(ecs, { 
    .name = "SpaceShip", 
    .add = ecs_ids( EcsPrefab ) 
});

ecs_set(ecs, SpaceShip, Defense, {50});

// Create two prefab instances
ecs_entity_t inst_1 = ecs_new_w_pair(world, EcsIsA, SpaceShip);
ecs_entity_t inst_2 = ecs_new_w_pair(world, EcsIsA, SpaceShip);

// Get instantiated component
const Defense *d = ecs_get(world, inst_1, Defense);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
struct Defense {
    float value;
};

// Create a SpaceShip prefab with a Defense component.
flecs::entity SpaceShip = world.prefab("SpaceShip")
    .set(Defense{50});

// Create two prefab instances
flecs::entity inst_1 = world.entity().is_a(SpaceShip);
flecs::entity inst_2 = world.entity().is_a(SpaceShip);

// Get instantiated component
const Defense *d = inst_1.get<Defense>();
```

</li>
<li><b class="tab-title">C#</b>

```cs
public record struct Defense(double Value);

// Create a SpaceShip prefab with a Defense component.
Entity spaceship = world.Entity("Spaceship")
    .Set(new Defense(50));

// Create two prefab instances
Entity inst1 = world.Entity().IsA(spaceship);
Entity inst2 = world.Entity().IsA(spaceship);

// Get instantiated component
ref readonly Defense attack = ref inst1.Get<Defense>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
#[derive(Component)]
struct Defense {
value: u32,
}

// Create a spaceship prefab with a Defense component.
let spaceship = world.prefab_named("spaceship").set(Defense { value: 50 });

// Create two prefab instances
let inst_1 = world.entity().is_a_id(spaceship);
let inst_2 = world.entity().is_a_id(spaceship);

// Get instantiated component
inst_1.get::<&Defense>(|defense| {
println!("Defense value: {}", defense.value);
});
```

</li>
</ul>
</div>

The following sections go over the different aspects of the prefab feature.

## The Prefab tag
Prefabs are regular entities with as only difference that prefabs by default are not matched by queries. This allows prefab entities to coexist with regular entities in the same world without impacting game logic. The mechanism used to exclude prefabs from queries is a builtin `Prefab` tag. The following example shows how to create a prefab:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t myPrefab = ecs_new(world);
ecs_add_id(world, myPrefab, EcsPrefab);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity myPrefab = world.entity()
    .add(flecs::Prefab);

// or the shortcut

flecs::entity myPrefab = world.prefab();
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity myPrefab = world.Prefab();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let myprefab = world.entity().add::<flecs::Prefab>();

// or the shortcut

let myprefab = world.prefab();
```

</li>
</ul>
</div>

Queries don't match prefab entities unless the `Prefab` tag is explicitly queried for. The following example shows how to match only prefabs by adding the `Prefab` tag to a query:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Only match prefab entities
ecs_query_t *q = ecs_query(world, {
    .terms = {
        { ecs_id(Position) },
        { EcsPrefab }
    }
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Only match prefab entities
world.query_builder<Position>()
    .with(flecs::Prefab)
    .build();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Only match prefab entities
world.QueryBuilder<Position>()
    .With(Ecs.Prefab)
    .Build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Only match prefab entities
world.query::<&Position>().with::<flecs::Prefab>().build();
```

</li>
</ul>
</div>

To match both regular and prefab entities, make the prefab term optional:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Only match prefab entities
ecs_query_t *q = ecs_query(world, {
    .terms = {
        { ecs_id(Position) },
        { EcsPrefab, .oper = EcsOptional }
    }
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Only match prefab entities
world.query_builder<Position>()
    .with(flecs::Prefab).optional()
    .build();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Only match prefab entities
world.QueryBuilder<Position>()
    .With(Ecs.Prefab).Optional()
    .Build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Only match prefab entities
world
.query::<&Position>()
.with::<flecs::Prefab>()
.optional()
.build();
```

</li>
</ul>
</div>

Instead of an optional term, queries can also specify a `MatchPrefabs` flag. This matches the same entities as an optional term, but since the term doesn't need to populate a field slightly improves performance:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Only match prefab entities
ecs_query_t *q = ecs_query(world, {
    .terms = {
        { ecs_id(Position) }
    },
    .flags = EcsQueryMatchPrefab
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Only match prefab entities
world.query_builder<Position>()
    .query_flags(EcsQueryMatchPrefab)
    .build();
```

</li>
<li><b class="tab-title">C#</b>

```cs
world.QueryBuilder()
    .QueryFlags(Ecs.QueryMatchPrefab)
    .Build();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Only match prefab entities
world
.query::<&Position>()
.query_flags(QueryFlags::MatchPrefab)
.build();
```

</li>
</ul>
</div>

## Component Inheritance
Entities can inherit components from prefabs. Inherited components are only stored once in memory, and shared across instances. This can be useful for static data that's shared across instances, such as material data, textures or meshes.

For a component to be inheritable, it needs to have the `(OnInstantiate, Inherit)` trait (for more details see the ComponentTraits manual). The following example shows what happens when a prefab with one inheritable and one non-inheritable component is instantiated:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(ecs, Health);
ECS_COMPONENT(ecs, Defense);

// Make Defense component inheritable
ecs_add_pair(world, ecs_id(Defense), 
    EcsOnInstantiate, EcsInherit);

// Create prefab
ecs_entity_t SpaceShip = ecs_entity(ecs, { 
    .name = "SpaceShip", 
    .add = ecs_ids( EcsPrefab ) 
});

ecs_set(ecs, SpaceShip, Defense, {50});
ecs_set(ecs, SpaceShip, Health, {100});

// Create prefab instance
ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, SpaceShip);

// Component is retrieved from instance
const Health *health = ecs_get(world, inst, Health);

// Component is retrieved from prefab
const Defense *defense = ecs_get(world, inst, Defense);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Make Defense component inheritable
world.component<Defense>()
    .add(flecs::OnInstantiate, flecs::Inherit);

// Create prefab
flecs::entity SpaceShip = world.prefab()
    .set(Health{100})
    .set(Defense{50});

// Create prefab instance
flecs::entity inst = world.entity().is_a(SpaceShip);

// Component is retrieved from instance
const Health *health = inst.get<Health>();

// Component is retrieved from prefab
const Defense *defense = inst.get<Defense>();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Make Defense component inheritable
world.Component<Defense>()
    .Add(Ecs.OnInstantiate, Ecs.Inherit);

// Create prefab
Entity spaceship = world.Prefab("Spaceship")
    .Set<Health>(new(100))
    .Set<Defense>(new(50));

// Create a prefab instance
Entity inst = world.Entity().IsA(spaceship);

// Component is retrieved from instance
ref readonly Attack attack = ref inst.Get<Attack>();

// Component is retrieved from prefab
ref readonly Defense defense = ref inst.Get<Defense>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Make Defense component inheritable
world
.component::<Defense>()
.add_trait::<(flecs::OnInstantiate, flecs::Inherit)>();

// Create prefab
let spaceship = world
.prefab()
.set(Health { value: 100 })
.set(Defense { value: 50 });

// Create prefab instance
let inst = world.entity().is_a_id(spaceship);

// Component is retrieved from instance
inst.get::<&Health>(|health| {
println!("Health value: {}", health.value);
});

// Component is retrieved from prefab
inst.get::<&Defense>(|defense| {
println!("Defense value: {}", defense.value);
});
```

</li>
</ul>
</div>

The `owns` operation can be used to tell whether a component is owned by the instance or inherited from a prefab:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
if (ecs_owns(world, inst, Defense)) {
    // not inherited
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
if (inst.owns<Defense>()) {
    // not inherited
}
```

</li>
<li><b class="tab-title">C#</b>

```cs
if (inst.Owns<Defense>()) {
    // not inherited
}
```
</li>
<li><b class="tab-title">Rust</b>

```rust
if inst.owns::<Defense>() {
// not inherited
}
```

</li>
</ul>
</div>

The `target_for` operation can be used to determine from which prefab (or regular entity) a component is inherited:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>
```c
ecs_entity_t inherited_from = 
    ecs_target_for(world, inst, Defense);
if (!inherited_from) {
    // not inherited
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity inherited_from = 
    inst.target_for<Defense>();
if (!inherited_from) {
    // not inherited
}
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity inheritedFrom = inst.TargetFor<Defense>();
if (inheritedFrom == 0) {
    // not inherited
}
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let inherited_from = inst.target::<Defense>(0);
if inherited_from.is_none() {
// not inherited
}
```

</li>
</ul>
</div>

## Component Overriding
When an instance inherits a component from a prefab, it can be overridden with a value that's specific to the instance. To override a component, simply add or set it on the instance. The following example shows how:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(ecs, Defense);

// Make Defense component inheritable
ecs_add_pair(world, ecs_id(Defense), 
    EcsOnInstantiate, EcsInherit);

// Create prefab
ecs_entity_t SpaceShip = ecs_entity(ecs, { 
    .name = "SpaceShip", 
    .add = ecs_ids( EcsPrefab ) 
});

ecs_set(ecs, SpaceShip, Defense, {50});

// Create prefab instances
ecs_entity_t inst_a = ecs_new_w_pair(world, EcsIsA, SpaceShip);
ecs_entity_t inst_b = ecs_new_w_pair(world, EcsIsA, SpaceShip);

// Override Defense only for inst_a
ecs_set(world, inst_a, Defense, {75});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Make Defense component inheritable
world.component<Defense>()
    .add(flecs::OnInstantiate, flecs::Inherit);

// Create prefab
flecs::entity SpaceShip = world.prefab()
    .set(Defense{50});

// Create prefab instance
flecs::entity inst_a = world.entity().is_a(SpaceShip);
flecs::entity inst_b = world.entity().is_a(SpaceShip);

// Override Defense only for inst_a
inst_a.set(Defense{75});
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Make Defense component inheritable
world.Component<Defense>()
    .Add(Ecs.OnInstantiate, Ecs.Inherit);

// Create prefab
Entity spaceship = world.Prefab("Spaceship")
    .Set<Defense>(new(50));

// Create a prefab instance
Entity instA = world.Entity().IsA(spaceship);
Entity instB = world.Entity().IsA(spaceship);

// Override Defense only for instA
instA.Set(new Defense(75));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Make Defense component inheritable
world
.component::<Defense>()
.add_trait::<(flecs::OnInstantiate, flecs::Inherit)>();

// Create prefab
let spaceship = world.prefab().set(Defense { value: 50 });

// Create prefab instance
let inst_a = world.entity().is_a_id(spaceship);
let inst_b = world.entity().is_a_id(spaceship);

// Override Defense only for inst_a
inst_a.set(Defense { value: 75 });
```

</li>
</ul>
</div>

When a component is overridden by adding it to an instance, the component on the instance is initialized with the value from the prefab component. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(ecs, Defense);

// Make Defense component inheritable
ecs_add_pair(world, ecs_id(Defense), 
    EcsOnInstantiate, EcsInherit);

// Create prefab
ecs_entity_t SpaceShip = ecs_entity(ecs, { 
    .name = "SpaceShip", 
    .add = ecs_ids( EcsPrefab ) 
});

ecs_set(ecs, SpaceShip, Defense, {50});

// Create prefab instances
ecs_entity_t inst_a = ecs_new_w_pair(world, EcsIsA, SpaceShip);
ecs_entity_t inst_b = ecs_new_w_pair(world, EcsIsA, SpaceShip);

// Override Defense only for inst_a
ecs_add(world, inst_a, Defense); // Initialized with value 50
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Make Defense component inheritable
world.component<Defense>()
    .add(flecs::OnInstantiate, flecs::Inherit);

// Create prefab
flecs::entity SpaceShip = world.prefab()
    .set(Defense{50});

// Create prefab instance
flecs::entity inst_a = world.entity().is_a(SpaceShip);
flecs::entity inst_b = world.entity().is_a(SpaceShip);

// Override Defense only for inst_a
inst_a.add<Defense>(); // Initialized with value 50
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Make Defense component inheritable
world.Component<Defense>()
    .Add(Ecs.OnInstantiate, Ecs.Inherit);

// Create prefab
Entity spaceship = world.Prefab("Spaceship")
    .Set<Health>(new(100))
    .Set<Defense>(new(50));

// Create a prefab instance
Entity instA = world.Entity().IsA(spaceship);
Entity instB = world.Entity().IsA(spaceship);

// Override Defense only for instA
instA.Add<Defense>(); // Initialized with value 50
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Make Defense component inheritable
world
.component::<Defense>()
.add_trait::<(flecs::OnInstantiate, flecs::Inherit)>();

// Create prefab
let spaceship = world.prefab().set(Defense { value: 50 });

// Create prefab instance
let inst_a = world.entity().is_a_id(spaceship);
let inst_b = world.entity().is_a_id(spaceship);

// Override Defense only for inst_a
inst_a.add::<Defense>(); // Initialized with value 50
```

</li>
</ul>
</div>

When an override is removed, the original value of the prefab is reexposed.

## Auto Overriding
When a component is configured to be inheritable, sometimes it can still be useful for a specific prefab to have the instances always own the component. This can be achieved with an auto override, which flags a component on the prefab to be always overridden on instantiation:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(ecs, Defense);

// Make Defense component inheritable
ecs_add_pair(world, ecs_id(Defense), 
    EcsOnInstantiate, EcsInherit);

// Create prefab
ecs_entity_t SpaceShip = ecs_entity(ecs, { 
    .name = "SpaceShip", 
    .add = ecs_ids( EcsPrefab ) 
});

// Set & auto override Defense
ecs_set(ecs, SpaceShip, Defense, {50});
ecs_auto_override(world, SpaceShip, Defense);

// Create prefab instances
ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, SpaceShip);
ecs_owns(world, inst, Defense); // true
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Make Defense component inheritable
world.component<Defense>()
    .add(flecs::OnInstantiate, flecs::Inherit);

// Create prefab
flecs::entity SpaceShip = world.prefab()
    .set_auto_override(Defense{50}); // Set & auto override Defense

// Create prefab instance
flecs::entity inst = world.entity().is_a(SpaceShip);
inst.owns<Defense>(); // true
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Make Defense component inheritable
world.Component<Defense>()
    .Add(Ecs.OnInstantiate, Ecs.Inherit);

// Create prefab
Entity spaceship = world.Prefab()
    .SetAutoOverride<Defense>(new(50)); // Set & auto override Defense

// Create prefab instance
Entity inst = world.Entity().IsA(spaceship);
inst.Owns<Defense>(); // true
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Make Defense component inheritable
world
.component::<Defense>()
.add_trait::<(flecs::OnInstantiate, flecs::Inherit)>();

// Create prefab
let spaceship = world.prefab().set_auto_override(Defense { value: 50 }); // Set & auto override Defense

// Create prefab instance
let inst = world.entity().is_a_id(spaceship);
inst.owns::<Defense>(); // true
```

</li>
</ul>
</div>

Auto overrides can also be added to prefabs that don't have the actual component. In this case the component is not copied from the prefab (there is nothing to copy), but is added & default constructed on the instance.

This also works for prefab children. Adding an auto override to a child entity without adding the component will add & default construct the component on the instance child.

## Prefab Variants
Prefabs can inherit from each other to create prefab variations without duplicating the prefab data. The following example shows a prefab and a prefab variant.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(ecs, Health);
ECS_COMPONENT(ecs, Defense);

// Create prefab
ecs_entity_t SpaceShip = ecs_entity(ecs, { 
    .name = "SpaceShip", 
    .add = ecs_ids( EcsPrefab ) 
});

ecs_set(ecs, SpaceShip, Defense, {50});
ecs_set(ecs, SpaceShip, Health, {100});

// Create prefab variant
ecs_entity_t freighter = ecs_entity(ecs, { 
    .name = "Freighter", 
    .add = ecs_ids( EcsPrefab, ecs_isa(SpaceShip) ) 
});

// Override the Health component of the freighter
ecs_set(ecs, freighter, Health, {150});

// Create prefab instance
ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, Freighter);
const Health *health = ecs_get(world, inst, Health); // 150
const Defense *defense = ecs_get(world, inst, Defense); // 50
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Create prefab
flecs::entity SpaceShip = world.prefab("SpaceShip")
    .set(Defense{50})
    .set(Health{100});

// Create prefab variant
flecs::entity Freighter = world.prefab("Freighter")
    .is_a(SpaceShip)
    .set(Health{150}); // Override the Health component of the freighter

// Create prefab instance
flecs::entity inst = world.entity().is_a(Freighter);
const Health *health = inst.get<Health>(); // 150
const Defense *defense = inst.get<Defense>(); // 50
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Create prefab
Entity spaceship = world.Prefab("Spaceship")
    .Set<Defense>(new(50))
    .Set<Health>(new(100));

// Create prefab variant
Entity freighter = world.Prefab("Freighter")
    .IsA(spaceShip)
    .Set<Health>(new(150)); // Override the Health component of the freighter

// Create prefab instance
Entity inst = world.Entity().IsA(freighter);
ref readonly Health health = ref inst.Get<Health>(); // 150
ref readonly Defense defense = ref inst.Get<Defense>(); // 50
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Create prefab
let spaceship = world
.prefab_named("spaceship")
.set(Defense { value: 50 })
.set(Health { value: 100 });

// Create prefab variant
let freighter = world
.prefab_named("Freighter")
.is_a_id(spaceship)
.set(Health { value: 150 }); // Override the Health component of the freighter

// Create prefab instance
let inst = world.entity().is_a_id(freighter);
inst.get::<&Health>(|health| {
println!("Health value: {}", health.value); // 150
});
inst.get::<&Defense>(|defense| {
println!("Defense value: {}", defense.value); // 50
});

```

</li>
</ul>
</div>

## Prefab Hierarchies
When a prefab has children, the entire subtree of a prefab is copied to the instance. Other than with the instance itself where components can be inherited from a prefab, child entities never inherit components from prefab children. Prefab hierarchies are created in the same way as entity hierarchies:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create a prefab hierarchy.
ecs_entity_t SpaceShip = ecs_entity(ecs, { 
    .name = "SpaceShip", 
    .add = ecs_ids( EcsPrefab ) 
});

ecs_entity_t Cockpit = ecs_entity(ecs, { 
    .name = "Cockpit",
    .parent = SpaceShip, // shortcut to add_pair(EcsChildOf)
    .add = ecs_ids( EcsPrefab )
});

// Instantiate the prefab hierarchy
ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, SpaceShip);

// Lookup instantiated child
ecs_entity_t inst_cockpit = ecs_lookup_child(ecs, inst, "Cockpit");
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity SpaceShip = world.prefab("SpaceShip");
flecs::entity Cockpit = world.prefab("Cockpit")
    .child_of(SpaceShip);

// Instantiate the prefab hierarchy
flecs::entity inst = ecs.entity().is_a(SpaceShip);

// Lookup instantiated child
flecs::entity inst_cockpit = inst.lookup("Cockpit");
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity spaceship = world.Prefab("Spaceship");
Entity cockpit = world.Prefab("Cockpit")
    .ChildOf(spaceship);

// Instantiate the prefab hierarchy
Entity inst = ecs.Entity().IsA(spaceship);

// Lookup instantiated child
Entity instCockpit = inst.Lookup("Cockpit");
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let spaceship = world.prefab_named("spaceship");
let cockpit = world.prefab_named("Cockpit").child_of_id(spaceship);

// Instantiate the prefab hierarchy
let inst = world.entity().is_a_id(spaceship);

// Lookup instantiated child
let inst_cockpit = inst.lookup("Cockpit");
```

</li>
</ul>
</div>

## Prefab Slots
When a prefab hierarchy is instantiated often code will want to refer to a specific instantiated child. A typical example is a turret prefab with a turret head that needs to rotate.

While it is possible to lookup a child by name and store it on a component, this adds boilerplate and reduces efficiency. Prefab slots make this easier.

A prefab child can be created as a slot. Slots are created as relationships on the instance, with as target of the relationship the instantiated child. The slot is added as a union relationship which doesn't fragment archetypes.

The following example shows how to create and use a prefab slot:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create a prefab hierarchy.
ecs_entity_t SpaceShip = ecs_entity(ecs, { 
    .name = "SpaceShip", 
    .add = ecs_ids( EcsPrefab ) 
});

ecs_entity_t Cockpit = ecs_entity(ecs, { 
    .name = "Cockpit",
    .parent = SpaceShip,
    .add = ecs_ids( 
        EcsPrefab, 
        ecs_pair(EcsSlotOf, SpaceShip) // mark as slot
    )
});

// Instantiate the prefab hierarchy
ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, SpaceShip);

// Lookup instantiated child
ecs_entity_t inst_cockpit = ecs_target(world, inst, Cockpit, 0);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity SpaceShip = world.prefab("SpaceShip");
flecs::entity Cockpit = world.prefab("Cockpit")
    .child_of(SpaceShip)
    .slot(); // Defaults to (SlotOf, SpaceShip)

// Instantiate the prefab hierarchy
flecs::entity inst = ecs.entity().is_a(SpaceShip);

// Lookup instantiated child
flecs::entity inst_cockpit = inst.target(CockPit);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity spaceship = world.Prefab("Spaceship");
Entity cockpit = world.Prefab("Cockpit")
    .ChildOf(spaceship)
    .SlotOf(spaceship);

// Instantiate the prefab hierarchy
Entity inst = ecs.Entity().IsA(spaceship);

// Lookup instantiated child
Entity instCockpit = inst.Target(cockpit);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let spaceship = world.prefab_named("Spaceship");
let cockpit = world.prefab_named("Cockpit").child_of_id(spaceship).slot(); // Defaults to (SlotOf, spaceship)

// Instantiate the prefab hierarchy
let inst = world.entity().is_a_id(spaceship);

// Lookup instantiated child
let inst_cockpit = inst.target_id(cockpit, 0);
```

</li>
</ul>
</div>

## Prefab Types (C++, C#)
Like entities and components, prefabs can be associated with a C++ type. This makes it easier to instantiate prefabs as it is not necessary to pass prefab handles around in the application. The following example shows how to associate types with prefabs:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C++</b>

```cpp
struct SpaceShip {};

// Create prefab associated with the SpaceShip type
world.prefab<SpaceShip>()
    .set<Defense>({ 50 })
    .set<Health>({ 100 });

// Instantiate prefab with type
flecs::entity inst = world.entity()
    .is_a<SpaceShip>();

// Lookup prefab handle
flecs::entity prefab = world.lookup("SpaceShip");
```

</li>
<li><b class="tab-title">C#</b>

```cs
public struct Spaceship { }

// Create prefab associated with the Spaceship type
world.Prefab<Spaceship>()
    .Set<Defense>(new(50))
    .Set<Health>(new(100));

// Instantiate prefab with type
Entity inst = world.Entity()
    .IsA<Spaceship>();

// Lookup prefab handle
Entity prefab = world.Lookup("Spaceship");
```

</li>
<li><b class="tab-title">Rust</b>

```rust
#[derive(Component)]
struct Spaceship;

// Create prefab associated with the spaceship type
world
.prefab_type::<Spaceship>()
.set(Defense { value: 50 })
.set(Health { value: 100 });

// Instantiate prefab with type
let inst = world.entity().is_a::<Spaceship>();

// Lookup prefab handle
let prefab = world.lookup("spaceship");
```

</li>
</ul>
</div>
