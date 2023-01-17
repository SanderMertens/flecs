# Systems
Systems are queries + a function that can be ran manually or get scheduled as part of a pipeline. To use systems, applications must build Flecs with the `FLECS_SYSTEM` addon (enabled by default).

An example of a simple system:

```c
// System implementation
void Move(ecs_iter_t *it) {
    // Get fields from system query
    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);

    // Iterate matched entities
    for (int i = 0; i < it->count, i++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

// System declaration
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, [in] Velocity);
```
```cpp
// System declaration
flecs::system sys = world.system<Position, const Velocity>("Move")
    .each([](Position& p, const Velocity &v) {
        // Each is invoked for each entity
        p.x += v.x;
        p.y += v.y;
    });
```

In C, a system can also be created with the `ecs_system_init` function / `ecs_system` shorthand which provides more flexibility. The same system can be created like this:

```c
ecs_entity_t ecs_id(Move) = ecs_system(world, {
    .entity = ecs_entity(world, { /* ecs_entity_desc_t */
        .name = "Move",
        .add = { ecs_dependson(EcsOnUpdate) }
    }),
    .query.filter.terms = { /* ecs_filter_desc_t::terms */
        { ecs_id(Position) },
        { ecs_id(Velocity), .inout = EcsIn }
    }
    .callback = Move
})
```

To manually run a system, do:

```c
ecs_run(world, ecs_id(Move), 0.0 /* delta_time */, NULL /* param */)
```
```cpp
sys.run();
```

By default systems are registered for a pipeline which orders systems by their "phase" (`EcsOnUpdate`). To run all systems in a pipeline, do:

```c
ecs_progress(world, 0 /* delta_time */);
```
```cpp
world.progress();
```

To run systems as part of a pipeline, applications must build Flecs with the `FLECS_PIPELINE` addon (enabled by default). To prevent a system from being registered as part of a pipeline, specify 0 as phase:

```c
ECS_SYSTEM(world, Move, 0, Position, [in] Velocity);
```
```cpp
flecs::system sys = world.system<Position, const Velocity>("Move")
    .kind(0)
    .each([](Position& p, const Velocity &v) { /* ... */ });
```

## System Iteration
Because systems use queries, the iterating code looks similar:

```c
// Query iteration
ecs_iter_t it = ecs_query_iter(world, q);

// Iterate tables matched by query
while (ecs_query_next(&it)) {
    // Get fields from query
    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);

    // Iterate matched entities
    for (int i = 0; i < it->count, i++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

// System code
void Move(ecs_iter_t *it) {
    // Get fields from system query
    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);

    // Iterate matched entities
    for (int i = 0; i < it->count, i++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}
```

Note how query iteration has an outer and an inner loop, whereas system iteration only has the inner loop. The outer loop for systems is iterated by the `ecs_run` function, which invokes the system function. When running a pipeline, this means that a system callback can be invoked multiple times per frame, once for each matched table.

In C++ system and query iteration uses the same `each`/`iter` functions:

```cpp
// Query iteration (each)
q.each([](Position& p, const Velocity &v) { /* ... */ });

// System iteration (each)
world.system<Position, const Velocity>("Move")
  .each([](Position& p, const Velocity &v) { /* ... */ });
```
```cpp
// Query iteration (iter)
q.iter([](flecs::iter& it, Position *p, const Velocity *v) {
    for (auto i : it) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
});

// System iteration (iter)
world.system<Position, const Velocity>("Move")
  .iter([](flecs::iter& it, Position *p, const Velocity *v) {
    for (auto i : it) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
  });
```

Just like in the C API, the `iter` function can be invoked multiple times per frame, once for each matched table. The `each` function is called once per matched entity.

Note that there is no significant performance difference between `iter` and `each`, which can both be vectorized by the compiler. By default `each` can actually end up being faster, as it is instanced (see query manual).

## Using delta_time
A system provides a `delta_time` which contains the time passed since the last frame:

```c
Position *p = ecs_field(it, Position, 1);
Velocity *v = ecs_field(it, Velocity, 2);

for (int i = 0; i < it->count, i++) {
    p[i].x += v[i].x * it->delta_time;
    p[i].y += v[i].y * it->delta_time;
}
```
```cpp
// delta_time() is available on flecs::entity
world.system<Position, const Velocity>("Move")
    .each([](flecs::entity e, Position& p, const Velocity &v) {
        // Each is invoked for each entity
        p.x += v.x * e.delta_time();
        p.y += v.y * e.delta_time();
    });

// delta_time() is available on flecs::iter
world.system<Position, const Velocity>("Move")
    .iter([](flecs::iter& it, Position *p, const Velocity *v) {
        // Each is invoked for each entity
        for (auto i : it) {
            p[i].x += v[i].x * it.delta_time();
            p[i].y += v[i].y * it.delta_time();
        }
    });
```

This is the value passed into `ecs_progress`:

```c
ecs_progress(world, delta_time);
```
```cpp
world.progress(delta_time);
```

Passing a value for `delta_time` is useful if you're running `progress()` from within an existing game loop that already has time management. Providing 0 for the argument, or omitting it in the C++ API will cause `progress()` to measure the time since the last call and use that as value for `delta_time`:

```c
ecs_progress(world, 0);
```
```cpp
world.progress();
```

A system may also use `delta_system_time`, which is the time elapsed since the last time the system was invoked. This can be useful when a system is not invoked each frame, for example when using a timer.

## Pipelines
A pipeline is a list of systems that is executed when the `ecs_progress`/`world::progress` function is invoked. Which systems are part of the pipeline is determined by a pipeline query. A pipeline query is a regular ECS query, which matches system entities. Flecs has a builtin pipeline with a predefined query, in addition to offering the ability to specify a custom pipeline query.

A pipeline by default orders systems by their entity id, to ensure deterministic order. This generally means that systems will be ran in the order they are declared, as entity ids are monotonically increasing. Note that this is not guaranteed: when an application deletes entities before creating a system, the system can receive a recycled id, which means it could be lower than the last issued id. For this reason it is recommended to prevent entity deletion while registering systems. When this can't be avoided, an application can create a custom pipeline with a user-defined `order_by` function (see custom pipeline).

Pipelines may utilize additional query mechanisms for ordering, such as `cascade` or `group_by`.

In addition to a system query, pipelines also analyze the components that systems are reading and writing to determine where to insert sync points. During a sync point enqueued commands are ran, which ensures that systems after a sync point can see all mutations from before a sync point.

### Builtin Pipeline
The builtin pipeline matches systems that depend on a _phase_. A phase is any entity with the `EcsPhase`/`flecs::Phase` tag. To add a dependency on a phase, the `DependsOn` relationship is used. This happens automatically when using the `ECS_SYSTEM` macro/`flecs::system::kind` method:

```c
// System is created with (DependsOn, OnUpdate)
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
```
```cpp
// System is created with (DependsOn, OnUpdate)
world.system<Position, Velocity>("Move")
  .kind(flecs::OnUpdate)
  .each([](Position& p, Velocity& v) {
    // ...
  });
```

Systems are ordered using a topology sort on the `DependsOn` relationship. Systems higher up in the topology are ran first. In the following example the order of systems is `InputSystem, MoveSystem, CollisionSystem`:

```
      PreUpdate
      /       \
 InputSystem  OnUpdate
               /     \
        MoveSystem  PostUpdate
                     /
                CollisionSystem
```

Flecs has the following builtin phases, listed in topology order:

- `EcsOnStart`
- `EcsOnLoad`
- `EcsPostLoad`
- `EcsPreUpdate`
- `EcsOnUpdate`
- `EcsOnValidate`
- `EcsPostUpdate`
- `EcsPreStore`
- `EcsOnStore`

In C++ the phase identifiers are namespaced:

- `flecs::OnStart`
- `flecs::OnLoad`
- `flecs::PostLoad`
- `flecs::PreUpdate`
- `flecs::OnUpdate`
- `flecs::OnValidate`
- `flecs::PostUpdate`
- `flecs::PreStore`
- `flecs::OnStore`

The `EcsOnStart`/`flecs::OnStart` phase is a special phase that is only ran the first time `progress()` is called.

An application can create custom phases, which can be (but don't need to be) branched off of existing ones:

```c
// Phases must have the EcsPhase tag
ecs_entity_t Physics = ecs_new_w_id(ecs, EcsPhase);
ecs_entity_t Collisions = ecs_new_w_id(ecs, EcsPhase);

// Phases can (but don't have to) depend on other phases which forces ordering
ecs_add_pair(ecs, Physics, EcsDependsOn, EcsOnUpdate);
ecs_add_pair(ecs, Collisions, EcsDependsOn, Physics);

// Custom phases can be used just like regular phases
ECS_SYSTEM(world, Collide, Collisions, Position, Velocity);
```

#### Builtin Pipeline Query
The builtin pipeline query looks like this:

Query DSL:
```
flecs.system.System, Phase(cascade(DependsOn)), !Disabled(up(DependsOn)), !Disabled(up(ChildOf))
```

C descriptor API:
```c
ecs_pipeline_init(world, &(ecs_pipeline_desc_t){
    .query.filter.terms = {
        { .id = ecs_id(EcsSystem) },
        { .id = EcsPhase, .src.flags = EcsCascade, .src.trav = EcsDependsOn },
        { .id = EcsDisabled, .src.flags = EcsUp, .src.trav = EcsDependsOn, .oper = EcsNot },
        { .id = EcsDisabled, .src.flags = EcsUp, .src.trav = EcsChildOf, .oper = EcsNot }
    }
});
```

C++ builder API:
```c
world.pipeline()
  .with(flecs::System)
  .with(flecs::Phase).cascade(flecs::DependsOn)
  .without(flecs::Disabled).up(flecs::DependsOn)
  .without(flecs::Disabled).up(flecs::ChildOf)
  .build();
```

### Custom pipeline
Applications can create their own pipelines which fully customize which systems are matched, and in which order they are executed. Custom pipelines can use phases and `DependsOn`, or they may use a completely different approach. This example shows how to create a pipeline that matches all systems with the `Foo` tag:

```c
ECS_TAG(world, Foo);

// Create custom pipeline
ecs_entity_t pipeline = ecs_pipeline_init(world, &(ecs_pipeline_desc_t){
    .query.filter.terms = {
        { .id = ecs_id(EcsSystem) }, // mandatory
        { .id = Foo }
    }
});

// Configure the world to use the custom pipeline
ecs_set_pipeline(world, pipeline);

// Create system
ECS_SYSTEM(world, Move, Foo, Position, Velocity);

// Runs the pipeline & system
ecs_progress(world, 0);
```
```cpp
// Create custom pipeline
flecs::entity pipeline = world.pipeline()
  .with(flecs::System)
  .with(Foo) // or .with<Foo>() if a type
  .build();

// Configure the world to use the custom pipeline
world.set_pipeline(pipeline);

// Create system
auto move = world.system<Position, Velocity>("Move")
  .kind(Foo) // or .kind<Foo>() if a type
  .each(...);

// Runs the pipeline & system
world.progress();
```

Note that `ECS_SYSTEM` kind parameter/`flecs::system::kind` add the provided entity both by itself as well as with a `DependsOn` relationship. As a result, the above `Move` system ends up with both:

- `Foo`
- `(DependsOn, Foo)`

This allows applications to still use the macro/builder API with custom pipelines, even if the custom pipeline does not use the `DependsOn` relationship. To avoid adding the `DependsOn` relationship, `0` can be passed to `ECS_SYSTEM` or `flecs::system::kind` followed by adding the tag manually:

```c
ecs_add(world, Move, Foo);
```
```cpp
move.add(Foo);
```

### Disabling systems
Because pipelines use regular ECS queries, adding the `EcsDisabled`/`flecs::Disabled` tag to a system entity will exclude the system from the pipeline. An application can use the `ecs_enable` function or `entity::enable`/`entity::disable` methods to enable/disable a system:

```c
// Disable system in C
ecs_enable(world, Move, false);
// Enable system in C
ecs_enable(world, Move, true);
```
```cpp
// Disable system in C++
s.disable();
// Enable system in C++
s.enable();
```

Additionally the `EcsDisabled`/`flecs::Disabled` tag can be added/removed directly:

```c
ecs_add_id(world, Move, EcsDisabled);
```
```c
s.add(flecs::Disabled);
```

Note that this applies both to builtin pipelines and custom pipelines, as entiites with the `Disabled` tag are ignored by default by queries.

Phases can also be disabled when using the builtin pipeline, which excludes all systems that depend on the phase. Note that is transitive, if `PhaseB` depends on `PhaseA` and `PhaseA` is disabled, systems that depend on both `PhaseA` and `PhaseB` will be excluded from the pipeline. For this reason, the builtin phases don't directly depend on each other, so that disabling `EcsOnUpdate` does not exclude systems that depend on `EcsPostUpdate`.

When the parent of a system is disabled, it will also be excluded from the builin pipeline. This makes it possible to disable all systems in a module with a single operation.

## Staging
When calling `progress()` the world enters a readonly state in which all ECS operations like `add`, `remove`, `set` etc. are enqueued as commands (called "staging"). This makes sure that it is safe for systems to iterate component arrays while enqueueing operations. Without staging, component storage arrays could be reallocated to a different memory location, which could cause system code to crash. Additionally, enqueueing operations makes it safe for multiple threads to iterate the same world without taking locks as thread gets its own command queue.

In general the framework tries its best to make sure that running code inside a system doesn't have different results than running it outside of a system, but because operations are enqueued as commands, this is not always the case. For example, the following code would return true outside of a system, but false inside of a system:

```cpp
if (!e.has<Tag>()) {
    e.add<Tag>();
    return e.has<Tag>();
}
```

Note that commands are only enqueued for ECS operations like `add`, `remove`, `set` etc. Reading or writing a queried for component directly does not enqueue commands. As a rule of thumb, anything that does not require calling an ECS function/method does not enqueue a command.

There are a number of things applications can do to force merging of operations, or to prevent operations from being enqueued as commands. To decide which mechanism to use, an application has to decide whether it needs:

1. Commands to be merged before another system
2. Operations not to be enqueued as commands.

The mechanisms to accomplish this are sync points for 1), and `no_readonly` systems for 2).

### Sync points
Sync points are moments during the frame where all commands are flushed to the storage. Systems that run after a sync point will be able to see all operations that happened up until the sync point. Sync points are inserted automatically by analyzing which commands could have been inserted and which components are being read by systems.

Because Flecs can't see inside the implementation of a system, pipelines can't know for which components a system could insert commands. This means that by default a pipeline assumes that systems insert no commands / that it is OK for commands to be merged at the end of the frame. To get commands to merge sooner, systems must be annotated with the components they write.

A pipeline tracks on a per-component basis whether commands could have been inserted for it, and when a component is being read. When a pipeline sees a read for a component for which commmands could have been inserted, a sync point is inserted before the system that reads. This ensures that sync points are only inserted when necessary:

- Multiple systems that enqueue commands can run before a sync point, possibly combining commands for multiple reads
- When a system is inactive (e.g. it doesn't match any entities) or is disabled, it will be ignored for sync point insertion
- Different combinations of modules have different sync requirements, automatic sync point insertion ensures that sync points are only inserted for the set of systems that are imported and are active.

To make the scheduler aware that a system can enqueue commands for a component, use the `out` modifier in combination with matching a component on an empty entity (`0`). This tells the scheduler that even though a system is not matching with the component, it is still "writing" it:

```c
// The '()' means, don't match this component on an entity, while `[out]` indicates 
// that the component is being written. This is interpreted by pipelines as a
// system that can potentially enqueue commands for the Transform component.
ECS_SYSTEM(world, SetTransform, EcsOnUpdate, Position, [out] Transform());
```
```c
// When using the descriptor API for creating the system, set the EcsIsEntity
// flag while leaving the id field to 0. This is equivalent to doing `()` in the DSL.
ecs_system(world, {
    .query.filter.terms = {
        { ecs_id(Position) },
        {
            .inout = EcsOut,
            .id = ecs_id(Transform), 
            .src.flags = EcsIsEntity, 
            .src.id = 0 /* Default value */
        }
    },
    /* ... */
});
```
```cpp
// In the C++ API, use the write method to indicate commands could be inserted.
world.system<Position>()
    .write<Transform>()
    .each( /* ... */);
```

This will cause insertion of a sync point before the next system that reads `Transform`. Similarly, a system can also be annotated with reading a component that it doesn't match with, which is useful when a system calls `get`:

```c
ECS_SYSTEM(world, SetTransform, EcsOnUpdate, Position, [in] Transform());
```
```c
ecs_system(world, {
    .query.filter.terms = {
        { ecs_id(Position) },
        {
            .inout = EcsIn,
            .id = ecs_id(Transform), 
            .src.flags = EcsIsEntity, 
            .src.id = 0 /* Default value */
        }
    },
    /* ... */
});
```
```cpp
// In the C++ API, use the read method to indicate a component is read using .get
world.system<Position>()
    .read<Transform>()
    .each( /* ... */);
```

### No readonly systems
By default systems are ran while the world is in "readonly" mode, where all ECS operations are enqueued as commands. Note that readonly mode only applies to "structural" changes, such as changing the components of an entity or other operations that mutate ECS data structures. Systems can still write component values while in readonly mode.

In some cases however, operations need to be immediately visible to a system. A typical example is a system that assigns tasks to resources, like assigning plates to a waiter. A system should only assign plates to a waiter that hasn't been assigned any plates yet, but to know which waiters are free, the operation that assigns a plate to a waiter must be immediately visible.

To accomplish this, systems can be marked with the `no_readonly` flag, which signals that a system should be ran while the world is not in readonly mode. This causes ECS operations to not get enqueued, and allows the system to directly see the results of operations. There are a few limitations to `no_readonly` systems:

- `no_readonly` systems are always single threaded
- operations on the iterated over entity must still be deferred

The reason for the latter limitation is that allowing for operations on the iterated over entity would cause the system to modify the storage it is iterating, which could cause undefined behavior similar to what you'd see when changing a vector while iterating it.

The following example shows how to create a `no_readonly` system:

```c
ecs_system(ecs, {
    .entity = ecs_entity(ecs, {
        .name = "AssignPlate",
        .add = { ecs_dependson(EcsOnUpdate) }
    }),
    .query.filter.terms = {
        { .id = Plate },
    },
    .callback = AssignPlate,
    .no_readonly = true // disable readonly mode for this system
});
```
```cpp
    ecs.system("AssignPlate")
        .with<Plate>()
        .no_readonly() // disable readonly mode for this system
        .iter([&](flecs::iter& it) { /* ... */ })
```

This ensures the world is not in readonly mode when the system is ran. Operations are however still enqueued as commands, which ensures that the system can enqueue commands for the entity that is being iterated over. To prevent commands from being enqueued, a system needs to suspend and resume command enqueueing. This is an extra step, but makes it possible for a system to both enqueue commands for the iterated over entity, as well as do operations that are immediately visible. An example:

```c
void AssignPlate(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        // ECS operations ran here are visible after running the system
        ecs_defer_suspend(it->world);
        // ECS operations ran here are immediately visible
        ecs_defer_resume(it->world);
        // ECS operations ran here are visible after running the system
    }
}
```
```cpp
.iter([](flecs::iter& it) {
    for (auto i : it) {
        // ECS operations ran here are visible after running the system
        it.world().defer_suspend();
        // ECS operations ran here are immediately visible
        it.world().defer_resume();
        // ECS operations ran here are visible after running the system
    }
});
```

Note that `defer_suspend` and `defer_resume` may only be called from within a `no_readonly` system.

### Threading
Systems in Flecs can be multithreaded. This requires both the system to be created as a multithreaded system, as well as configuring the world to have a number of worker threads. To create worker threads, use the `set_threads` function:

```c
ecs_set_threads(world, 4); // Create 4 worker threads
```
```cpp
world.set_threads(4);
```

To create a multithreaded system, use the `multi_threaded` flag:

```c
ecs_system(ecs, {
    .entity = ecs_entity(ecs, {
        .add = { ecs_dependson(EcsOnUpdate) }
    }),
    .query.filter.terms = {
        { .id = ecs_id(Position) }
    },
    .callback = Dummy,
    .multi_threaded = true // run system on multiple threads
});
```
```cpp
world.system<Position>()
  .multi_threaded()
  .each( /* ... */ );
```

By default systems are created as single threaded. Single threaded systems are always ran on the main thread. Multithreaded systems are ran on all worker threads. The scheduler runs each multithreaded system on all threads, and divides the number of matched entities across the threads. The way entities are allocated to threads ensures that the same entity is always processed by the same thread, until the next sync point. This means that in an ideal case, all systems in a frame can run until completion without having to synchronize.

The way the scheduler ensures that the same entities are processed by the same threads is by slicing up the entities in a table into N slices, where N is the number of threads. For a table that has 1000 entities, the first thread will process entities 0..249, thread 2 250..499, thread 3 500..749 and thread 4 entities 750..999. For more details on this behavior, see `ecs_worker_iter`/`flecs::iterable::worker_iter`.

## Timers
When running a pipeline, systems are ran each time `progress()` is called. The `FLECS_TIMER` addon makes it possible to run systems at a specific time interval or rate.

### Interval
The following example shows how to run systems at a time interval:

```c
// Using the ECS_SYSTEM macro
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, [in] Velocity);
ecs_set_interval(world, ecs_id(Move), 1.0); // Run at 1Hz
```
```c
// Using the ecs_system_init function/ecs_system macro:
ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "Move",
        .add = { ecs_dependson(EcsOnUpdate) }
    }),
    .query.filter.terms = {
        { ecs_id(Position) }, 
        { ecs_id(Velocity), .inout = EcsIn }
    },
    .callback = Move,
    .interval = 1.0 // Run at 1Hz
});
```
```cpp
world.system<Position, const Velocity>()
    .interval(1.0) // Run at 1Hz
    .each(...);
```

### Rate
The following example shows how to run systems every Nth tick with a rate filter:

```c
// Using the ECS_SYSTEM macro
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, [in] Velocity);
ecs_set_rate(world, ecs_id(Move), 2); // Run every other frame
```
```c
// Using the ecs_system_init function/ecs_system macro:
ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "Move",
        .add = { ecs_dependson(EcsOnUpdate) }
    }),
    .query.filter.terms = {
        { ecs_id(Position) }, 
        { ecs_id(Velocity), .inout = EcsIn }
    },
    .callback = Move,
    .rate = 2.0 // Run every other frame
});
```
```cpp
world.system<Position, const Velocity>()
    .rate(2) // Run every other frame
    .each(...);
```

### Tick source
Instead of setting the interval or rate directly on the system, an application may also create a separate entity that holds the time or rate filter, and use that as a "tick source" for a system. This makes it easier to use the same settings across groups of systems:

```c
// Using the ECS_SYSTEM macro
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, [in] Velocity);

// Passing 0 to entity argument will create a new entity. Similarly a rate 
// filter entity could be created with ecs_set_rate(world, 0, 2)
ecs_entity_t tick_source = ecs_set_interval(world, 0, 1.0);

// Set tick source for system
ecs_set_tick_source(world, ecs_id(Move), tick_source);
```
```c
// Passing 0 to entity argument will create a new entity. Similarly a rate 
// filter entity could be created with ecs_set_rate(world, 0, 2)
ecs_entity_t tick_source = ecs_set_interval(world, 0, 1.0);

// Using the ecs_system_init function/ecs_system macro:
ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "Move",
        .add = { ecs_dependson(EcsOnUpdate) }
    }),
    .query.filter.terms = {
        { ecs_id(Position) }, 
        { ecs_id(Velocity), .inout = EcsIn }
    },
    .callback = Move,
    .tick_source = tick_source // Set tick source for system
});
```
```cpp
// A rate filter can be created with .rate(2)
flecs::entity tick_source = world.timer()
    .interval(1.0);

world.system<Position, const Velocity>()
    .tick_source(tick_source) // Set tick source for system
    .each(...);
```

Interval filters can be paused and resumed:
```c
// Pause timer
ecs_stop_timer(world, tick_source);

// Resume timer
ecs_start_timer(world, tick_source);
```
```cpp
// Pause timer
tick_source.stop();

// Resume timer
tick_source.start();
```

An additional advantage of using shared interval/rate filter between systems is that it guarantees that systems are ran at the same tick. When a system is disabled, its interval/rate filters aren't updated, which means that when the system is reenabled again it would be out of sync with other systems that had the same interval/rate. specified. When using a shared tick source however the system is guaranteed to run at the same tick as other systems with the same tick source, even after the system is reenabled.

### Nested tick sources
One tick source can be used as the input of another (rate) tick source. The rate tick source will run at each Nth tick of the input tick source. This can be used to create nested tick sources, like in the following example:

```c
// Tick at 1Hz
ecs_entity_t each_second = ecs_set_interval(world, 0, 1.0);

// Tick each minute
ecs_entity_t each_minute = ecs_set_rate(world, 0, 60);
ecs_set_tick_source(world, each_minute, each_second);

// Tick each hour
ecs_entity_t each_hour = ecs_set_rate(world, 0, 60);
ecs_set_tick_source(world, each_hour, each_minute);
```
```cpp
// Tick at 1Hz
flecs::entity each_second = world.timer()
    .interval(1.0);

// Tick each minute
flecs::entity each_minute = world.timer()
    .rate(60, each_second);

// Tick each hour
flecs::entity each_hour = world.timer()
    .rate(60, each_minute);
```

Systems can also act as each others tick source:

```c
// Tick at 1Hz
ecs_entity_t each_second = ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "EachSecond",
        .add = { ecs_dependson(EcsOnUpdate) }
    }),
    .callback = Dummy,
    .interval = 1.0
});

// Tick each minute
ecs_entity_t each_minute = ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "EachMinute",
        .add = { ecs_dependson(EcsOnUpdate) }
    }),
    .callback = Dummy,
    .tick_source = each_second,
    .rate = 60
});

// Tick each hour
ecs_entity_t each_hour = ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "EachHour",
        .add = { ecs_dependson(EcsOnUpdate) }
    }),
    .callback = Dummy,
    .tick_source = each_minute,
    .rate = 60
});
```
```cpp
// Tick at 1Hz
flecs::entity each_second = world.system("EachSecond")
    .interval(1.0)
    .iter([](flecs::iter& it) { /* ... */ });

// Tick each minute
flecs::entity each_minute = world.system("EachMinute")
    .tick_source(each_second)
    .rate(60)
    .iter([](flecs::iter& it) { /* ... */ });

// Tick each hour
flecs::entity each_hour = world.system("EachHour")
    .tick_source(each_minute)
    .rate(60)
    .iter([](flecs::iter& it) { /* ... */ });
```
