# Flecs Query Language

## Introduction
The Flecs Query Language is a string-based representation of queries. The query language makes it easy to create queries at runtime, which makes it useful for tooling, modding or for requesting data from game servers.

This manual is primarily focused on describing the query syntax. For more details on specific query feature, see the query manual.

## Example
```c
// Match spaceship entities that are docked to a planet
SpaceShip, (DockedTo, $planet), Planet($planet)
```

## The Basics
An expression in the Flecs Query Language consists of a list of comma separated "terms", where each term in the query is a condition that entities matching the query must satisfy. 

### Components
The most basic kind of condition is "the entity must have this component". The following expression is an example of a query that matches all entities that have both `Position` and `Velocity`:

```c
// Match entities that have Position and Velocity
Position, Velocity
```

Note that this query matches all entities that _at least_ have the `Position` and `Velocity` components; entities with components in addition to `Position` and `Velocity` also match the query.

### Pairs
The following expression is an example of a query that matches two pairs:
```c
// Match entities that have (Likes, Bob) and (Eats, Apples)
(Likes, Bob), (Eats, Apples)
```

## Operators
Query operators change how a term is matched against entities. Only a single operator can be applied to a term at a time. The following sections go over the different operators in the query language.

### Not
The `not` operator, specified with the `!` character, inverts the result of a term and makes it possible to match entities that _do not_ satisfy a condition. The following expression is an example of a query with a `not` operator:

```c
// Match entities that have Position but not Velocity
Position, !Velocity
```

### Or
The `or` operator, specified with the `||` character, makes it possible to chain together a list of terms of which at least one term must be true. The following expression is an example of a query with an `or` operator:

```c
// Match entities that have Position and Velocity or Mass
Position, Velocity || Mass
```

### Optional
The `optional` operator, specified with the `?` character, optionally matches a component. Optional terms do not change which entities are matched by a query, but can be useful for various reasons:

- Fetching a component in a query is more efficient than `get`
- It allows for a single query to do what would otherwise have to be split up across several queries

The following expression is an example of a query with an `optional` operator:

```c
// Match entities that have Position and optionally Velocity
Position, ?Velocity
```

### AndFrom
the `andfrom` operator allows a query to match a list of components that another entity has. The entities used for the component list are typically prefabs, as they are not matched with queries themselves.

The following expression is an example of a query with an `andfrom` operator:

```c
// Match entities with Position and all components that MyType has
Position, and|MyType
```

The MyType entity could be created as shown in the following example:

```cpp
flecs::entity my_type = world.prefab("MyType")
    .add<Velocity>()
    .add<Mass>();
```

This would cause the above query to be equivalent to:

```c
Position, Velocity, Mass
```

### NotFrom
the `notfrom` operator allows a query to not match a list of components that another entity has. The entities used for the component list are typically prefabs, as they are not matched with queries themselves.

The following expression is an example of a query with an `notfrom` operator:

```c
// Match entities with Position and not any of the components that MyType has
Position, not|MyType
```

The MyType entity could be created as shown in the following example:

```cpp
flecs::entity my_type = world.prefab("MyType")
    .add<Velocity>()
    .add<Mass>();
```

This would cause the above query to be equivalent to:

```c
Position, !Velocity, !Mass
```

### OrFrom
the `orfrom` operator allows a query to match at least one of a list of components that another entity has. The entities used for the component list are typically prefabs, as they are not matched with queries themselves.

The following expression is an example of a query with an `orfrom` operator:

```c
// Match entities with Position and at least one of the components that MyType has
Position, or|MyType
```

The MyType entity could be created as shown in the following example:

```cpp
flecs::entity my_type = world.prefab("MyType")
    .add<Velocity>()
    .add<Mass>();
```

This would cause the above query to be equivalent to:

```c
Position, Velocity || Mass
```

## Wildcards
Query expressions can use wildcards to match any or all instances of a matching component or pair. Wildcards may appear in all parts of a term. The following examples are all valid wildcard queries:

```c
Position, (Likes, *)
Position, (*, Dogs)
Position, (*, *)
Position, *
```

Query results contain information about the exact component or pair that was matched with the `ecs_field_id()` and `flecs::iter::id()` functions. This allows an application to inspect what was actually matched by a wildcard.

### Wildcard wildcard (*)
The following expression is an example of a query that uses a wildcard:

```c
// Match entities with a (Likes, *) pair
// Return all matching pairs
(Likes, *)
```

The `*` wildcard returns all matching instances of the wildcard. If an entity has both `(Likes, Dogs)` and `(Likes, Cats)`, it will be returned twice by the query, once for each pair.

If a query has multiple wildcards, each permutation of the matched results will be returned. The following expression is an example of a query that has multiple wildcards:

```c
// Match entities with (Likes, *) and (Eats, *) pairs
// Return all pair permutations
(Likes, *), (Eats, *)
```

If a single entity has `(Likes, Dogs)` and `(Likes, Cats)`, and has `(Eats, Pizza)` and `(Eats, Salad)`, that entity will yield four results:

- `(Likes, Dogs)`, `(Eats, Pizza)`
- `(Likes, Dogs)`, `(Eats, Salad)`
- `(Likes, Cats)`, `(Eats, Pizza)`
- `(Likes, Cats)`, `(Eats, Salad)`

### Any wildcard (_)
The `any` (`_`) wildcard returns at most one result per wildcard. The following expression is an example of a query that uses an `any` wildcard:

```c
// Match entities with a (Likes, *) pair
// Return at most one result per entity
(Likes, _)
```

If an entity has both `(Likes, Dogs)` and `(Likes, Cats)`, the query will return only one result. The location of the `any` wildcard in the matched id will be replaced with `*`, indicating that no specific pair was matched. The above query would return the following id:

- `(Likes, *)`

If a query has multiple `any` wildcards, only a single result is returned. The following expression is an example of a query that has multiple wildcards:

```c
// Match entities with (Likes, *) and (Eats, *) pairs
// Return at most one result per entity
(Likes, _), (Eats, _)
```

If a single entity has `(Likes, Dogs)` and `(Likes, Cats)`, and has `(Eats, Pizza)` and `(Eats, Salad)`, that entity will yield one result:

- `(Likes, *)`, `(Eats, *)`

## Variables
Query variables constrain which values a wildcard can assume by ensuring that the value that was matched by a wildcard in one term is used in all other terms. The following expression is an example of a query that uses variables:

```c
// Match all entities that eat what they like
(Likes, $food), (Eats, $food)
```

If a single entity has `(Likes, Dogs)` and `(Likes, Pizza)`, and has `(Eats, Pizza)` and `(Eats, Salad)`, that entity will yield only one result:

- `(Likes, Pizza)`, `(Eats, Pizza)`

Note how this is a strict subset of the results that would be returned by the following query:

```c
(Likes, *), (Eats, *)
```

Which would return:

- `(Likes, Dogs)`, `(Eats, Pizza)`
- `(Likes, Dogs)`, `(Eats, Salad)`
- `(Likes, Pizza)`, `(Eats, Pizza)`
- `(Likes, Pizza)`, `(Eats, Salad)`

Variables with names that that start with a `_` are treated as anonymous, and are not accessible when a query is iterated.

## Source
All query terms have a "source", which is the entity on which the term is matched. If no term source is specified, it defaults to the `$this` variable. The following expressions show the same query without and with explicit source:

```c
// Implicit source
Position, Velocity
```
```c
// Explicit source
Position($this), Velocity($this)
```

Note how both terms have the same `$this` source. Using the same variable ensures that both components are matched on the same entity.

The following expressions show how to use pair queries without and with explicit source:

```c
// Implicit source
(Likes, Dogs), (Eats, Salad)
```
```c
// Explicit source
Likes($this, Dogs), Eats($this, Salad)
```

A single query can have terms that are matched on more than one source. The following sections describe the supported source kinds.

### Static source
A static source is a term that is always matched on an entity that is known at query creation time. A static source is specified by just using the name of the entity on which the component should be matched:

```c
// Match TimeOfDay component on 'Game' entity
TimeOfDay(Game)
```

### Singleton source
A singleton is a special case of a static source, where the source is the same as the component. The following expression shows an example of a singleton source:


```c
TimeOfDay($)
```

This query is equivalent to:

```c
TimeOfDay(TimeOfDay)
```

A singleton may also be used as the second element in a pair as shown in the next example:

```c
TimeOfDay($this, $)
```

This query is equivalent to:

```c
TimeOfDay($this, TimeOfDay)
```

### Variable source
A variable source is a variable that is used as term source. As mentioned already, when no source is specified, a term implicitly uses the builtin `$this` variable as source:

```c
// Match entities with both Position and Velocity
Position($this), Velocity($this)
```

A variable used as source may appear in a different location in other terms. For example, the following expression uses a variable to match all entities that have components with the `Serializable` component:

```c
Serializable($component), $component($this)
```

The following example matches all spaceship entities that are docked to a planet:

```c
SpaceShip($this), DockedTo($this, $planet), Planet($planet)
```

The following example matches all entities that are eating healthy, but do not like what they are eating:

```c
Eats($this, $food), !Likes($this, $food), Healthy($food)
```

## Traversal
Query traversal makes it possible to match a component by traversing a relationship until an entity with the component has been found. A common use case for this is a transform system, where a `Transform` component is matched both on an entity and its parent.

The following expression shows an example of a query that matches a `Transform` component both on an entity and its parent:

```c
Transform($this), Transform($this|up ChildOf)
```

The same query can be specified without the `$this` variable:

```c
Transform, Transform(up ChildOf)
```

As `ChildOf` is the default traversal relationship, this query can be further shortened to:

```c
Transform, Transform(up)
```

The `cascade` modifier is similar to `up` but returns results in breadth-first order. This is typically used in transform systems to ensure parents are transformed before children. The following expression shows an example with `cascade`:

```c
Transform, Transform(cascade)
```

The `desc` modifier can be used in combination with `cascade` to return results in reverse order:

```c
Transform, Transform(cascade|desc)
```

The `self` traversal modifier can be used in combination with `up` to first test if the entity itself has the component before traversing the hierarchy:

```c
// First try matching Style on self, find on parent if not found
Position, Style(self|up)
```

When a component has the `(OnInstantiate, Inherit)` trait, queries will automatically insert `self|up` traversal for the `IsA` relationship. The following two queries are equivalent, if `Style` has the `(OnInstantiate, Inherit)` trait:

```cpp
Position, Style
```
```cpp
Position, Style(self|up IsA)
```

Traversal modifiers can be used with any relationship that has the `Traversable` trait:

```cpp
world.component<MyRelationship>().add(flecs::Traversable);
```

When a query matches a component that is inherited from, a query will automatically traverse the `IsA` relationship downwards to find all subclasses. For example, if `MeleeUnit` has an `IsA` relationship to `Unit`, the following query matches entities with `Unit` and `MeleeUnit`:

```c
Unit
```

To prevent queries from evaluating component inheritance, the `self` modifier can be added to the component:

```c
Unit|self
```

For terms with an explicit source, the `self` modifier comes before the parentheses:

```c
Unit|self($this)
```

When a query matches a relationship that has the `Transitive` trait, it will traverse the relationship up or down depending on which parts of the query are variable. To prevent a query from matching results transitively, add the `self` modifier to the second element of a pair:

```c
LocatedIn($this, SanFrancisco|self)
```

This will only match entities that have `(LocatedIn, SanFrancisco)` and not, for example, entities with `(LocatedIn, GoldenGateBridge)`.

## Advanced

### Equality operators
Equality operators allow queries to match variables with specific values or names. The following example shows a query that matches a variable against with a specific entity:

```c
SpaceShip($this), $this == UssEnterprise || $this == Voyager
```

The `!=` operator can be used to negate a result:

```c
SpaceShip($this), $this != UssEnterprise
```

Queries may also compare two variables:

```c
PoweredBy($this, $source), $this != $source
```

When a string is used as operand, the operation will test if the name of the entity matches:

```c
SpaceShip($this), $this == "UssEnterprise"
```

The `~=` operator can be used to do a fuzzy comparison, equivalent to the behavior of the `substr` function:

```c
SpaceShip($this), $this ~= "Uss"
```

The result of `~=` can be negated by prefixing the expression with a `!`:

```c
SpaceShip($this), $this ~= "!Uss"
```

When an equality operator is the first term that populates a variable, it will assign the variable:

```c
$this == "UssEnterprise", SpaceShip($this)
```

### Lookup variables
Variables can be used as the starting point of a by-name lookup. This can be useful when matching hierarchies that have a well-defined structure. The following expression is an example of a query with a lookup variable:

```c
// Match all spaceship entities where the cockpit has no power
SpaceShip($this), !Powered($this.cockpit)
```

This query will look for an child entity named `cockpit` in the scope of the matched entity for `$this`, and use that entity to match with `Powered`. If no entity with the name `cockpit` is found, the term will evaluate to false.

### Member matching
Queries can match against the values of component members if they are of the `ecs_entity_t` type. The following expression shows an example of how to match against a `direction` member in a `Movement` component:

```c
Movement.direction($this, Left)
```

The same query with an implicit source:

```c
(Movement.direction, Left)
```

A member expression can be used in combination with variables:

```c
(Thrusters.left, $thruster), Active($thruster)
```

### Dependent variables
When a variable is used first in a term that is conditionally evaluated, any subsequent terms that use the variable will only be evaluated if the variable was set. This allows for the creation of simple branches within queries. The following expression shows an example of dependent variables:

```c
// $animal and $food are set conditionally
(Likes, $animal) || (Eats, $food), 
Friendly($animal), // Evaluated if (Likes, $animal) matched
Healthy($food)     // Evaluated if (Eats, $food) matched
```

Dependent variables can also be created from optional terms:

```c
// Planet($object) is only evaluated if (DockedTo, $object)
// returned a result.
SpaceShip, ?(DockedTo, $object), Planet($object)
```

### Query scopes
Query scopes can be used to apply an operator to the result of more than one term. Currently query scopes are only supported in combination with `not` operators. The following expressions show examples of query scopes:

```c
// Match spaceships where none of the engines are healthy
SpaceShip, !{ (Engine, $engine), Healthy($healthy) }
```

```c
// Match spaceships where all of the engines
SpaceShip, !{ (Engine, $engine), !Healthy($healthy) }
```
