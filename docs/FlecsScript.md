# Flecs Script

## Introduction
Flecs Script is a runtime interpreted DSL for creating entities and components that is optimized for defining scenes, assets and configuration. In a nutshell, Flecs Script is to ECS what HTML/JSX is to a browser.

Some of the features of Flecs Script are:

- Native support for named entities, hierarchies and inheritance
- Assign component values
- Expressions and variables (`$var + 10`)
- Conditionals (`if $var > 10`)
- Native integration with templates (procedural assets)

To learn Flecs Script, check out the [Tutorial](FlecsScriptTutorial.md)!

## Example

```c
using flecs.meta

struct MaxSpeed {
  value = f32
}

struct Position {
  x = f32
  y = f32
}

Prefab SpaceShip {
  MaxSpeed: {value: 100}

  cockpit {
    Position: {x: -10, y: 0}
  }
}

my_spaceship : SpaceShip {
  Position: {x: 10, y: 20}
}
```

## The Basics
This section goes over the basic syntax over Flecs Script.

### Entities
An entity is created by specifying an identifier followed by a scope. Example:

```c
my_entity {}
```

An entity scope can contain components and child entities. The following example shows how to add a child entity:

```c
my_parent {
  my_child {}
}
```

Note how a scope is also added to the child entity.

To create anonymous entities, use the `_` identifier:

```c
_ {
  my_child {} // named child with anonymous parent
}
```

### Tags
A tag can be added to an entity by simply specifying the tag's identifier in an entity scope. Example:

```c
SpaceShip {} // Define SpaceShip tag

my_entity {
  SpaceShip // Add SpaceShip to my_entity
}
```

### Pairs
Pairs are added to entities by adding them to an entity scope, just like tags:

```c
Likes {}
Pizza {}

my_entity {
  (Likes, Pizza)
}
```

### Components
Components are specified like tags, but with an additional value:

```c
my_entity {
  Position: {x: 10, y: 20}
}
```

For a component to be assignable with a value, it also needs to be described in the reflection framework.

A component can also be added without a value. This will create a default constructed component. Example:

```c
my_entity {
  Position
}
```

Components can be defined in a script:

```c
using flecs.meta

struct Position {
  x = f32
  y = f32
}

my_entity {
  Position: {x: 10, y: 20}
}
```

Components can be pairs:

```c
my_entity {
  (Start, Position): {x: 0,  y: 0}
  (Stop,  Position): {x: 10, y: 20}
}
```

### Namespacing
When referring to child entities or components, identifiers need to include the parent path as well as the entity name. Paths are provided as lists of identifiers separated by a dot (`.`):

```c
Sun.Earth {
  solarsystem.Planet
}
```

To avoid having to repeatedly type the same paths, use the `using` statement (see below).

### Singletons
To create a singleton component, use `$` as the entity identifier:

```c
$ {
  TimeOfDay: { t: 0.5 }
}
```

Multiple singleton components can be specified in the same scope:

```c
$ {
  TimeOfDay: { t: 0.5 }
  Player: { name: "bob" }
}

```

### Entity kinds
An entity can be created with a "kind", which is a component specified before the entity name. This is similar to adding a tag or component in a scope, but can provide a more natural way to describe things. For example:

```c
SpaceShip my_spaceship {}
```

This is equivalent to doing:

```c
my_spaceship {
  SpaceShip
}
```

When using the entity kind syntax, the scope is optional:

```c
SpaceShip my_spaceship // no {}
```

If the specified kind is a component, a value can be specified between parentheses:

```c
CheckBox my_checkbox(checked: true)
```

When the entity kind is a component, a value will always be assigned even if none is specified. This is different from component assignments in a scope. Example:

```c
CheckBox my_checkbox(checked: true)

// is equivalent to

my_checkbox {
  CheckBox: {checked: true}
}
```

```c
CheckBox my_checkbox

// is equivalent to

my_checkbox {
  CheckBox: {}
}
```

#### Builtin kinds
Applications can specify the following builtin kinds which provide convenience shortcuts to commonly used features:

```c
prefab SpaceShip

// is equivalent to

Prefab spaceship
```

```c
prefab SpaceShip {
  slot CockPit
}

// is equivalent to

prefab SpaceShip {
  CockPit {
    (SlotOf, SpaceShip)
  }
}
```

### Inheritance
Scripts can natively specify inheritance relationships between entities, which is useful in particular for prefabs. Example:

```c
Prefab SpaceShip {
  MaxSpeed: {value: 100}
}

my_spaceship : SpaceShip {}
```

When specifying inheritance, the scope is optional:

```c
my_spaceship : SpaceShip // no {}
```

This is equivalent to doing:

```c
my_spaceship {
  (IsA, SpaceShip)
}
```

### Relationship hierarchies
By default entity hierarchies are created with the `ChildOf` relationship. Other relationships can also be used to create hierarchies by combining a pair with a scope. Example:

```c
(IsA, Thing) {
  (IsA, Organism) {
    (IsA, Plant) {
      Tree {}
    }
    (IsA, Animal) {
      Human {}
    }
  }
}
```

## Advanced Features

### Module statement
The `module` statement puts all contents of a script in a module. Example:

```c
module components.transform

// Creates components.transform.Position
struct Position {
  x = f32
  y = f32
}
```

The `components.transform` entity will be created with the `Module` tag.

### Using statement
The `using` keyword imports a namespace into the current namespace. Example:

```c
// Without using
flecs.meta.struct Position {
  x = flecs.meta.f32
  y = flecs.meta.f32
}
```
```c
// With using
using flecs.meta

struct Position {
  x = f32
  y = f32
}
```

The `using` keyword only applies to the scope in which it is specified. Example:

```c
// Scope without using
my_engine {
  game.engines.FtlEngine: {active: true}
}
```
```c
// Scope with using
my_spaceship {
  using game.engines

  FtlEngine: {active: true}
}
```

A `using` statement may end with a wildcard (`*`). This will import all namespaces matching the path. Example:

```c
using flecs.*

struct Position {
  x = f32
  y = f32
}
```

### With statement
When you're building a scene or asset you may find yourself often repeating the same components for multiple entities. To avoid this, a `with` statement can be used. For example:

```c
with SpaceShip {
  MillenniumFalcon {}
  UssEnterprise {}
  UssVoyager {}
  Rocinante {}
}
```

This is equivalent to doing:

```c
MillenniumFalcon {
  SpaceShip
}

UssEnterprise {
  SpaceShip
}

UssVoyager {
  SpaceShip
}

Rocinante {
  SpaceShip
}
```

With statements can contain multiple tags:

```c
with SpaceShip, HasWeapons {
  MillenniumFalcon {}
  UssEnterprise {}
  UssVoyager {}
  Rocinante {}
}
```

With statements can contain component values, specified between parentheses:

```c
with Color(38, 25, 13) {
  pillar_1 {}
  pillar_2 {}
  pillar_3 {}
}
```

### Variables
Scripts can contain variables, which are useful for often repeated values. Variables are created with the `const` keyword. Example:

```c
const pi = 3.1415926

my_entity {
  Rotation: {angle: $pi}
}
```

Variables can be combined with expressions:

```c
const pi = 3.1415926
const pi_2 = $pi * 2

my_entity {
  Rotation: {angle: $pi / 2}
}
```

In the above examples, the type of the variable is inferred. Variables can also be provided with an explicit type:

```c
const wood = Color: {38, 25, 13}
```

Variables can be used in component values as shown in the previous examples, or can be used directly as component. Example:

```c
const wood = Color: {38, 25, 13}

my_entity {
  $wood
}

// is equivalent to

my_entity {
  Color: {38, 25, 13}
}
```

Additionally, variables can also be used in combination with `with` statements:

```c
const wood = Color: {38, 25, 13}

with $color {
  pillar_1 {}
  pillar_2 {}
  pillar_3 {}
}
```

### Component values
A script can use the value of a component that is looked up on a specific entity. The following example fetches the `width` and `depth` members from the `Level` component, that is fetched from the `Game` entity:

```c
grid {
  Grid: { Game[Level].width, Game[Level].depth }
}
```

To reduce the number of component lookups in a script, the component value can be stored in a variable:

```c
const level = Game[Level]

tiles {
  Grid: { width: $level.width, $level.depth, prefab: Tile }
}
```

The requested component is stored by value, not by reference. Adding or removing components to the entity will not invalidate the component data. If the requested component does not exist on the entity, script execution will fail.

### If statement
Parts of a script can be conditionally executed with an if statement. Example:

```c
const daytime = bool: false

lantern {
  Color: {210, 255, 200}

  if $daytime {
    Emissive: { value: 0 }
  } else {
    Emissive: { value: 1 }
  }
}
```

### Default components
A scope can have a default component, which means entities in that scope can assign values of that component without having to specify the component name. 

There are different ways to specify a default component. One way is to use a `with` statement. Default component values are assigned with the `=` operator, and don't need a `{}` surrounding the value. Example:

```c
with Position {
  ent_a = 10, 20
  ent_b = 20, 30
}
```

Another way a default components are derived is from the entity kind. If an entity is specified with a kind, a `DefaultChildComponent` component will be looked up on the kind to find the default component for the scope, if any. For example:

```c
// Create a PositionList tag with a DefaultChildComponent
PositionList {
  DefaultChildComponent: {Position}
}

// Derive default component for scope from PositionList
PositionList plist {
  ent_a = 10, 20
  ent_b = 10, 20
  ent_c = 10, 20
}
```

A common use of default components is when creating structs. `struct` is a component with `member` as default child component. Example:

```c
struct Position {
  x = f32
  y = f32
}

// is equivalent to

struct Position {
  member x(f32)
  member y(f32)
}
```

Note how `member` is also used as kind for the children. This means that children of `x` and `y` derive their default child component from `member`, which is set to `member`. This makes it easy to create nested members:

```c
struct Line {
  start {
    x = f32
    y = f32
  }
  stop {
    x = f32
    y = f32
  }
}

// is equivalent to

struct Line {
  member start {
    member x(f32)
    member y(f32)
  }
  member stop {
    member x(f32)
    member y(f32)
  }
}
```

### Semicolon operator
Multiple statements can be combined on a single line when using the semicolon operator. Example:

```c
my_spaceship {
  SpaceShip; HasFtl
}
```

## Comma operator
The comma operator can be used as a shortcut to create multiple entities in a scope. Example:

```c
my_spaceship {
  pilot_a,
  pilot_b,
  pilot_c
}

// is equivalent to

my_spaceship {
  pilot_a {}
  pilot_b {}
  pilot_c {}
}
```

This allows for a more natural way to describe things like enum types:

```c
enum Color {
  Red,
  Green,
  Blue
}

// is equivalent to

enum Color {
  constant Red
  constant Green
  constant Blue
}
```

## Templates
Templates are parametrized scripts that can be used to create procedural assets. Templates can be created with the `template` keyword. Example:

```c
template Square {
  Color: {255, 0, 0}
  Rectangle: {width: 100, height: 100}
}
```

The script contents of an template are not ran immediately. Instead they are ran whenever an template is _instantiated_. To instantiate an template, add it as a regular component to an entity:

```c
my_entity {
  Square
}

// is equivalent to

my_entity {
  Color: {255, 0, 0}
  Rectangle: {width: 100, height: 100}
}
```

Templates are commonly used in combination with the kind syntax:

```c
Square my_entity
```

Templates can be parametrized with properties. Properties are variables that are exposed as component members. To create a property, use the `prop` keyword. Example:

```c
template Square {
  prop size = i32: 10
  prop color = Color: {255, 0, 0}

  $color
  Rectangle: {width: $size, height: $size}
}

Square my_entity(size: 20, color: {38, 25, 13})
```

Template scripts can do anything a regular script can do, including creating child entities. The following example shows how to create an template that uses a nested template to create children:

```c
template Tree {
  prop height = f32: 10

  const wood_color = Color: {38, 25, 13}
  const leaves_color = Color: {51, 76, 38}

  const canopy_height = 2
  const trunk_height = $height - $canopy_height
  const trunk_width = 2

  Trunk {
    Position: {0, ($height / 2), 0}
    Rectangle: {$trunk_width, $trunk_height}
    $wood_color
  }

  Canopy {
    const canopy_y = $trunk_height + ($canopy_height / 2)

    Position3: {0, $canopy_y, 0}
    Box: {$canopy_width, $canopy_height}
    $leaves_color
  }
}

template Forest {
  Tree(height: 5) {
    Position: {x: -10}
  }

  Tree(height: 10) {
    Position: {x: 0}
  }

  Tree(height: 7) {
    Position: {x: 10}
  }
}

Forest my_forest
```

## API
This section goes over how to run scripts in an application.

### Run once
To run a script once, use the `ecs_script_run` function. Example:

```c
const char *code = "my_spaceship {}";

if (ecs_script_run(world, "my_script_name", code)) {
  // error
}
```

Alternatively a script can be ran directly from a file:

```c
if (ecs_script_run_file(world, "my_script.flecs")) {
  // error
}
```

If a script fails, the entities created by the script will not be automatically deleted. When a script contains templates, script resources will not get cleaned up until the entities associated with the templates are deleted.

### Run multiple times
A script can be ran multiple times by using the `ecs_script_parse` and `ecs_script_eval` functions. Example:

```c
const char *code = "my_spaceship {}";

ecs_script_t *script = ecs_script_parse(
  world, "my_script_name", code); 
if (!script) {
  // error
}

if (ecs_script_eval(script)) {
  // error
}

// Run again
if (ecs_script_eval(script)) {
}

// Free script resources
ecs_script_free(script);
```

If a script fails, the entities created by the script will not be automatically deleted. When a script contains templates, script resources will not get cleaned up until the entities associated with the templates are deleted.

### Managed script
Managed scripts are scripts that are associated with an entity, and can be ran multiple times. Entities created by a managed script are tagged with the script. When script execution fails, the entities associated with the script will be deleted. Additionally, if after executing the script again an entity is no longer created by the script, it will also be deleted.

To run a managed script, do:

```c
const char *code = "my_spaceship {}";

ecs_entity_t s = ecs_script(world, {
  .code = code
});

if (!s) {
  // error
}
```

To update the code of a managed script, use the `ecs_script_update` function:

```c
if (ecs_script_update(world, s, 0, new_code)) {
  // error
}
```

When a script contains templates, script resources will not get cleaned up until the entities associated with the templates are deleted.
