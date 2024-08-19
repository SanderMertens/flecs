# Observers

## Introduction
Observers are a mechanism that allows applications to react to events. Events can be either user defined or builtin, where the latter communicates changes in the ECS such as adding and removing components. Observers are similar to systems, in that they are queries that are combined with a callback. The difference between systems and observers is that systems are executed periodically for all matching _entities_, whereas observers are executed whenever a matching _event_ occurs.

## Example

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Observer callback
void OnSetPosition(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);

    for (int i = 0; i < it->count; i ++) {
        printf("Position set: {%f, %f}\n", p[i].x, p[i].y);
    }
}

// Create observer that is invoked whenever Position is set
ecs_observer(world, {
    .query.terms = {{ ecs_id(Position) }},
    .events = { EcsOnSet },
    .callback = OnSetPosition
});

// Alternatively this macro shortcut can be used:
// ECS_OBSERVER(world, OnSetPosition, EcsOnSet, Position);

ecs_entity_t e = ecs_new(world);
ecs_set(world, e, Position, {10, 20}); // Invokes observer
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Create observer that is invoked whenever Position is set
world.observer<Position>()
    .event(flecs::OnSet)
    .each([](flecs::entity e, Position& p) {
        std::cout << "Position set: {" << p.x << ", " << p.y << "}\n";
    });

world.entity().set(Position{10, 20}); // Invokes observer
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Create observer that is invoked whenever Position is set
world.Observer<Position>()
    .Event(Ecs.OnSet)
    .Each((Iter it, int i, ref Position p) =>
    {
        Console.WriteLine($"OnSet: ({p.X}, {p.Y})");
    });

world.Entity().Set(new Position(10, 20)); // Invokes observer
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Create observer that is invoked whenever Position is set
world
    .observer::<flecs::OnSet, &Position>()
    .each_entity(|e, p| {
        println!("Position set: {{ {}, {} }}", p.x, p.y);
    });

world.entity().set(Position { x: 10.0, y: 20.0 }); // Invokes observer
```

</li>
</ul>
</div>

## The Basics
This section goes over basic observer features.

### Usage
Observers provide a flexible out of the box event delivery mechanism for applications. It is not a one-size-fits all feature however, and this section provides information that helps with making a decision on whether observers are a good fit, or not so much.

The first thing that is important to know about is that observers are primarily a mechanism for delivering events that match _queries_. In a way they can be considered as the reactive counterpart to systems. This means that almost all of the flexibility and features that Flecs queries provide are also available to observers, which goes far beyond what typical event implementations provide.

That flexibility does come at a cost however. For most non-trivial observers, a query has to be evaluated before the observer is invoked. This means that for simple use cases, a basic event queue is always going to outperform observers.

#### Limitations
Observers may not provide the features that are required for an event implementation. Here are a few things that observers can't (easily) do:

- Peek ahead in the event queue
- Have different event queues per entity (possible, but inefficient/difficult)
- Sort events before processing them
- Process events on multiple threads
- Bubble up events (event propagation - see below - pushes events downwards)
- Stop event propagation

#### Good Use Cases for Observers
Good use cases for observers are scenarios where you need to respond to a structural change in the ECS, like a component that is being added or removed to an entity. Another good use case for observers is if you need to respond to changes in a component that is always assigned through a `set` operation. A typical example is a `Window` component, where you can resize a window by setting the component.

Another good application for observers is when you have events that are infrequent (like a window resize) and the builtin observer API provides everything that's needed.

#### Bad Use Cases for Observers
If you find yourself adding or removing components just to trigger observer events, that's a bad application for observers. Not only would that be an expensive solution for a simple problem, it would also be unreliable because features like command batching impact how and when events are emitted.

Another rule of thumb is that if you can solve something with a system, it should probably be solved with a system. Running something every frame may sound expensive when compared to reacting to aperiodic events, but systems are much more efficient to run, and have more predictable performance. You can also use marker tags in combination with a not operator to prevent a system from running repeatedly for the same entity.

#### Observers vs. Hooks
Hooks at face value appear to provide functionality that is similar to observers. There are `on_add`, `on_remove` and `on_set` hooks, just as there are observers that are invoked for `OnAdd`, `OnRemove` and `OnSet` events. The intended use cases for hooks and observers are almost opposites of each other. What gives?

Hooks are part of the "interface" of a component, just like how constructors and destructors are. You could consider hooks as the counterpart to OOP methods in ECS. They define the behavior of a component, but can only be invoked through mutations on the component data. You can only configure a single `on_add`, `on_remove` and `on_set` hook per component, just like you can only have a single constructor and destructor. Hooks also receive priority treatment: they are always invoked before observers -or in the case of a remove operation- after observers.

Observers on the other hand are a mechanism that enable _other_ parts of the application to respond to events related to a component. There can be many observers for a single component, registered by different parts of the application.

Here's a list with the differences between hooks and observers:

- There can be only one hook per event/component
- There can be many observers for a single event/component
- Hooks can only match a single event/component
- Observers match queries which can have multiple terms
- Hooks can only be configured for components, not tags
- Hooks are much more efficient than observers
- Hooks can mutate a component
- Observers should never mutate a component
- Once a component is in use, hooks cannot be changed
- Observers can be dynamically added & removed

### OnAdd Events
Observers can subscribe to `OnAdd` events to get notified whenever a component, tag or pair is added to an entity. An event only fires when the component is actually added to the entity, so not on each `add` operation. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_new(world);

// OnAdd observer fires
ecs_add(world, e, Position);

// OnAdd observer doesn't fire, entity already has component
ecs_add(world, e, Position);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.entity();

// OnAdd observer fires
e.add<Position>();

// OnAdd observer doesn't fire, entity already has component
e.add<Position>();
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity e = world.Entity();

// OnAdd observer fires
e.Add<Position>();

// OnAdd observer doesn't fire, entity already has component
e.Add<Position>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.entity();

// OnAdd observer fires
e.add::<Position>();

// OnAdd observer doesn't fire, entity already has component
e.add::<Position>();
```

</li>
</ul>
</div>

An `OnAdd` observer is invoked after a component constructor and `on_add` hook is invoked. If an observer accesses the value of a component it will be a valid constructed object. However, if an `OnAdd` observer was invoked as part of a `set` operation, the value assigned to the component in the `set` operation will not(!) be visible on the `OnAdd` observer. This means that for components that do not have a constructor, the component value passed to the observer will be uninitialized.

### OnSet Events
Observers can subscribe to an `OnSet` event to get notified whenever a component is assigned with a new value. An `OnSet` event will be generated each time the `set` operation is called, or when `modified` is called. `OnSet` observers are _not_ invoked when a system directly modifies a component. An application will have to manually call `modified` to make sure observers `OnSet` are invoked. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_new(world);

// OnAdd observer fires first, then OnSet observer fires
ecs_set(world, e, Position, {10, 20});

// OnAdd observer doesn't fire, OnSet observer fires
ecs_set(world, e, Position, {10, 20});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.entity();

// OnAdd observer fires first, then OnSet observer fires
e.set(Position{10, 20});

// OnAdd observer doesn't fire, OnSet observer fires
e.set(Position{10, 20});
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity e = world.Entity();

// OnAdd observer fires first, then OnSet observer fires
e.Set(new Position(10, 20));

// OnAdd observer doesn't fire, OnSet observer fires
e.Set(new Position(10, 20));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.entity();

// OnAdd observer fires first, then OnSet observer fires
e.set(Position { x: 10.0, y: 20.0 });

// OnAdd observer doesn't fire, OnSet observer fires
e.set(Position { x: 10.0, y: 20.0 });
```

</li>
</ul>
</div>

### OnSet and Inheritance
To ensure that OnSet events can be used reliably to detect component changes, events can be produced by operations that change inheritance relationships or operate on inherited from components. This is enabled by default for components with the `(OnInstantiate, Inherit)` trait. To prevent this behavior, add the `self` modifier to an observer term. The following inheritance scenarios produce OnSet events. All scenarios assume that the component has the `(OnInstantiate, Inherit)` trait.

#### Adding an IsA pair
When an IsA pair is added to an entity, an OnSet event is generated for each newly inherited component:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t p = ecs_insert(world, {EcsPrefab}, ecs_value(Position, {10, 20}));
ecs_entity_t i = ecs_new(world);

// Produces OnSet event for Position
ecs_add_pair(world, i, EcsIsA, p);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity p = world.prefab().set(Position{10, 20});

// Produces OnSet event for Position
flecs::entity i = world.entity().is_a(p);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity p = world.Prefab().Set(new Position(10, 20));

// Produces OnSet event for Position
Entity i = world.Entity().IsA(p);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let p = world.prefab().set(Position { x: 10.0, y: 20.0 });

// Produces OnSet event for Position
let i = world.entity().is_a_id(p);
```

</li>
</ul>
</div>

If the base entity has a component that the entity already had no event is generated. Similarly, if a component from a base entity is already provided by another base entity, and the new base entity does not become the primary source for the component, no OnSet event is generated. A base entity is the primary source for a component if:

- There are no other base entities that provide the component, or
- It has the lowest entity id of all base entities that provide the component

#### Removing an Override
When an overridden component is removed, the inherited component is reexposed which effectively changes the value of the component for the entity. An OnSet event will be produced for the inherited component:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t p = ecs_insert(world, {EcsPrefab}, ecs_value(Position, {10, 20}));

// Produces OnSet event for inherited Position component
ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);

// Override component. Produces regular OnSet event.
ecs_set(world, i, Position, {20, 30});

// Reexposes inherited component, produces OnSet event
ecs_remove(world, i, Position);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity p = world.prefab().set(Position{10, 20});

// Produces OnSet event for inherited Position component
flecs::entity i = world.entity().is_a(p);

// Override component. Produces regular OnSet event.
i.set(Position{20, 30});

// Reexposes inherited component, produces OnSet event
i.remove<Position>();
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity p = world.Prefab().Set(new Position(10, 20));

// Produces OnSet event for inherited Position component
Entity i = world.Entity().IsA(p);

// Override component. Produces regular OnSet event.
i.Set(new Position(20, 30));

// Reexposes inherited component, produces OnSet event
i.Remove<Position>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let p = world.prefab().set(Position { x: 10.0, y: 20.0 });

// Produces OnSet event for inherited Position component
let i = world.entity().is_a_id(p);

// Override component. Produces regular OnSet event.
i.set(Position { x: 20.0, y: 30.0 });

// Reexposes inherited component, produces OnSet event
i.remove::<Position>();
```

</li>
</ul>
</div>

#### Setting an Inherited Component
When an inherited component is modified, an OnSet event is propagated to all entities that inherit the component:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t p = ecs_insert(world, {EcsPrefab}, ecs_value(Position, {10, 20}));

// Produces OnSet event for inherited Position component
ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);

// Produces OnSet event for inherited Position
ecs_set(world, p, Position, {20, 30});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity p = world.prefab().set(Position{10, 20});

// Produces OnSet event for Position
flecs::entity i = world.entity().is_a(p);
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity p = world.Prefab().Set(new Position(10, 20));

// Produces OnSet event for Position
Entity i = world.Entity().IsA(p);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let p = world.prefab().set(Position { x: 10.0, y: 20.0 });

// Produces OnSet event for Position
let i = world.entity().is_a_id(p);
```

</li>
</ul>
</div>

### OnRemove Events
Observers can subscribe to `OnRemove` events to get notified whenever a component, tag or pair is removed from an entity. An event only fires when the component is actually removed from the entity, so not on each `remove` operation. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

// OnRemove observer fires
ecs_remove(world, e, Position);

// OnRemove observer doesn't fire, entity doesn't have the component
ecs_add(world, e, Position);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.entity().set(Position{10, 20});

// OnRemove observer fires
e.remove<Position>();

// OnRemove observer doesn't fire, entity doesn't have the component
e.remove<Position>();
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity e = world.Entity().Set(new Position(10, 20));

// OnRemove observer fires
e.Remove<Position>();

// OnRemove observer doesn't fire, entity doesn't have the component
e.Remove<Position>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.entity().set(Position { x: 10.0, y: 20.0 });

// OnRemove observer fires
e.remove::<Position>();

// OnRemove observer doesn't fire, entity doesn't have the component
e.remove::<Position>();
```

</li>
</ul>
</div>

## Multi-event Observers
A single observer can subscribe for multiple events:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Observer that listens for both OnAdd and OnRemove events
ecs_observer(world, {
    .query.terms = {{ ecs_id(Position) }},
    .events = { EcsOnAdd, EcsOnRemove },
    .callback = OnPosition
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Observer that listens for both OnAdd and OnRemove events
world.observer<Position>()
    .event(flecs::OnAdd)
    .event(flecs::OnRemove)
    .each([](flecs::entity e, Position& p) {
        // ...
    });
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Observer that listens for both OnAdd and OnRemove events
world.Observer<Position>()
    .Event(Ecs.OnAdd)
    .Event(Ecs.OnRemove)
    .Each((Iter it, int i, ref Position p) =>
    {
        // ...
    });
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Observer that listens for both OnAdd and OnRemove events
world
    .observer::<flecs::OnAdd, &Position>()
    .add_event::<flecs::OnRemove>()
    .each_entity(|e, p| {
// ...
});
```

</li>
</ul>
</div>

The iterator object provided to the observer callback provides information on which event triggered. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
void OnPosition(ecs_iter_t *it) {
    if (it->event == EcsOnAdd) {
        // ...
    } else if (it->event == EcsOnRemove) {
        // ...
    }
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.observer<Position>()
    .event(flecs::OnAdd)
    .event(flecs::OnRemove)
    .each([](flecs::iter& it, size_t i, Position&) {
        if (it.event() == flecs::OnAdd) {
            // ...
        } else if (it.event() == flecs::OnRemove) {
            // ...
        }
    });
```

</li>
<li><b class="tab-title">C#</b>

```cs
world.Observer<Position>()
    .Event(Ecs.OnAdd)
    .Event(Ecs.OnRemove)
    .Each((Iter it, int i, ref Position p) =>
    {
        if (it.Event() == Ecs.OnAdd) {
            // ...
        } else if (it.Event() == Ecs.OnRemove) {
            // ...
        }
    });
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .observer::<flecs::OnAdd, &Position>()
    .add_event::<flecs::OnRemove>()
    .each_iter(|it, i, p| {
        if it.event() == flecs::OnAdd::ID {
        // ...
        } else if it.event() == flecs::OnRemove::ID {
        // ...
        }
    });
```

</li>
</ul>
</div>

Alternatively, an observer can also use `Wildcard` as event, which will create an observer that listens for any kind of event that matches the observer. Wildcard event observers do add significant overhead to ECS operations for a specific component, so they should be used sparingly. A typical use case for wildcard event observers is logging or debugging. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Observer that listens for all events for Position
ecs_observer(world, {
    .query.terms = {{ ecs_id(Position) }},
    .events = { EcsWildcard },
    .callback = OnPosition
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Observer that listens for all events for Position
world.observer<Position>()
    .event(flecs::Wildcard)
    .each([](flecs::entity e, Position& p) {
        // ...
    });
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Observer that listens for all events for Position
world.Observer<Position>()
    .Event(Ecs.Wildcard)
    .Each((Iter it, int i, ref Position p) =>
    {
        // ...
    });
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Observer that listens for all events for Position
world
    .observer::<flecs::Wildcard, &Position>()
    .each_entity(|e, p| {
        // ...
    });
```

</li>
</ul>
</div>

## Multi-term Observers
Observers use queries to match events. This makes observers similar to systems, which are also callbacks invoked for matching entities, except that observers match their query against events. This means that observers can match multiple components, use operators, query traversal and more. A simple example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Observer that listens for entities with both Position and Velocity
void OnSetPosition(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    for (int i = 0; i < it->count; i ++) {
        // ...
    }
}

ecs_observer(world, {
    .query.terms = {{ ecs_id(Position) }, { ecs_id(Velocity) }},
    .events = { EcsOnAdd },
    .callback = OnPosition
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Observer that listens for entities with both Position and Velocity
world.observer<Position, Velocity>()
    .event(flecs::OnAdd)
    .each([](flecs::entity e, Position& p, Velocity& v) {
        // ...
    });
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Observer that listens for entities with both Position and Velocity
world.Observer<Position, Velocity>()
    .Event(Ecs.OnAdd)
    .Each((Iter it, int i, ref Position p, ref Velocity v) =>
    {
        // ...
    });
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Observer that listens for entities with both Position and Velocity
world
    .observer::<flecs::OnAdd, (&Position, &Velocity)>()
    .each_entity(|e, (p, v)| {
        // ...
    });
```

</li>
</ul>
</div>

Observers with multiple terms will only be invoked for entities that match all terms. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_new(world);

// Does not trigger "Position, Velocity" observer
ecs_add(world, e, Position);

// Entity now matches "Position, Velocity" query, triggers observer
ecs_add(world, e, Velocity);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.entity();

// Does not trigger "Position, Velocity" observer
e.add<Position>();

// Entity now matches "Position, Velocity" query, triggers observer
e.add<Velocity>();
```

</li>
<li><b class="tab-title">C#</b>

```cs
Entity e = world.Entity();

// Does not trigger "Position, Velocity" observer
e.Add<Position>();

// Entity now matches "Position, Velocity" query, triggers observer
e.Add<Velocity>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.entity();

// Does not trigger "Position, Velocity" observer
e.add::<Position>();

// Entity now matches "Position, Velocity" query, triggers observer
e.add::<Velocity>();
```

</li>
</ul>
</div>

Internally observers with multiple terms are implemented with multiple single-term observers. Whenever a single-term observer triggers, the observer query is evaluated against the source of the event. Something to consider is that single-term observers do not have the query evaluation step, which makes them more performant than multi-term observers.

### Filter Terms
A multi-term observer by default will trigger for events on any term, as long as the event source matches the observer query. In some scenarios this is not desirable, and an observer should only trigger on one or more specific terms, while applying the other terms only as a filter. This can be accomplished with filter terms. The following example shows how:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Observer that only triggers on Position, not on Velocity
ecs_observer(world, {
    .query.terms = {
        { ecs_id(Position) }, 
        { ecs_id(Velocity), .inout = EcsInOutFilter }
    },
    .events = { EcsOnSet },
    .callback = OnPosition
});

ecs_entity_t e = ecs_new(world);

// Doesn't trigger, entity doesn't have Velocity
ecs_set(world, e, Position, {10, 20});

// Doesn't trigger, Velocity is a filter term
ecs_set(world, e, Velocity, {1, 2});

// Triggers, entity now matches observer query
ecs_set(world, e, Position, {20, 30});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Observer that only triggers on Position, not on Velocity
world.observer<Position>()
    .with<Velocity>().filter()
    .event(flecs::OnAdd)
    .each([](flecs::entity e, Position& p) {
        // ...
    });

flecs::entity e = world.entity();

// Doesn't trigger, entity doesn't have Velocity
e.set(Position{10, 20});

// Doesn't trigger, Velocity is a filter term
e.set(Velocity{1, 2});

// Triggers, entity now matches observer query
e.set(Position{20, 30});
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Observer that only triggers on Position, not on Velocity
world.Observer<Position>()
    .With<Velocity>().Filter()
    .Event(Ecs.OnAdd)
    .Each((Iter it, int i, ref Position p) =>
    {
        // ...
    });

Entity e = world.Entity();

// Doesn't trigger, entity doesn't have Velocity
e.Set(new Position(10, 20));

// Doesn't trigger, Velocity is a filter term
e.Set(new Velocity(1, 2));

// Triggers, entity now matches observer query
e.Set(new Velocity(20, 30));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Observer that only triggers on Position, not on Velocity
world
    .observer::<flecs::OnAdd, &Position>()
    .with::<Velocity>()
    .filter()
    .each_entity(|e, p| {
        // ...
    });

let e = world.entity();

// Doesn't trigger, entity doesn't have Velocity
e.set(Position { x: 10.0, y: 20.0 });

// Doesn't trigger, Velocity is a filter term
e.set(Velocity { x: 1.0, y: 2.0 });

// Triggers, entity now matches observer query
e.set(Position { x: 20.0, y: 30.0 });
```

</li>
</ul>
</div>

### Event Downgrading
When an OnSet observer requests both components and tags, the events for the tag terms are "downgraded" to an OnAdd event. The reason this happens is because tags cannot be set, and can therefore not produce OnSet events. Downgrading the event (as opposed to failing to create the observer) allows for OnSet observers that have both components and tags. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// OnSet observer with both component and tag
ecs_observer(world, {
    .query.terms = {
        { ecs_id(Position) }, 
        { Npc } // Tag
    },
    .events = { EcsOnSet },
    .callback = MyObserver
});

ecs_entity_t e = ecs_new(world);

// Doesn't trigger, entity doesn't have Npc
ecs_set(world, e, Position, {10, 20});

// Produces and OnAdd event & triggers observer
ecs_add(world, e, Npc);

// Produces an OnSet event & triggers observer
ecs_set(world, e, Position, {20, 30});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// OnSet observer with both component and tag
world.observer<Position>()
    .with<Npc>() // Tag
    .event(flecs::OnSet)
    .each([](flecs::entity e, Position& p) {
        // ...
    });

flecs::entity e = world.entity();

// Doesn't trigger, entity doesn't have Npc
e.set(Position{10, 20});

// Produces and OnAdd event & triggers observer
e.add<Npc>();

// Produces an OnSet event & triggers observer
e.set(Position{20, 30});
```

</li>
<li><b class="tab-title">C#</b>

```cs
// OnSet observer with both component and tag
world.Observer<Position>()
    .With<Npc>() // Tag
    .Event(Ecs.OnSet)
    .Each((Iter it, int i, ref Position p) =>
    {
        // ...
    });

Entity e = world.Entity();

// Doesn't trigger, entity doesn't have Npc
e.Set(new Position(10, 20));

// Produces and OnAdd event & triggers observer
e.Add<Npc>();

// Produces an OnSet event & triggers observer
e.Set(new Position(20, 30));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// OnSet observer with both component and tag
world
    .observer::<flecs::OnSet, &Position>()
    .with::<Npc>() // Tag
    .each_entity(|e, p| {
        // ...
    });

let e = world.entity();

// Doesn't trigger, entity doesn't have Npc
e.set(Position { x: 10.0, y: 20.0 });

// Produces and OnAdd event & triggers observer
e.add::<Npc>();

// Produces an OnSet event & triggers observer
e.set(Position { x: 20.0, y: 30.0 });
```

</li>
</ul>
</div>

### Event Inversion
Because observers match events against queries, this also means they support all of the query operators such as `and`, `optional`, `or` and `not`. The `not` operator is noteworthy, as it needs to invert the event to make sure the observer is triggered correctly. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Observer with a Not term
ecs_observer(world, {
    .query.terms = {
        { ecs_id(Position) }, 
        { ecs_id(Velocity), .oper = EcsNot }
    },
    .events = { EcsOnAdd },
    .callback = MyObserver
});

ecs_entity_t e = ecs_new(world);

// Triggers the observer
ecs_set(world, e, Position, {10, 20});

// Doesn't trigger the observer, entity doesn't match the observer query
ecs_set(world, e, Velocity, {1, 2});

// Triggers the observer, as the Velocity term was inverted to OnRemove
ecs_remove(world, e, Velocity);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Observer with a Not term
world.observer<Position>()
    .without<Velocity>()
    .event(flecs::OnAdd)
    .each([](flecs::entity e, Position& p) {
        // ...
    });

flecs::entity e = world.entity();

// Triggers the observer
e.set(Position{10, 20});

// Doesn't trigger the observer, entity doesn't match the observer query
e.set(Velocity{1, 2});

// Triggers the observer, as the Velocity term was inverted to OnRemove
e.remove<Velocity>();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Observer with a Not term
world.Observer<Position>()
    .Without<Velocity>()
    .Event(Ecs.OnAdd)
    .Each((Iter it, int i, ref Position p) =>
    {
        // ...
    });

Entity e = world.Entity();

// Triggers the observer
e.Set(new Position(10, 20));

// Doesn't trigger the observer, entity doesn't match the observer query
e.Set(new Velocity(1, 2));

// Triggers the observer, as the Velocity term was inverted to OnRemove
e.Remove<Velocity>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Observer with a Not term
world
    .observer::<flecs::OnAdd, &Position>()
    .without::<Velocity>()
    .each_entity(|e, p| {
        // ...
    });

let e = world.entity();

// Triggers the observer
e.set(Position { x: 10.0, y: 20.0 });

// Doesn't trigger the observer, entity doesn't match the observer query
e.set(Velocity { x: 1.0, y: 2.0 });

// Triggers the observer, as the Velocity term was inverted to OnRemove
e.remove::<Velocity>();
```

</li>
</ul>
</div>

Inversion also works the other way around: a `not` term will be inverted to use an `OnAdd` event for an `OnRemove` observer. Note that in either case, the observer will be invoked with the observer event, e.g. `OnAdd` for an `OnAdd` observer, and `OnRemove` for an `OnRemove` observer.

Inversion also applies to `OnSet` events: an `OnSet` event will be inverted to `OnRemove` when it is used in combination with a `not` term.

## Monitors
A monitor is an observer that fires when an entity starts and stops matching a query. Whether an entity starts or stops matching is communicated with an `OnAdd` or `OnRemove` event. Monitors can only specify a single `Monitor` event. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
void MyMonitor(ecs_iter_t *it) {
    if (it->event == EcsOnAdd) {
        for (int i = 0; i < it->count; i ++) {
            // Entity started matching query
        }
    } else if (it->event == EcsOnRemove) {
        for (int i = 0; i < it->count; i ++) {
            // Entity stopped matching query
        }
    }
}

// Monitor observer
ecs_observer(world, {
    .query.terms = {
        { ecs_id(Position) }, 
        { ecs_id(Velocity) }
    },
    .events = { EcsMonitor },
    .callback = MyMonitor
});

ecs_entity_t e = ecs_new(world);

// Doesn't trigger the monitor, entity doesn't match
ecs_set(world, e, Position, {10, 20});

// Entity now matches, triggers monitor with OnAdd event
ecs_set(world, e, Velocity, {1, 2});

// Entity no longer matches, triggers monitor with OnRemove event
ecs_remove(world, e, Position);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Monitor observer
world.observer<Position, Velocity>()
    .event(flecs::Monitor)
    .each([](flecs::iter& it, size_t i, Position& p, Velocity& v) {
        if (it.event() == flecs::OnAdd) {
            // Entity started matching query
        } else if (it.event() == flecs::OnRemove) {
            // Entity stopped matching query
        }
    });

flecs::entity e = world.entity();

// Doesn't trigger the monitor, entity doesn't match
e.set(Position{10, 20});

// Entity now matches, triggers monitor with OnAdd event
e.set(Velocity{1, 2});

// Entity no longer matches, triggers monitor with OnRemove event
e.remove<Position>();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Monitor observer
world.Observer<Position, Velocity>()
    .Event(Ecs.Monitor)
    .Each((Iter it, int i, ref Position p, ref Velocity v) =>
    {
        if (it.Event() == Ecs.OnAdd) {
            // Entity started matching query
        } else if (it.Event() == Ecs.OnRemove) {
            // Entity stopped matching query
        }
    });

Entity e = world.Entity();

// Doesn't trigger the monitor, entity doesn't match
e.set(new Position(10, 20));

// Entity now matches, triggers monitor with OnAdd event
e.Set(new Velocity(1, 2));

// Entity no longer matches, triggers monitor with OnRemove event
e.Remove<Velocity>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Monitor observer
world
    .observer::<flecs::Monitor, (&Position, &Velocity)>()
    .each_iter(|it, i, (p, v)| {
        if it.event() == flecs::OnAdd::ID {
            // Entity started matching query
        } else if it.event() == flecs::OnRemove::ID {
            // Entity stopped matching query
        }
    });

let e = world.entity();

// Doesn't trigger the monitor, entity doesn't match
e.set(Position { x: 10.0, y: 20.0 });

// Entity now matches, triggers monitor with OnAdd event
e.set(Velocity { x: 1.0, y: 2.0 });

// Entity no longer matches, triggers monitor with OnRemove event
e.remove::<Position>();
```

</li>
</ul>
</div>

Monitors are implemented by evaluating the observer query twice: once on the previous archetype of the entity, and one on the current archetype of the entity. The following table shows when the monitor observer is invoked:

| Previous matches | Current matches | Invoked with event |
|------------------|-----------------|--------------------|
| No | No | - |
| No | Yes | `OnAdd` |
| Yes | Yes | - |
| Yes | No | `OnRemove` |

Note that because monitors have to evaluate the query twice, they are more expensive to evaluate than regular observers.

## Yield Existing
Observers can be created with the "yield existing" property, which invokes the observer with all entities that already match the observer. This can make it easier to make code order-independent, as entities created before the observer will still trigger the observer. Yield existing only works with `OnAdd` and `OnSet` events. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Entity created before the observer
ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));

// Yield existing observer
ecs_observer(world, {
    .query.terms = {
        { ecs_id(Position) }
    },
    .events = { EcsOnAdd },
    .callback = MyObserver,
    .yield_existing = true
});

// Observer is invoked for e1

// Fires observer as usual
ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Entity created before the observer
flecs::entity e1 = world.entity().set(Position{10, 20});

// Yield existing observer
world.observer<Position, Velocity>()
    .event(flecs::OnAdd)
    .yield_existing()
    .each([](flecs::iter& it, size_t i, Position& p, Velocity& v) {
        // ...
    });

// Observer is invoked for e1

// Fires observer as usual
flecs::entity e2 = world.entity().set(Position{10, 20});
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Entity created before the observer
Entity e1 = world.Entity().Set(new Position(10, 20));

// Yield existing observer
world.Observer<Position, Velocity>()
    .Event(Ecs.OnAdd)
    .YieldExisting()
    .Each((Iter it, int i, ref Position p, ref Velocity v) =>
    {
        // ...
    });

// Observer is invoked for e1

// Fires observer as usual
Entity e2 = world.Entity().Set(new Position(10, 20));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Entity created before the observer
let e1 = world.entity().set(Position { x: 10.0, y: 20.0 });

// Yield existing observer
world
    .observer::<flecs::OnAdd, (&Position, &Velocity)>()
    .yield_existing()
    .each_iter(|it, i, (p, v)| {
        // ...
    });

// Observer is invoked for e1

// Fires observer as usual
let e2 = world.entity().set(Position { x: 10.0, y: 20.0 });
```

</li>
</ul>
</div>

## Fixed Source Terms
Observers can be created with fixed source terms, which are terms that are matched on a single entity. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Entity used for fixed source
ecs_entity_t Game = ecs_insert(world, ecs_value(TimeOfDay, {0}));

// Observer with fixed source
ecs_observer(world, {
    .query.terms = {
        { ecs_id(TimeOfDay), .src.id = Game } // Match TimeOfDay on Game
    },
    .events = { EcsOnSet },
    .callback = MyObserver
});

// Triggers observer
ecs_set(world, Game, TimeOfDay, {1});

// Does not trigger observer
ecs_entity_t e = ecs_insert(world, ecs_value(TimeOfDay, {0}));
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Entity used for fixed source
flecs::entity Game = world.entity().set(TimeOfDay{0});

// Observer with fixed source
world.observer<TimeOfDay>()
    .term_at(0).src(Game) // Match TimeOfDay on Game
    .event(flecs::OnSet)
    .each([](flecs::iter& it, size_t i, TimeOfDay& t) {
        // ...
    });

// Triggers observer
Game.set(TimeOfDay{1});

// Does not trigger observer
flecs::entity e = world.entity().set(TimeOfDay{0});
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Entity used for fixed source
Entity game = world.Entity().Set(new TimeOfDay(0));

// Observer with fixed source
world.Observer<Game>()
    .TermAt(0).Src(game) // Match TimeOfDay on Game
    .Event(Ecs.OnSet)
    .Each((Iter it, int i, ref Position p, ref Velocity v) =>
    {
        // ...
    });

// Triggers observer
game.Set(new TimeOfDay(1));

// Does not trigger observer
Entity e = world.Entity().Set(new TimeOfDay(0));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Entity used for fixed source
let game = world.entity().set(TimeOfDay { value: 0.0 });

// Observer with fixed source
world
    .observer::<flecs::OnSet, &TimeOfDay>()
    .term_at(0)
    .set_src_id(game) // Match TimeOfDay on game
    .each_iter(|it, i, time| {
        // ...
    });

// Triggers observer
game.set(TimeOfDay { value: 1.0 });

// Does not trigger observer
let e = world.entity().set(TimeOfDay { value: 0.0 });
```

</li>
</ul>
</div>

Observers may match terms on multiple different sources. However, when an observer matches components both on the `$this` source (default) and on a fixed source, the fixed source terms will not match events. The reason for this is that otherwise emitting an event for a fixed source term would mean iterating all matching entities for the `$this` term. If an observer only has fixed source terms, events will be matched for each of the terms.

### Singletons
Singletons are a special case of fixed source term, where the component is matched on itself. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_singleton_set(world, TimeOfDay, {0});

// Observer with singleton source
ecs_observer(world, {
    .query.terms = {
        { ecs_id(TimeOfDay), .src.id = ecs_id(TimeOfDay) }
    },
    .events = { EcsOnSet },
    .callback = MyObserver
});

// Triggers observer
ecs_singleton_set(world, TimeOfDay, {1});

// Does not trigger observer
ecs_entity_t e = ecs_insert(world, ecs_value(TimeOfDay, {0}));
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.set(TimeOfDay{0});

// Observer with singleton source
world.observer<TimeOfDay>()
    .term_at(0).singleton()
    .event(flecs::OnSet)
    .each([](flecs::iter& it, size_t i, TimeOfDay& t) {
        // ...
    });

// Triggers observer
world.set(TimeOfDay{1});

// Does not trigger observer
flecs::entity e = world.entity().set(TimeOfDay{0});
```

</li>
<li><b class="tab-title">C#</b>

```cs
world.Set(new TimeOfDay(0));

// Observer with singleton source
world.Observer<TimeOfDay>()
    .TermAt(0).Singleton()
    .Event(Ecs.OnSet)
    .Each((Iter it, int i, ref TimeOfDay t) =>
    {
        // ...
    });

// Triggers observer
world.Set(new TimeOfDay(1));

// Does not trigger observer
Entity e = world.Entity().Set(new TimeOfDay(0));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world.set(TimeOfDay { value: 0.0 });

// Observer with singleton source
world
    .observer::<flecs::OnSet, &TimeOfDay>()
    .term_at(0)
    .singleton()
    .each_iter(|it, i, time| {
        // ...
    });

// Triggers observer
world.set(TimeOfDay { value: 1.0 });

// Does not trigger observer
let e = world.entity().set(TimeOfDay { value: 0.0 });
```

</li>
</ul>
</div>

## Event Propagation
When an observer has a query that uses (up) relationship traversal, events are propagated along the relationship edge. For example, when an observer requests component `Position` from a parent entity, setting `Position` on the parent will propagate an `OnSet` event along the `ChildOf` edge, notifying all child entities of the parent. 

Events propagate until a leaf entity is found, or an entity with the propagated component is found. For example, if an event for `Position` is propagated from a parent to a child with `Position`, that event will not be propagated to the child's children. This ensures that the results are consistent with up traversal, where a relationship is traversed upwards until the first entity with the component is found. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create an observer that matches OnSet(Position) events on self and a parent
ecs_observer(world, {
    .query.terms = {{ ecs_id(Position), .src.id = EcsSelf|EcsUp /*, .trav = EcsChildOf (default) */ }},
    .events = { EcsOnSet },
    .callback = OnSetPosition
});

ecs_entity_t parent = ecs_new(world);
ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

// Invokes observer twice: once for the parent and once for the child
ecs_set(world, parent, Position, {10, 20});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Create an observer that matches OnSet(Position) events on self and a parent
world.observer<Position>()
    .term_at(0).self().up() // .trav(flecs::ChildOf) (default)
    .event(flecs::OnSet)
    .each([](flecs::entity e, Position& p) {
        // ...
    });

flecs::entity parent = world.entity();
flecs::entity child = world.entity().child_of(parent);

// Invokes observer twice: once for the parent and once for the child
parent.set(Position{10, 20});
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Create an observer that matches OnSet(Position) events on self and a parent
world.Observer<Position>()
    .TermAt(0).Self().Up()  // .Trav(Ecs.ChildOf) (default)
    .Event(Ecs.OnSet)
    .Each((Iter it, int i, ref Position p) =>
    {
        // ...
    });

Entity parent = world.Entity();
Entity child = world.Entity().ChildOf(parent);

// Invokes observer twice: once for the parent and once for the child
parent.Set(new Position(10, 20));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Create an observer that matches OnSet(Position) events on self and a parent
world
    .observer::<flecs::OnSet, &Position>()
    .term_at(0)
    .self_()
    .up() // .trav(flecs::ChildOf) (default)
    .each_entity(|e, p| {
        // ...
    });

let parent = world.entity();
let child = world.entity().child_of_id(parent);

// Invokes observer twice: once for the parent and once for the child
parent.set(Position { x: 10.0, y: 20.0 });
```

</li>
</ul>
</div>

## Event Forwarding
Event forwarding, like event propagation, is a mechanism that propagates events along relationship edges. The difference between propagation and forwarding is that event forwarding produces events when a relationship pair is added to an entity. For example, if a `(ChildOf, my_parent)` pair is added to an entity, and `my_parent` has components `Position` and `Velocity`, an `OnAdd` event is produced for each component and emitted for the child. 

Event forwarding allows applications to write order independent code, where it doesn't matter whether a relationship pair was added before or after the target of that pair received new components. It is the opposite of event propagation: where event propagation "pushes" an existing event downwards, event forwarding "pulls" new events from parent entities.

Only reachable components are forwarded. If an entity has a parent and grandparent that both have `Position`, only the `Position` component from the parent will result in an event. Just like with event propagation, this ensures consistency with the behavior of up traversal in queries.

Both adding and removing pairs can result in event forwarding, where adding a pair results in forwarded `OnAdd` events, and removing a pair results in forwarded `OnRemove` events. `OnSet` events can also be forwarded, but is only supported for `IsA` pairs, where adding an `(IsA, my_prefab)` pair will result in an `OnSet` event for all reachable components of that prefab.

The following code shows an example of event forwarding:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create an observer that matches OnAdd(Position) events on a parent
ecs_observer(world, {
    .query.terms = {{ ecs_id(Position), .src.id = EcsUp /*, .trav = EcsChildOf (default) */ }},
    .events = { EcsOnAdd },
    .callback = OnSetPosition
});

ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {10, 20}));

// Forwards OnAdd event for Position to child
ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Create an observer that matches OnAdd(Position) events on a parent
world.observer<Position>()
    .term_at(0).up() // .trav(flecs::ChildOf) (default)
    .event(flecs::OnAdd)
    .each([](flecs::entity e, Position& p) {
        // ...
    });

flecs::entity parent = world.entity().set(Position{10, 20});

// Forwards OnAdd event for Position to child
flecs::entity child = world.entity().child_of(parent);
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Create an observer that matches OnAdd(Position) events on a parent
world.Observer<Position>()
    .TermAt(0).Up() // .Trav(Ecs.ChildOf) (default)
    .Event(Ecs.OnAdd)
    .Each((Iter it, int i, ref Position p) =>
    {
        // ...
    });

Entity parent = world.Entity().Set(new Position(10, 20));

// Forwards OnAdd event for Position to child
Entity child = world.Entity().ChildOf(parent);
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Create an observer that matches OnAdd(Position) events on a parent
world
    .observer::<flecs::OnAdd, &Position>()
    .term_at(0)
    .up() // .trav(flecs::ChildOf) (default)
    .each_entity(|e, p| {
        // ...
    });

let parent = world.entity().set(Position { x: 10.0, y: 20.0 });

// Forwards OnAdd event for Position to child
let child = world.entity().child_of_id(parent);
```

</li>
</ul>
</div>

## Custom Events
Applications can register custom events to reuse the observer mechanism for purposes other than monitoring ECS events. Custom events, just like builtin events, require three pieces of information:

- An event
- One or more components
- A single event source

Consider adding `Position` to entity `my_entity`. This event would look like:

- Event: `OnAdd`
- Component: `Position`
- Source: `my_entity`

The difference for a custom event is that it replaces the event with a custom entity id that is created by the application. This is what a custom event could look like:

- Event: `Synchronized`
- Component: `Position`
- Source: `my_entity`

Just like with regular events, the entity must have the component that is emitted. This ensures that we can safely pass a reference to the component to an observer callback.

Custom events are emitted with the `emit` or `enqueue` functions (more later on the latter). The following example shows how to emit and listen for a custom event:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create a custom event
ecs_entity_t Synchronized = ecs_new(world);

// Create an observer that matches a custom event
ecs_observer(world, {
    .query.terms = {{ ecs_id(Position) }},
    .events = { Synchronized },
    .callback = OnSynchronizedPosition
});

ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

// Emit custom event
ecs_emit(world, &(ecs_event_desc_t) {
    .event = Synchronized,
    .entity = e,
    .ids = &(ecs_type_t){
        .array = (ecs_id_t[]){ ecs_id(Position) }, 
        .count = 1 
    }
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Create a custom event
struct Synchronized { };

// Alternatively, an plain entity could also be used as event
// flecs::entity Synchronized = world.entity();

// Create an observer that matches a custom event
world.observer<Position>()
    .event<Synchronized>()
    .each([](flecs::entity e, Position& p) {
        // ...
    });

flecs::entity e = world.entity().set(Position{10, 20});

// Emit custom event
world.emit<Synchronized>()
    .id<Position>()
    .entity(e)
    .emit();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Create a custom event
public struct Synchronized;

// Alternatively, an plain entity could also be used as event
// Entity Synchronized = world.Entity();

// Create an observer that matches a custom event
world.Observer<Position>()
    .Event<Synchronized>()
    .Each((Iter it, int i, ref Position p) =>
    {
        // ...
    });

Entity e = world.Entity().Set(new Position(10, 20));

// Emit custom event
world.Emit<Synchronized>()
    .Id<Position>()
    .Entity(e)
    .Emit();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Create a custom event
#[derive(Component)]
struct Synchronized;

// Alternatively, an plain entity could also be used as event
// let Synchronized = world.entity();

// Create an observer that matches a custom event
world
    .observer::<Synchronized, &Position>()
    .each_entity(|e, p| {
// ...
});

let e = world.entity().set(Position { x: 10.0, y: 20.0 });

// Emit custom event
world
    .event()
    .add::<Position>()
    .entity(e)
    .emit(&Synchronized);
```

</li>
</ul>
</div>

### Entity Observers
In many cases an application may want to emit an event for a specific entity without also specifying a component. This is enabled by entity observers. Entity observers are regular observers with the `Any` wildcard specified as the component, essentially expressing that an observer is interested in the source and the event but not in the component.

The following code shows an example of how to use entity observers:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create a custom event
ecs_entity_t Clicked = ecs_new(world);

// Create entity
ecs_entity_t widget = ecs_entity(world, { .name = "widget" });

// Create an entity observer
ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = widget }}
    .events = { Clicked },
    .callback = OnClick
});

// Emit entity event. Note how no component ids are provided.
ecs_emit(world, &(ecs_event_desc_t) {
    .event = Clicked,
    .entity = widget
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Create a custom event
struct Clicked { };

// Create entity
flecs::entity widget = world.entity("widget");

// Create an entity observer
widget.observe<Clicked>([]() {
    // ...
});

// Emit entity event
widget.emit<Clicked>();
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Create a custom event
public struct Clicked;

// Create entity
Entity widget = world.Entity("widget");

// Create an entity observer
widget.Observe<Click>(() =>
{
    // ...
});

widget.Emit<Clicked>();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Create a custom event
#[derive(Component)]
struct Clicked;

// Create entity
let widget = world.entity_named("widget");

// Create an entity observer
widget.observe::<Clicked>(|| {
    // ...
});

// Emit entity event
widget.emit(&Clicked);
```

</li>
</ul>
</div>

Events can be components, which makes it possible to add event-specific data:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
void OnResize(ecs_iter_t *it) {
    Resize *p = it->param; // Obtain event data from it->param member
    // ...
}

// Register a component that will be used as event
ECS_COMPONENT(world, Resize);

// Create entity
ecs_entity_t widget = ecs_entity(world, { .name = "widget" });

// Create an entity observer
ecs_observer(world, {
    // Not interested in any specific component
    .query.terms = {{ EcsAny, .src.id = widget }}
    .events = { ecs_id(Resize) },
    .callback = OnResize
});

// Emit entity event.
ecs_emit(world, &(ecs_event_desc_t) {
    .event = ecs_id(Resize),
    .entity = widget,
    .param = &(Resize){100, 200}
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Create a custom event
struct Resize { };

// Create entity
flecs::entity widget = world.entity("widget");

// Create an entity observer
widget.observe<Resize>([](Resize& r) {
    // ...
});

// Emit entity event
widget.emit<Resize>({100, 200});
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Create a custom event
public record struct Resize(double Width, double Height);

// Create entity
Entity widget = world.Entity("widget");

// Create an entity observer
widget.Observe<Resize>((ref Resize r) =>
{
    // ...
});

widget.Emit<Resize>(new(100, 200));
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Create a custom event
#[derive(Component)]
struct Resize {
    width: u32,
    height: u32,
}

// Create entity
let widget = world.entity_named("widget");

// Create an entity observer
widget.observe_payload::<&Resize>(|r| {
// ...
});

// Emit entity event
widget.emit(&Resize {
width: 100,
height: 200,
});
```

</li>
</ul>
</div>

### Enqueue vs. Emit
Events can be emitted with either the `emit` or `enqueue` operation. The `emit` operation invokes observers directly, whereas `enqueue` will enqueue the event in the command queue if the world is in deferred mode. When the world is not in deferred mode, `enqueue` defaults to the behavior of `emit`.

When `enqueue` adds an event to the command queue, the event data is copied in, meaning that the application does not need to keep the event data alive. This is done using the regular `copy` hook that can be registered using `ecs_set_hooks`. In C++ the copy assignment operator is used. Note that as a result, for an event with data to be enqueued in C++, the type has to be copyable. If no copy hook is registered, the behavior defaults to a `memcpy`.

## Observer Execution
Observers are always executed when the operation that triggered the observer happens, on the thread where the operation is executed. This means that when a component is added to an entity, all `OnAdd` observers will have been invoked by the time the operation is executed. When operations are deferred, because observers are always executed when the operation is executed, invoking the observer will also be delayed. In practice this often means that since most operations are deferred, most observers will also be invoked during sync points.

An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_observer(world, {
    .query.terms = {{ ecs_id(Position) }},
    .events = { EcsOnSet },
    .callback = OnSetPosition
});

// Observer is invoked as part of operation
ecs_set(world, e, Position, {10, 20});

ecs_defer_begin(world);
ecs_add(world, e, Position, {30, 40});
// Operation is delayed until here, observer is also invoked here
ecs_defer_end(world);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.observer<Position>()
    .event(flecs::OnSet)
    .each([](flecs::entity e, Position& p) {
        // ...
    });

// Observer is invoked as part of operation
e.set(Position{10, 20});

world.defer_begin();
e.set(Position{20, 30});
// Operation is delayed until here, observer is also invoked here
world.defer_end();
```

</li>
<li><b class="tab-title">C#</b>

```cs
world.Observer<Position>()
    .Event(Ecs.OnSet)
    .Each((Iter it, int i, ref Position p) =>
    {
        // ...
    });

// Observer is invoked as part of operation
e.Set(new Position(10, 20));

world.DeferBegin();
e.Set(new Position(20, 30));
// Operation is delayed until here, observer is also invoked here
world.DeferEnd();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .observer::<flecs::OnSet, &Position>()
    .each_entity(|e, p| {
        // ...
    });

// Observer is invoked as part of operation
e.set(Position { x: 10.0, y: 20.0 });

world.defer_begin();
e.set(Position { x: 20.0, y: 30.0 });
// Operation is delayed until here, observer is also invoked here
world.defer_end();
```

</li>
</ul>
</div>

### Observer disabling
Just like systems, observers can be disabled which prevents them from being invoked. Additionally, when the module in which an observer is stored is disabled, all observers are disabled as well. The same happens for systems (when using the default pipeline). This makes it easy to disable all logic in a module with a single operation.

### Observer Ordering
When observers are invoked, there are a few things to keep in mind when considering the order in which things happen:

#### Observer order is undefined
When two observers match the same event, the order in which they are executed is undefined. Applications should never rely on observer order, not even if the observed order is apparently "correct" for the application logic. The order in which observers, while deterministic, depends on many different things, and it is easy to break the order.

#### Event order is undefined between entities
No assumptions should be made about the order in which events are emitted for different entities. This allows the implementation to batch commands for a single entity together, which can greatly improve efficiency.

#### OnAdd & OnRemove order is undefined
OnAdd and OnRemove observers may be triggered in an order that is different from the order in which the events were emitted, even within the same entity. This is also done to allow the implementation to batch commands.

#### OnSet order is maintained
The order in which OnSet events are delivered is the same as in which they were emitted. This allows applications to make assumptions about the (component) state of other entities in the observer code and makes it easier to write code that is agnostic to whether operations are deferred or executed immediately.

#### Custom event order is maintained
Just like for OnSet events, the ordering for custom events is maintained.

#### Hooks and Events
Hooks always have a well defined order with respect to events:

- `on_add` hooks are invoked before `OnAdd` events
- `on_set` hooks are invoked before `OnSet` events
- `on_remove` hooks are invoked after `OnRemove` events.

#### Children are cleaned up before parents
When a parent and its children are deleted, `OnRemove` observers will be invoked for children first, under the condition that there are no cycles in the relationship graph of the deleted entities. This order is maintained for any relationship that has the `(OnDeleteTarget, Delete)` trait (see the Component Traits manual for more details).

When an entity graph contains cycles, order is undefined. This includes cycles that can be formed using different relationships.
