# Flecs Script

## Introduction
Flecs Script is a runtime interpreted DSL for creating entities and components that is optimized for defining scenes, assets and configuration. In a nutshell, Flecs Script is to ECS what HTML/JSX is to a browser.

Some of the features of Flecs Script are:

- Native support for named entities, hierarchies and inheritance
- Assign component values
- Expressions and variables (`var + 10`)
- Conditionals and loops (`if var > 10`, `for i in [0..10]`)
- Native integration with templates (procedural assets)

## Example

```cpp
struct MaxSpeed(value: f32)

struct Position(x: f32, y: f32)

prefab SpaceShip {
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

```cpp
my_entity {}
```

An entity scope can contain components and child entities. The following example shows how to add a child entity:

```cpp
my_parent {
  my_child {}
}
```

Note how a scope is also added to the child entity.

To create anonymous entities, leave out the entity name:

```cpp
{
  my_child {} // named child with anonymous parent
}
```

Alternatively, the `_` placeholder can be used to indicate an anomyous entity:

```cpp
_ {
  my_child {} // named child with anonymous parent
}
```

The `_` placeholder can be useful in combination with syntax constructs that require an identifier token, such as inheritance:

```cpp
// anonymous entity that inherits from SpaceShip
_ : SpaceShip { }
```

Entity names can be specified using a string. This allows for entities with names that contain special characters, like spaces:

```cpp
"my parent" {
  "my child" {}
}
```

String names can be combined with string interpolation (see below) to create names that are computed when the script is evaluated:

```cpp
"USS_$name" {}
```

By default children are created using the `ChildOf` hierarchy storage. To select the `Parent` hierarchy storage, add the `tree` annotation:

```cpp
@tree Parent
my_parent {
  my_child {}
}
```

### Tags
A tag can be added to an entity by simply specifying the tag's identifier in an entity scope. Example:

```cpp
SpaceShip {} // Define SpaceShip tag

my_entity {
  SpaceShip // Add SpaceShip to my_entity
}
```

### Pairs
Pairs are added to entities by adding them to an entity scope, just like tags:

```cpp
Likes {}
Pizza {}

my_entity {
  (Likes, Pizza)
}
```

### Components
Components are specified like tags, but with an additional value:

```cpp
my_entity {
  Position: {x: 10, y: 20}
}
```

For a component to be assignable with a value, it also needs to be described in the reflection framework.

A component can also be added without a value. This will create a default constructed component. Example:

```cpp
my_entity {
  Position
}
```

The value after the `:` is an expression. For components that hold a single value, such as a scalar type, the value can be assigned directly without curly braces:

```cpp
my_entity {
  Mass: 100
  Mass: 50 + 50
  Mass: weight
}
```

Components can be defined in a script (see [Type definitions](#type-definitions)):

```cpp
struct Position(x: f32, y: f32)

my_entity {
  Position: {x: 10, y: 20}
}
```

Components can be pairs:

```cpp
my_entity {
  (Start, Position): {x: 0,  y: 0}
  (Stop,  Position): {x: 10, y: 20}
}
```

### Namespacing
When referring to child entities or components, identifiers need to include the parent path as well as the entity name. Paths are provided as lists of identifiers separated by a dot (`.`):

```cpp
Sun {
  Earth {
    solarsystem.Planet
  }
}
```

To avoid having to repeatedly type the same paths, use the `using` statement (see below).

### Singletons
To create a singleton component, use `$` as the entity identifier:

```cpp
$ {
  TimeOfDay: { t: 0.5 }
}
```

Multiple singleton components can be specified in the same scope:

```cpp
$ {
  TimeOfDay: { t: 0.5 }
  Player: { name: "bob" }
}
```

### Entity kinds
An entity can be created with a "kind", which is a component specified before the entity name. This is similar to adding a tag or component in a scope, but can provide a more natural way to describe things. For example:

```cpp
SpaceShip my_spaceship {}
```

This is equivalent to doing:

```cpp
my_spaceship {
  SpaceShip
}
```

When using the entity kind syntax, the scope is optional:

```cpp
SpaceShip my_spaceship // no {}
```

If the specified kind is a component, a value can be specified between parentheses:

```cpp
CheckBox my_checkbox(checked: true)
```

When the entity kind is a component, a value will always be assigned even if none is specified. This is different from component assignments in a scope. Example:

```cpp
CheckBox my_checkbox(checked: true)

// is equivalent to

my_checkbox {
  CheckBox: {checked: true}
}
```

```cpp
CheckBox my_checkbox

// is equivalent to

my_checkbox {
  CheckBox: {}
}
```

### Newlines
Newlines function as statement separators. Multiple statements can be combined on a single line with the semicolon (`;`) operator:

```cpp
e {
  Position: {10, 20}; Velocity: {1, 2}
}
```

Newlines after opening a scope (`{`) or before closing a scope (`}`) are not mandatory:

```cpp
e { Position: {10, 20}; Velocity: {1, 2} }
```

#### Builtin kinds
Applications can specify the following builtin kinds which provide convenience shortcuts to commonly used features:

```cpp
prefab SpaceShip

// is equivalent to

Prefab spaceship
```

### Inheritance
Scripts can natively specify inheritance relationships between entities, which is useful in particular for prefabs. Example:

```cpp
prefab SpaceShip {
  MaxSpeed: {value: 100}
}

my_spaceship : SpaceShip {}
```

The `:` notation is short for adding an `IsA` relationship with the relationship syntax:

```cpp
my_spaceship {
  (IsA, SpaceShip)
}
```

### Relationship hierarchies
By default entity hierarchies are created with the `ChildOf` relationship. Other relationships can also be used to create hierarchies by combining a pair with a scope. Example:

```cpp
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

## Expressions
Scripts can contain expressions, which allow for computing values from inputs such as component values, template properties and variables. Here are some examples of valid Flecs script expressions:

```cpp
const x = 10 + 20 * 30
const x = 10 * (20 + 30)
const x = var * 10
const x = pow(var, 2)
const x = e.parent().name()
const x: Position = {10, 20}
const x: Position = {x: 10, y: 20}
```

The following sections describe the different features of expressions.

### Operators
The following operators are supported in expressions, in order of precedence:

| **Symbol** | **Description**        | **Example**            |
|------------|------------------------|------------------------|
| `!`        | Logical NOT            | `!10`                  |
| `*`        | Multiplication         | `10 * 20`              |
| `/`        | Division               | `10 / 20`              |
| `%`        | Modulus                | `10 % 3`               |
| `+`        | Addition               | `10 + 20`              |
| `-`        | Subtraction/negative   | `10 - 20`, `-(10, 20)` |
| `<<`       | Bitwise left shift     | `10 << 1`              |
| `>>`       | Bitwise right shift    | `10 >> 1`              |
| `>`        | Greater than           | `10 > 20`              |
| `>=`       | Greater than or equal  | `10 >= 20`             |
| `<`        | Less than              | `10 < 20`              |
| `<=`       | Less than or equal     | `10 <= 20`             |
| `==`       | Equality               | `10 == 20`             |
| `!=`       | Not equal              | `10 != 20`             |
| `&`        | Bitwise AND            | `2 & 6`                |
| `\|`       | Bitwise OR             | `2 \| 4`               |
| `&&`       | Logical AND            | `true && false`        |
| `\|\|`     | Logical OR             | `true \|\| false`      |

### Values
The following table lists the different kinds of values that are supported in expressions:

| **Value kind**            | **Type**      | **Example**                       |
|---------------------------|---------------|-----------------------------------|
| **Integer**               | `i64`         | `42`, `-100`, `0`, `0x1A`         |
| **Floating Point**        | `f64`         | `3.14`, `-2.718`, `1e6`, `0.0`    |
| **String**                | `string`      | `"Hello, World!"`, `"123"`, `""`  |
| **Multiline string**      | `string`      | \``Hello World`\`                 |
| **Entity**                | `entity`      | `spaceship`, `spaceship.pilot`    |
| **Enum/Bitmask values**   | from lvalue   | `Red`, `Blue`, `Lettuce \| Bacon` |
| **Composites**            | from lvalue   | `{x: 10, y: 20}`, `{10, 20}`      |
| **Collections**           | from lvalue   | `[1, 2, 3]`                       |

#### Initializers
Initializers are values that are used to initialize composite and collection members. Composite values are initialized by initializers that are delimited by `{}`, while collection initializers are delimited by `[]`. Furthermore, composite initializers can specify which member of the composite value should be initialized. Here are some examples of initializer expressions:

```cpp
{}
{10, 20}
{x: 10, y: 20}
{{10, 20}, {30, 40}}
{start: {x: 10, y: 20}, stop: {x: 10, y: 20}}
[10, 20, 30]
[{10, 20}, {30, 40}, {50, 60}]
{x += 10, y *= 2}
```

Composite initializers must always be assigned to an lvalue of a well defined type. This can either be a typed variable, component assignment, function parameter or in the case of nested initializers, an element of another initializer. For example, this is a valid usage of an initializer:

```cpp
const x: Position = {10, 20}
```

while this is an invalid usage of an initializer:

```cpp
// Invalid, unknown type for initializer
const x = {10, 20}
```

Collection initializers do not require a well defined type (see vector literals).

### Vector literals
When a collection initializer is not assigned to an lvalue of a well defined type, it evaluates to a vector. The element type of the vector is derived from the initializer elements, where the most expressive element type determines the vector type:

```cpp
const a = [10, 20, 30]        // vector<i64>
const b = [10, 10.5, 20]      // vector<f64>
const c = ["foo", "bar"]      // vector<string>
```

Element types that cannot be implicitly converted to each other, such as numbers and strings, cannot be mixed in the same vector literal.

Ranges can also be assigned, in which case they materialize into a vector with the values in the range (the end of the range is exclusive):

```cpp
const v = [1 .. 5]            // vector<i32> [1, 2, 3, 4]
```

When assigning variables to elements in a composite initializer, applications can use the following shorthand notation if the variable names are the same as the member name of the element:

```cpp
// Normal notation
Tree: {color: color, height: height}

// Shorthand notation
Tree: {color: $, height: $}
```

Initializer expressions may contain add assignment (`+=`) or multiply assignment (`*=`) operators. These operators allow an initializer to modify an existing value. An example:

```cpp
e {
  Position: {10, 20}
  Position: {x += 1, y += 2}
}

// e will have Position{11, 22}
```

This can be especially useful when used in combination with templates (see below):

```cpp
template Tree {
  prop height: f32 = 4

  // Make sure tree doesn't sink through the ground
  Position: {y += height} 
}

e {
  Position: {10, 0}
  Tree: {height: 3}
}

// e will have Position{10, 3}
```

### Match expressions
Match expressions can be used to conditionally assign a value. An example:

```cpp
const x = 1

// y will be assigned with value 10
const y = match x {
  1: 10
  2: 20
  3: 30
}
```

The input to a match expression must be matched by one of its cases. If the input is not matched, script execution will fail. Match expressions can include an "any" case, which is selected when none of the other cases match:

```cpp
const x = 4

// y will be assigned with value 100
const y = match x {
  1: 10
  2: 20
  3: 30
  _: 100
}
```

Match expressions can be used to assign components:

```cpp
e {
  Position: match i {
    1: {10, 20}
    2: {20, 30}
    3: {40, 50}
  }
}
```

A case is terminated by a newline, a `;`, or the closing `}` of the match expression. This makes it possible to write a match expression on a single line, as long as every case that is not the last one is terminated with a `;`:

```cpp
const y = match x { 1: 10; 2: 20; 3: 30 }
```

The type of a match expression is derived from the case values. When the case statements in a match contain values of multiple types, the most expressive type is selected. The algorithm for determining the most expressive type is the same as the one used to determine the type for binary expressions. When a match expression contains values with conflicting types, script execution will fail.

### Component values
An expression can use the value of a component that is looked up on a specific entity. The following example fetches the `width` and `depth` members from the `Level` component, that is fetched from the `Game` entity:

```cpp
grid {
  Grid: { Game[Level].width, Game[Level].depth }
}
```

To reduce the number of component lookups in a script, the component value can be stored in a variable:

```cpp
const level = Game[Level]

tiles {
  Grid: { width: level.width, level.depth, prefab: Tile }
}
```

### New expressions
A new expression is the `new` keyword followed by an entity statement. New expressions can be used to create entities inside of expressions. The following are examples of valid new expressions:

```cpp
// Create a new anonymous entity, assign to variable x
const x = new {}

// Create a new anonymous entity with Position component, assign to variable x
const x = new {
  Position: {10, 20}
}

// Create a new entity with name Foo and Position component, assign to variable x
const x = new Foo {
  Position: {10, 20}
}
```

New expressions can be used anywhere where an expression of an entity type is expected. The following example shows how to use a new expression inside of an initializer:

```cpp
// Create entity with TrafficLight component which has red, orange and green
// members of type entity.
e {
  TrafficLight: {
    red: new { Color: {255, 0, 0} }
    orange: new { Color: {255, 128, 0} }
    green: new { Color: {0, 255, 0} }
  }
}
```

The behavior of new expressions is exactly the same as entity statements in that they respect the context in which they are used, such as the current hierarchy scope and `with` statements:

```cpp
some_parent {
  // Create new anonymous child of some_parent with Position component, assign 
  // to variable x
  const x = new { Position: {10, 20} }
}

with Position(10, 20) {
  // Create new anonymous entity with Position: {10, 20}
  const x = new { }
}
```

All features that are supported by entity statements are also available for new expressions, such as the ability to have children:

```cpp
const x = new {
  Positiion: {10, 20}

  // Child of anonymous entity created by new expression
  child {
    Position: {20, 30}
  }
}
```

The primary use case for new expressions is to make it possible to create anonymous entities that can be referred to afterwards by a script. Without new expressions this is not possible, as illustrated here:

```cpp
// Create anonymous entities
{
  Color: {255, 0, 0}
}
{
  Color: {255, 128, 0}
}
{
  Color: {0, 255, 0}
}

e {
  TrafficLight: {
    // Can't refer to anonymous entities here
    red: // ???
    orange: // ???
    green: // ???
  }
}
```

Without new expressions the only workaround is to use named entities, but this introduces overhead and increases memory footprint. With new expressions the example can be expressed with just anonymous entities:

```cpp
// Create anonymous entities, store in variables
const red = new {
  Color: {255, 0, 0}
}

const orange = new {
  Color: {255, 128, 0}
}

const green = new {
  Color: {0, 255, 0}
}

e {
  TrafficLight: {
    // Assign variables to members
    red: red
    orange: orange
    green: green
  }
}
```

A new expression may only create a single entity.

### String interpolation
Flecs script supports interpolated strings, which are strings that can contain expressions. String interpolation supports two forms, where one allows for easy embedding of variables, whereas the other allows for embedding any kind of expression. The following example shows an embedded variable:

```cpp
const x = "The value of PI is $PI"
```

The following example shows how to use an expression:

```cpp
const x = "The circumference of the circle is {2 * $PI * $r}"
```

To prevent evaluating expressions in an interpolated string, the `$` and `{` characters can be escaped:

```cpp
const x = "The value of variable \$x is $x"
```

#### Formatting interpolated numbers
Interpolated `f32` and `f64` values can include a format specifier after the expression, separated by a colon:

```cpp
const value = 12.3456
const x = "{value:.2}" // 12.35
```

The complete syntax is:

```text
{expression:[[fill]align][+][0][width][.precision][e|E]}
```

| Part | Description |
|------|-------------|
| `fill` | Character used for padding. It must be immediately followed by an alignment character. The default is a space. |
| `<` | Align the value to the left. |
| `^` | Center the value. |
| `>` | Align the value to the right. This is the default. |
| `+` | Always include a sign, including for positive values. |
| `0` | Pad numeric values with leading zeroes. The sign, when present, is placed before the zeroes. |
| `width` | Minimum width of the formatted value. Values wider than this are not truncated. |
| `.precision` | Number of digits after the decimal point. |
| `e` | Use scientific notation with a lowercase exponent. |
| `E` | Use scientific notation with an uppercase exponent. |

For example:

```cpp
const value = 12.5
const left = "{value:*<13}" // 12.500000****
const center = "{value:*^13}" // **12.500000**
const right = "{value:*>13}" // ****12.500000
const zeroes = "{value:013}"  // 000012.500000
const sign = "{value:+}"    // +12.500000
const exp = "{value:.2e}"  // 1.25e+01
```

Width and precision can be integer literals, variables, or parenthesized expressions.

```cpp
const value = 12.3456
const width = 10
const precision = 2

const a = "{value:width}"
const c = "{value:(width + 2)}"
const d = "{value:.precision}"
const f = "{value:.(precision + 1)}"
```

Width and precision values must be between `0` and `1024`, inclusive. Values outside this range produce an error.

### Types
The type of an expression is determined by the kind of expression, its operands and the context in which the expression is evaluated. The words "type" and "component" can be used interchangeably, as every type in Flecs is a component, and every component is a type. For component types to be used with scripts, they have to be described using the meta reflection addon.

The following sections go over the different kinds of expressions and how their types are derived.

#### Unary expressions
Unary expressions have a single operand, with the operator preceding it. The following table shows the different unary operators with the expression type:

| **Operator** | **Expression Type**     |
|--------------|-------------------------|
| `!`          | `bool`                  |
| `-`          | Same as operand.        |

#### Binary expressions
Binary expressions have two operands. The following table shows the different binary operators with the expression type. The operand type is the type to which the operands must be castable for it to be a valid expression.

| **Symbol** | **Expression type**  | **Operand type**     |
|------------|----------------------|----------------------|
| `*`        | other (see below)    | Numbers              |
| `/`        | `f64`                | Numbers              |
| `+`        | other (see below)    | Numbers              |
| `-`        | other (see below)    | Numbers              |
| `%`        | `i64`                | `i64`                |
| `<<`       | other (see below)    | Integers             |
| `>>`       | other (see below)    | Integers             |
| `>`        | `bool`               | Numbers              |
| `>=`       | `bool`               | Numbers              |
| `<`        | `bool`               | Numbers              |
| `<=`       | `bool`               | Numbers              |
| `==`       | `bool`               | Values               |
| `!=`       | `bool`               | Values               |
| `&`        | other (see below)    | Integers             |
| `\|`       | other (see below)    | Integers             |
| `&&`       | `bool`               | `bool`               |
| `\|\|`     | `bool`               | `bool`               |

For the operators where the expression type is listed as "other" the type is derived by going through these steps:
- If the types of the operands are equal, the expression type will be the operand type.
- If the types are different:
  - For literal values, find the smallest storage type without losing precision. If operand types are now equal, use that.
  - Find the most expressive type of the two operands (see below)
  - If a cast to the most expressive type does not result in a loss of precision, use that.
  - If the types are both numbers follow these rules in order:
    - If one of the types is a floating point, use `f64`
    - If one of the types is an integer, use `i64`
    - If neither, throw a type incompatible error

For equality expressions (using the `==` or `!=` operators), additional rules are used:
 - If one of the operands is a bool, cast the other operand to a bool as well. This ensures that expressions such as `2 == true` evaluate to true.
 - If one of the operands is a floating point value and the other operand is a literal, the literal is cast to the floating point type of the other operand. This ensures that expressions such as `f32_value == 0.1` evaluate to true.
 - Equality expressions where both operands are floating point literals are invalid, as comparing two floating point literals for equality is usually a mistake.

Type expressiveness is determined by the kind of type and its storage size. The following tables show the expressiveness and storage scores:

| **Type**     | **Expressiveness Score** |
|--------------|---------------------------|
| bool         | 1                         |
| char         | 2                         |
| u8           | 2                         |
| u16          | 3                         |
| u32          | 4                         |
| uptr         | 5                         |
| u64          | 6                         |
| i8           | 7                         |
| i16          | 8                         |
| i32          | 9                         |
| iptr         | 10                        |
| i64          | 11                        |
| f32          | 12                        |
| f64          | 13                        |
| string       | -1                        |
| entity       | -1                        |

| **Type**     | **Storage Score** |
|--------------|-------------------|
| bool         | 1                 |
| char         | 1                 |
| u8           | 2                 |
| u16          | 3                 |
| u32          | 4                 |
| uptr         | 6                 |
| u64          | 7                 |
| i8           | 1                 |
| i16          | 2                 |
| i32          | 3                 |
| iptr         | 5                 |
| i64          | 6                 |
| f32          | 3                 |
| f64          | 4                 |
| string       | -1                |
| entity       | -1                |

The function to determine whether a type is implicitly castable is:

```cpp
bool implicit_cast_allowed(from, to) {
  if (expressiveness(to) >= expressiveness(from)) {
    return storage(to) >= storage(from);
  } else {
    return false;
  }
}
```

If either the expressiveness or storage scores are negative, the operand types are not implicitly castable.

#### Vector operations
If the left operand of a binary expression is of a vector type, the operation will be executed for each of its operands. A vector type is a type that meets the following criteria: 

- The type must be a primitive or struct type.
- If the type is a struct type:
  - All members must be of the same type.
  - The member type must be primitive.

For example:

```cpp
// Valid vector type: all members are of the same primitive type
struct Position {
  float x;
  float y;
  float z;
};

// Not a valid vector type: members are not of a primitive type
struct Line {
  Position start;
  Position stop;
};

// Not a valid vector type: not all members are of the same type
struct Rgba {
  int8_t r;
  int8_t g;
  int8_t b;
  float a;
};
```

An example of a vector operation:

```cpp
const p0: Position = {10, 20, 30}
const p1 = p0 + 1 // {11, 21, 31}
```

#### Swizzle operations
When a member is accessed on a vector type whose members all have single-letter names, and the accessed member cannot be resolved to an existing member, the accessor is interpreted as a swizzle. A swizzle builds a new value from the members that match its letters, in the order they are specified. The result obtains the type of the lvalue it is assigned to.

The members of a swizzle may appear in any order, and may be repeated. For a type with members `r`, `g`, `b`, the swizzles `rgb`, `bgr`, `rrr` and `bb` are all valid.

For example:

```cpp
const p: Position = {10, 20, 30}
e {
  // Swizzle desugars to {p.z, p.y, p.x}
  Velocity: p.zyx // {30, 20, 10}
}
```

#### Lvalues
Lvalues are the left side of assignments. There are two kinds of assignments possible in Flecs script:
- Variable initialization
- Initializer initialization

The type of an expression can be influenced by the type of the lvalue it is assigned to. For example, if the lvalue is a variable of type `Position`, the assigned initializer will also be of type `Position`:

```cpp
const p: Position = {10, 20}
```

Similarly, when an initializer is used inside of an initializer, it obtains the type of the initializer element. In the following example the outer initializer is of type `Line`, while the inner initializers are of type `Point`:

```cpp
const l: Line = {{10, 20}, {30, 40}}
```

Another notable example where this matters is for enum and bitmask constants. Consider the following example:

```cpp
const c: Color = Red
```

Here, `Red` is a resolvable identifier, even though the fully qualified identifier is `Color.Red`. However, because the type of the lvalue is of enum type `Color`, the expression `Red` will be resolved in the scope of `Color`.

### Functions
Expressions can call functions. Functions in Flecs script can have arguments of any type, and must return a value. The following snippet shows examples of function calls:

```cpp
const x = sqrt(100)
const x = pow(100, 2)
const x = add({10, 20}, {30, 40})
```

Functions can be defined in scripts or by using the C/C++ API. Flecs also comes with a set of builtin functions for common math utilities and functions that provide access to ECS features. Math functions are defined by the script math addon, which must be explicitly enabled by defining `FLECS_SCRIPT_MATH`.

A function can be created in code by doing:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_function(world, {
    .name = "sum",
    .return_type = ecs_id(ecs_i64_t),
    .params = {
        { .name = "a", .type = ecs_id(ecs_i64_t) },
        { .name = "b", .type = ecs_id(ecs_i64_t) }
    },
    .callback = sum
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.function("sum")
  .return_type<int64_t>()
  .param<int64_t>("a")
  .param<int64_t>("b")
  .callback(sum)
  .build();
```
</li>
</ul>
</div>

Define the callback before registering the function:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
void sum(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    const int64_t *a = argv[0].ptr;
    const int64_t *b = argv[1].ptr;
    *(int64_t*)result->ptr = *a + *b;
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
void sum(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    const int64_t *a = static_cast<const int64_t*>(argv[0].ptr);
    const int64_t *b = static_cast<const int64_t*>(argv[1].ptr);
    *static_cast<int64_t*>(result->ptr) = *a + *b;
}
```
</li>
</ul>
</div>

The following syntax can be used to define a function in a script:

```rust
fn add(a: i32, b: i32) -> i32 {
    a + b // last expression is return value
}

Foo { Position: {add(1, 2), add(10, 20)} }
```

Script functions are created and called in the same way as functions created with the API.

Function bodies may only contain expressions and const variables, for example:

```rust
fn poly(x: i32) -> i32 {
    const x2: i32 = x * x
    const x3: i32 = x2 * x
    x3 + x2
}
```

Control flow statement such as `if` and `for` are not allowed inside of a function. To expression conditional logic, functions can use `match` expressions:

```rust
fn factorial(n: i32) -> i32 {
    match n {
        0: 1
        _: factorial(n - 1) * n
    }
}
```

### Methods
Methods are functions that are called on instances of the method's type. The first argument of a method is the instance on which the method is called. The following snippet shows examples of method calls:

```cpp
const x = v.length()
const x = v1.add(v2)
```

Methods are defined outside of scripts by using the Flecs Script API.

A method can be created in code by doing:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_method(world, {
    .name = "add",
    .parent = ecs_id(ecs_i64_t),
    .return_type = ecs_id(ecs_i64_t),
    .params = {
        { .name = "a", .type = ecs_id(ecs_i64_t) }
    },
    .callback = sum
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.method<int64_t>("add")
  .return_type<int64_t>()
  .param<int64_t>("a")
  .callback(sum)
  .build();
```
</li>
</ul>
</div>

### Vector functions
Vector functions are functions that accept arguments of a builtin `ScriptVectorType` type. This allows these functions to accept any type that is a valid vector type (see Vector operations).

Here is a usage example of a vector function:

```cpp
const red: Rgb = {255, 0, 0}
const blue: Rgb = {0, 0, 255}
const purple = lerp(red, blue, 0.5)
```

When a vector function is called, all of the arguments provided to parameters of `ScriptVectorType` must be of the same type. The following code is therefore not valid:

```cpp
const red: Rgb = {255, 0, 0}
const p: Position = {10, 20, 30}
const red_p = lerp(red, p, 0.5) // Illegal: red and p are of different types
```

Vector functions are registered like normal functions, but instead of specifying a `callback`, the application sets `vector_callbacks`. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_function(world, {
    .name = "lerp",
    .return_type = EcsScriptVectorType,
    .params = {
        { "a", EcsScriptVectorType },
        { "b", EcsScriptVectorType },
        { "t", ecs_id(ecs_f64_t) }
    },
    .vector_callbacks = {
        [EcsF32] = lerp_f32,
        [EcsF64] = lerp_f64
    }
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.function("lerp")
  .return_type(EcsScriptVectorType)
  .param("a", EcsScriptVectorType)
  .param("b", EcsScriptVectorType)
  .param<double>("t")
  .vector_callback(flecs::F32, lerp_f32)
  .vector_callback(flecs::F64, lerp_f64)
  .build();
```
</li>
</ul>
</div>

The signature for vector functions accepts an additional argument for the number of elements in the vector type. Define these callbacks before registering the function. The `f32` implementation is shown below; use `double` for the vector elements in `lerp_f64`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
void lerp_f32(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    const float *a = argv[0].ptr;
    const float *b = argv[1].ptr;
    double t = *(double*)argv[2].ptr;
    float *r = result->ptr;
    for (int i = 0; i < elem_count; i ++) {
        r[i] = a[i] + t * (b[i] - a[i]);
    }
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
void lerp_f32(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    const float *a = static_cast<const float*>(argv[0].ptr);
    const float *b = static_cast<const float*>(argv[1].ptr);
    double t = *static_cast<const double*>(argv[2].ptr);
    float *r = static_cast<float*>(result->ptr);
    for (int i = 0; i < elem_count; i ++) {
        r[i] = a[i] + t * (b[i] - a[i]);
    }
}
```
</li>
</ul>
</div>

In the function documentation below the type of vector parameters is written as `[]`.

### Builtin functions and constants
The following table lists builtin core functions in the `flecs.script.core` namespace:

| **Function Name** | **Description**                          | **Return Type**  | **Arguments**        |
|-------------------|------------------------------------------|------------------|----------------------|
| `pair`            | Returns a pair identifier                | `id`             | (`entity`, `entity`) |

The following table lists builtin methods on the `flecs.meta.entity` type:

| **Method Name**   | **Description**                          | **Return Type**  | **Arguments**     |
|-------------------|------------------------------------------|------------------|-------------------|
| `name`            | Returns entity name                      | `string`         | `()`              |
| `path`            | Returns entity path                      | `string`         | `()`              |
| `parent`          | Returns entity parent                    | `entity`         | `()`              |
| `has`             | Returns whether entity has component     | `bool`           | `(id)`            |

The following table lists doc methods on the `flecs.meta.entity` type:

| **Method Name**  | **Description**                           | **Return Type**  | **Arguments**     |
|-------------------|------------------------------------------|------------------|-------------------|
| `doc_name`        | Returns entity doc name                  | `string`         | `()`              |
| `doc_uuid`        | Returns entity doc uuid                  | `string`         | `()`              |
| `doc_brief`       | Returns entity doc brief description     | `string`         | `()`              |
| `doc_detail`      | Returns entity doc detailed description  | `string`         | `()`              |
| `doc_link`        | Returns entity doc link                  | `string`         | `()`              |
| `doc_color`       | Returns entity doc color                 | `string`         | `()`              |

To use the doc functions, make sure to use a Flecs build compiled with `FLECS_DOC` (enabled by default).

The following table lists math functions in the `flecs.script.math` namespace:

| **Function Name** | **Description**                          | **Return Type** | **Arguments**       |
|-------------------|------------------------------------------|-----------------|---------------------|
| `cos`             | Compute cosine                           | `f64`           | `(f64)`             |
| `sin`             | Compute sine                             | `f64`           | `(f64)`             |
| `tan`             | Compute tangent                          | `f64`           | `(f64)`             |
| `acos`            | Compute arc cosine                       | `f64`           | `(f64)`             |
| `asin`            | Compute arc sine                         | `f64`           | `(f64)`             |
| `atan`            | Compute arc tangent                      | `f64`           | `(f64)`             |
| `atan2`           | Compute arc tangent with two parameters  | `f64`           | `(f64, f64)`        |
| `cosh`            | Compute hyperbolic cosine                | `f64`           | `(f64)`             |
| `sinh`            | Compute hyperbolic sine                  | `f64`           | `(f64)`             |
| `tanh`            | Compute hyperbolic tangent               | `f64`           | `(f64)`             |
| `acosh`           | Compute area hyperbolic cosine           | `f64`           | `(f64)`             |
| `asinh`           | Compute area hyperbolic sine             | `f64`           | `(f64)`             |
| `atanh`           | Compute area hyperbolic tangent          | `f64`           | `(f64)`             |
| `exp`             | Compute exponential function             | `f64`           | `(f64)`             |
| `ldexp`           | Generate value from significant and exponent | `f64`       | `(f64, f32)`        |
| `log`             | Compute natural logarithm                | `f64`           | `(f64)`             |
| `log10`           | Compute common logarithm                 | `f64`           | `(f64)`             |
| `exp2`            | Compute binary exponential function      | `f64`           | `(f64)`             |
| `log2`            | Compute binary logarithm                 | `f64`           | `(f64)`             |
| `pow`             | Raise to power                           | `f64`           | `(f64, f64)`        |
| `sqrt`            | Compute square root                      | `f64`           | `(f64)`             |
| `sqr`             | Compute square                           | `f64`           | `(f64)`             |
| `ceil`            | Round up value                           | `f64`           | `(f64)`             |
| `floor`           | Round down value                         | `f64`           | `(f64)`             |
| `round`           | Round to nearest                         | `f64`           | `(f64)`             |
| `abs`             | Compute absolute value                   | `f64`           | `(f64)`             |
| `min`             | Return smallest of two values            | `f64`           | `(f64, f64)`        |
| `max`             | Return largest of two values             | `f64`           | `(f64, f64)`        |
| `clamp`           | Clamp value between minimum/maximum      | `[]`            | `([] v, [] min, f64 max)` |
| `lerp`            | Interpolate between two values           | `[]`            | `([] a, [] b, f64 t)` |
| `smoothstep`      | Smooth interpolation between two values  | `[]`            | `([] a, [] b, f64 t)` |
| `dot`             | Return dot product for two vectors       | `f64`           | `([] a, [] b)`      |
| `length`          | Return length of vector                  | `f64`           | `([] v)`            |
| `length_sq`       | Return squared length of vector          | `f64`           | `([] v)`            |
| `normalize`       | Normalize vector                         | `[]`            | `([] v)`            |
| `perlin2`         | 2D perlin noise function                 | `f64`           | `(f64 x, f64 y)`    |

The following table lists the constants in the `flecs.script.math` namespace:

| **Function Name** | **Description**                           | **Type**       | **Value**            |
|-------------------|-------------------------------------------|----------------|----------------------|
| `E`               | Euler's number                            | `f64`          | `2.71828182845904523536028747135266250` |
| `PI`              | Ratio of circle circumference to diameter | `f64`          | `3.14159265358979323846264338327950288` |

The following table lists methods of the `flecs.script.math.Rng` type:

| **Method Name**   | **Description**                                    | **Return Type** | **Arguments**       |
|-------------------|----------------------------------------------------|-----------------|---------------------|
| `u`               | Returns random unsigned integer between 0 and max  | `u64`           | `(u64 max)`         |
| `f`               | Returns random floating point between 0 and max    | `f64`           | `(f64 max)`         |

The random number generator can be used like this:

```cpp
const rng: flecs.script.math.Rng = {}
const x = rng.f(1.0)
```

To use the math functions, make sure to use a Flecs build compiled with the `FLECS_SCRIPT_MATH` addon (disabled by default) and that the module is imported:

```cpp
ECS_IMPORT(world, FlecsScriptMath);
```

### Platform constants
The script platform addon exposes constants in the `flecs.script.platform` namespace that describe the operating system and compiler that the application was built with. This makes it possible to write scripts that conditionally load assets or configuration based on the platform.

The following table lists the string constants in the `flecs.script.platform` namespace:

| **Constant Name** | **Description**                          | **Type**       | **Possible Values**                                                   |
|-------------------|------------------------------------------|----------------|-----------------------------------------------------------------------|
| `os`              | Operating system the build targets       | `string`       | `windows`, `android`, `linux`, `freebsd`, `darwin`, `emscripten`, `unknown` |
| `compiler`        | Compiler the build was compiled with     | `string`       | `msvc`, `clang`, `mingw`, `gcc`, `unknown`                            |

The following table lists the boolean constants in the `flecs.script.platform` namespace. A constant is `true` when the application was built for that platform or compiler, and `false` otherwise:

| **Constant Name** | **Description**                          | **Type**       |
|-------------------|------------------------------------------|----------------|
| `WINDOWS`         | Whether the build targets Windows        | `bool`         |
| `POSIX`           | Whether the build targets a POSIX system | `bool`         |
| `ANDROID`         | Whether the build targets Android        | `bool`         |
| `LINUX`           | Whether the build targets Linux          | `bool`         |
| `FREEBSD`         | Whether the build targets FreeBSD        | `bool`         |
| `DARWIN`          | Whether the build targets macOS/iOS      | `bool`         |
| `EMSCRIPTEN`      | Whether the build targets Emscripten     | `bool`         |
| `MINGW`           | Whether the build was compiled with MinGW| `bool`         |
| `GNU`             | Whether the build was compiled with GCC  | `bool`         |

The platform constants can be used like this:

```cpp
using flecs.script

const platform_name = platform.os

if platform.WINDOWS {
  // ...
}
```

To use the platform constants, make sure to use a Flecs build compiled with the `FLECS_SCRIPT_PLATFORM` addon (disabled by default) and that the module is imported:

```cpp
ECS_IMPORT(world, FlecsScriptPlatform);
```

## Variables
Scripts can contain variables, which are useful for often repeated values. Variables are created with the `const` keyword. Example:

```cpp
const pi = 3.1415926

my_entity {
  Rotation: {angle: pi}
}
```

Variables can be combined with expressions:

```cpp
const pi = 3.1415926
const pi_2 = pi * 2

my_entity {
  Rotation: {angle: pi / 2}
}
```

In the above examples, the type of the variable is inferred. Variables can also be provided with an explicit type:

```cpp
const wood: Color = {38, 25, 13}
```

When the name of a variable clashes with an entity, it can be disambiguated by prefixing the variable name with a `$`:

```cpp
const pi = 3.1415926
const pi_2 = pi * 2

pi {
  Rotation: {angle: $pi / 2}
}
```

Variables can be used in component values as shown in the previous examples. To assign a variable to a component, use the variable as the component expression. Example:

```cpp
const wood: Color = {38, 25, 13}

my_entity {
  Color: wood
}

// is equivalent to

my_entity {
  Color: {38, 25, 13}
}
```

### Exported variables
Variables can be exported by prefixing a variable declaration with the `export` keyword. Exported variables can be accessed by the application and from other scripts. The following example shows an exported variable:

```cpp
// Script 1
export const pi = 3.1415926
```

This variable can now be accessed from another script:

```cpp
// Script 2
const pi_2 = pi * 2
```

Exported variables are created as children of the scope in which they are defined:

```cpp
math {
  export const pi = 3.1415926
}
```

This will make the variable available to other scripts as `math.pi`.

The `ecs_const_var_init` function is used to create exported variables. The following example shows how the same variable can be created from native code:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
double pi_value = 3.1415926;

ecs_const_var(world, {
  .name = "pi",
  .parent = ecs_lookup(world, "math"),
  .type = ecs_id(ecs_f64_t),
  .value = &pi_value
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.const_var("pi", 3.1415926, world.lookup("math"));
```
</li>
</ul>
</div>

Exported variables can be used as configuration that is loaded into an application from a script. The following example shows how to load an exported variable from native code after it has been defined in a script or has been created with `ecs_const_var_init`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t pi = ecs_lookup(world, "math.pi");
ecs_value_t v = ecs_const_var_get(world, pi);
const double *value = v.ptr;
if (value) {
  printf("pi = %f\n", *value);
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
double pi = world.get_const_var<double>("math::pi");
printf("pi = %f\n", pi);
```
</li>
</ul>
</div>

The following example loads `math.flecs` from a native module. The file defines `export const pi = 3.1415926` in the module scope:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
double math_pi;

void MathImport(ecs_world_t *world) {
  ECS_MODULE(world, Math);

  ecs_script(world, { .filename = "math.flecs" });

  ecs_entity_t pi = ecs_lookup(world, "math.pi");
  ecs_value_t v = ecs_const_var_get(world, pi);
  math_pi = *(double*)v.ptr;
}

int main(void) {
  ecs_world_t *world = ecs_init();
  ECS_IMPORT(world, Math);
  double pi_2 = math_pi * 2;
  printf("2 * pi = %f\n", pi_2);
  return ecs_fini(world);
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
struct math {
  inline static double pi;

  math(flecs::world& world) {
    world.module<math>();
    world.script()
      .filename("math.flecs")
      .run();

    pi = world.get_const_var<double>("::math::pi");
  }
};

int main() {
  flecs::world world;
  world.import<math>();
  double pi_2 = math::pi * 2;
  printf("2 * pi = %f\n", pi_2);
}
```
</li>
</ul>
</div>

An `export const` variable may not be modified over its lifetime. To create a variable that is allowed to be changed, use `export mut`:

```cpp
export mut speed = 10
```

The `ecs_mut_var_init` function is used to create mutable exported variables from
native code:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
float difficulty_value = 1.0;

ecs_entity_t difficulty = ecs_mut_var(world, {
  .name = "difficulty",
  .type = ecs_id(ecs_f32_t),
  .value = &difficulty_value
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity difficulty = world.mut_var("difficulty", 1.0f);
```
</li>
</ul>
</div>

Updating a mut variable notifies reactive scripts that depend on it:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t difficulty = ecs_lookup(world, "difficulty");
ecs_value_t v = ecs_mut_var_get(world, difficulty);
*(float*)v.ptr = 2.0;
ecs_mut_var_modified(world, difficulty);
```
</li>
<li><b class="tab-title">C++</b>

```cpp
world.set_mut_var("difficulty", 2.0f);
```
</li>
</ul>
</div>

## Control flow

### If statement
Parts of a script can be conditionally executed with an if statement. Example:

```cpp
const daytime: bool = false

lantern {
  Color: {210, 255, 200}

  if daytime {
    Emissive: { value: 0 }
  } else {
    Emissive: { value: 1 }
  }
}
```

If statements can be chained with `else if`:

```cpp
const state = 0

traffic_light {
  if state == 0 {
    Color: {0, 1, 0}
  } else if state == 1 {
    Color: {0.5, 0.5, 0}
  } else if state == 1 {
    Color: {1, 0, 0}
  }
}
```

### For statement
Parts of a script can be repeated with a for loop. Example:

```cpp
for i in 0..10 {
  Lantern() {
    Position: {x: i * 5}
  }
}
```

The values specified in the range can be an expression:

```cpp
for i in 0..count {
  // ...
}
```

When creating entities in a for loop, ensure that they are unique or the for loop will overwrite the same entity:

```cpp
for i in 0..10 {
  // overwrites entity "e" 10 times
  e: { Position: {x: i * 5} }
}
```

To avoid this, scripts can either create anonymous entities:

```cpp
for i in 0..10 {
  // creates 10 anonymous entities
  { Position: {x: i * 5} }
}
```

Or use a unique string expression for the entity name:

```cpp
for i in 0..10 {
  // creates entities with names e_0, e_1, ... e_9
  "e_$i" { Position: {x: i * 5} }
}
```

Ranges can also be enclosed in brackets:

```cpp
for i in [0..10] {
  // ...
}
```

A range loop can be given a second loop variable, in which case the first variable is the zero-based iteration index and the second variable is the range value:

```cpp
for (index, value) in [5..10] {
  // (0, 5), (1, 6), ... (4, 9)
}
```

For loops can also iterate the elements of arrays, vectors and maps:

```cpp
for elem in arrayExpr {
  { Position: {elem, elem * 2} }
}
```

Maps can be iterated with up to three loop variables. With a single variable the loop iterates the map values. When two variables are specified, the first variable contains the key of the current element. A third variable can be added in the middle, which contains the zero-based iteration index:

```cpp
for elem in mapExpr {
  { Position: {elem, elem * 2} }
}

for (key, elem) in mapExpr {
  "e_{key}" { Position: {elem, elem * 2} }
}

for (key, index, elem) in mapExpr {
  "e_{key}" { Position: {index, elem * 2} }
}
```

A `continue` statement skips the remaining statements of the current iteration and moves the loop to the next iteration:

```cpp
for i in 0..5 {
  if i == 2 {
    continue
  }

  // creates entities e_0, e_1, e_3 and e_4
  "e_{i}" {}
}
```

## Script header
The header of a script may contain `module`, `include` and `using` statements. After the first non-header statement, no more header statements may occur. Header statements must always be created in the root scope.

### Module statement
The module statement will create a module entity with the specified name, and create all script contents in that module. A script may only contain a single module statement. Example:

```cpp
module game

spaceship { // game.spaceship
  Ftl: {true}
}
```

The `game` module will be created with the `flecs.core.Module` tag.

Module statements may be specified as paths:

```cpp
module game.infinity_and_beyond

spaceship { // game.infinity_and_beyond.spaceship
  Ftl: {true}
}
```

### Include statement
The `include` statement loads another script file. Example:

```cpp
include components
include scenes/level_1.flecs
```

The path is resolved relative to the directory of the current script. Paths containing `..` and absolute paths are not allowed.

If the included path does not end in `.flecs`, the extension is appended automatically.

When `include` is used from a managed script (see [Managed script](#managed-script)), the included script is also loaded as a managed script. If a managed script at that path already exists, it is not loaded again. When used from a non-managed script, the included script is executed in place and no script entity is created.

If a script contains a `module` statement, `include` statements must appear after the `module` statement.

### Using statement
The `using` keyword imports a namespace into the current namespace. Example:

```cpp
// Without using
my_engine {
  game.engines.FtlEngine: {active: true}
}
```
```cpp
// With using
using game.engines

my_engine {
  FtlEngine: {active: true}
}
```

If a script contains `module` or `include` statements, `using` statements must be placed after both.

A `using` statement may end with a wildcard (`*`). This will import all namespaces matching the path. Example:

```cpp
using game.*

my_engine {
  FtlEngine: {active: true}
}
```

## Reactivity
Managed scripts are reactive, which means they will be reevaluated when the data that they depend on changes. A managed script is one that uses the following API:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t s = ecs_script(world, {
  .code = "e {}"
});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity s = world.script()
  .code("e {}")
  .run();
```
</li>
</ul>
</div>

See "Managed scripts" for more details. The following sections go over the reactivity features of flecs script.

### Reactivity basics
The following example illustrates a simple reactive script:

```cpp
const t = game[TimeOfDay]

light {
  Position: {10, 20}

  if t.daylight < 0.5{
    Emissive: {1}
  } else {
    Emissive: {0}
  }
}
```

The value of the `Emissive` component depends on whether `game[TimeOfDay].daylight` is smaller than 0.5. This condition is not just evaluated when the script runs. It will be treated as an invariant, meaning that if `game[TimeOfDay].daylight` changes, the value of `Emissive` must change as well.

The script runtime implements "fine grained reactivity". In short this means that flecs tracks which parts of a script depend on which inputs, and that when an input changes, only the code that depends on that input is ran. For this example that means it will only set `Emissive` and not `Position`.

Value dependencies are tracked recursively. An input can be assigned, modified and stored through many indirections. The script runtime will still pick it up as dependency. For example, in the following code example `Emmissive` will still change when `TimeOfDay` changes:

```cpp
const t = game[TimeOfDay]
const isDay = t.daylight > 0.5

light {
  Position: {10, 20}

  if !isDay {
    Emissive: {1}
  } else {
    Emissive: {0}
  }
}
```

Reactive updates are non-destructive. This means that entity handles remain valid before and after the update. For this example it means that `light` will still be the same entity before and after `TimeOfDay` changes. This is useful, as it means that we can safely add components to entities defined in scripts.

Scripts responds to the following reactive inputs:
- Component values (e.g. `game[TimeOfDay]`)
- Component presence (e.g. `game?[TimeOfDay]`)
- Mutable variable changes (e.g. `export mut speed = 10`)
- Template properties (`prop height: i32 = 3`, see below)
- Components resolved on variables (e.g. `$var[TimeOfDay]`)

Scripts additionally respond to entities or components not yet existing by deferring their execution. For example, when a script refers to `game[TimeOfDay]` but the `game` entity doesn't exist yet, or it doesn't have `TimeOfDay` yet, the script will monitor the world for those entities to become available.

Scripts subscribe to `OnSet` events to get notified of component changes. This works out of the box with operations such as `set()` or `assign()`, but if a component reference returned by `ensure()` or `get_mut()` is assigned, it needs a separate call to `modified()` for the script to see it. Additionally, if a system modifies a component directly, it will also have to call `modified()` on the component:

```cpp
game.set(TimeOfDay{0.6}); // Emits OnSet, will get picked up by script

TimeOfDay& t = game.ensure<TimeOfDay>();
t.daylight = 0.6; // Script can't see this yet
game.modified<TimeOfDay>(); // Script can see update
```

### Conditional entities
Scripts can have entities whose existence depends on a reactive value. For example:

```cpp
if game[TimeOfDay].daylight < 0.5 {
  light {
    Position: {10, 20}
    Emissive: {1}
  }
}
```

This script has a different kind of invariant: the `light` entity must only exist when `daylight` is lower than 0.5. When `TimeOfDay` is modified with a `daylight` value higher or equal to 0.5, the script will delete the `light` entity.

### Conditional components
Scripts can have components whose existence depends on a reactive value. For example:

```cpp
light {
  Position: {10, 20}

  if game[TimeOfDay].daylight < 0.5 {
    Emissive: {1}
  }
}
```

The invariant in this script is that the `light` entity must only have the `Emissive` component when the value of `daylight` is below 0.5. If the value is higher or equal to 0.5, the script will remove the `Emissive` component from the `light` entity.

A challenge with deciding whether a component has to be removed is that it could be defined in more than one scope, for example:

```cpp
light {
  Position: {10, 20}

  Emissive: {0}

  if game[TimeOfDay].daylight < 0.5 {
    Emissive: {1}
  }
}
```

To satisfy this script, when the value of `daylight` becomes lower than 0.5, `Emissive` should not be removed. Its value should instead be changed to `{0}`. This turns a local decision ("branch not taken, remove component") into a global decision ("was there any other scope that assigned `Emissive`"). Things get more complex with multiple conditional assignments:

```cpp
light {
  Position: {10, 20}

  if game?[Lights] {
    Emissive: {game[Lights].value}
  }

  if game[TimeOfDay].daylight < 0.5 {
    Emissive: {1}
  }
}
```

To handle these cases correctly, a script would have to build a table of all conditions affecting a component, and assign an action based on which conditions are true. To avoid this complexity, scripts enforce a simple rule that sidesteps this problem: **components must be owned by a single scope**. This means that the above two examples will throw an error.

A component may be assigned in two mutually exclusive scopes:

```cpp
light {
  Position: {10, 20}

  if game[TimeOfDay].daylight < 0.5 {
    Emissive: {1}
  } else {
    Emissive: {0}
  }
}
```

Scopes are currently only considered mutually exclusive when they are in the `if`, `else` or `else if` branches of a single if chain. The following example will not parse, even though strictly speaking the scopes are mutually exclusive:

```cpp
light {
  Position: {10, 20}

  if game[TimeOfDay].daylight < 0.5 {
    Emissive: {1}
  }
  if game[TimeOfDay].daylight >= 0.5 {
    Emissive: {0}
  }
}
```

Additionally, components may be _partially_ assigned in multiple scopes, as long as there is one scope that owns the component:

```cpp
light {
  Position: {10, 20}

  Emissive: {0} // scope that owns the component

  if game?[Lights] {
    Emissive: {daylight: game[Lights].value} // partial assignment
  }

  if game[TimeOfDay].daylight < 0.5 {
    Emissive: {daylight: 1} // partial assignment
  }
}
```

### Loops
Entities created in a loop do not survive a reactive update, unless they are named. For example, here is an example of a loop that creates `count` anonymous entities:

```cpp
const count = game[Count].value

for i in 0..count {
  { Position: {i, i * 2 } }
}
```

When `count` changes, all previous anonymous entities will be deleted before the for loop is reevaluated. To prevent this from happening, entities in a loop must be named:

```cpp
for i in 0..count {
  "e_{i}" { Position: {i, i * 2} }
}
```

This will create a named slot for each entity that is tracked across reactive updates, which prevents deleting the entities on each update.

While somewhat wasteful, this following example also work as expected:

```cpp
for i in 0..count {
  e { Position: {i, i * 2} }
}
```

This will create a single entity named `e` with `Position: {count, count * 2}`. When `count` is 0, `e` will be deleted. Similarly, this also works:

```cpp
e {
  for i in 0..count {
    Position: {i, i * 2}
  }
}
```

This also creates a single entity named `e` with `Position: {count, count * 2}`. When `count` is 0, `Position` will be removed.

### Computed values
Consider the following script:

```cpp
const t = game[TimeOfDay]
const isDay = t.daylight > 0.5

if !isDay {
  for i in 0..lightCount {
    "light_{i}" {
      Emissive: {1}
    }
  }
}
```

Now imagine that `TimeOfDay` is assigned many times, from 0.1 to 0.11, 0.12, 0.13, 0.14, ... We would be doing a lot of redundant work, essentially assigning the `Emissive` component to the same value on each update, unless the value changes to above 0.5, at which point `Emissive` changes value.

To avoid this kind of overhead, `isDay` is treated as a _computed value_. Computed values are cached between updates, so that subsequent updates can tell whether code actually needs to be reran. This happens automatically to significantly reduce the amount of redundant work that reactive updates do.

### Templates
Templates are reactive parameterized scripts that can be used to create procedural assets. Templates can be created with the `template` keyword. A simple example:

```cpp
template Square {
  Color: {255, 0, 0}
  Rectangle: {width: 100, height: 100}
}
```

The script contents of an template are not ran immediately. Instead they are ran whenever an template is _instantiated_. To instantiate an template, add it as a regular component to an entity:

```cpp
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

```cpp
Square my_entity
```

#### Prop variables
Templates can be parameterized with prop variables. To create a prop variable, use the `prop` keyword. Example:

```cpp
template Square {
  prop size = 10
  prop color: Color = {255, 0, 0}

  Color: color
  Rectangle: {width: size, height: size}
}

Square my_entity(size: 20, color: {38, 25, 13})
```

Prop variables are reactive, just like how a component value (`game[TimeOfDay]`) is reactive. This means that when the value of a prop changes, the template is reevaluated, following the same rules as described above.

#### Mut variables
Templates can have `mut` variables. A `mut` variable is reactive state that is not exposed as a `prop`. For example, a button may have a `hover` `mut` variable. Changes to the `mut` variable will cause the template to update (just like with props), but unlike props, `hover` is not passed to the template.

```cpp
template Button {
  prop text = "Howdy"
  mut hover = false

  if hover {
    BackgroundColor: {255, 0, 0}
  } else {
    BackgroundColor: {128, 0, 0}
  }

  // ...
}
```

Code outside of a template can access mut variables:

```cpp
Button b("So long")

if (b[Button.mut].hover) {
  // ...
}
```

#### This variable
Templates can use the `this` variable to refer to the current instance. For example, the following code:

```cpp
template LikesSelf {
  (Likes, this)
}

Bob {
  LikesSelf
}
```

will cause `Bob` to end up with `(Likes, Bob)`.

The `this` variable can be used to read other components of the template instance:

```cpp
template Building {
  Rgb: {100, 50, 10}

  if this?[Damaged] {
    Tint: {0, 0, 0, 0.5}
  }
}
```

#### Inheritance
Templates can inherit from each other. This can be used to create templates that accept and instantiate other templates. For example, consider we want to create a `Building` template with a customizable facade. We could build a template like this, but we would have no way to instantiate the facade because we do not know its type:

```cpp
template Building {
  prop facade: entity = 0
  prop floors: i32
  prop floorHeight: f32

  for i in 0..floors {
    // ??
  }
}
```

Instead, what we can do is define a `Facade` base type and have a template inherit from it:

```cpp
struct Facade(height: f32)

template VictorianFacade : Facade {
  prop height: f32

  Rgb: {120, 170, 120}
  Box: {1, height, 1}
}
```

We can then use the `Facade` type in the prop definition, and instantiate the template-specific facade:

```cpp
template Building {
  prop facade: template Facade
  prop floors: i32
  prop floorHeight: f32

  for i in 0..floors {
    facade: {floorHeight} // provide value of type FAcade
  }
}
```

This makes it possible to use templates as primitive for procedural generation templates, where a generic template specifies the "grammar" of an object (for example a building), with a set of derived templates that implement the style and/or content.

#### Setting props from native code
To update template props from native code, mirror the template type with a native type that has the same name, namespace and members. An example:

```cpp
// script.flecs
template Tree {
  prop width: i32 = 1
  prop height: i32 = 3
  
  trunk {
    Box: {1, 1, 1}
  }
  canopy {
    Box: {width, height - 1, width}
  }
}
```
<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
typedef struct Tree {
  int32_t width;
  int32_t height;
} Tree;

ecs_script(world, { .filename = "script.flecs" });
ECS_COMPONENT(world, Tree);

ecs_entity_t tree = ecs_new(world);
ecs_set(world, tree, Tree, {5, 10});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
struct Tree {
  int32_t width;
  int32_t height;
};

world.script()
  .filename("script.flecs")
  .run();

world.entity()
  .set(Tree{5, 10});
```
</li>
</ul>
</div>

Setting `mut` variables works in a similar way, but with a type called `mut` that is in the scope of the template:

```cpp
// script.flecs
template Button {
  prop text = "Hello World"
  mut hover = false
  mut active = false

  Panel(text) {
    if hover {
      BackgroundColor: {255, 0, 0}
    }
    if active {
      BorderColor: {0, 255, 0}
    }
  }
}
```

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
typedef struct Button {
  char *text;
} Button;

typedef struct ButtonMut {
  bool hover;
  bool active;
} ButtonMut;

ecs_script(world, { .filename = "script.flecs" });
ECS_COMPONENT(world, Button);

ecs_entity_t ecs_id(ButtonMut) = ecs_component(world, {
  .entity = ecs_entity(world, { .name = "Button.mut" }),
  .type = { .size = sizeof(ButtonMut), .alignment = ECS_ALIGNOF(ButtonMut) }
});

ecs_entity_t button = ecs_new(world);
ecs_set(world, button, Button, {"Howdy"});
ecs_set(world, button, ButtonMut, {true, false});
```
</li>
<li><b class="tab-title">C++</b>

```cpp
struct Button {
  char *text;

  struct mut {
    bool hover;
    bool active;
  };
};

world.script()
  .filename("script.flecs")
  .run();

flecs::entity button = world.entity()
  .set(Button{ecs_os_strdup("Howdy")});

button.set(Button::mut{true, false});
```
</li>
</ul>
</div>

The C++ example allocates the string because setting an rvalue transfers its ownership to the reflected component.

#### Example
The following code shows a  more complex example with templates that create children and uses nested templates:

```cpp
template Tree {
  prop height = 10

  const wood_color: Color = {38, 25, 13}
  const leaves_color: Color = {51, 76, 38}

  const canopy_height = 2
  const trunk_height = height - canopy_height
  const trunk_width = 2

  Trunk {
    Position: {0, (height / 2), 0}
    Rectangle: {trunk_width, trunk_height}
    Color: wood_color
  }

  Canopy {
    const canopy_y = trunk_height + (canopy_height / 2)

    Position3: {0, canopy_y, 0}
    Box: {canopy_width, canopy_height}
    Color: leaves_color
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

## Async blocks
Scripts and templates can contain `async` blocks. An `async` block is a piece of code that runs asynchronously from the rest of the script: it is started when the script is evaluated (or when the template is instantiated) and can suspend on `await` statements without blocking the application. This makes it possible to write logic that reacts to events over time, such as a slider that waits for drag events.

```cpp
template Slider {
  prop value: f32 = 35
  mut actualValue: f32 = value

  bg {
    Rect: {250, 40}
  }

  async {
    const e = await on.drag(this)
    actualValue = e.value
  }
}

Slider slider
```

The following rules apply to `async` blocks:

- A script or template can have any number of `async` blocks.
- An `async` block must be declared in the root scope of a script or template.
- An `async` block runs once. It is started when the script is evaluated or the template is instantiated, and stops when it reaches the end of the block.
- An `async` block can contain `const` declarations, `await` statements, `try`/`catch`, `if`, `for` and `while` statements, and assignments to `mut` variables. It cannot create entities or add components.
- Variables that are visible at the location of the `async` block (such as props, muts and consts of a template) are captured by value when the block is started. To read the current value of a component, use the `this` variable (for example `this[Slider].value`).
- When a template instance is deleted, the template is removed from the instance, or the script is updated, the `async` blocks of the instance are cancelled.
- An `async` block requires the `FLECS_SCRIPT_ASYNC` addon.

### Progressing async blocks
An `async` block is only advanced when the application calls `ecs_script_tasks_progress()`. When the pipeline addon is enabled this happens automatically in the `EcsPreUpdate` phase by the `flecs.script.ProgressTasks` system, so applications that call `ecs_progress()` don't need to do anything.

```c
// Advance all async blocks that are ready to make progress
ecs_script_tasks_progress(world);
```

### Assigning mut variables
Code in an `async` block of a template can assign values to the `mut` variables of the template. This updates the `mut` component of the template instance, which causes the template to update just like when the value is set from native code:

```cpp
template Button {
  mut hover = false

  if hover {
    BackgroundColor: {255, 0, 0}
  } else {
    BackgroundColor: {128, 0, 0}
  }

  async {
    await on.hover(this)
    hover = true
  }
}
```

Assignments are only allowed in `async` blocks, and only to `mut` variables.

### While statement
An `async` block can use a `while` statement to keep running for as long as a condition is true:

```cpp
template Counter {
  mut count = 0

  Text: {"{count}"}

  async {
    while count < 10 {
      await on.click(this)
      count = count + 1
    }
  }
}
```

The `continue` statement can be used inside a `while` loop to skip to the next evaluation of the condition. The `while` statement is only allowed inside `async` blocks.

## With statement
When you're building a scene or asset you may find yourself often repeating the same components for multiple entities. To avoid this, a `with` statement can be used. For example:

```cpp
with SpaceShip {
  MillenniumFalcon {}
  UssEnterprise {}
  UssVoyager {}
  Rocinante {}
}
```

This is equivalent to doing:

```cpp
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

```cpp
with SpaceShip, HasWeapons {
  MillenniumFalcon {}
  UssEnterprise {}
  UssVoyager {}
  Rocinante {}
}
```

With statements can contain component values, specified between parentheses:

```cpp
with Color(38, 25, 13) {
  pillar_1 {}
  pillar_2 {}
  pillar_3 {}
}
```

## Type definitions
Scripts can define component types by using the type entities from the `flecs.meta` module (`struct`, `enum`, `bitmask`) as entity kind, followed by an initializer list that describes the type.

### Structs
A struct is defined by specifying the struct members in the initializer list, where each member is specified as `name: type`:

```cpp
struct Position(x: f32, y: f32)
```

The member type can be any registered type, including other types defined in a script. This makes it possible to create nested structs:

```cpp
struct Point(x: f32, y: f32)
struct Line(start: Point, stop: Point)
```

Members are created as child entities of the struct with the `flecs.meta.Member` component. The `name: type` notation is a shorthand that only sets the member type. To specify additional fields of the `Member` component, assign an initializer to the member instead of a type:

```cpp
// Member with a type and array size
struct Points(values: {f32, count: 3})
```

The initializer is assigned to the `Member` component of the member entity, which means all fields of `flecs.meta.Member` can be set, either by position or by name:

```cpp
// Same as {f32, count: 3}
struct Points(values: {type: f32, count: 3})

// Member with a unit (requires the units module)
struct Car(speed: {f32, unit: flecs.units.Speed.KiloMetersPerHour})
```

#### Struct inheritance
A struct can inherit the members of another struct by specifying a base struct after the struct name. The derived struct has all members of the base struct, followed by its own members:

```cpp
struct Point(x: f32, y: f32)
struct Point3D : Point(z: f32)
```

In this example `Point3D` values will have `x`, `y` and `z` members. The `Point3D` type will have an `(IsA, Point)` relationship.

### Enums
An enum is defined by listing its constants in the initializer list:

```cpp
enum Color(Red, Green, Blue)
```

Constants are assigned with incrementing values, starting at zero. In the above example `Red` has value 0, `Green` has value 1 and `Blue` has value 2.

Constants can also be assigned explicitly with the `name: value` notation:

```cpp
enum Prio(Low: 1, Medium: 5, High: 10)
```

Implicit and explicit values can be mixed. A constant without a value continues counting from the last assigned value:

```cpp
// A = 0, B = 10, C = 11
enum Mix(A, B: 10, C)
```

By default enum constants are stored as `i32`. A different underlying type can be specified by adding a configuration scope to the initializer list with the `underlying_type` key:

```cpp
enum Color(Red, Green, Blue, {underlying_type: u64})
```

Constant values must fit in the range of the underlying type.

### Bitmasks
A bitmask is defined the same way as an enum:

```cpp
// Bacon = 1, Lettuce = 2, Tomato = 4
bitmask Toppings(Bacon, Lettuce, Tomato)
```

Constants without a value are assigned with incrementing powers of two. Explicit values can be assigned with the `name: value` notation:

```cpp
bitmask Flags(A: 1, B: 2, Both: 3)
```

Bitmask constants are stored as `u32`, which cannot be overridden.

## API
This section goes over how to run scripts in an application.

### Run once
To run a script once, use `ecs_script_run` in C or `world.script_run` in C++:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
const char *code = "my_spaceship {}";

if (ecs_script_run(world, "my_script_name", code, NULL)) {
  ecs_err("script failed");
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
const char *code = "my_spaceship {}";

if (world.script_run("my_script_name", code)) {
  ecs_err("script failed");
}
```
</li>
</ul>
</div>

Alternatively a script can be run directly from a file:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
if (ecs_script_run_file(world, "my_script.flecs")) {
  ecs_err("script failed");
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
if (world.script_run_file("my_script.flecs")) {
  ecs_err("script failed");
}
```
</li>
</ul>
</div>

If a script fails, the entities created by the script will not be automatically deleted. When a script contains templates, script resources will not get cleaned up until the entities associated with the templates are deleted.

### Run multiple times
A script can be run multiple times by parsing it once and evaluating it repeatedly. In C++, the returned `flecs::parsed_script` owns the parsed script and frees it when it goes out of scope. It can be moved, and must be destroyed before its world.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
const char *code = "my_spaceship {}";

ecs_script_t *script = ecs_script_parse(
  world, "my_script_name", code, NULL, NULL);
if (script) {
  if (ecs_script_eval(script, NULL, NULL)) {
    ecs_err("script failed");
  }
  if (ecs_script_eval(script, NULL, NULL)) {
    ecs_err("script failed");
  }
  ecs_script_free(script);
} else {
  ecs_err("script parsing failed");
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
const char *code = "my_spaceship {}";

auto script = world.script_parse("my_script_name", code);
if (script) {
  if (script.eval()) {
    ecs_err("script failed");
  }
  if (script.eval()) {
    ecs_err("script failed");
  }
} else {
  ecs_err("script parsing failed");
}
```
</li>
</ul>
</div>

If a script fails, the entities created by the script will not be automatically deleted. When a script contains templates, script resources will not get cleaned up until the entities associated with the templates are deleted.

### Managed script
Managed scripts are scripts that are discoverable and modifiable in the world. A managed script is associated with an entity. To create a managed script, do:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
const char *code = "my_spaceship {}";

ecs_entity_t s = ecs_script(world, {
  .code = code
});

if (!s) {
  ecs_err("could not create script entity");
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
const char *code = "my_spaceship {}";

flecs::entity s = world.script()
  .code(code)
  .run();

if (!s) {
  ecs_err("could not create script entity");
}
```
</li>
</ul>
</div>

To load a managed script from a file, set `.filename = "game.flecs"` in C or use `.filename("game.flecs")` in C++ instead of setting the code. A missing file can prevent the script entity from being created.

To update the code of a managed script, use `ecs_script_update` in C or `world.script_update` in C++:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
if (ecs_script_update(world, s, 0, new_code)) {
  ecs_err("script update failed");
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
if (world.script_update(s, new_code)) {
  ecs_err("script update failed");
}
```
</li>
</ul>
</div>

When a managed script contains code that has errors, the managed script will still exist in the world. To discover whether a managed script has errors, use the following code:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
const EcsScript *script = ecs_get(world, s, EcsScript);
if (script->error) {
  ecs_err("error: %s", script->error);
}
```
</li>
<li><b class="tab-title">C++</b>

```cpp
const flecs::Script& script = s.get<flecs::Script>();
if (script.error) {
  ecs_err("error: %s", script.error);
}
```
</li>
</ul>
</div>
