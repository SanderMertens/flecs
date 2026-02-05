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
using flecs.meta

struct MaxSpeed {
  value = f32
}

struct Position {
  x = f32
  y = f32
}

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

Components can be defined in a script:

```cpp
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

```cpp
my_entity {
  (Start, Position): {x: 0,  y: 0}
  (Stop,  Position): {x: 10, y: 20}
}
```

### Namespacing
When referring to child entities or components, identifiers need to include the parent path as well as the entity name. Paths are provided as lists of identifiers separated by a dot (`.`):

```cpp
Sun.Earth {
  solarsystem.Planet
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

#### Builtin kinds
Applications can specify the following builtin kinds which provide convenience shortcuts to commonly used features:

```cpp
prefab SpaceShip

// is equivalent to

Prefab spaceship
```

```cpp
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
const x: 10 + 20 * 30
const x: 10 * (20 + 30)
const x: $var * 10
const x: pow($var, 2)
const x: e.parent().name()
const x: Position: {10, 20}
const x: Position: {x: 10, y: 20}
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

Initializers must always be assigned to an lvalue of a well defined type. This can either be a typed variable, component assignment, function parameter or in the case of nested initializers, an element of another initializer. For example, this is a valid usage of an initializer:

```cpp
const x = Position: {10, 20}
```

while this is an invalid usage of an initializer:

```cpp
// Invalid, unknown type for initializer
const x: {10, 20}
```

When assigning variables to elements in a composite initializer, applications can use the following shorthand notation if the variable names are the same as the member name of the element:

```cpp
// Normal notation
Tree: {color: $color, height: $height}


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
  prop height = f32: 4

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
const x: = 1

// y will be assigned with value 10
const y: match x {
  1: 10
  2: 20
  3: 30
}
```

The input to a match expression must be matched by one of its cases. If the input is not matched, script execution will fail. Match expressions can include an "any" case, which is selected when none of the other cases match:

```cpp
const x: 4

// y will be assigned with value 100
const y: match x {
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

The type of a match expression is derived from the case values. When the case statements in a match contain values of multiple types, the most expressive type is selected. The algorithm for determining the most expressive type is the same as the one used to determine the type for binary expressions. When a match expression contains values with conflicting types, script execution will fail.

### New expressions
A new expression is the `new` keyword followed by an entity statement. New expressions can be used to create entities inside of expressions. The following are examples of valid new expressions:

```cpp
// Create a new anonymous entity, assign to variable x
const x: new {}

// Create a new anonymous entity with Position component, assign to variable x
const x: new {
  Position: {10, 20}
}

// Create a new entity with name Foo and Position component, assign to variable x
const x: new Foo {
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
  const x: new { Position: {10, 20} }
}

with Position(10, 20) {
  // Create new anonymous entity with Position: {10, 20}
  const x: new { }
}
```

All features that are supported by entity statements are also available for new expressions, such as the ability to have children:

```cpp
const x: new {
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
const red: new {
  Color: {255, 0, 0}
}

const orange: new {
  Color: {255, 128, 0}
}

const green: new {
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

A new expression may only create a single entity, so comma operators are not supported.

### String interpolation
Flecs script supports interpolated strings, which are strings that can contain expressions. String interpolation supports two forms, where one allows for easy embedding of variables, whereas the other allows for embedding any kind of expression. The following example shows an embedded variable:

```cpp
const x: "The value of PI is $PI"
```

The following example shows how to use an expression:

```cpp
const x: "The circumference of the circle is {2 * $PI * $r}"
```

To prevent evaluating expressions in an interpolated string, the `$` and `{` characters can be escaped:

```cpp
const x: "The value of variable \$x is $x"
```

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
 - Equality expressions between floating point numbers are invalid

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
const p0 = Position: {10, 20, 30}
const p1 = p0 + 1 // {11, 21, 31}
```

#### Lvalues
Lvalues are the left side of assignments. There are two kinds of assignments possible in Flecs script:
- Variable initialization
- Initializer initialization

The type of an expression can be influenced by the type of the lvalue it is assigned to. For example, if the lvalue is a variable of type `Position`, the assigned initializer will also be of type `Position`:

```cpp
const p = Position: {10, 20}
```

Similarly, when an initializer is used inside of an initializer, it obtains the type of the initializer element. In the following example the outer initializer is of type `Line`, while the inner initializers are of type `Point`:

```cpp
const l = Line: {{10, 20}, {30, 40}}
```

Another notable example where this matters is for enum and bitmask constants. Consider the following example:

```cpp
const c = Color: Red
```

Here, `Red` is a resolvable identifier, even though the fully qualified identifier is `Color.Red`. However, because the type of the lvalue is of enum type `Color`, the expression `Red` will be resolved in the scope of `Color`.

### Functions
Expressions can call functions. Functions in Flecs script can have arguments of any type, and must return a value. The following snippet shows examples of function calls:

```cpp
const x: sqrt(100)
const x: pow(100, 2)
const x: add({10, 20}, {30, 40})
```

Currently functions can only be defined outside of scripts by the Flecs Script API. Flecs comes with a set of builtin and math functions. Math functions are defined by the script math addon, which must be explicitly enabled by defining `FLECS_SCRIPT_MATH`.

A function can be created in code by doing:

```cpp
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

Function implementations looks like this:

```cpp
void sum(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    int64_t *a = argv[0].ptr;
    int64_t *b = argv[1].ptr;
    *(int64_t*)result->ptr = *a + *b;
}
```

### Methods
Methods are functions that are called on instances of the method's type. The first argument of a method is the instance on which the method is called. The following snippet shows examples of method calls:

```cpp
const x: v.length()
const x: v1.add(v2)
```

Just like functions, methods can currently only be defined outside of scripts by using the Flecs Script API.

A method can be created in code by doing:

```cpp
ecs_method(world, {
    .name = "add",
    .parent = ecs_id(ecs_i64_t), // Add method to i64
    .return_type = ecs_id(ecs_i64_t),
    .params = {
        { .name = "a", .type = ecs_id(ecs_i64_t) }
    },
    .callback = sum
});
```

### Vector functions
Vector functions are functions that accept arguments of a builtin `ScriptVectorType` type. This allows these functions to accept any type that is a valid vector type (see Vector operations).

Here is a usage example of a vector function:

```cpp
const red = Rgb: {255, 0, 0}
const blue = Rgb: {0, 0, 255}
const purple: lerp(red, blue, 0.5)
```

When a vector function is called, all of the arguments provided to parameters of `ScriptVectorType` must be of the same type. The following code is therefore not valid:

```cpp
const red = Rgb: {255, 0, 0}
const p = Position: {10, 20, 30}
const red_p: lerp(red, p, 0.5) // Illegal: red and p are of different types
```

Vector functions are registered like normal functions, but instead of specifying a `callback`, the application sets `vector_callbacks`. An example:

```cpp
ecs_function(world, {
    .name = "lerp",
    .return_type = EcsScriptVectorType,
    .params = {
        { "a", EcsScriptVectorType },
        { "b", EcsScriptVectorType },
        { "t", ecs_id(ecs_f64_t) },
    },
    .vector_callbacks = {
        [EcsF32] = lerp_f32,
        [EcsF64] = lerp_f64
    }
});
```

The signature for vector functions accepts an additional argument for the number of elements in the vector type:

```cpp
void lerp_f32(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    float *a = argv[0].ptr;
    float *b = argv[1].ptr;
    double t = *(double*)argv[2].ptr;
    float *r = result->ptr;
    for (int i = 0; i < elem_count; i ++) {
        r[i] = a[i] + t * (b[i] - a[i]);
    }
}
```

In the function documentation below the type of vector parameters is written as `[]`.

### Builtin functions and constants
The following table lists builtin core functions in the `flecs.script.core` namespace:

| **Function Name** | **Description**             | **Return Type** | **Arguments**               |
|-------------------|-----------------------------|-----------------|-----------------------------|
| `pair`            | Returns a pair identifier   | `id`            | (`entity`, `entity`)        |

The following table lists builtin methods on the `flecs.meta.entity` type:

| **Method Name**   | **Description**                        | **Return Type** | **Arguments**        |
|-------------------|----------------------------------------|-----------------|----------------------|
| `name`            | Returns entity name                    | `string`        | `()`                 |
| `path`            | Returns entity path                    | `string`        | `()`                 |
| `parent`          | Returns entity parent                  | `entity`        | `()`                 |
| `has`             | Returns whether entity has component   | `bool`          | `(id)`               |

The following table lists doc methods on the `flecs.meta.entity` type:

| **Method Name**  | **Description**                           | **Return Type**  | **Arguments**        |
|-------------------|------------------------------------------|------------------|----------------------|
| `doc_name`        | Returns entity doc name                  | `string`         | `()`                 |
| `doc_uuid`        | Returns entity doc uuid                  | `string`         | `()`                 |
| `doc_brief`       | Returns entity doc brief description     | `string`         | `()`                 |
| `doc_detail`      | Returns entity doc detailed description  | `string`         | `()`                 |
| `doc_link`        | Returns entity doc link                  | `string`         | `()`                 |
| `doc_color`       | Returns entity doc color                 | `string`         | `()`                 |

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
| `dot`             | Return dot product for two vectors       | `f64`           | `([] a, [] b)` |
| `length`          | Return length of vector                  | `f64`           | `([] v)` |
| `length_sq`       | Return squared length of vector          | `f64`           | `([] v)` |
| `normalize`       | Normalize vector                         | `[]`            | `([] v)` |
| `perlin2`         | 2D perlin noise function                 | `f64`           | `(f64 x, f64 y)` |

The following table lists the constants in the `flecs.script.math` namespace:

| **Function Name** | **Description**                           | **Type** | **Value**            |
|-------------------|-------------------------------------------|----------|----------------------|
| `E`               | Euler's number                            | `f64`    | `2.71828182845904523536028747135266250` |
| `PI`              | Ratio of circle circumference to diameter | `f64`    | `3.14159265358979323846264338327950288` |

The following table lists methods of the `flecs.script.math.Rng` type:

| **Method Name**   | **Description**                                    | **Return Type** | **Arguments**       |
|-------------------|----------------------------------------------------|-----------------|---------------------|
| `u`               | Returns random unsigned integer between 0 and max  | `u64`           | `(u64 max)`         |
| `f`               | Returns random floating point between 0 and max    | `f64`           | `(f64 max)`         |

The random number generator can be used like this:

```cpp
const rng = flecs.script.math.Rng: {}
const x: $rng.f(1.0)
```

To use the math functions, make sure to use a Flecs build compiled with the `FLECS_SCRIPT_MATH` addon (disabled by default) and that the module is imported:

```cpp
ECS_IMPORT(world, FlecsScriptMath);
```

## Templates
Templates are parameterized scripts that can be used to create procedural assets. Templates can be created with the `template` keyword. Example:

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

Templates can be parameterized with properties. Properties are variables that are exposed as component members. To create a property, use the `prop` keyword. Example:

```cpp
template Square {
  prop size: 10
  prop color = Color: {255, 0, 0}

  $color
  Rectangle: {width: size, height: size}
}

Square my_entity(size: 20, color: {38, 25, 13})
```

Just like `const` variables, `prop` variables can explicitly specify a type or implicitly derive their type from the assigned (default) value.

Template scripts can do anything a regular script can do, including creating child entities. The following example shows how to create an template that uses a nested template to create children:

```cpp
template Tree {
  prop height: 10

  const wood_color = Color: {38, 25, 13}
  const leaves_color = Color: {51, 76, 38}

  const canopy_height: 2
  const trunk_height: $height - $canopy_height
  const trunk_width: 2

  Trunk {
    Position: {0, ($height / 2), 0}
    Rectangle: {$trunk_width, $trunk_height}
    $wood_color
  }

  Canopy {
    const canopy_y: $trunk_height + ($canopy_height / 2)

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

## Module statement
The `module` statement puts all contents of a script in a module. Example:

```cpp
module components.transform

// Creates components.transform.Position
struct Position {
  x = f32
  y = f32
}
```

The `components.transform` entity will be created with the `Module` tag.

## Using statement
The `using` keyword imports a namespace into the current namespace. Example:

```cpp
// Without using
flecs.meta.struct Position {
  x = flecs.meta.f32
  y = flecs.meta.f32
}
```
```cpp
// With using
using flecs.meta

struct Position {
  x = f32
  y = f32
}
```

The `using` keyword only applies to the scope in which it is specified. Example:

```cpp
// Scope without using
my_engine {
  game.engines.FtlEngine: {active: true}
}
```
```cpp
// Scope with using
my_spaceship {
  using game.engines

  FtlEngine: {active: true}
}
```

A `using` statement may end with a wildcard (`*`). This will import all namespaces matching the path. Example:

```cpp
using flecs.*

struct Position {
  x = f32
  y = f32
}
```

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

## Variables
Scripts can contain variables, which are useful for often repeated values. Variables are created with the `const` keyword. Example:

```cpp
const pi: 3.1415926

my_entity {
  Rotation: {angle: pi}
}
```

Variables can be combined with expressions:

```cpp
const pi: 3.1415926
const pi_2: $pi * 2

my_entity {
  Rotation: {angle: pi / 2}
}
```

In the above examples, the type of the variable is inferred. Variables can also be provided with an explicit type:

```cpp
const wood = Color: {38, 25, 13}
```

When the name of a variable clashes with an entity, it can be disambiguated by prefixing the variable name with a `$`:

```cpp
const pi: 3.1415926
const pi_2: $pi * 2

pi {
  Rotation: {angle: $pi / 2}
}
```

Variables can be used in component values as shown in the previous examples, or can be used directly as component. When used like this, the variable name must be prefixed with a `$`. Example:

```cpp
const wood = Color: {38, 25, 13}

my_entity {
  $wood
}

// is equivalent to

my_entity {
  Color: {38, 25, 13}
}
```

Additionally, variables can also be used in combination with `with` statements. When used like this the variable name must also be prefixed with a `$`:

```cpp
const wood = Color: {38, 25, 13}

with $color {
  pillar_1 {}
  pillar_2 {}
  pillar_3 {}
}
```

#### Exported variables
Variables can be exported by prefixing a variable declaration with the `export` keyword. Exported variables can be accessed by the application and from other scripts. The following example shows an exported variable:

```cpp
// Script 1
export const pi: 3.1415926
```

This variable can now be accessed from another script:

```cpp
// Script 2
const pi_2: pi * 2
```

Exported variables are created as children of the scope in which they are defined:

```cpp
math {
  export const pi: 3.1415926
}
```

This will make the variable available to other scripts as `math.pi`.

The `ecs_const_var_init` function is used to create exported variables. The following example shows how the same variable can be created from C code:

```cpp
double pi_value = 3.1415926;

ecs_const_var(world, {
  .name = "pi",
  .parent = ecs_lookup(world, "math"),
  .type = ecs_id(ecs_f64_t),
  .value = &pi_value
});
```

Exported variables can be used as configuration that is loaded into an application from a script. The following example shows how to load an exported variable from C after it has been defined in a script or has been created with `ecs_const_var_init`:

```cpp
ecs_entity_t pi = ecs_lookup(world, "math.pi");
ecs_value_t v = ecs_const_var_get(world, pi);
double *value = v.ptr;
if (value) {
  // Use value
}
```

The following example shows how exported variables can be used in combination with modules in C++:

```cpp
struct math {
  inline static double pi;

  math(flecs::world& world) {
    world.script()
      .filename("math.flecs")
      .run();

    world.const_var("pi", pi);
  }
}

// Import module
world.import<math>();


// Use value
double pi_2 = math::pi * 2;
```

## Component values
A script can use the value of a component that is looked up on a specific entity. The following example fetches the `width` and `depth` members from the `Level` component, that is fetched from the `Game` entity:

```cpp
grid {
  Grid: { Game[Level].width, Game[Level].depth }
}
```

To reduce the number of component lookups in a script, the component value can be stored in a variable:

```cpp
const level: Game[Level]

tiles {
  Grid: { width: $level.width, $level.depth, prefab: Tile }
}
```

The requested component is stored by value, not by reference. Adding or removing components to the entity will not invalidate the component data. If the requested component does not exist on the entity, script execution will fail.

## If statement
Parts of a script can be conditionally executed with an if statement. Example:

```cpp
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

If statements can be chained with `else if`:

```cpp
const state: 0

traffic_light {
  if $state == 0 {
    Color: {0, 1, 0}
  } else if $state == 1 {
    Color: {0.5, 0.5, 0}
  } else if $state == 1 {
    Color: {1, 0, 0}
  }
}
```

## For statement
Parts of a script can be repeated with a for loop. Example:

```cpp
for i in 0..10 {
  Lantern() {
    Position: {x: $i * 5}
  }
}
```

The values specified in the range can be an expression:

```cpp
for i in 0..$count {
  // ...
}
```

When creating entities in a for loop, ensure that they are unique or the for loop will overwrite the same entity:

```cpp
for i in 0..10 {
  // overwrites entity "e" 10 times
  e: { Position: {x: $i * 5} }
}
```

To avoid this, scripts can either create anonymous entities:

```cpp
for i in 0..10 {
  // creates 10 anonymous entities
  _ { Position: {x: $i * 5} }
}
```

Or use a unique string expression for the entity name:

```cpp
for i in 0..10 {
  // creates entities with names e_0, e_1, ... e_9
  "e_$i" { Position: {x: $i * 5} }
}
```

## Default components
A scope can have a default component, which means entities in that scope can assign values of that component without having to specify the component name. 

There are different ways to specify a default component. One way is to use a `with` statement. Default component values are assigned with the `=` operator, and don't need a `{}` surrounding the value. Example:

```cpp
with Position {
  ent_a = 10, 20
  ent_b = 20, 30
}
```

Another way a default components are derived is from the entity kind. If an entity is specified with a kind, a `DefaultChildComponent` component will be looked up on the kind to find the default component for the scope, if any. For example:

```cpp
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

```cpp
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

```cpp
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

## Semicolon operator
Multiple statements can be combined on a single line when using the semicolon operator. Example:

```cpp
my_spaceship {
  SpaceShip; HasFtl
}
```

## Comma operator
The comma operator can be used as a shortcut to create multiple entities in a scope. Example:

```cpp
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

```cpp
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

## API
This section goes over how to run scripts in an application.

### Run once
To run a script once, use the `ecs_script_run` function. Example:

```cpp
const char *code = "my_spaceship {}";

if (ecs_script_run(world, "my_script_name", code)) {
  // error
}
```

Alternatively a script can be ran directly from a file:

```cpp
if (ecs_script_run_file(world, "my_script.flecs")) {
  // error
}
```

If a script fails, the entities created by the script will not be automatically deleted. When a script contains templates, script resources will not get cleaned up until the entities associated with the templates are deleted.

### Run multiple times
A script can be ran multiple times by using the `ecs_script_parse` and `ecs_script_eval` functions. Example:

```cpp
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

```cpp
const char *code = "my_spaceship {}";

ecs_entity_t s = ecs_script(world, {
  .code = code
});

if (!s) {
  // error
}
```

To update the code of a managed script, use the `ecs_script_update` function:

```cpp
if (ecs_script_update(world, s, 0, new_code)) {
  // error
}
```

When a script contains templates, script resources will not get cleaned up until the entities associated with the templates are deleted.
