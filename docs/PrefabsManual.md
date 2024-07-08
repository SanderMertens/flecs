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
file record struct Defense(double Value);

// Create a SpaceShip prefab with a Defense component.
Entity SpaceShip = world.Entity("SpaceShip")
    .Set(new Defense(50));

// Create two prefab instances
Entity Inst1 = world.Entity().IsA(SpaceShip);
Entity Inst2 = world.Entity().IsA(SpaceShip);

// Get instantiated component
ref readonly Attack attack = ref inst_1.Get<Defense>();
```

</li>
</ul>
</div>

The following sections go over the different aspects of the prefab feature.

### The Prefab tag
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
TODO
```

</li>
</ul>
</div>

### Component Inheritance
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
Entity SpaceShip = world.Prefab("SpaceShip")
    .Set<Health>(new(100))
    .Set<Defense>(new(50));

// Create a prefab instance
Entity inst = world.Entity().IsA(SpaceShip);

// Component is retrieved from instance
ref readonly Attack attack = ref inst.Get<Attack>();

// Component is retrieved from prefab
ref readonly Defense defense = ref inst.Get<Defense>();
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
if (!inheritedFrom) {
    // not inherited
}
```

</li>
</ul>
</div>

### Component Overriding
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
Entity SpaceShip = world.Prefab("SpaceShip")
    .Set<Defense>(new(50));

// Create a prefab instance
Entity inst_a = world.Entity().IsA(SpaceShip);
Entity inst_b = world.Entity().IsA(SpaceShip);

// Override Defense only for inst_a
inst_a.Set(new Defense(75));
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
Entity SpaceShip = world.Prefab("SpaceShip")
    .Set<Health>(new(100))
    .Set<Defense>(new(50));

// Create a prefab instance
Entity inst_a = world.Entity().IsA(SpaceShip);
Entity inst_b = world.Entity().IsA(SpaceShip);

// Override Defense only for inst_a
inst_a.Add<Defense>(); // Initialized with value 50
```

</li>
</ul>
</div>

When an override is removed, the original value of the prefab is reexposed.

### Auto Overriding
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
Entity SpaceShip = world.Prefab()
    .SetAutoOverride(Defense{50}); // Set & auto override Defense

// Create prefab instance
Entity inst = world.Entity().IsA(SpaceShip);
inst.Owns<Defense>(); // true
```

</li>
</ul>
</div>

Auto overrides can also be added to prefabs that don't have the actual component. In this case the component is not copied from the prefab (there is nothing to copy), but is added & default constructed on the instance.

This also works for prefab children. Adding an auto override to a child entity without adding the component will add & default construct the component on the instance child.

### Prefab Variants
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
Entity SpaceShip = world.Prefab("SpaceShip")
    .set<Defense>(new (50))
    .set<Health>(new (100));

// Create prefab variant
Entity Freighter = world.Prefab("Freighter")
    .IsA(SpaceShip)
    .set<Health>(new (150)); // Override the Health component of the freighter

// Create prefab instance
Entity inst = world.Entity().IsA(Freighter);
ref readonly Attack attack = ref inst.get<Health>(); // 150
ref readonly Defense defense = ref inst.get<Defense>(); // 50
```

</li>
</ul>
</div>

### Prefab Hierarchies
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
Entity SpaceShip = world.Prefab("SpaceShip");
Entity Cockpit = world.Prefab("Cockpit")
    .ChildOf(SpaceShip);

// Instantiate the prefab hierarchy
Entity inst = ecs.Entity().IsA(SpaceShip);

// Lookup instantiated child
Entity inst_cockpit = inst.Lookup("Cockpit");
```

</li>
</ul>
</div>

### Prefab Slots
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
Entity SpaceShip = world.Prefab("SpaceShip");
Entity Cockpit = world.Prefab("Cockpit")
    .ChildOf(SpaceShip)
    .SlotOf(SpaceShip);

// Instantiate the prefab hierarchy
Entity inst = ecs.Entity().IsA(SpaceShip);

// Lookup instantiated child
Entity inst_cockpit = inst.Target(CockPit);
```

</li>
</ul>
</div>

### Prefab Types (C++, C#)
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
file struct SpaceShip { }

// Create prefab associated with the SpaceShip type
world.Prefab<SpaceShip>()
    .Set<Defense>(new (50))
    .Set<Health>(new (100));

// Instantiate prefab with type
Entity inst = world.Entity()
    .IsA<SpaceShip>();

// Lookup prefab handle
Entity prefab = world.Lookup("SpaceShip");
```

</li>
</ul>
</div>

