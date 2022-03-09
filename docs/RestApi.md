# REST API
This document provides an overview of the REST API. The Flecs REST API enables (web) clients to inspect the contents of the ECS store, by remotely running queries and requesting entities.

For the C/C++ REST API reference, [see the REST addon documentation](https://flecs.docsforge.com/master/api-rest/).

## Explorer
The REST API can be used together with the Flecs explorer to remotely monitor the entities of a Flecs application. To use the explorer, enable the REST API in your application by instantiating the `EcsRest`/`flecs::Rest` component:

```c
ecs_singleton_set(world, EcsRest, {0});
```
```cpp
world.set<flecs::Rest>({});
```

Alternatively, enable the REST API through the flecs app ([FLECS_APP](https://flecs.docsforge.com/master/api-app/)) interface:

```c
ecs_app_run(world, &(ecs_app_desc_t){
    .enable_rest = true
})
```
```cpp
world.app().enable_rest().run();
```

When the application is running, navigate to https://flecs.dev/explorer. The explorer should connect automatically to your application. For more information and troubleshooting, see the [Explorer repository README](https://github.com/flecs-hub/explorer).

## Endpoints
This section describes the endpoints of the REST API.

### entity
```
/entity/<path>
```
The entity endpoint requests data from an entity. The path is the entity
path or name of the entity to query for. The reply is formatted according to the [JSON serializer Entity](JsonFormat.md#entity) type.

The following parameters can be provided to the endpoint:

#### **path**
Add path (name) for entity.

**Default**: true

#### **label**
Add label (from doc framework) for entity.

**Default**: false

#### **id_labels**
Add labels (from doc framework) for (component) ids.

**Default**: false

#### **base**
Add base components.

**Default**: true

#### **values**
Add component values.

**Default**: false

#### **private**
Add private components.

**Default**: false

#### **type_info**
Add reflection data for component types. Requires values=true.

**Default**: false

#### **Example**:
```
/entity/my_entity
/entity/my_entity?values=true
/entity/parent/child
```

### query
```
/query?q=<query>
```
The query endpoint requests data for a query. The implementation uses the
rules query engine. The reply is formatted as an [JSON serializer Iterator](JsonFormat.md#iterator) type.

The following parameters can be provided to the endpoint:

#### **term_ids**
Add top-level "ids" array with components as specified by query.

**Default**: true

#### **ids**
Add result-specific "ids" array with components as matched. Can be different from top-level "ids" array for queries with wildcards.

**Default**: true
  
#### **subjects**
Add result-specific "subjects" array with component source. A 0 element indicates the component is matched on the current (This) entity.

**Default**: true
    
#### **variables**
Add result-specific "variables" array with values for variables, if any.

**Default**: true
 
#### **is_set**
Add result-specific "is_set" array with boolean elements indicating whether component was matched (used for optional terms).

**Default**: true

#### **values**
Add result-specific "values" array with component values. A 0 element indicates a component that could not be serialized, which can be either
because no reflection data was registered, because the component has no
data, or because the query didn't request it.

**Default**: true

#### **entities**
Add result-specific "entities" array with matched entities.

**Default**: true

#### **entity_labels**
Include doc name for entities.

**Default**: false

#### **variable_labels**
Include doc name for variables.

**Default**: false

#### **duration**
Include measurement on how long it took to serialize result.

**Default**: false
    
#### **type_info**
Add top-level "type_info" array with reflection data on the type in
the query. If a query element has a component that has no reflection
data, a 0 element is added to the array.

**Default**: false

#### Example:
```
/query?q=Position
/query?q=Position&values=true
/query?q=Position%2CVelocity
```