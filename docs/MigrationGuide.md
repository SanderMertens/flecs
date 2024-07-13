# Migration guide
This guide will help migrating Flecs v3 code bases over to Flecs v4. While the guide attempts to be as complete as possible, v4 is a big release, so some things are inevitably missed. If you see something that you think belongs in the migration guide, feel free to create a PR!

Note that this is _not_ a comprehensive list of things that changed in v4. This document only intends to document the breaking changes between v3 and v4.

## Queries
The three query implementations in v3 (`filter`, `query`, `rule`) have been merged into a single `query` API in v4. This means that any usage of `filter` and `rule` should now be replaced with `query`.

Additionally the following things have changed:

- **Query fields now start from 0 instead of 1(!)**
- `ecs_term_id_t` is now called `ecs_term_ref_t`.
- Term ref flags (such as `self`, `up`) are now applied with a bitwise OR mask to `ecs_term_ref_t::id`.
- The `trav` field has been moved from `ecs_term_ref_t` to `ecs_term_t`.
- When query traversal flags such as `up` are provided, the traversal relationship defaults to `ChildOf` (this used to be `IsA`).
- If no traversal flags are provided, the default is still `self|up(IsA)` for inheritable components.
- The `ecs_query_desc_t::instanced` member no longer exists. Instancing must be specified with a query flag (`.flags = EcsQueryIsInstanced`).
- Stack-allocated query (filter) objects are no longer supported.
- It is no longer possible to provide user-allocated term arrays. The max number of terms is now 32 and can be configured with `FLECS_TERM_COUNT_MAX`.
- The `ecs_query_changed` function has been split up into a function that only accepts a query (`ecs_query_changed`) and one that only accepts an iterator (`ecs_iter_changed`).
- The `ecs_query_skip` function has been renamed to `ecs_iter_skip`.
- The `ecs_query_set_group` function has been renamed to `ecs_iter_set_group`.
- The values in the `ecs_iter_t::columns` array have changed, and may change again in the near future. Applications should not directly depend on it.
- The `EcsParent` convenience constant is gone, and can now be replaced with just `EcsUp`.
- `ecs_query_desc_t::group_by_id` has been renamed to `ecs_query_desc_t::group_by`
- `ecs_query_desc_t::group_by` has been renamed to `ecs_query_desc_t::group_by_callback`
- `ecs_query_desc_t::order_by_component` has been renamed to `ecs_query_desc_t::order_by`
- `ecs_query_desc_t::order_by` has been renamed to `ecs_query_desc_t::order_by_callback`
- The `ecs_query_next_table` and `ecs_query_populate` functions have been removed.
- The `ecs_query_table_count` and `ecs_query_empty_table_count` functions have been replaced with `ecs_query_count`, which now returns a struct.
- The `ecs_query_t` struct is now public, which means that many of the old accessor functions (like `ecs_query_get_ctx`, `ecs_query_get_binding_ctx`) are no longer necessary.
- The subquery feature has been removed.

A before/after example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// v3
ecs_filter_t *q = ecs_filter(world, {
    .terms = {
        { .id = ecs_id(Position) }
        { .id = ecs_id(Gravity), .src.id = Game, .src.flags = EcsSelf|EcsUp }
    }
});

ecs_iter_t it = ecs_filter_iter(world, q);
while (ecs_filter_next(&it)) {
    Position *p = ecs_field(&it, Position, 1);
    Gravity *g = ecs_field(&it, Gravity, 2);

    for (int i = 0; i < it.count; i ++) {
        // ...
    }
}

ecs_filter_fini(q);
```

```c
// v4
ecs_query_t *q = ecs_query(world, {
    .terms = {
        { .id = ecs_id(Position) }
        { .id = ecs_id(Gravity), .src.id = Game|EcsSelf|EcsUp }
    }
});

ecs_iter_t it = ecs_query_iter(world, f);
while (ecs_query_next(&it)) {
    Position *p = ecs_field(&it, Position, 0);
    Gravity *g = ecs_field(&it, Gravity, 1);

    for (int i = 0; i < it.count; i ++) {
        // ...
    }
}

ecs_query_fini(q);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// v3
auto q = world.filter_builder<Position, Gravity>()
    .term_at(2).src(Game).self().up()
    .build();

q.each([](Position& p, Gravity& v) {
    // ...
});
```

```cpp
// v4
auto q = world.query_builder<Position, Gravity>()
    .term_at(1).src(Game).self().up()
    .build();

q.each([](Position& p, Gravity& v) {
    // ...
});
```

</li>
</ul>
</div>

### Caching
In v3, a `query` was always cached. In v4, by default queries are created _uncached_. There are three conditions under which queries become cached:

- When a cache kind is configured that specifies caching
- When the query is associated with an entity
- When `order_by`, `group_by` or `cascade` is used

System queries are cached by default (they are always associated with the system entity). See the query manual for more details. The following example shows the difference between creating a cached query in v3 and v4:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// v3
ecs_query_t *q = ecs_query(world, {
    .filter.terms = {
        { .id = ecs_id(Position) },
        { .id = ecs_id(Velocity) },
    }
});
```

```c
// v4

// Create cached query with cache kind
ecs_query_t *q = ecs_query(world, {
    .filter.terms = {
        { .id = ecs_id(Position) },
        { .id = ecs_id(Velocity) },
    },
    .cache_kind = EcsQueryCacheAuto // cache terms that are cacheable
});

// Create cached query with associated entity
ecs_query_t *q = ecs_query(world, {
    .entity = ecs_entity(world, { .name = "MyQuery" }),
    .terms = {
        { .id = ecs_id(Position) },
        { .id = ecs_id(Velocity) },
    }
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// v3
auto q = world.query<Position, Velocity>();
```

```cpp
// v4

// Create cached query with cache kind
auto q = world.query_builder<Position, Velocity>()
    .cache_kind(flecs::QueryCacheAuto)
    .build();

// Create cached query with associated entity
auto q = world.query<Position, Velocity>("MyQuery");
```

</li>
</ul>
</div>

### Iter function
In C++, the `iter` iteration function no longer exists, and the signature of the `run` function has changed. An example:

```cpp
// v3
auto q = world.query<Position>();

q.iter([](flecs::iter& it, Position *p) {
    for (auto i : it) {
        p[i].x ++;
    }
});
```

```cpp
// v4
auto q = world.query<Position>();

q.run([](flecs::iter& it) {
    while (it.next()) {
        auto p = it.field<Position>(0);
        for (auto i : it) {
            p[i].x ++;
        }
    }
});
```

### Query DSL
The query DSL has remained mostly the same but there are a few small changes. 

Traversal flags are specified differently in v4:

```c
// v3
Gravity(game:self|up(ChildOf))
```
```c
// v4
Gravity(game|self|up ChildOf)
```

Component id flags are now specified with lower case:

```c
// v3
OVERRIDE | Position
```
```c
// v4
auto_override | Position
```

In v4 the `$this` variable must be specified with a lower case:

```c
// v3
Position($This)
```
```c
// v4
Position($this)
```

The `parent` traversal flag has been removed. Because `ChildOf` is now the default relationship when a traversal flag is specified, it can be replaced with just `up`:

```c
// v3
Position(parent)
```
```c
// v4
Position(up)
```

The `$(Relationship)` notation was removed from the DSL in v4:
```c
// v3
$(Movement)
```
```c
// v4
(Movement, $Movement)
```

## Systems & Pipelines
Systems have remained mostly the same. A list of the things that changed:

- `ecs_readonly_begin` got a new `multi_threaded` parameter (added in 3.2.12)
- `no_readonly` has been renamed to `immediate`

## Observers
Observers have remained mostly the same. A list of the things that changed:

- The `UnSet` event no longer exists. `OnRemove` can in v4 be used instead.
- The `EcsIterable` component/iterable interface no longer exist. `yield_existing` is now only supported for builtin events.
- The observer `run` callback now behaves like a regular system runner and no longer requires calling `ecs_observer_default_run_action`. To get the old behavior where `run` is called before the observer query is evaluated, specify the `EcsObserverBypassQuery` observer flag.

## Term iterators
Term iterators have been removed from v4, and have been replaced with the easier to use `ecs_each` API. An example:

```c
// v3
ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t){ .id = ecs_id(Position) });
while (ecs_term_next(&it)) {
    Position *p = ecs_field(&it, Position, 1);
    for (int i = 0; i < it.count; i ++) {
        // ...
    }
}
```

```c
// v4
ecs_iter_t it = ecs_each(world Position);
while (ecs_each_next(&it)) {
    Position *p = ecs_field(&it, Position, 0);
    for (int i = 0; i < it.count; i ++) {
        // ...
    }
}
```

## Entity Names
Entities can now be created with names that are numbers, e.g. `1000`. Lookups for numbers now result in a regular by name lookup for an entity with the number as name. To lookup an entity with a specific id using the lookup functions, use the `#` prefix. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// v3
ecs_entity_t e = ecs_lookup(world, "1000");
```

```c
// v4
ecs_entity_t e = ecs_lookup(world, "#1000");
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// v3
flecs::entity e = world.lookup("1000");
```

```cpp
// v4
flecs::entity e = world.lookup("#1000");
```

</li>
</ul>
</div>

Additionally, entity name functions have changed:

- `ecs_get_fullpath` is now `ecs_get_path`
- `ecs_get_path` is now `ecs_get_path_from`
- `ecs_lookup_fullpath` is now `ecs_lookup`
- `ecs_lookup_path` is now `ecs_lookup_from`

## ensure / get_mut
These changes got introduced in 3.2.12, but are mentioned here as many users will likely see these changes first when upgrading to v4:

 - `ensure` is renamed to `make_alive`
 - `ensure_id` is renamed to `make_alive_id`
 - the return type of `get_mut` in the C++ API is changed from a T* to a T&
 - `get_mut` is renamed to `ensure`
 - a new `get_mut` function is added that doesn't add the component

## ecs_new
The following changes were made to the `new` family of functions in the C API:

- `ecs_new(world, T)` got renamed to `ecs_new_w`
- `ecs_new_id` got renamed to `ecs_new`

In v3, `ecs_new` took into account values set by `ecs_set_scope` and `ecs_set_with`. In v4 this is no longer the case, and the `ecs_new_w` function will only return the entity with the specified component. To get the old behavior that takes into account scope and with, use `ecs_entity_init`.

## Flecs Script
The Flecs script syntax changed how components and entities are specified:

```c
// v3
ent {
    - Position{10, 20}
    child
}
```
```c
// v4
ent {
    Position: {10, 20}
    child {}
}
```

For more details, see the [Flecs script manual](FlecsScript.md).

## Inheritance
In v4, components no longer inherit by default when instantiating a prefab. Instead, components by default override, which is equivalent to how auto overriding worked in v3. To inherit a component, add the `(OnInstantiate, Inherit)` trait to the component. 

The `EcsDontInherit` v3 trait is equivalent to the `(OnInstantiate, DontInherit)` v4 trait. The `EcsAlwaysOverride` v3 trait is now the default in v4. It can also be explicitly specified as `(OnInstantiate, Override)`.

An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// v3
ecs_entity_t p = ecs_new_prefab(world, "SpaceShip");
ecs_set(world, p, MaxSpeed, {100});
ecs_set(world, p, Damage, {0});
ecs_override(world, p, Damage);

ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
```

```c
// v4
ecs_add_pair(world, ecs_id(MaxSpeed), EcsOnInstantiate, EcsInherit);

ecs_entity_t p = ecs_entity(world, {
    .name = "SpaceShip", .add = ecs_ids(EcsPrefab) });
ecs_set(world, p, MaxSpeed, {100});
ecs_set(world, p, Damage, {0});

ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// v3
flecs::entity p = world.prefab("SpaceShip")
    .set(MaxSpeed{100})
    .set_override(Damage{0});

flecs::entity i = world.entity().is_a(p);
```

```cpp
// v4
world.component<MaxSpeed>().add(flecs::OnInstantiate, flecs::Inherit);

flecs::entity p = world.prefab("SpaceShip")
    .set(MaxSpeed{100})
    .set(Damage{0});

flecs::entity i = world.entity().is_a(p);
```

</li>
</ul>
</div>

Additionally, the `override` operation has been renamed to `auto_override`.

## Component disabling
In v4, components can only be disabled if they have the `CanToggle` trait. An example:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// v3
ecs_entity_t e = ecs_new_w(world, Position);
ecs_enable_component(world, e, Position, false);
```

```c
// v4
ecs_add_id(world, ecs_id(Position), EcsCanToggle);

ecs_entity_t e = ecs_new_w(world, Position);
ecs_enable_component(world, e, Position, false);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// v3
world.entity()
    .set(Position{10, 20})
    .disable<Position>();
```

```cpp
// v4
world.component<Position>().add(flecs::CanToggle);

world.entity()
    .set(Position{10, 20})
    .disable<Position>();
```

</li>
</ul>
</div>

## Union relationships
In v3, Union relationships where encoded on the entity type with a `(Union, Relationship)` pair. In v4, union relationships are encoded with a `(Relationship, Union)` pair.

The API for unions has not changed meaningfully. However, the union storage has changed, which might impact performance (positively or negatively).

## Snapshots
The snapshot feature has been removed from v4.

## Tree flattening
The tree flattening feature (`ecs_flatten`) has been removed. It will be replaced eventually with a new implementation that addresses the shortcomings of the old feature.

## Addons
The following addons have been removed/merged with other addons:

- `FLECS_META_C` (merged with `FLECS_META`)
- `FLECS_PLECS` (renamed to `FLECS_SCRIPT`)
- `FLECS_EXPR` (merged with `FLECS_SCRIPT`)
- `FLECS_PARSER` (merged with `FLECS_SCRIPT`)
- `FLECS_SNAPSHOT` (feature got removed)
- `FLECS_RULES` (merged with queries)
- `MONITOR` (merged with `STATS`)

## Misc

- The `EcsTag` trait has been renamed to `EcsPairIsTag`.
- `DefaultChildComponent` is now a component (was a tag in v3).
- The `ecs_set_automerge` functionality has been removed from v4.
- The `ecs_async_stage_new` function has been renamed to `ecs_stage_new`.
- `ecs_set` no longer returns a new entity if 0 is passed (use `ecs_insert` instead).
- `ecs_set_entity_generation` has been renamed to `ecs_set_version`.
- The `ecs_term_copy`/`ecs_term_move`/`ecs_term_fini` functions have been removed. Terms no longer exist by themselves, and term resources are now owned by the query object.
- The `ecs_iter_poly` function has been removed. To iterate all entities in the world, now use `ecs_get_entities`.
- `ecs_field_column` has been renamed to `ecs_field_column_index`.
