# Systems
Systems are queries + a function that can be ran manually or get scheduled as part of a pipeline. To use systems, applications must build Flecs with the `FLECS_SYSTEM` addon (enabled by default).

An example of a simple system:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// System implementation
void Move(ecs_iter_t *it) {
    // Get fields from system query
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    // Iterate matched entities
    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

// System declaration
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, [in] Velocity);
```

In C, a system can also be created with the `ecs_system_init` function / `ecs_system` shorthand which provides more flexibility. The same system can be created like this:

```c
ecs_entity_t ecs_id(Move) = ecs_system(world, {
    .entity = ecs_entity(world, { /* ecs_entity_desc_t */
        .name = "Move",
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .query.terms = { /* ecs_query_desc_t::terms */
        { ecs_id(Position) },
        { ecs_id(Velocity), .inout = EcsIn }
    }
    .callback = Move
})
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// System declaration
flecs::system sys = world.system<Position, const Velocity>("Move")
    .each([](Position& p, const Velocity &v) {
        // Each is invoked for each entity
        p.x += v.x;
        p.y += v.y;
    });
```
</li>
<li><b class="tab-title">C#</b>

```cs
// System declaration
Routine sys = world.Routine<Position, Velocity>("Move")
    .Each((ref Position p, ref Velocity v) =>
    {
        // Each is invoked for each entity
        p.X += v.X;
        p.Y += v.Y;
    });
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// System declaration
world
    .system_named::<(&mut Position, &Velocity)>("Move")
    .each(|(p, v)| {
        p.x += v.x;
        p.y += v.y;
    });
```
</li>
</ul>
</div>

To manually run a system, do:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_run(world, ecs_id(Move), 0.0 /* delta_time */, NULL /* param */)
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::system sys = ...;
sys.run();
```
</li>
<li><b class="tab-title">C#</b>

```cs
Routine sys = ...;
sys.Run();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let sys = ...;
sys.run();
```
</li>
</ul>
</div>

By default systems are registered for a pipeline which orders systems by their "phase" (`EcsOnUpdate`). To run all systems in a pipeline, do:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_progress(world, 0 /* delta_time */);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::world world = ...;
world.progress();
```
</li>
<li><b class="tab-title">C#</b>

```cs
using World world = World.Create();
world.Progress();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let world = World::new();
world.progress();
```
</li>
</ul>
</div>

To run systems as part of a pipeline, applications must build Flecs with the `FLECS_PIPELINE` addon (enabled by default). To prevent a system from being registered as part of a pipeline, specify 0 as phase:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_SYSTEM(world, Move, 0, Position, [in] Velocity);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::system sys = world.system<Position, const Velocity>("Move")
    .kind(0)
    .each([](Position& p, const Velocity &v) { /* ... */ });
```
</li>
<li><b class="tab-title">C#</b>

```cs
Routine sys = world.Routine<Position, Velocity>("Move")
    .Kind(0)
    .Each((ref Position p, ref Velocity v) => { /* ... */ });
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .system_named::<(&mut Position, &Velocity)>("Move")
    .kind_id(0)
    .each(|(p, v)| { /* ... */ });
```
</li>
</ul>
</div>

## System Iteration
Because systems use queries, the iterating code looks similar:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Query iteration
ecs_iter_t it = ecs_query_iter(world, q);

// Iterate tables matched by query
while (ecs_query_next(&it)) {
    // Get fields from query
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    // Iterate matched entities
    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

// System code
void Move(ecs_iter_t *it) {
    // Get fields from system query
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    // Iterate matched entities
    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// Query iteration (each)
q.each([](Position& p, const Velocity &v) { /* ... */ });

// System iteration (each)
world.system<Position, const Velocity>("Move")
  .each([](Position& p, const Velocity &v) { /* ... */ });
```
```cpp
// Query iteration (run)
q.run([](flecs::iter& it) {
    while (it.next()) {
        auto p = it.field<Position>(0);
        auto v = it.field<const Velocity>(1);

        for (auto i : it) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
        }
    }
});

// System iteration (iter)
world.system<Position, const Velocity>("Move")
    .run([](flecs::iter& it) {
        while (it.next()) {
            auto p = it.field<Position>(0);
            auto v = it.field<const Velocity>(1);

            for (auto i : it) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
            }
        }
    });
```

The `run()` function can be invoked multiple times per frame, once for each matched table. The `each` function is called once per matched entity.

Note that there is no significant performance difference between `iter()` and `each`, which can both be vectorized by the compiler.

</li>
<li><b class="tab-title">C#</b>

```cs
// Query iteration (Each)
q.Each((ref Position p, ref Velocity v) => { /* ... */ });

// System iteration (Each)
world.Routine<Position, Velocity>("Move")
    .Each((ref Position p, ref Velocity v) => { /* ... */ });
```
```cs
// Query iteration (Iter)
q.Iter((Iter it, Field<Position> p, Field<Velocity> v) =>
{
    foreach (int i in it) 
    {
        p[i].X += v[i].X;
        p[i].Y += v[i].Y;
    }
});

// System iteration (Iter)
world.Routine<Position, Velocity>("Move")
    .Iter((Iter it, Field<Position> p, Field<Velocity> v) =>
    {
        foreach (int i in it)
        {
            p[i].X += v[i].X;
            p[i].Y += v[i].Y;
        }
    });
```

The `Iter` function can be invoked multiple times per frame, once for each matched table. The `Each` function is called once per matched entity.
</li>
<li><b class="tab-title">Rust</b>

```rust
// `.each_entity` if you need the associated entity.

// Query iteration (each)
q.each(|(p, v)| { /* ... */ });

// System iteration (each)
world
    .system_named::<(&mut Position, &Velocity)>("Move")
    .each(|(p, v)| { /* ... */ });
```
```rust
// Query iteration (run)
q.run(|mut it| {
    while it.next() {
        let mut p = it
            .field::<Position>(0)
            .expect("query term changed and not at the same index anymore");
        let v = it
            .field::<Velocity>(1)
            .expect("query term changed and not at the same index anymore");
        for i in it.iter() {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
        }
    }
});

// System iteration (run)
world
    .system_named::<(&mut Position, &Velocity)>("Move")
    .run(|mut it| {
        while it.next() {
            let mut p = it
                .field::<Position>(0)
                .expect("query term changed and not at the same index anymore");
            let v = it
                .field::<Velocity>(1)
                .expect("query term changed and not at the same index anymore");
            for i in it.iter() {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
            }
        }
    });
```
```rust
// Query iteration (run_iter)
q.run_iter(|it, (p, v)| {
    for i in it.iter() {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
});

// System iteration (run_iter)
world
    .system_named::<(&mut Position, &Velocity)>("Move")
    .run_iter(|it, (p, v)| {
        for i in it.iter() {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
        }
    });
```

The `run` function can be invoked multiple times per frame, once for each matched table. The `each` function is called once per matched entity.

Note that there is no significant performance difference between `run` and `each`, which can both be vectorized by the compiler.
</li>
</ul>
</div>

Note how query iteration has an outer and an inner loop, whereas system iteration only has the inner loop. The outer loop for systems is iterated by the `ecs_run` function, which invokes the system function. When running a pipeline, this means that a system callback can be invoked multiple times per frame, once for each matched table.

## Using delta_time
A system provides a `delta_time` which contains the time passed since the last frame:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
Position *p = ecs_field(it, Position, 0);
Velocity *v = ecs_field(it, Velocity, 1);

for (int i = 0; i < it->count; i++) {
    p[i].x += v[i].x * it->delta_time;
    p[i].y += v[i].y * it->delta_time;
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.system<Position, const Velocity>("Move")
    .each([](flecs::iter& it, size_t, Position& p, const Velocity &v) {
        p.x += v.x * it.delta_time();
        p.y += v.y * it.delta_time();
    });

world.system<Position, const Velocity>("Move")
    .run([](flecs::iter& it) {
        while (it.next()) {
            auto p = it.field<Position>(0);
            auto v = it.field<const Velocity>(1);

            for (auto i : it) {
                p[i].x += v[i].x * it.delta_time();
                p[i].y += v[i].y * it.delta_time();
            }
        }
    });
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Routine<Position, Velocity>("Move")
    .Each((Iter it, int i, ref Position p, ref Velocity v) =>
    {
        p.X += v.X * it.DeltaTime();
        p.Y += v.Y * it.DeltaTime();
    });

world.Routine<Position, Velocity>("Move")
    .Iter((Iter it, Field<Position> p, Field<Velocity> v) =>
    {
        foreach (int i in it)
        {
            p[i].X += v[i].X * it.DeltaTime();
            p[i].Y += v[i].Y * it.DeltaTime();
        }
    });
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .system_named::<(&mut Position, &Velocity)>("Move")
    .each_iter(|it, i, (p, v)| {
        p.x += v.x * it.delta_time();
        p.y += v.y * it.delta_time();
    });
    
world
    .system_named::<(&mut Position, &Velocity)>("Move")
    .run_iter(|it, (p, v)| {
        for i in it.iter() {
            p[i].x += v[i].x * it.delta_time();
            p[i].y += v[i].y * it.delta_time();
        }
    });
```
</li>
</ul>
</div>

This is the value passed into `ecs_progress`:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_progress(world, delta_time);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.progress(delta_time);
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Progress(deltaTime);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world.progress_time(delta_time);
```
</li>
</ul>
</div>

Passing a value for `delta_time` is useful if you're running `progress()` from within an existing game loop that already has time management. Providing 0 for the argument, or omitting it in the C++ API will cause `progress()` to measure the time since the last call and use that as value for `delta_time`:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_progress(world, 0);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.progress();
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Progress();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world.progress();
```
</li>
</ul>
</div>

A system may also use `delta_system_time`, which is the time elapsed since the last time the system was invoked. This can be useful when a system is not invoked each frame, for example when using a timer.

## Tasks
A task is a system that matches no entities. Tasks are ran once per frame, and are useful for running code that is not related to entities. An example of a task system:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// System function
void PrintTime(ecs_iter_t *it) {
    printf("Time: %f\n", it->delta_time);
}

// System declaration using the ECS_SYSTEM macro
ECS_SYSTEM(world, PrintTime, EcsOnUpdate, 0);

// System declaration using the descriptor API
ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "PrintTime",
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .callback = PrintTime
});

// Runs PrintTime
ecs_progress(world, 0);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.system("PrintTime")
    .kind(flecs::OnUpdate)
    .run([](flecs::iter& it) {
        printf("Time: %f\n", it.delta_time());
    });

// Runs PrintTime
world.progress();
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Routine("PrintTime")
    .Kind(Ecs.OnUpdate)
    .Iter((Iter it) =>
    {
        Console.WriteLine($"Time: {it.DeltaTime()}");
    });

// Runs PrintTime
world.progress();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world.system_named::<()>("PrintTime").run(|mut it| {
    while it.next() {
        println!("Time: {}", it.delta_time());
    }
});

// Runs PrintTime
world.progress();
```
</li>
</ul>
</div>

Tasks may query for components from a fixed source or singleton:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// System function
void PrintTime(ecs_iter_t *it) {
    // Get singleton component
    Game *g = ecs_field(it, Game, 0);

    printf("Time: %f\n", g->time);
}

// System declaration using the ECS_SYSTEM macro
ECS_SYSTEM(world, PrintTime, EcsOnUpdate, Game($));

// System declaration using the descriptor API
ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "PrintTime",
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .query.terms = {
        { .id = ecs_id(Game), .src.id = ecs_id(Game) } // Singleton source
    },
    .callback = PrintTime
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.system<Game>("PrintTime")
    .term_at(0).singleton()
    .kind(flecs::OnUpdate)
    .each([](Game& g) {
        printf("Time: %f\n", g.time);
    });
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Routine<Game>("PrintTime")
    .TermAt(1).Singleton()
    .Kind(Ecs.OnUpdate)
    .Each((ref Game g) =>
    {
        Console.WriteLine($"Time: {g.Time}");
    });
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .system_named::<&Game>("PrintTime")
    .term_at(0)
    .singleton()
    .kind::<flecs::pipeline::OnUpdate>()
    .run_iter(|it, game| {
        println!("Time: {}", game[0].time);
    });
```
</li>
</ul>
</div>

## Pipelines
A pipeline is a list of systems that is executed when the `ecs_progress`/`world::progress` function is invoked. Which systems are part of the pipeline is determined by a pipeline query. A pipeline query is a regular ECS query, which matches system entities. Flecs has a builtin pipeline with a predefined query, in addition to offering the ability to specify a custom pipeline query.

A pipeline by default orders systems by their entity id, to ensure deterministic order. This generally means that systems will be ran in the order they are declared, as entity ids are monotonically increasing. Note that this is not guaranteed: when an application deletes entities before creating a system, the system can receive a recycled id, which means it could be lower than the last issued id. For this reason it is recommended to prevent entity deletion while registering systems. When this can't be avoided, an application can create a custom pipeline with a user-defined `order_by` function (see custom pipeline).

Pipelines may utilize additional query mechanisms for ordering, such as `cascade` or `group_by`.

In addition to a system query, pipelines also analyze the components that systems are reading and writing to determine where to insert sync points. During a sync point enqueued commands are ran, which ensures that systems after a sync point can see all mutations from before a sync point.

### Builtin Pipeline
The builtin pipeline matches systems that depend on a _phase_. A phase is any entity with the `EcsPhase`/`flecs::Phase` tag. To add a dependency on a phase, the `DependsOn` relationship is used. This happens automatically when using the `ECS_SYSTEM` macro/`flecs::system::kind` method:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// System is created with (DependsOn, OnUpdate)
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// System is created with (DependsOn, OnUpdate)
world.system<Position, Velocity>("Move")
  .kind(flecs::OnUpdate)
  .each([](Position& p, Velocity& v) {
    // ...
  });
```
</li>
<li><b class="tab-title">C#</b>

```cs
// System is created with (DependsOn, OnUpdate)
world.Routine<Position, Velocity>("Move")
    .Kind(Ecs.OnUpdate)
    .Each((ref Position p, ref Velocity v) =>
    {
        // ...
    });
```
</li>
<li><b class="tab-title">Rust</b>

```rust
 // System is created with (DependsOn, OnUpdate)
 world
     .system_named::<(&mut Position, &Velocity)>("Move")
     .kind::<flecs::pipeline::OnUpdate>()
     .each(|(p, v)| {
         // ...
     });
```
</li>
</ul>
</div>

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
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

- `EcsOnStart`
- `EcsOnLoad`
- `EcsPostLoad`
- `EcsPreUpdate`
- `EcsOnUpdate`
- `EcsOnValidate`
- `EcsPostUpdate`
- `EcsPreStore`
- `EcsOnStore`
</li>
<li><b class="tab-title">C++</b>

- `flecs::OnStart`
- `flecs::OnLoad`
- `flecs::PostLoad`
- `flecs::PreUpdate`
- `flecs::OnUpdate`
- `flecs::OnValidate`
- `flecs::PostUpdate`
- `flecs::PreStore`
- `flecs::OnStore`
</li>
<li><b class="tab-title">C#</b>

- `Ecs.OnStart`
- `Ecs.OnLoad`
- `Ecs.PostLoad`
- `Ecs.PreUpdate`
- `Ecs.OnUpdate`
- `Ecs.OnValidate`
- `Ecs.PostUpdate`
- `Ecs.PreStore`
- `Ecs.OnStore`
</li>
<li><b class="tab-title">Rust</b>

- `flecs::pipeline::OnStart`
- `flecs::pipeline::OnLoad`
- `flecs::pipeline::PostLoad`
- `flecs::pipeline::PreUpdate`
- `flecs::pipeline::OnUpdate`
- `flecs::pipeline::OnValidate`
- `flecs::pipeline::PostUpdate`
- `flecs::pipeline::PreStore`
- `flecs::pipeline::OnStore`
</li>
</ul>
</div>

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

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_pipeline_init(world, &(ecs_pipeline_desc_t){
    .query.terms = {
        { .id = EcsSystem },
        { .id = EcsPhase, .src.flags = EcsCascade, .trav = EcsDependsOn },
        { .id = EcsDisabled, .src.flags = EcsUp, .trav = EcsDependsOn, .oper = EcsNot },
        { .id = EcsDisabled, .src.flags = EcsUp, .trav = EcsChildOf, .oper = EcsNot }
    }
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.pipeline()
  .with(flecs::System)
  .with(flecs::Phase).cascade(flecs::DependsOn)
  .without(flecs::Disabled).up(flecs::DependsOn)
  .without(flecs::Disabled).up(flecs::ChildOf)
  .build();
```
</li>
<li><b class="tab-title">Query DSL</b>

```
flecs.system.System, Phase(cascade(DependsOn)), !Disabled(up(DependsOn)), !Disabled(up(ChildOf))
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Pipeline()
    .With(Ecs.System)
    .With(Ecs.Phase).Cascade(Ecs.DependsOn)
    .Without(Ecs.Disabled).Up(Ecs.DependsOn)
    .Without(Ecs.Disabled).Up(Ecs.ChildOf)
    .Build();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .pipeline()
    .with::<flecs::system::System>()
    .with::<flecs::pipeline::Phase>()
    .cascade_type::<flecs::DependsOn>()
    .without::<flecs::Disabled>()
    .up_type::<flecs::DependsOn>()
    .without::<flecs::Disabled>()
    .up_type::<flecs::ChildOf>()
    .build();
```

</li>
</ul>
</div>

### Custom pipeline
Applications can create their own pipelines which fully customize which systems are matched, and in which order they are executed. Custom pipelines can use phases and `DependsOn`, or they may use a completely different approach. This example shows how to create a pipeline that matches all systems with the `Foo` tag:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_TAG(world, Foo);

// Create custom pipeline
ecs_entity_t pipeline = ecs_pipeline_init(world, &(ecs_pipeline_desc_t){
    .query.terms = {
        { .id = EcsSystem }, // mandatory
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
</li>
<li><b class="tab-title">C++</b>

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
</li>
<li><b class="tab-title">C#</b>

```cs
// Create custom pipeline
Pipeline pipeline = world.Pipeline()
    .With(Ecs.System)
    .With(foo) // or .With<Foo>() if a type
    .Build();

// Configure the world to use the custom pipeline
world.SetPipeline(pipeline);

// Create system
Routine move = world.Routine<Position, Velocity>("Move")
    .Kind(foo) // or .Kind<Foo>() if a type
    .Each(...);

// Runs the pipeline & system
world.Progress();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// Create custom pipeline
let pipeline = world
    .pipeline()
    .with::<flecs::system::System>()
    .with::<Foo>() // or `.with_id(foo) if an id`
    .build();

// Configure the world to use the custom pipeline
world.set_pipeline_id(pipeline);

// Create system
world
    .system_named::<(&mut Position, &Velocity)>("Move")
    .kind::<Foo>() // or `.kind_id(foo) if an id`
    .each(|(p, v)| {
        p.x += v.x;
        p.y += v.y;
    });
// Runs the pipeline & system
world.progress();
```
</li>
</ul>
</div>

Note that `ECS_SYSTEM` kind parameter/`flecs::system::kind` add the provided entity both by itself as well as with a `DependsOn` relationship. As a result, the above `Move` system ends up with both:

- `Foo`
- `(DependsOn, Foo)`

This allows applications to still use the macro/builder API with custom pipelines, even if the custom pipeline does not use the `DependsOn` relationship. To avoid adding the `DependsOn` relationship, `0` can be passed to `ECS_SYSTEM` or `flecs::system::kind` followed by adding the tag manually:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_add(world, Move, Foo);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
move.add(Foo);
```
</li>
<li><b class="tab-title">C#</b>

```cs
move.Entity.Add(foo);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
move_sys.add::<Foo>();
```
</ul>
</div>

#### Pipeline switching performance
When running a multithreaded application, switching pipelines can be an expensive operation. The reason for this is that it requires tearing down and recreating the worker threads with the new pipeline context. For this reason it can be more efficient to use queries that allow for enabling/disabling groups of systems vs. switching pipelines.

For example, the builtin pipeline excludes groups of systems from the schedule that:
- have the `Disabled` tag
- have a parent (module) with the `Disabled` tag
- depend on a phase with the `Disabled` tag

### Disabling systems
Because pipelines use regular ECS queries, adding the `EcsDisabled`/`flecs::Disabled` tag to a system entity will exclude the system from the pipeline. An application can use the `ecs_enable` function or `entity::enable`/`entity::disable` methods to enable/disable a system:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Disable system in C
ecs_enable(world, Move, false);
// Enable system in C
ecs_enable(world, Move, true);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// Disable system in C++
s.disable();
// Enable system in C++
s.enable();
```
</li>
<li><b class="tab-title">C#</b>

```cs
// Disable system in C#
s.Entity.Disable();
// Enable system in C#
s.Entity.Enable();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// Disable system
s.disable_self();
// Enable system
s.enable_self();
```
</li>
</ul>
</div>

Additionally the `EcsDisabled`/`flecs::Disabled` tag can be added/removed directly:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_add_id(world, Move, EcsDisabled);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
s.add(flecs::Disabled);
```
</li>
<li><b class="tab-title">C#</b>

```cs
s.Entity.Add(Ecs.Disabled);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
sys.add::<flecs::Disabled>();
```
</li>
</ul>
</div>

Note that this applies both to builtin pipelines and custom pipelines, as entities with the `Disabled` tag are ignored by default by queries.

Phases can also be disabled when using the builtin pipeline, which excludes all systems that depend on the phase. Note that is transitive, if `PhaseB` depends on `PhaseA` and `PhaseA` is disabled, systems that depend on both `PhaseA` and `PhaseB` will be excluded from the pipeline. For this reason, the builtin phases don't directly depend on each other, so that disabling `EcsOnUpdate` does not exclude systems that depend on `EcsPostUpdate`.

When the parent of a system is disabled, it will also be excluded from the builtin pipeline. This makes it possible to disable all systems in a module with a single operation.

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

The mechanisms to accomplish this are sync points for 1), and `immediate` systems for 2).

### Sync points
Sync points are moments during the frame where all commands are flushed to the storage. Systems that run after a sync point will be able to see all operations that happened up until the sync point. Sync points are inserted automatically by analyzing which commands could have been inserted and which components are being read by systems.

Because Flecs can't see inside the implementation of a system, pipelines can't know for which components a system could insert commands. This means that by default a pipeline assumes that systems insert no commands / that it is OK for commands to be merged at the end of the frame. To get commands to merge sooner, systems must be annotated with the components they write.

A pipeline tracks on a per-component basis whether commands could have been inserted for it, and when a component is being read. When a pipeline sees a read for a component for which commands could have been inserted, a sync point is inserted before the system that reads. This ensures that sync points are only inserted when necessary:

- Multiple systems that enqueue commands can run before a sync point, possibly combining commands for multiple reads
- When a system is inactive (e.g. it doesn't match any entities) or is disabled, it will be ignored for sync point insertion
- Different combinations of modules have different sync requirements, automatic sync point insertion ensures that sync points are only inserted for the set of systems that are imported and are active.

To make the scheduler aware that a system can enqueue commands for a component, use the `out` modifier in combination with matching a component on an empty entity (`0`). This tells the scheduler that even though a system is not matching with the component, it is still "writing" it:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

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
    .query.terms = {
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
</li>
<li><b class="tab-title">C++</b>

```cpp
// In the C++ API, use the write method to indicate commands could be inserted.
world.system<Position>()
    .write<Transform>()
    .each( /* ... */);
```
</li>
<li><b class="tab-title">C#</b>

```cs
// In the C# API, use the write method to indicate commands could be inserted.
world.Routine<Position>()
    .Write<Transform>()
    .Each( /* ... */);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// In the Rust API, use the write method to indicate commands could be inserted.
world.system::<&Position>().write::<Transform>().each(|p| {
    // ...
});
```
</li>
</ul>
</div>

This will cause insertion of a sync point before the next system that reads `Transform`. Similarly, a system can also be annotated with reading a component that it doesn't match with, which is useful when a system calls `get`:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_SYSTEM(world, SetTransform, EcsOnUpdate, Position, [in] Transform());
```
```c
ecs_system(world, {
    .query.terms = {
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
</li>
<li><b class="tab-title">C++</b>

```cpp
// In the C++ API, use the read method to indicate a component is read using .get
world.system<Position>()
    .read<Transform>()
    .each( /* ... */);
```
</li>
<li><b class="tab-title">C#</b>

```cs
// In the C# API, use the read method to indicate a component is read using .Get
world.Routine<Position>()
    .Read<Transform>()
    .Each( /* ... */);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// In the Rust API, use the read method to indicate a component is read using .get
world.system::<&Position>().read::<Transform>().each(|p| {
    // ...
});
```
</li>
</ul>
</div>

### Immediate systems
By default systems are ran while the world is in "readonly" mode, where all ECS operations are enqueued as commands. Readonly here means that structural changes, such as changing the components of an entity are deferred. Systems can still write component values while in readonly mode.

In some cases however, operations need to be immediately visible to a system. A typical example is a system that assigns tasks to resources, like assigning plates to a waiter. A system should only assign plates to a waiter that hasn't been assigned any plates yet, but to know which waiters are free, the operation that assigns a plate to a waiter must be immediately visible.

To accomplish this, systems can be marked with the `immediate` flag, which signals that a system should be ran while the world is not in readonly mode. This causes ECS operations to not get enqueued, and allows the system to directly see the results of operations. There are a few limitations to `immediate` systems:

- `immediate` systems are always single threaded
- operations on the iterated over entity must still be deferred

The reason for the latter limitation is that allowing for operations on the iterated over entity would cause the system to modify the storage it is iterating, which could cause undefined behavior similar to what you'd see when changing a vector while iterating it.

The following example shows how to create a `immediate` system:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_system(ecs, {
    .entity = ecs_entity(ecs, {
        .name = "AssignPlate",
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .query.terms = {
        { .id = Plate },
    },
    .callback = AssignPlate,
    .immediate = true // disable readonly mode for this system
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
    ecs.system("AssignPlate")
        .with<Plate>()
        .immediate() // disable readonly mode for this system
        .run([&](flecs::iter& it) { /* ... */ })
```
</li>
<li><b class="tab-title">C#</b>

```cs
ecs.Routine("AssignPlate")
    .With<Plate>()
    .NoReadonly() // disable readonly mode for this system
    .Iter((Iter it) => { /* ... */ })
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .system_named::<&Plate>("AssignPlate")
    .immediate(true) // disable readonly mode for this system
    .run(|mut it| {
        while it.next() {
            // ...
        }
    });
```
</li>
</ul>
</div>

This ensures the world is not in readonly mode when the system is ran. Operations are however still enqueued as commands, which ensures that the system can enqueue commands for the entity that is being iterated over. To prevent commands from being enqueued, a system needs to suspend and resume command enqueueing. This is an extra step, but makes it possible for a system to both enqueue commands for the iterated over entity, as well as do operations that are immediately visible. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

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
</li>
<li><b class="tab-title">C++</b>

```cpp
.run([](flecs::iter& it) {
    while (it.next()) {
        for (auto i : it) {
            // ECS operations ran here are visible after running the system
            it.world().defer_suspend();
            // ECS operations ran here are immediately visible
            it.world().defer_resume();
            // ECS operations ran here are visible after running the system
        }
    }
});
```
</li>
<li><b class="tab-title">C#</b>

```cs
.Iter((Iter it) =>
{
    foreach (int i in it)
    {
        // ECS operations ran here are visible after running the system
        it.World().DeferSuspend();
        // ECS operations ran here are immediately visible
        it.World().DeferResume();
        // ECS operations ran here are visible after running the system
    }
});
```
</li>
<li><b class="tab-title">Rust</b>

```rust
.run(|mut it| {
    while it.next() {
        // ECS operations ran here are visible after running the system
        it.world().defer_suspend();
        // ECS operations ran here are immediately visible
        it.world().defer_resume();
        // ECS operations ran here are visible after running the system
    }
});
```
</li>
</ul>
</div>

Note that `defer_suspend` and `defer_resume` may only be called from within a `immediate` system.

### Threading
Systems in Flecs can be multithreaded. This requires both the system to be created as a multithreaded system, as well as configuring the world to have a number of worker threads. To create worker threads, use the `set_threads` function:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_set_threads(world, 4); // Create 4 worker threads
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.set_threads(4);
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.SetThreads(4);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world.set_threads(4);
```
</li>
</ul>
</div>

To create a multithreaded system, use the `multi_threaded` flag:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_system(ecs, {
    .entity = ecs_entity(ecs, {
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .query.terms = {
        { .id = ecs_id(Position) }
    },
    .callback = Dummy,
    .multi_threaded = true // run system on multiple threads
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.system<Position>()
  .multi_threaded()
  .each( /* ... */ );
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Routine<Position>()
  .MultiThreaded()
  .Each( /* ... */ );
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world.system::<&Position>().multi_threaded().each(|p| {
    // ...
});
```
</li>
</ul>
</div>

By default systems are created as single threaded. Single threaded systems are always ran on the main thread. Multithreaded systems are ran on all worker threads. The scheduler runs each multithreaded system on all threads, and divides the number of matched entities across the threads. The way entities are allocated to threads ensures that the same entity is always processed by the same thread, until the next sync point. This means that in an ideal case, all systems in a frame can run until completion without having to synchronize.

The way the scheduler ensures that the same entities are processed by the same threads is by slicing up the entities in a table into N slices, where N is the number of threads. For a table that has 1000 entities, the first thread will process entities 0..249, thread 2 250..499, thread 3 500..749 and thread 4 entities 750..999. For more details on this behavior, see `ecs_worker_iter`/`flecs::iterable::worker_iter`.

### Threading with Async Tasks
Systems in Flecs can also be multithreaded using an external asynchronous task system. Instead of creating regular worker threads using `set_threads`, use the `set_task_threads` function and provide the OS API callbacks to create and wait for task completion using your job system.
This can be helpful when using Flecs within an application which already has a job queue system to handle multithreaded tasks.
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_set_task_threads(world, 4); // Create 4 worker task threads for the duration of each ecs_progress update
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.set_task_threads(4);
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.SetTaskThreads(4);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world.set_task_threads(4);
```
</li>
</ul>
</div>

For simplicity, these task callbacks use the same format as Flecs `ecs_os_api_t` thread APIs. In fact, you could provide your regular os thread api functions to create short-duration threads for multithreaded system processing.
Create multithreaded systems using the `multi_threaded` flag as with `ecs_set_threads` above.

When `ecs_progress` is called, your `ecs_os_api.task_new_` callback will be called once for every task thread needed to create task threads on demand. When `ecs_progress` is complete, your `ecs_os_api.task_join_` function will be called to clean up each task thread.
By providing callback functions which create and remove tasks for your specific asynchronous task system, you can use Flecs with any kind of async task management scheme. 
The only limitation is that your async task manager must be able to create and execute the number of simultaneous tasks specified in `ecs_set_task_threads` and must exist for the duration of `ecs_progress`.

## Timers
When running a pipeline, systems are ran each time `progress()` is called. The `FLECS_TIMER` addon makes it possible to run systems at a specific time interval or rate.

### Interval
The following example shows how to run systems at a time interval:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

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
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .query.terms = {
        { ecs_id(Position) }, 
        { ecs_id(Velocity), .inout = EcsIn }
    },
    .callback = Move,
    .interval = 1.0 // Run at 1Hz
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.system<Position, const Velocity>()
    .interval(1.0) // Run at 1Hz
    .each(...);
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Routine<Position, Velocity>()
    .Interval(1.0f) // Run at 1Hz
    .Each(...);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world.system::<&Position>()
    .interval(1.0) // Run at 1Hz
    .each(|p| {
    // ...
});
```
</li>
</ul>
</div>

### Rate
The following example shows how to run systems every Nth tick with a rate filter:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

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
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .query.terms = {
        { ecs_id(Position) }, 
        { ecs_id(Velocity), .inout = EcsIn }
    },
    .callback = Move,
    .rate = 2.0 // Run every other frame
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.system<Position, const Velocity>()
    .rate(2) // Run every other frame
    .each(...);
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Routine<Position, Velocity>()
    .Rate(2) // Run every other frame
    .Each(...);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .system::<&Position>()
    .rate(2) // Run every other frame
    .each(|p| {
        // ...
    });
```
</li>
</ul>
</div>

### Tick source
Instead of setting the interval or rate directly on the system, an application may also create a separate entity that holds the time or rate filter, and use that as a "tick source" for a system. This makes it easier to use the same settings across groups of systems:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

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
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .query.terms = {
        { ecs_id(Position) }, 
        { ecs_id(Velocity), .inout = EcsIn }
    },
    .callback = Move,
    .tick_source = tick_source // Set tick source for system
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// A rate filter can be created with .rate(2)
flecs::entity tick_source = world.timer()
    .interval(1.0);

world.system<Position, const Velocity>()
    .tick_source(tick_source) // Set tick source for system
    .each(...);
```
</li>
<li><b class="tab-title">C#</b>

```cs
// A rate filter can be created with .Rate(2)
TimerEntity tickSource = world.Timer()
    .Interval(1.0f);

world.Routine<Position, Velocity>()
    .TickSource(tickSource) // Set tick source for system
    .Each(...);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// Timer not yet implemented in Rust
```
</li>
</ul>
</div>

Interval filters can be paused and resumed:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Pause timer
ecs_stop_timer(world, tick_source);

// Resume timer
ecs_start_timer(world, tick_source);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// Pause timer
tick_source.stop();

// Resume timer
tick_source.start();
```
</li>
<li><b class="tab-title">C#</b>

```cs
// Pause timer
tickSource.Stop();

// Resume timer
tickSource.Start();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// Timer addon yet to be implemented in rust
```
</li>
</ul>
</div>

An additional advantage of using shared interval/rate filter between systems is that it guarantees that systems are ran at the same tick. When a system is disabled, its interval/rate filters aren't updated, which means that when the system is reenabled again it would be out of sync with other systems that had the same interval/rate specified. When using a shared tick source however the system is guaranteed to run at the same tick as other systems with the same tick source, even after the system is reenabled.

### Nested tick sources
One tick source can be used as the input of another (rate) tick source. The rate tick source will run at each Nth tick of the input tick source. This can be used to create nested tick sources, like in the following example:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

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
</li>
<li><b class="tab-title">C++</b>

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
</li>
<li><b class="tab-title">C#</b>

```cs
// Tick at 1Hz
TimerEntity eachSecond = world.Timer()
    .Interval(1.0f);

// Tick each minute
TimerEntity eachMinute = world.Timer()
    .Rate(60, eachSecond);

// Tick each hour
TimerEntity eachHour = world.Timer()
    .Rate(60, eachMinute);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// Timer not yet implemented in Rust
```
</li>
</ul>
</div>

Systems can also act as each others tick source:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Tick at 1Hz
ecs_entity_t each_second = ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "EachSecond",
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .callback = Dummy,
    .interval = 1.0
});

// Tick each minute
ecs_entity_t each_minute = ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "EachMinute",
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .callback = Dummy,
    .tick_source = each_second,
    .rate = 60
});

// Tick each hour
ecs_entity_t each_hour = ecs_system(world, {
    .entity = ecs_entity(world, {
        .name = "EachHour",
        .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
    }),
    .callback = Dummy,
    .tick_source = each_minute,
    .rate = 60
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// Tick at 1Hz
flecs::entity each_second = world.system("EachSecond")
    .interval(1.0)
    .run([](flecs::iter& it) { /* ... */ });

// Tick each minute
flecs::entity each_minute = world.system("EachMinute")
    .tick_source(each_second)
    .rate(60)
    .run([](flecs::iter& it) { /* ... */ });

// Tick each hour
flecs::entity each_hour = world.system("EachHour")
    .tick_source(each_minute)
    .rate(60)
    .run([](flecs::iter& it) { /* ... */ });
```
</li>
<li><b class="tab-title">C#</b>

```cs
// Tick at 1Hz
Routine eachSecond = world.Routine("EachSecond")
    .Interval(1.0f)
    .Iter((Iter it) => { /* ... */ });

// Tick each minute
Routine eachMinute = world.Routine("EachMinute")
    .TickSource(eachSecond)
    .Rate(60)
    .Iter((Iter it) => { /* ... */ });

// Tick each hour
Routine eachHour = world.Routine("EachHour")
    .TickSource(eachMinute)
    .Rate(60)
    .Iter((Iter it) => { /* ... */ });
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// Timer not yet implemented in Rust
```
</li>
</ul>
</div>
