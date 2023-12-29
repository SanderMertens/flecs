# REST API
This document provides an overview of the REST API. The Flecs REST API enables (web) clients to inspect the contents of the ECS store, by remotely running queries and requesting entities.

## Enable the REST API
The REST API can be enabled in an application by instantiating the `EcsRest`/`flecs::Rest` component:

```c
// Start REST API with default parameters
ecs_singleton_set(world, EcsRest, {0});
```
```cpp
// Start REST API with default parameters
world.set<flecs::Rest>({});
```

When an application uses the app addon [FLECS_APP](https://www.flecs.dev/flecs/group__c__addons__app.html) the REST interface can be enabled like this:

```c
// Start application main loop, enable REST interface
ecs_app_run(world, &(ecs_app_desc_t){
    .enable_rest = true
});
```
```cpp
// Start application main loop, enable REST interface
world.app()
  .enable_rest()
  .run();
```

To test if the REST API is working, navigate to http://localhost:27750/entity/flecs/core/World. Upon success this request should return a reply that looks like:
```json
{"path":"World", "ids":[["flecs.rest.Rest"], ["flecs.core.Identifier", "flecs.core.Name"], ["flecs.core.Identifier", "flecs.core.Symbol"], ["flecs.core.ChildOf", "flecs.core"], ["flecs.doc.Description", "flecs.core.Name"], ["flecs.doc.Description", "flecs.doc.Brief"]]}
```

When the monitor module is imported, the REST API provides a `stats` endpoint with statistics for different time intervals:

```c
// Import monitor addon
ECS_IMPORT(world, FlecsMonitor);
```
```cpp
world.import<flecs::monitor>();
```

When an application uses the app addon [FLECS_APP](https://www.flecs.dev/flecs/group__c__addons__app.html) the monitoring can be enabled like this:
```c
// Start application main loop, enable REST interface and monitoring
ecs_app_run(world, &(ecs_app_desc_t){
    .enable_rest = true,
    .enable_monitor = true
});
```
```cpp
// Start application main loop, enable REST interface and monitoring
world.app()
  .enable_rest()
  .enable_monitor()
  .run();
```

For the full C/C++ API reference [see the REST addon documentation](https://www.flecs.dev/flecs/group__c__addons__rest.html).

## Explorer
An application with REST enabled can be remotely monitored with the [Flecs Explorer](https://github.com/flecs-hub/explorer). When the application is running with REST enabled, navigate to https://flecs.dev/explorer. This should connect the Explorer to the application.

When the connection is successful, the Explorer should look similar to this:

![Remote Explorer](img/explorer-remote.png)

The remote icon next to the title should be visible. If the connection is not successful it could be that the explorer did not receive a response fast enough. To force the explorer to connect remotely, add `?remote=true` to the request: https://flecs.dev/explorer?remote=true.

If connection issues persist, the browser could be preventing connections to a local application. See the [Explorer repository README](https://github.com/flecs-hub/explorer) for more information on how to workaround this and other issues.

**Tip**: To show the application title in the explorer, pass the command line arguments to `ecs_init`/`flecs::world::world`:

```c
ecs_world_t *world = ecs_init_w_args(argc, argv);
```
```c++
flecs::world world(argc, argc);
```

### Queries
The search bar in the explorer makes it possible to directly query the ECS storage. A query can be entered in the "Search" box, and needs to be specified in the query DSL (see [query manual](Queries.md#query-dsl)). An example:

![Remote Explorer](img/explorer-query.png)

The default query engine used by the search bar is the rules engine, which means it is possible to use features like query variables:

![Remote Explorer](img/explorer-rules.png)

It is also possible to inspect the results of existing queries. This can be done by entering a `?-`, followed by the name of the query. Queries can be given a name by setting the `.entity` field to a named entity in C:

```c
ecs_query_t *q = ecs_query(world, {
  .entity = ecs_entity(world, { .name = "Move" }),
  .filter.terms = {
    { ecs_id(Position) },
    { ecs_id(Velocity) },
  }
});
```

In C++ a name can be provided to the query factory method:

```cpp
auto q = world.query<Position, Velocity>("Move");
```

This also works for filters and rules. Because systems are queries, the name of a system can be entered to inspect the entities matched by the system:

![Remote Explorer](img/explorer-system.png)

When a named rule query has variables, variables can be optionally provided as arguments to the query. The following example provides the value `Apples` to the query variable `food` for query `eats_query`, which constrains the results to only show results with `Apples`:

![Remote Explorer](img/explorer-arguments.png)

The underlying query that was used for this screenshot was created like this:

```c
ecs_rule(world, {
    .entity = ecs_entity(world, { .name = "eats_query" }),
    .expr = "(Eats, $food)"
});
```

## Endpoints
This section describes the endpoints of the REST API.

### entity
```
GET /entity/<path>
```
The entity endpoint requests data from an entity. The path is the entity path or name of the entity to query for. The reply is formatted according to the [JSON serializer Entity](JsonFormat.md#entity) type.

The following parameters can be provided to the endpoint:

#### path
Add path (name) for entity.

**Default**: true

#### label
Add label (from doc framework) for entity.

**Default**: false

#### brief
Add brief description (from doc framework) for entity.

**Default**: false

#### link
Add link (from doc framework) for entity.

**Default**: false

#### id_labels
Add labels (from doc framework) for (component) ids.

**Default**: false

#### base
Add base components.

**Default**: true

#### values
Add component values.

**Default**: false

#### private
Add private components.

**Default**: false

#### type_info
Add reflection data for component types.

**Default**: false

#### alerts
Add active alerts for entity.

**Default**: false

#### Example
```
/entity/my_entity
/entity/my_entity?values=true
/entity/parent/child
```

### enable
```
PUT /enable/<path>
```
The enable endpoint enables the entity specified by the path by removing the `flecs.core.Disabled` component. If the entity does not have the `flecs.core.Disabled` component, the endpoint has no side effects.

### disable
```
PUT /disable/<path>
```
The disable endpoint disables the entity specified by the path by adding the `flecs.core.Disabled` component. If the entity already has the `flecs.core.Disabled` component, the endpoint has no side effects.

### delete
```
PUT /delete/<path>
```
The delete endpoint deletes the entity specified by the path.

### set
```
PUT /set/<path>&data={...}
```
The set endpoint sets components for the entity specified by the path. The data argument uses the same layout as the `ids` and `values` fields produced by the `ecs_entity_to_json` function. An example:

```json
{
  "ids": [["Position"], ["Velocity"], ["Eats", "Apples"]],
  "values": [
    {"x": 10, "y": 20},
    {"x": 1, "y": 1},
    {"amount": 3}
  ]
}
```

### query
```
GET /query?q=<query>
```
The query endpoint requests data for a query. The implementation uses the
rules query engine. The reply is formatted as an [JSON serializer Iterator](JsonFormat.md#iterator) type.

The following parameters can be provided to the endpoint:

#### name
Specify the name of an existing query or system.

#### offset
Skip the first _offset_ number of entities.

**Default**: 0

#### limit
Return at most _limit_ number of entities.

**Default**: 100

#### term_ids
Add top-level "ids" array with components as specified by query.

**Default**: false

#### term_labels
Add top-level "id_labels" array with doc names of components as specified by query.

**Default**: false

#### ids
Add result-specific "ids" array with components as matched. Can be different from top-level "ids" array for queries with wildcards.

**Default**: false

#### id_labels
Add result-specific "id_labels" array with doc names of components as matched. Can be different from top-level "term_labels" array for queries with wildcards.

**Default**: false

#### sources
Add result-specific "sources" array with component source. A 0 element indicates the component is matched on the current (This) entity.

**Default**: false

#### variables
Add result-specific "variables" array with values for variables, if any.

**Default**: true

#### is_set
Add result-specific "is_set" array with boolean elements indicating whether component was matched (used for optional terms).

**Default**: false

#### values
Add result-specific "values" array with component values. A 0 element indicates a component that could not be serialized, which can be either
because no reflection data was registered, because the component has no
data, or because the query didn't request it.

**Default**: false

#### entities
Add result-specific "entities" array with matched entities.

**Default**: true

#### entity_labels
Include doc name for entities.

**Default**: false

#### entity_ids
Include numerical ids for entities.

**Default**: false

#### variable_labels
Include doc name for variables.

**Default**: false

#### variable_ids
Include numerical ids for variables.

**Default**: false

#### duration
Include measurement on how long it took to serialize result.

**Default**: false

#### type_info
Add top-level "type_info" array with reflection data on the type in
the query. If a query element has a component that has no reflection
data, a 0 element is added to the array.

**Default**: false

#### Example:
```
/query?q=Position
/query?q=Position&values=true
/query?q=Position%2CVelocity
/query?name=systems.Move
```

### stats
```
GET /stats/<category>/<period>
```
The stats endpoint returns statistics for a specified category or period. This endpoint requires the monitor module to be imported (see above). The supported categories are:

- `world`
- `pipeline`

The supported periods are:

- 1s
- 1m
- 1h
- 1d
- 1w
