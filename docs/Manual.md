# Manual

## Introduction
Nobody likes to read manuals, and you should be able to get up and running with Flecs by using the quickstart, by looking at examples and by checking the documentation in the flecs header files. However, if you truly want to know how something works, or why it works that way, the manual is the right place to go. With that said, the manual is not exhaustive, and it complements the other sources of documentation.

## Design Goals

### 1. Performance
Flecs is designed from the ground up to provide blazing fast iteration speeds in systems that can be vectorized by default, while minimizing cache misses. In addition, Flecs has a unique graph-based storage engine that allows for extremely fast add, remove and bulk operations. These features, amongst others, ensure that applications can get the most out of the underlying hardware.

### 2. Portability
Flecs has been implemented in C99 and features an external interface that is C89 compatible to ensure it is portable to a wide range of platforms. The framework contains a flexible operating system abstraction API that enables an application to easily port the library to new platforms.

### 3. Reusability
ECS has the potential for being a platform for the development of reusable, loosely coupled, plug and play features like input, physics and rendering. Flecs modules enable such features to be packaged in a loosely coupled way so that applications can simply import them, while guaranteeing a correct execution order. In addition, Flecs has features like time management that ensure a consistent baseline across modules.

### 4. Usability
Flecs is designed first and foremost to be a framework that simplifies the development of games and simulations. Rather than just providing a vanilla ECS implementation, Flecs provides many features that are commonly found in game development frameworks such as hierarchies, prefabs and time management, all integrated seamlessly with the core ECS system.

### 5. Extensibility
Flecs is used with other frameworks and game engines, and as such not all of its features are useful in each application. For that reason Flecs has a modular design, so that applications can easily remove features from the core that they do not need. Additionally, since many features are built on top of the ECS core, applications can easily extend or reimplement them.

### 6. Have fun!
There are few things as satisfying as building games. If nothing else, Flecs has been built to enable creative visions both big and small. I'm having a lot of fun building Flecs, I hope you will have fun using it, and that your users will have fun playing your games :)

## API design

### Naming conventions

```c
// Component names ('Position') use PascalCase
typedef struct Position {
    float x;
    float y; // Component members ('y') use snake_case
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

// System names ('Move') use PascalCase. API types use snake_case_t
void Move(ecs_iter_t *it) {
    // Functions use snake_case
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();

    // Declarative function-style macros use SCREAMING_SNAKE_CASE
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    // Module names are PascalCase
    ECS_IMPORT(world, MyModule);

    // Enumeration constants ('EcsOnUpdate') use PascalCase
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    // Function wrapper macros use snake_case
    ecs_entity_t e = ecs_new(world);

    // Builtin entities use PascalCase
    ecs_add(world, EcsWorld, Position);

    return ecs_fini(world);
}
```

### Idempotence
Many operations in the Flecs API are idempotent, meaning that invoking an operation once has the same effect as invoking an operation multiple times with the same parameters. For example:

```c
ecs_add(world, e, Position);
```

Has the same effect as:

```c
ecs_add(world, e, Position);
ecs_add(world, e, Position);
```

This simplifies application code as it can be written in a declarative style, where the only thing that matters is that after the operation has been invoked, the post condition of the operation is satisfied.

Some operations are idempotent but have side effects, like `ecs_set`:

```c
ecs_set(world, e, Position, {10, 20});
ecs_set(world, e, Position, {10, 20});
```

The effect of invoking this operation once is the same as invoking the operation multiple times, but both invocations can invoke an OnSet observer which can introduce side effects.

All declarative macros (`ECS_COMPONENT`, `ECS_SYSTEM`, ...) are idempotent:

```c
{
    ECS_COMPONENT(world, Position);
}
{
    ECS_COMPONENT(world, Position);
}
```

The second time the `ECS_COMPONENT` macro is evaluated, the first instance will be found and returned. Note that because these macros may declare variables, they cannot be defined twice in the same C scope.

### Error handling
As a result of the idempotent design of many operations, the API has a very small error surface. There are essentially two conditions under which an operation is unable to fulfill its postcondition:

- The application provides invalid inputs to an operation
- The operating system is unable to fulfill a request, like a failure to allocate memory

When either of those conditions occur, the library will throw an assertion in debug mode (the source is not compiled with `NDEBUG`). Except for errors caused by the OS, errors are almost always caused by the invocation of a single operation, which makes applications easy to debug.

This approach has several advantages. Application code does not need to check for errors. If an error occurs, the assertion will cause application execution to halt. As a result of this, application code is cleaner and more robust, as it is impossible to forget to handle an error condition.

### Memory ownership
Most of the API is handle based, as many API constructs are implemented using entities. There are a few instances where an application will interface with memory managed by the framework, or when an application needs to provide memory it manages to the API. In these scenarios there are four rules:

- If an operation accepts a `const T*`, the application retains ownership of the memory
- If an operation accepts a `T*`, ownership is transferred from application to framework
- If an operation returns a `const T*`, the framework retains ownership of the memory
- If an operation returns a `T*`, ownership is transferred from framework to application

The `ecs_get_name` operation is an example where the framework retains ownership:

```c
const char *name = ecs_get_name(world, e);
```

The `ecs_get_path` operation is an example where the ownership is transferred to the application:

```c
char *path = ecs_get_path(world, e);
```

Memory for which ownership has been transferred to the application will need to be freed by the application. This should be done by the `ecs_os_free` operation:

```c
ecs_os_free(path);
```

### Macros
The C99 API heavily relies on function-style macros, probably more than you would see in other libraries. The number one reason for this is that an ECS framework needs to work with user-defined types, and C does not provide out of the box support for generics. A few strategies have been employed in the API to improve its overall ergonomics, type safety and readability. Let's start with a simple example:

```c
typedef struct Position {
    float x;
    float y;
} Position;

ECS_COMPONENT(world, Position);

ecs_entity_t e = ecs_new_w(world, Position);
```

From a readability perspective this code looks fine as we can easily tell what is happening here. Though if we take a closer look, we can see that a typename is used where we expect an expression, and that is not possible in plain C. So what is going on?

Let's first remove the `ECS_COMPONENT` macro and replace it with equivalent code (details are omitted for brevity):

```c
ecs_entity_t ecs_id(Position) = ecs_component_init(world, &(ecs_component_desc_t){
    .entity.name = "Position",
    .size = sizeof(Position),
    .alignment = ECS_ALIGNOF(Position)
});
```

The first line actually registers the component with Flecs, and captures its name and size. The result is stored in a variable with name `ecs_id(Position)`. Here, `ecs_id` is a macro that translates the typename of the component to a variable name. The actual name of the variable is:

```c
FLECS__EPosition
```

ECS operations that accept a typename, such as `ecs_get` will look for the `FLECS__E` variable:
```c
Position *p = ecs_get(world, e, Position);
```

Translates into:

```c
Position *p = (Position*)ecs_get_id(world, e, ecs_id(Position));
```

As you can see, the `ecs_get` macro casts the result of the function to the correct type, so a compiler will throw a warning when an application tries to assign the result of the operation to a variable of the wrong type.

Similarly, `ecs_set` is a macro that ensures that anything we pass into it is of the right type:

```c
ecs_set(world, e, Position, {10, 20});
```

Translates into:

```c
ecs_set_id
    (world, e, ecs_id(Position), sizeof(Position),
    &(Position){10, 20});
```

In addition to casting the value to the right type and passing in the component, this macro also captures the size of the type, which saves Flecs from having to do a component data lookup.

Understanding how the macros work will go a long way in being able to write effective code in Flecs, and will lead to less surprises when debugging the code.

## Modules
Modules allow an application to split up systems and components into separate decoupled units. The purpose of modules is to make it easier to organize systems and components for large projects. Additionally, modules also make it easier to split off functionality into separate compilation units.

A module consists out of a couple parts:

- The declaration of the components, tags, and systems of the module contents
- An import function that loads the module contents for a world

The declaration of the module contents module is typically located in a separate module header file, and look like this for a module named "Vehicles":

```c
typedef struct Car {
    float speed;
} Car;

typedef struct Bus {
    float speed;
} Bus;

typedef struct MotorCycle {
    float speed;
} MotorCycle;

/* Components are declared with ECS_COMPONENT_DECLARE */
extern ECS_COMPONENT_DECLARE(Car);
extern ECS_COMPONENT_DECLARE(Bus);
extern ECS_COMPONENT_DECLARE(MotorCycle);

/* Tags are declared with ECS_DECLARE */
extern ECS_DECLARE(Moving);

/* Systems are declared with ECS_SYSTEM_DECLARE */
extern ECS_SYSTEM_DECLARE(Move);

/* Forward declaration to the import function */
/* The function name has to follow the convention: <ModuleName>Import */
void VehiclesImport(ecs_world_t *world);
```

The import function for this module is typically located in a module source file, and would look like this:

```c
void VehiclesImport(ecs_world_t *world) {
    /* Define the module */
    ECS_MODULE(world, Vehicles);

    /* Declare components, tags, systems, and assign them to the module with their respective _DEFINE macros */
    ECS_COMPONENT_DEFINE(world, Car);
    ECS_COMPONENT_DEFINE(world, Bus);
    ECS_COMPONENT_DEFINE(world, MotorCycle);
    ECS_TAG_DEFINE(world, Moving);
    ECS_SYSTEM_DEFINE(world, Move, EcsOnUpdate, Car, Moving);
}
```

After the module has been defined, it can be imported in an application like this:

```c
ecs_world_t *world = ecs_init();

/* Import module, which invokes the module import function */
ECS_IMPORT(world, Vehicles);

/* The module contents can now be used */
ecs_entity_t e = ecs_new_w(world, Car);
```

Module contents are namespaced, which means that the identifiers of the content of the module (components, tags, systems) are stored in the scope of the module. For the above example module, everything would be stored in the `vehicles` scope. To resolve the `Car` component by name, an application would have to do:

```c
ecs_entity_t car_entity = ecs_lookup(world, "vehicles.Car");
```

Note that even though the module name is specified with uppercase, the name is stored with lowercase. This is because the naming convention for modules in C is PascalCase, whereas the stored identifiers use snake_case. If a module name contains several uppercase letters, this will be translated to a nested module. For example, the C module name `MySimpleModule` will be translated to `my.simple.module`.

### Modules in C++
A module in C++ is defined as a class where the module contents are defined in the constructor. The above Vehicles module would look like this in C++:

```cpp
/* In C++ tags can be defined as empty structs */
struct Moving { };

/* Module implementation */
struct vehicles {
    vehicles(flecs::world& world) {
        world.component<Car>();
        world.component<Bus>();
        world.component<MotorCycle>();
        world.component<Moving>();

        world.system<Car, Moving>("Move")
            .each([](flecs::entity e, Car& car, Moving) {
                /* System implementation */
            });
    }
}
```

An application can import the module in C++ like this:

```cpp
flecs::world world;
world.import<vehicles>();
```

## Deferred operations
Applications can defer entity with the `ecs_defer_begin` and `ecs_defer_end` functions. This records all operations that happen inside the begin - end block, and executes them when `ecs_defer_end` is called. Deferred operations are useful when an application wants to make modifications to an entity while iterating, as doing this without deferring an operation could modify the underlying data structure. An example:

```c
ecs_defer_begin(world);
    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_set(world, e, Velocity, {1, 1});
ecs_defer_end(world);
```

The effects of these operations will not be visible until the `ecs_defer_end` operation.

There are a few things to keep in mind when deferring:
- creating a new entity will always return a new id which increases the last used id counter of the world
- `ecs_ensure` returns a pointer initialized with the current component value, and does not take into account deferred set or ensure operations
- if an operation is called on an entity which was deleted while deferred, the operation will ignored by `ecs_defer_end`
- if a child entity is created for a deleted parent while deferred, the child entity will be deleted by `ecs_defer_end`

