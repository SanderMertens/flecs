# Flecs Quickstart
This document provides a quick overview of the different features and concepts in Flecs with short examples. This is a good resource if you're just getting started or just want to get a better idea of what kind of features are available in Flecs!

## Building Flecs
To use Flecs, copy the [flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.c) and [flecs.h](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.h) files from the repository root to your project's source folder. When building, make sure your build system is setup to do the following:

- If it is a C++ project, make sure to compile [flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.c) as C code, for example by using `gcc`/`clang` instead of `g++`/`clang++`.

- If you are building on Windows and you're not using the Microsoft Visual Studio compiler, make sure to add `-lWs2_32` to **the end(!)** of the linker command. The socket API is used for connecting to Flecs explorer.

- When compiling Flecs with `gcc`/`clang`, add `-std=gnu99` to the compiler command. This ensures that addons that rely on time & socket functions are compiled correctly.

- C++ files that use Flecs must be compiled with `-std=c++0x` (C++11) or higher.

### Dynamic linking
To build Flecs as a dynamic library, remove this line from the top of the [flecs.h](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.h) file:

```c
#define flecs_STATIC
```

When compiling [flecs.c](https://raw.githubusercontent.com/SanderMertens/flecs/master/flecs.c), make sure to define `flecs_EXPORTS`, for example by adding `-Dflecs_EXPORTS` to the compiler command.

Alternatively Flecs can also be built as a dynamic library with the cmake, meson, bazel or [bake](https://github.com/SanderMertens/bake) build files provided in the repository. These use the files from `src` to build as opposed to the amalgamated files, which is better suited for development.

### Building with CMake

Locate `flecs` on your system (either by cloning or as a submodule) and use `add_subdirectory` or use `FetchContent` to download the source code from the master branch of the [flecs repository](https://github.com/SanderMertens/flecs). After that, add the following to your `CMakeLists.txt` file:

```cmake
target_link_libraries(${PROJECT_NAME} flecs::flecs_static)
```

### Building with Bake

Download or `git clone` the [flecs repository](https://github.com/SanderMertens/flecs) and run `bake` from inside the directory. After that, add the following to your `project.json` file's value property:

```json
"use": ["flecs"]
```

### Running tests (bake)
First make sure you have [bake](https://github.com/SanderMertens/bake) installed (see the bake repository for instructions). 

Run the following commands to run all tests (use `-j` to specify the number of threads):

```bash
# Core test suite
bake run test/api -- -j 4

# Addon tests
bake run test/addons -- -j 4

# Reflection tests
bake run test/meta -- -j 4

# C++ tests
bake run test/cpp_api -- -j 4
```

To run tests with asan enabled, add `--cfg sanitize` to the command:

```bash
bake run --cfg sanitize test/api -- -j 4
```

#### Running tests (cmake, experimental)
First make sure to clone [bake](https://github.com/SanderMertens/bake).

Run the following commands to run all the tests:

```bash
# Generate make files for Flecs and tests
cmake -DFLECS_TESTS=ON -DBAKE_DIRECTORY="path to cloned bake repository"

# Build flecs and test suites
cmake --build . -j 4

# Run the tests
ctest -C Debug --verbose
```

### Emscripten
When building for emscripten, add the following command line options to the `emcc` link command:
```bash 
-s ALLOW_MEMORY_GROWTH=1 
-s STACK_SIZE=1mb
-s EXPORTED_RUNTIME_METHODS=cwrap 
-s MODULARIZE=1 
-s EXPORT_NAME="my_app"
```

### Addons
Flecs has a modular architecture that makes it easy to only build the features you really need. By default all addons are built. To customize a build, first define `FLECS_CUSTOM_BUILD`, then add defines for the addons you need. For example:

```c
#define FLECS_CUSTOM_BUILD  // Don't build all addons
#define FLECS_SYSTEM        // Build FLECS_SYSTEM
```

Additionally, you can also specify addons to exclude from a build by adding `NO` to the define:

```c
#define FLECS_NO_LOG
```

The following addons can be configured:

Addon         | Description                                      | Define              |
--------------|--------------------------------------------------|---------------------|
[Cpp](/flecs/group__cpp.html)                              | C++11 API                                        | FLECS_CPP           |
[Module](/flecs/group__c__addons__module.html)             | Organize game logic into reusable modules        | FLECS_MODULE        |
[System](/flecs/group__c__addons__system.html)             | Create & run systems                             | FLECS_SYSTEM        |
[Pipeline](/flecs/group__c__addons__pipeline.html)         | Automatically schedule & multithread systems     | FLECS_PIPELINE      |
[Timer](/flecs/group__c__addons__timer.html)               | Run systems at time intervals or at a rate       | FLECS_TIMER         |
[Meta](/flecs/group__c__addons__meta.html)                 | Flecs reflection system                          | FLECS_META          |
[Units](/flecs/group__c__addons__units.html)               | Builtin unit types                               | FLECS_UNITS         |
[JSON](/flecs/group__c__addons__json.html)                 | JSON format                                      | FLECS_JSON          |
[Doc](/flecs/group__c__addons__doc.html)                   | Add documentation to components, systems & more  | FLECS_DOC           |
[Http](/flecs/group__c__addons__http.html)                 | Tiny HTTP server for processing simple requests  | FLECS_HTTP          |
[Rest](/flecs/group__c__addons__rest.html)                 | REST API for showing entities in the browser     | FLECS_REST          |
[Script](/flecs/group__c__addons__script.html)             | DSL for assets, scenes and configuration         | FLECS_SCRIPT        |
[Stats](/flecs/group__c__addons__stats.html)               | Functions for collecting statistics              | FLECS_STATS         |
[Metrics](/flecs/group__c__addons__metrics.html)           | Create metrics from user-defined components      | FLECS_METRICS       |
[Alerts](/flecs/group__c__addons__alerts.html)             | Create alerts from user-defined queries          | FLECS_ALERTS        |
[Log](/flecs/group__c__addons__log.html)                   | Extended tracing and error logging               | FLECS_LOG           |
[Journal](/flecs/group__c__addons__journal.html)           | Journaling of API functions                      | FLECS_JOURNAL       |
[App](/flecs/group__c__addons__app.html)                   | Flecs application framework                      | FLECS_APP           |
[OS API Impl](/flecs/group__c__addons__os__api__impl.html) | Default OS API implementation for Posix/Win32    | FLECS_OS_API_IMPL   |

## Concepts
This section contains an overview of all the different concepts in Flecs and how they wire together. The sections in the quickstart go over them in more detail and with code examples.

![Flecs Overview](img/flecs-quickstart-overview.png)

### World
The world is the container for all ECS data. It stores the entities and their components, does queries and runs systems. Typically there is only a single world, but there is no limit on the number of worlds an application can create.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_world_t *world = ecs_init();

// Do the ECS stuff

ecs_fini(world);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::world world;

// Do the ECS stuff
```
</li>
<li><b class="tab-title">C#</b>

```cs
using World world = World.Create();

// Do the ECS stuff
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let world = World::new();

// Do the ECS stuff
```
</li>
</ul>
</div>

### Entity
An entity is a unique thing in the world, and is represented by a 64 bit id. Entities can be created and deleted. If an entity is deleted it is no longer considered "alive". A world can contain up to 4 billion(!) alive entities. Entity identifiers contain a few bits that make it possible to check whether an entity is alive or not.
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_new(world);
ecs_is_alive(world, e); // true!

ecs_delete(world, e);
ecs_is_alive(world, e); // false!
```
</li>
<li><b class="tab-title">C++</b>

```cpp
auto e = world.entity();
e.is_alive(); // true!

e.destruct();
e.is_alive(); // false!
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity e = world.Entity();
e.IsAlive(); // true!

e.Destruct();
e.IsAlive(); // false!
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.entity();
e.is_alive(); // true!

e.destruct();
e.is_alive(); // false!
```
</li>
</ul>
</div>

Entities can have names which makes it easier to identify them in an application. In C++ the name can be passed to the constructor. If a name is provided during entity creation time and an entity with that name already exists, the existing entity will be returned.
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

In C a name can be assigned with the `ecs_entity_init` function or `ecs_entity` macro. 
```c
ecs_entity_t e = ecs_entity(world, { .name = "Bob" });

printf("Entity name: %s\n", ecs_get_name(world, e));
```
</li>
<li><b class="tab-title">C++</b>

```cpp
auto e = world.entity("Bob");

std::cout << "Entity name: " << e.name() << std::endl;
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity e = world.Entity("Bob");

Console.WriteLine($"Entity name: {e.Name()}");
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.entity_named("bob");

println!("Entity name: {}", e.name());
```
</li>
</ul>
</div>

Entities can be looked up by name with the `lookup` function:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Bob");
```
</li>
<li><b class="tab-title">C++</b>

```cpp
auto e = world.lookup("Bob");
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity e = world.Lookup("Bob");
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.lookup("bob");
```
</li>
</ul>
</div>

### Id
An id is a 64 bit number that can encode anything that can be added to an entity. In flecs this can be either a component, tag or a pair. A component is data that can be added to an entity. A tag is an "empty" component. A pair is a combination of two component/tag ids which is used to encode entity relationships. All entity/component/tag identifiers are valid ids, but not all ids are valid entity identifier.

The following sections describe components, tags and pairs in more detail.

### Component
A component is a type of which instances can be added and removed to entities. Each component can be added only once to an entity (though not really, see [Pair](#pair)). In C applications components must be registered before use. By default in C++ this happens automatically.
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);

ecs_entity_t e = ecs_new(world);

// Add a component. This creates the component in the ECS storage, but does not
// assign it with a value.
ecs_add(world, e, Velocity);

// Set the value for the Position & Velocity components. A component will be
// added if the entity doesn't have it yet.
ecs_set(world, e, Position, {10, 20});
ecs_set(world, e, Velocity, {1, 2});

// Get a component
const Position *p = ecs_get(world, e, Position);

// Remove component
ecs_remove(world, e, Position);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
auto e = world.entity();

// Add a component. This creates the component in the ECS storage, but does not
// assign it with a value.
e.add<Velocity>();

// Set the value for the Position & Velocity components. A component will be
// added if the entity doesn't have it yet.
e.set<Position>({10, 20})
 .set<Velocity>({1, 2});

// Get a component
const Position *p = e.get<Position>();

// Remove component
e.remove<Position>();
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity e = world.Entity();

// Add a component. This creates the component in the ECS storage, but does not
// assign it with a value.
e.Add<Velocity>();

// Set the value for the Position & Velocity components. A component will be
// added if the entity doesn't have it yet.
e.Set<Position>(new(10, 20))
 .Set<Velocity>(new(1, 2));

// Get a component
ref readonly Position p = ref e.Get<Position>();

// Remove component
e.Remove<Position>();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.entity();

// Add a component. This creates the component in the ECS storage, but does not
// assign it with a value.
e.add::<Velocity>();

// Set the value for the Position & Velocity components. A component will be
// added if the entity doesn't have it yet.
e.set(Position { x: 10.0, y: 20.0 })
 .set(Velocity { x: 1.0, y: 2.0 });

// Get a component
e.get::<&Position>(|p| {
    println!("Position: ({}, {})", p.x, p.y);
});

// Remove component
e.remove::<Position>();
```
</li>
</ul>
</div>

Each component is associated by a unique entity identifier by Flecs. This makes it possible to inspect component data, or attach your own data to components. 
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

C applications can use the `ecs_id` macro to get the entity id for a component.
```c
ECS_COMPONENT(world, Position);

ecs_entity_t pos_e = ecs_id(Position);
printf("Name: %s\n", ecs_get_name(world, pos_e)); // outputs 'Name: Position'

// It's possible to add components like you would for any entity
ecs_add(world, pos_e, Serializable);
```
</li>
<li><b class="tab-title">C++</b>

C++ applications can use the `world::entity` function.
```cpp
flecs::entity pos_e = world.entity<Position>();
std::cout << "Name: " << pos_e.name() << std::endl;  // outputs 'Name: Position'

// It's possible to add components like you would for any entity
pos_e.add<Serializable>();
```
</li>
<li><b class="tab-title">C#</b>

C# applications can use the `World.Entity()` function.
```cs
Entity posE = world.Entity<Position>();
Console.WriteLine($"Name: {posE.Name()}"); // outputs 'Name: Position'

// It's possible to add components like you would for any entity
posE.Add<Serializable>();
```
</li>
<li><b class="tab-title">Rust</b>

Rust applications can use the `world::entity_from` function.
```rust
let pos_e = world.entity_from::<Position>();

println!("Name: {}", pos_e.name()); // outputs 'Name: Position'

// It's possible to add components like you would for any entity
pos_e.add::<Serializable>();
```
</li>
</ul>
</div>

The thing that makes an ordinary entity a component is the `EcsComponent` (or `flecs::Component`, in C++) component. This is a builtin component that tells Flecs how much space is needed to store a component, and can be inspected by applications:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(world, Position);

ecs_entity_t pos_e = ecs_id(Position);

const EcsComponent *c = ecs_get(world, pos_e, EcsComponent);
printf("Component size: %u\n", c->size);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity pos_e = world.entity<Position>();

const EcsComponent *c = pos_e.get<flecs::Component>();
std::cout << "Component size: " << c->size << std::endl;
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity posE = world.Entity<Position>();

ref readonly EcsComponent c = ref posE.Get<EcsComponent>();
Console.WriteLine($"Component size: {c.size}");
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let pos_e = world.entity_from::<Position>();

pos_e.get::<&flecs::Component>(|c| {
    println!("Component size: {}", c.size);
});
```
</li>
</ul>
</div>

Because components are stored as regular entities, they can in theory also be deleted. To prevent unexpected accidents however, by default components are registered with a tag that prevents them from being deleted. If this tag were to be removed, deleting a component would cause it to be removed from all entities. For more information on these policies, see [Relationship cleanup properties](Relationships.md#cleanup-properties).

### Tag
A tag is a component that does not have any data. In Flecs tags can be either empty types (in C++) or regular entities (C & C++) that do not have the `EcsComponent` component (or have an `EcsComponent` component with size 0). Tags can be added & removed using the same APIs as adding & removing components, but because tags have no data, they cannot be assigned a value. Because tags (like components) are regular entities, they can be created & deleted at runtime.
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create Enemy tag
ecs_entity_t Enemy = ecs_new(world);

// Create entity, add Enemy tag
ecs_entity_t e = ecs_new(world);

ecs_add_id(world, e, Enemy);
ecs_has_id(world, e, Enemy); // true!

ecs_remove_id(world, e, Enemy);
ecs_has_id(world, e, Enemy); // false!
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// Option 1: create Tag as empty struct
struct Enemy { };

// Create entity, add Enemy tag
auto e = world.entity().add<Enemy>();
e.has<Enemy>(); // true!

e.remove<Enemy>();
e.has<Enemy>(); // false!


// Option 2: create Tag as entity
auto Enemy = world.entity();

// Create entity, add Enemy tag
auto e = world.entity().add(Enemy);
e.has(Enemy); // true!

e.remove(Enemy);
e.has(Enemy); // false!
```
</li>
<li><b class="tab-title">C#</b>

```cs
// Option 1: create Tag as empty struct
public struct Enemy { }

// Create entity, add Enemy tag
Entity e = world.Entity().Add<Enemy>();
e.Has<Enemy>(); // true!

e.Remove<Enemy>();
e.Has<Enemy>(); // false!


// Option 2: create Tag as entity
Entity Enemy = world.Entity();

// Create entity, add Enemy tag
Entity e = world.Entity().Add(Enemy);
e.Has(Enemy); // true!

e.Remove(Enemy);
e.Has(Enemy); // false!
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// Option 1: create Tag as empty struct
#[derive(Component)]
struct Enemy;

// Create entity, add Enemy tag
let e = world.entity().add::<Enemy>();
e.has::<Enemy>(); // true!

e.remove::<Enemy>();
e.has::<Enemy>(); // false!

// Option 2: create Tag as entity
let enemy = world.entity();

// Create entity, add Enemy tag
let e = world.entity().add_id(enemy);
e.has_id(enemy); // true!

e.remove_id(enemy);
e.has_id(enemy); // false!
```
</li>
</ul>
</div>

Note that both options in the C++ example achieve the same effect. The only difference is that in option 1 the tag is fixed at compile time, whereas in option 2 the tag can be created dynamically at runtime.

When a tag is deleted, the same rules apply as for components (see [Relationship cleanup properties](Relationships.md#cleanup-properties)).

### Pair
A pair is a combination of two entity ids. Pairs can be used to store entity relationships, where the first id represents the relationship kind and the second id represents the relationship target (called "object"). This is best explained by an example:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create Likes relationship
ecs_entity_t Likes = ecs_new(world);

// Create a small graph with two entities that like each other
ecs_entity_t Bob = ecs_new(world);
ecs_entity_t Alice = ecs_new(world);

ecs_add_pair(world, Bob, Likes, Alice); // Bob likes Alice
ecs_add_pair(world, Alice, Likes, Bob); // Alice likes Bob
ecs_has_pair(world, Bob, Likes, Alice); // true!

ecs_remove_pair(world, Bob, Likes, Alice);
ecs_has_pair(world, Bob, Likes, Alice); // false!
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// Create Likes relationship as empty type (tag)
struct Likes { };

// Create a small graph with two entities that like each other
auto Bob = world.entity();
auto Alice = world.entity();

Bob.add<Likes>(Alice); // Bob likes Alice
Alice.add<Likes>(Bob); // Alice likes Bob
Bob.has<Likes>(Alice); // true!

Bob.remove<Likes>(Alice);
Bob.has<Likes>(Alice); // false!
```
</li>
<li><b class="tab-title">C#</b>

```cs
// Create Likes relationship as empty type (tag)
public struct Likes { }

// Create a small graph with two entities that like each other
Entity Bob = world.Entity();
Entity Alice = world.Entity();

Bob.Add<Likes>(Alice); // Bob likes Alice
Alice.Add<Likes>(Bob); // Alice likes Bob
Bob.Has<Likes>(Alice); // true!

Bob.Remove<Likes>(Alice);
Bob.Has<Likes>(Alice); // false!
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// Create Likes relationship as empty type (tag)
#[derive(Component)]
struct Likes;

// Create a small graph with two entities that like each other
let bob = world.entity();
let alice = world.entity();

bob.add_first::<Likes>(alice); // bob likes alice
alice.add_first::<Likes>(bob); // alice likes bob
bob.has_first::<Likes>(alice); // true!

bob.remove_first::<Likes>(alice);
bob.has_first::<Likes>(alice); // false!
```
</li>
</ul>
</div>

A pair can be encoded in a single 64 bit identifier by using the `ecs_pair` macro in C, or the `world.pair` function in C++:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_id_t id = ecs_pair(Likes, Bob);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::id id = world.pair<Likes>(Bob);
```
</li>
<li><b class="tab-title">C#</b>

```cs
Id id = world.Pair<Likes>(bob);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let id = world.id_first::<Likes>(bob);
```
</li>
</ul>
</div>

The following examples show how to get back the elements from a pair:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
if (ecs_id_is_pair(id)) {
    ecs_entity_t relationship = ecs_pair_first(world, id);
    ecs_entity_t target = ecs_pair_second(world, id);
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::id id = ...;
if (id.is_pair()) {
    auto relationship = id.first();
    auto target = id.second();
}
```
</li>
<li><b class="tab-title">C#</b>

```cs
Id id = ...;
if (id.IsPair())
{
    Entity relationship = id.First();
    Entity target = id.Second();
}
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let id = world.id_from::<(Likes, Apples)>();
if id.is_pair() {
    let relationship = id.first_id();
    let target = id.second_id();
}
```
</li>
</ul>
</div>

A component or tag can be added multiple times to the same entity as long as it is part of a pair, and the pair itself is unique:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_add_pair(world, Bob, Eats, Apples);
ecs_add_pair(world, Bob, Eats, Pears);
ecs_add_pair(world, Bob, Grows, Pears);

ecs_has_pair(world, Bob, Eats, Apples); // true!
ecs_has_pair(world, Bob, Eats, Pears);  // true!
ecs_has_pair(world, Bob, Grows, Pears); // true!
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity bob = ...;
bob.add(Eats, Apples);
bob.add(Eats, Pears);
bob.add(Grows, Pears);

bob.has(Eats, Apples); // true!
bob.has(Eats, Pears);  // true!
bob.has(Grows, Pears); // true!
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity Bob = ...;
Bob.Add(Eats, Apples);
Bob.Add(Eats, Pears);
Bob.Add(Grows, Pears);

Bob.Has(Eats, Apples); // true!
Bob.Has(Eats, Pears);  // true!
Bob.Has(Grows, Pears); // true!
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let bob = world.entity();
bob.add_id((eats, apples));
bob.add_id((eats, pears));
bob.add_id((grows, pears));

bob.has_id((eats, apples)); // true!
bob.has_id((eats, pears)); // true!
bob.has_id((grows, pears)); // true!
```
</li>
</ul>
</div>

The `target` function can be used in C and C++ to get the object for a relationship:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t o = ecs_get_target(world, Alice, Likes, 0); // Returns Bob
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity alice = ...;
auto o = alice.target<Likes>(); // Returns Bob
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity Alice = ...;
Entity o = Alice.Target<Likes>(); // Returns Bob
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let alice = world.entity().add_first::<Likes>(bob);
let o = alice.target::<Likes>(0); // Returns bob
```
</li>
</ul>
</div>

Entity relationships enable lots of interesting patterns and possibilities. Make sure to check out the [Relationships manual](Relationships.md).

### Hierarchies
Flecs has builtin support for hierarchies with the builtin `EcsChildOf` (or `flecs::ChildOf`, in C++) relationship. A hierarchy can be created with the regular relationship API, or with the `child_of` shortcut in C++:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t parent = ecs_new(world);

// ecs_new_w_pair is the same as ecs_new_id + ecs_add_pair
ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

// Deleting the parent also deletes its children
ecs_delete(world, parent);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
auto parent = world.entity();
auto child = world.entity().child_of(parent);

// Deleting the parent also deletes its children
parent.destruct();
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity parent = world.Entity();
Entity child = world.Entity().ChildOf(parent);

// Deleting the parent also deletes its children
parent.Destruct();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let parent = world.entity();
let child = world.entity().child_of_id(parent);

// Deleting the parent also deletes its children
parent.destruct();
```
</li>
</ul>
</div>

When entities have names, they can be used together with hierarchies to generate path names or do relative lookups:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t parent = ecs_entity(world, {
    .name = "parent"
});

ecs_entity_t child = ecs_entity(world, {
    .name = "child"
});

ecs_add_pair(world, child, EcsChildOf, parent);

char *path = ecs_get_path(world, child);
printf("%s\n", path); // output: 'parent.child'
ecs_os_free(path);

ecs_lookup(world, "parent.child");         // returns child
ecs_lookup_from(world, parent, "child");   // returns child
```
</li>
<li><b class="tab-title">C++</b>

```cpp
auto parent = world.entity("parent");
auto child = world.entity("child").child_of(parent);
std::cout << child.path() << std::endl; // output: 'parent::child'

world.lookup("parent::child"); // returns child
parent.lookup("child"); // returns child
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity parent = world.Entity("parent");
Entity child = world.Entity("child").ChildOf(parent);
Console.WriteLine(child.Path()); // output: 'parent.child'

world.Lookup("parent.child"); // returns child
parent.Lookup("child"); // returns child
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let parent = world.entity_named("parent");
let child = world.entity_named("child").child_of_id(parent);

println!("Child path: {}", child.path().unwrap()); // output: 'parent::child'

world.lookup("parent::child"); // returns child
parent.lookup("child"); // returns child
```
</li>
</ul>
</div>

Queries (see below) can use hierarchies to order data breadth-first, which can come in handy when you're implementing a transform system:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
    .terms = {
        { ecs_id(Position) },
        { ecs_id(Position), .src = {
            .flags = EcsCascade,       // Breadth-first order
            .trav = EcsChildOf // Use ChildOf relationship for traversal
        }}
    }
});

ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
    Position *p = ecs_field(&it, Position, 0);
    Position *p_parent = ecs_field(&it, Position, 1);
    for (int i = 0; i < it.count; i++) {
        // Do the thing
    }
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
auto q = world.query_builder<Position, Position>()
    .term_at(1).parent().cascade()
    .build();

q.each([](Position& p, Position& p_parent) {
    // Do the thing
});
```
</li>
<li><b class="tab-title">C#</b>

```cs
Query q = world.QueryBuilder<Position, Position>()
    .TermAt(1).Parent().Cascade()
    .Build();

q.Each((ref Position p, ref Position pParent) =>
{
    // Do the thing
});
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let q = world
    .query::<(&Position, &mut Position)>()
    .term_at(1)
    .parent()
    .cascade()
    .build();

q.each(|(p, p_parent)| {
    // Do the thing
});
```
</li>
</ul>
</div>

### Type
The type (often referred to as "archetype") is the list of ids an entity has. Types can be used for introspection which is useful when debugging, or when for example building an entity editor. The most common thing to do with a type is to convert it to text and print it:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_COMPONENT(world, Position);
ECS_COMPONENT(world, Velocity);

ecs_entity_t e = ecs_new(world);
ecs_add(world, e, Position);
ecs_add(world, e, Velocity);

const ecs_type_t *type = ecs_get_type(world, e);
char *type_str = ecs_type_str(world, type);
printf("Type: %s\n", type_str); // output: 'Position,Velocity'
ecs_os_free(type_str);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
auto e = ecs.entity()
    .add<Position>()
    .add<Velocity>();

std::cout << e.type().str() << std::endl; // output: 'Position,Velocity'
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity e = ecs.Entity()
    .Add<Position>()
    .Add<Velocity>();

Console.WriteLine(e.Type().Str()); // output: 'Position,Velocity'
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let e = world.entity().add::<Position>().add::<Velocity>();

println!("Components: {}", e.archetype().to_string().unwrap()); // output: 'Position,Velocity'
```
</li>
</ul>
</div>

A type can also be iterated by an application:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
const ecs_type_t *type = ecs_get_type(world, e);
for (int i = 0; i < type->count; i++) {
    if (type->array[i] == ecs_id(Position)) {
        // Found Position component!
    }
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
e.each([&](flecs::id id) {
    if (id == world.id<Position>()) {
        // Found Position component!
    }
});
```
</li>
<li><b class="tab-title">C#</b>

```cs
e.Each((Id id) =>
{
    if (id == world.Id<Position>()) 
    {
        // Found Position component!
    }
});
```
</li>
<li><b class="tab-title">Rust</b>

```rust
e.each_component(|id| {
    if id == world.component_id::<Position>() {
        // Found Position component!
    }
});
```
</li>
</ul>
</div>

### Singleton
A singleton is a single instance of a component that can be retrieved without an entity. The functions for singletons are very similar to the regular API:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Set singleton component
ecs_singleton_set(world, Gravity, { 9.81 });

// Get singleton component
const Gravity *g = ecs_singleton_get(world, Gravity);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// Set singleton component
world.set<Gravity>({ 9.81 });

// Get singleton component
const Gravity *g = world.get<Gravity>();
```
</li>
<li><b class="tab-title">C#</b>

```cs
// Set singleton component
world.Set<Gravity>(new(9.81));

// Get singleton component
ref readonly Gravity g = ref world.Get<Gravity>();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// Set singleton component
world.set(Gravity { x: 10, y: 20 });

// Get singleton component
world.get::<&Gravity>(|g| {
    println!("Gravity: {}, {}", g.x, g.y);
});
```
</li>
</ul>
</div>

Singleton components are created by adding the component to its own entity id. The above code examples are shortcuts for these regular API calls:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_set(world, ecs_id(Gravity), Gravity, {10, 20});

const Gravity *g = ecs_get(world, ecs_id(Gravity), Gravity);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity grav_e = world.entity<Gravity>();

grav_e.set<Gravity>({10, 20});

const Gravity *g = grav_e.get<Gravity>();
```
</li>
<li><b class="tab-title">C#</b>

```cs
Entity gravE = world.Entity<Gravity>();

gravE.Set<Gravity>(new(10, 20));

ref readonly Gravity g = ref gravE.Get<Gravity>();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let grav_e = world.entity_from::<Gravity>();

grav_e.set(Gravity { x: 10, y: 20 });

grav_e.get::<&Gravity>(|g| {
    println!("Gravity: {}, {}", g.x, g.y);
});
```
</li>
</ul>
</div>

The following examples show how to query for a singleton component:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Create query that matches Gravity as singleton
ecs_query_t *q = ecs_query(ecs, {
    .terms = {
        // Regular component
        { .id = ecs_id(Velocity) },
        // A singleton is a component matched on itself
        { .id = ecs_id(Gravity), .src.id = ecs_id(Gravity) }
    }
});

// Create a system using the query DSL with a singleton:
ECS_SYSTEM(world, ApplyGravity, EcsOnUpdate, Velocity, Gravity($));
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.query_builder<Velocity, Gravity>()
    .term_at(1).singleton()
    .build();
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.QueryBuilder<Velocity, Gravity>()
    .TermAt(1).Singleton()
    .Build();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .query::<(&Velocity, &Gravity)>()
    .term_at(1)
    .singleton()
    .build();
```
</li>
</ul>
</div>

### Query
Queries are the main mechanism for finding and iterating through entities. Queries are used in many parts of the API, such as for systems and observers. The following example shows a simple query:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>
```c
ecs_query_t *q = ecs_query(world, {
    .terms = {
        { ecs_id(Position) },
        { ecs_pair(EcsChildOf, parent) }
    }
});

// Iterate the query results. Because entities are grouped by their type there
// are two loops: an outer loop for the type, and an inner loop for the entities
// for that type.
ecs_iter_t it = ecs_query_iter(world, q);
while (ecs_query_next(&it)) {
    // Each type has its own set of component arrays
    Position *p = ecs_field(&it, Position, 0);

    // Iterate all entities for the type
    for (int i = 0; i < it.count; i++) {
        printf("%s: {%f, %f}\n", ecs_get_name(world, it.entities[i]),
            p[i].x, p[i].y);
    }
}

ecs_query_fini(f);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// For simple queries the world::each function can be used
world.each([](Position& p, Velocity& v) { // flecs::entity argument is optional
    p.x += v.x;
    p.y += v.y;
});

// More complex queries can first be created, then iterated
auto q = world.query_builder<Position>()
    .with(flecs::ChildOf, parent)
    .build();

// Option 1: the each() callback iterates over each entity
q.each([](flecs::entity e, Position& p) {
    std::cout << e.name() << ": {" << p.x << ", " << p.y << "}" << std::endl;
});

// Option 2: the run() callback offers more control over the iteration
q.run([](flecs::iter& it) {
    while (it.next()) {
        auto p = it.field<Position>(0);

        for (auto i : it) {
            std::cout << it.entity(i).name()
                << ": {" << p[i].x << ", " << p[i].y << "}" << std::endl;
        }
    }
});
```
</li>
<li><b class="tab-title">C#</b>

```cs
// For simple queries the each function can be used
world.Each((ref Position p, ref Velocity v) => // Entity argument is optional
{
    p.X += v.X;
    p.Y += v.Y;
});

// More complex filters can first be created, then iterated
using Query q = world.QueryBuilder<Position>()
    .With(Ecs.ChildOf, parent)
    .Build();

// Option 1: The Each() callback that iterates each entity
q.Each((Entity e, ref Position p) =>
{
    Console.WriteLine($"{e.Name()}: ({p.X}, {p.Y})")
});

// Option 2: The Iter() callback provides more control over the iteration
q.Iter((Iter it, Field<Position> p) =>
{
    foreach (int i in it)
        Console.WriteLine($"{it.Entity(i).Name()}: ({p[i].X}, {p[i].Y})")
});
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// For simple queries the world::each function can be used
world.each::<(&mut Position, &Velocity)>(|(p, v)| {
    // EntityView argument is optional, use each_entity to get it
    p.x += v.x;
    p.y += v.y;
});

// More complex queries can first be created, then iterated
let q = world
    .query::<&Position>()
    .with_id((flecs::ChildOf::ID, parent))
    .build();

// Option 1: the each() callback iterates over each entity
q.each_entity(|e, p| {
    println!("{}: ({}, {})", e.name(), p.x, p.y);
}); 

// Option 2: the run() callback offers more control over the iteration
q.run(|mut it| {
    while it.next() {
        let p = it.field::<Position>(0).unwrap();

        for i in it.iter() {
            println!("{}: ({}, {})", it.entity(i).name(), p[i].x, p[i].y);
        }
    }
});
```
</li>
</ul>
</div>

Queries can use operators to exclude components, optionally match components or match one out of a list of components. Additionally filters may contain wildcards for terms which is especially useful when combined with pairs.

The following example shows a query that matches all entities with a parent that do not have `Position`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
    .terms = {
        { ecs_pair(EcsChildOf, EcsWildcard) }
        { ecs_id(Position), .oper = EcsNot },
    }
});

// Iteration code is the same
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::query<> q = world.query_builder()
    .with(flecs::ChildOf, flecs::Wildcard)
    .with<Position>().oper(flecs::Not)
    .build();

// Iteration code is the same
```
</li>
<li><b class="tab-title">C#</b>

```cs
using Query q = world.QueryBuilder()
    .With(Ecs.ChildOf, Ecs.Wildcard)
    .With<Position>().Oper(Ecs.Not)
    .Build();

// Iteration code is the same
```
</li>
<li><b class="tab-title">Rust</b>

```rust
let q = world
    .query::<()>()
    .with::<(flecs::ChildOf, flecs::Wildcard)>()
    .with::<Position>()
    .set_oper(OperKind::Not)
    .build();

// Iteration code is the same
```
</li>
</ul>
</div>

See the [query manual](Queries.md) for more details.

### System
A system is a query combined with a callback. Systems can be either ran manually or ran as part of an ECS-managed main loop (see [Pipeline](#pipeline)). The system API looks similar to queries:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Option 1, use the ECS_SYSTEM convenience macro
ECS_SYSTEM(world, Move, 0, Position, Velocity);
ecs_run(world, Move, delta_time, NULL); // Run system

// Option 2, use the ecs_system_init function/ecs_system macro
ecs_entity_t move_sys = ecs_system(world, {
    .query.terms = {
        { ecs_id(Position) },
        { ecs_id(Velocity) },
    },
    .callback = Move
});

ecs_run(world, move_sys, delta_time, NULL); // Run system

// The callback code (same for both options)
void Move(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x * it->delta_time;
        p[i].y += v[i].y * it->delta_time;
    }
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
// Use each() function that iterates each individual entity
auto move_sys = world.system<Position, Velocity>()
    .each([](flecs::iter& it, size_t, Position& p, Velocity& v) {
        p.x += v.x * it.delta_time();
        p.y += v.y * it.delta_time();
    });

    // Just like with queries, systems have both the run() and
    // each() methods to iterate entities.

move_sys.run();
```
</li>
<li><b class="tab-title">C#</b>

```cs
// Use Each() function that iterates each individual entity
Routine moveSys = world.Routine<Position, Velocity>()
    .Each((Entity e, ref Position p, ref Velocity v) =>
    {
        p.X += v.X * it.DeltaTime();
        p.Y += v.Y * it.DeltaTime();
    });

    // Just like with queries, systems have both the Iter() and
    // Each() methods to iterate entities.

moveSys.Run();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
// Use each_entity() function that iterates each individual entity
let move_sys = world
    .system::<(&mut Position, &Velocity)>()
    .each_iter(|it, i, (p, v)| {
        p.x += v.x * it.delta_time();
        p.y += v.y * it.delta_time();
    });

// Just like with queries, systems have both the run() and
// each() methods to iterate entities.

move_sys.run();
```
</li>
</ul>
</div>

Systems are stored as entities with an `EcsSystem` component (`flecs::System` in C++), similar to components. That means that an application can use a system as a regular entity:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
printf("System: %s\n", ecs_get_name(world, move_sys));
ecs_add(world, move_sys, EcsOnUpdate);
ecs_delete(world, move_sys);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
std::cout << "System: " << move_sys.name() << std::endl;
move_sys.add(flecs::OnUpdate);
move_sys.destruct();
```
</li>
<li><b class="tab-title">C#</b>

```cs
Console.WriteLine($"System: {moveSys.Name()}");
moveSys.Entity.Add(Ecs.OnUpdate);
moveSys.Entity.Destruct();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
println!("System: {}", move_sys.name());
move_sys.add::<flecs::pipeline::OnUpdate>();
move_sys.destruct();
```
</li>
</ul>
</div>

### Pipeline
A pipeline is a list of tags that when matched, produces a list of systems to run. These tags are also referred to as a system "phase". Flecs comes with a default pipeline that has the following phases:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
EcsOnLoad
EcsPostLoad
EcsPreUpdate
EcsOnUpdate
EcsOnValidate
EcsPostUpdate
EcsPreStore
EcsOnStore
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::OnLoad
flecs::PostLoad
flecs::PreUpdate
flecs::OnUpdate
flecs::OnValidate
flecs::PostUpdate
flecs::PreStore
flecs::OnStore
```
</li>
<li><b class="tab-title">C#</b>

```cs
Ecs.OnLoad
Ecs.PostLoad
Ecs.PreUpdate
Ecs.OnUpdate
Ecs.OnValidate
Ecs.PostUpdate
Ecs.PreStore
Ecs.OnStore
```
</li>
<li><b class="tab-title">Rust</b>

```rust
flecs::pipeline::OnLoad;
flecs::pipeline::PostLoad;
flecs::pipeline::PreUpdate;
flecs::pipeline::OnUpdate;
flecs::pipeline::OnValidate;
flecs::pipeline::PostUpdate;
flecs::pipeline::PreStore;
flecs::pipeline::OnStore;
```
</li>
</ul>
</div>

When a pipeline is executed, systems are ran in the order of the phases. This makes pipelines and phases the primary mechanism for defining ordering between systems. The following code shows how to assign systems to a pipeline, and how to run the pipeline with the `progress()` function:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
ECS_SYSTEM(world, Transform, EcsPostUpdate, Position, Transform);
ECS_SYSTEM(world, Render, EcsOnStore, Transform, Mesh);

ecs_progress(world, 0); // run systems in default pipeline
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.system<Position, Velocity>("Move").kind(flecs::OnUpdate).each( ... );
world.system<Position, Transform>("Transform").kind(flecs::PostUpdate).each( ... );
world.system<Transform, Mesh>("Render").kind(flecs::OnStore).each( ... );

world.progress();
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Routine<Position, Velocity>("Move").Kind(Ecs.OnUpdate).Each( ... );
world.Routine<Position, Transform>("Transform").Kind(Ecs.PostUpdate).Each( ... );
world.Routine<Transform, Mesh>("Render").Kind(Ecs.OnStore).Each( ... );

world.Progress();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .system_named::<(&mut Position, &Velocity)>("Move")
    .kind::<flecs::pipeline::OnUpdate>()
    .each(|(p, v)| {});

world
    .system_named::<(&mut Position, &Transform)>("Transform")
    .kind::<flecs::pipeline::PostUpdate>()
    .each(|(p, t)| {});
    
world
    .system_named::<(&Transform, &mut Mesh)>("Render")
    .kind::<flecs::pipeline::OnStore>()
    .each(|(t, m)| {});

world.progress();
```
</li>
</ul>
</div>

Because phases are just tags that are added to systems, applications can use the regular API to add/remove systems to a phase:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_remove_id(world, Move, EcsOnUpdate);
ecs_add_id(world, Move, EcsPostUpdate);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
move_sys.add(flecs::OnUpdate);
move_sys.remove(flecs::PostUpdate);
```
</li>
<li><b class="tab-title">C#</b>

```cs
moveSys.Add(Ecs.OnUpdate);
moveSys.Remove(Ecs.PostUpdate);
```
</li>
<li><b class="tab-title">Rust</b>

```rust
move_sys.add::<flecs::pipeline::OnUpdate>();
move_sys.remove::<flecs::pipeline::PostUpdate>();
```
</li>
</ul>
</div>

Inside a phase, systems are guaranteed to be ran in their declaration order.

### Observer
Observers are callbacks that are invoked when one or more events matches the query of an observer. Events can be either user defined or builtin. Examples of builtin events are `OnAdd`, `OnRemove` and `OnSet`.

When an observer has a query with more than one component, the observer will not be invoked until the entity for which the event is emitted satisfies the entire query.

An example of an observer with two components:
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_observer(world, {
    .query.terms = { { ecs_id(Position) }, { ecs_id(Velocity) }},
    .event = { EcsOnSet },
    .callback = OnSetPosition
});

// Callback code is same as system

ecs_entity_t e = ecs_new(world);    // Doesn't invoke the observer
ecs_set(world, e, Position, {10, 20}); // Doesn't invoke the observer
ecs_set(world, e, Velocity, {1, 2});   // Invokes the observer
ecs_set(world, e, Position, {20, 40}); // Invokes the observer
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.observer<Position, Velocity>("OnSetPosition")
    .event(flecs::OnSet)
    .each( ... ); // Callback code is same as system

auto e = ecs.entity();     // Doesn't invoke the observer
e.set<Position>({10, 20}); // Doesn't invoke the observer
e.set<Velocity>({1, 2});   // Invokes the observer
e.set<Position>({20, 30}); // Invokes the observer
```
</li>
<li><b class="tab-title">C#</b>

```cs
world.Observer<Position, Velocity>("OnSetPosition")
    .Event(Ecs.OnSet)
    .Each( ... ); // Callback code is same as system

Entity e = ecs.Entity();      // Doesn't invoke the observer
e.Set<Position>(new(10, 20)); // Doesn't invoke the observer
e.Set<Velocity>(new(1, 2));   // Invokes the observer
e.Set<Position>(new(20, 30)); // Invokes the observer
```
</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .observer_named::<flecs::OnSet, (&Position, &Velocity)>("OnSetPosition")
    .each(|(p, v)| {}); // Callback code is same as system

let e = world.entity(); // Doesn't invoke the observer
e.set(Position { x: 10.0, y: 20.0 }); // Doesn't invoke the observer
e.set(Velocity { x: 1.0, y: 2.0 }); // Invokes the observer
e.set(Position { x: 30.0, y: 40.0 }); // Invokes the observer
```
</li>
</ul>
</div>

### Module
A module is a function that imports and organizes components, systems, triggers, observers, prefabs into the world as reusable units of code. A well designed module has no code that directly relies on code of another module, except for components definitions. All module contents are stored as child entities inside the module scope with the `ChildOf` relationship. The following examples show how to define a module in C and C++:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Module header (e.g. MyModule.h)
typedef struct {
    float x;
    float y;
} Position;

extern ECS_COMPONENT_DECLARE(Position);

// The import function name has to follow the convention: <ModuleName>Import
void MyModuleImport(ecs_world_t *world);

// Module source (e.g. MyModule.c)
ECS_COMPONENT_DECLARE(Position);

void MyModuleImport(ecs_world_t *world) {
    ECS_MODULE(world, MyModule);
    ECS_COMPONENT_DEFINE(world, Position);
}

// Import code
ECS_IMPORT(world, MyModule);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
struct my_module {
    my_module(flecs::world& world) {
        world.module<my_module>();

        // Define components, systems, triggers, ... as usual. They will be
        // automatically created inside the scope of the module.
    }
};

// Import code
world.import<my_module>();
```
</li>
<li><b class="tab-title">C#</b>

```cs
public struct MyModule : IFlecsModule
{
    public void InitModule(ref World world)
    {
        world.Module<MyModule>();

        // Define components, systems, triggers, ... as usual. They will be
        // automatically created inside the scope of the module.
    }
};

// Import code
world.Import<MyModule>();
```
</li>
<li><b class="tab-title">Rust</b>

```rust
#[derive(Component)]
struct MyModule;

impl Module for MyModule {
    fn module(world: &World) {
        world.module::<MyModule>("MyModule");
        // Define components, systems, triggers, ... as usual. They will be
        // automatically created inside the scope of the module.
    }
}

// Import code
world.import::<MyModule>();
```

</li>
</ul>
</div>
