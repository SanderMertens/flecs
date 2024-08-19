# Flecs Remote API
The Flecs Remote API makes it possible to remotely interface with a Flecs application. Possible applications of the Flecs Remote API are:

- Remote tooling such as the [Flecs Explorer](https://www.flecs.dev/explorer/)
- Web applications that use Flecs as a backend store
- Web applications that connect to a Flecs wasm image
- Saving (and loading) ECS data to a file
- Saving (and loading) ECS data to a database

The Flecs Remote API consists out of these components:

- A JSON serializer/deserializer
- A REST API
- A client-side JavaScript library that wraps around the REST API
- The [Flecs Explorer](https://www.flecs.dev/explorer/)

## Explorer Quickstart
The most common usage of the Flecs Remote API is the [Flecs Explorer](https://www.flecs.dev/explorer/). To allow the Flecs explorer to access an application, add this code:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Optional, gather statistics for explorer
ECS_IMPORT(world, FlecsStats);

// Creates REST server on default port (27750)
ecs_singleton_set(world, EcsRest, {0});

// Runs the system serving up REST requests
while (ecs_progress(world, 0)) { }
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Optional, gather statistics for explorer
world.import<flecs::stats>();

// Creates REST server on default port (27750)
world.set<flecs::Rest>({});

// Runs the system serving up REST requests
while (world.progress()) { }
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Optional, gather statistics for explorer
world.Import<Ecs.Stats>();

// Creates REST server on default port (27750)
world.Set<flecs.EcsRest>(default);

// Runs the system serving up REST requests
while (world.Progress()) { }
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Optional, gather statistics for explorer
world.import::<stats::Stats>();

// Creates REST server on default port (27750)
world.set(flecs::rest::Rest::default());

// Runs the system serving up REST requests
while world.progress() {}
```
</li>
</ul>
</div>

or, using the app addon:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_world_t *world = ecs_init();

ecs_app_run(world, &(ecs_app_desc_t) {
    // Optional, gather statistics for explorer
    .enable_stats = true 
    .enable_rest = true,
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.app()
  // Optional, gather statistics for explorer
  .enable_stats()
  .enable_rest()
  .run();
```

</li>
<li><b class="tab-title">C#</b>

```cs
world.App()
  // Optional, gather statistics for explorer
  .EnableStats()
  .EnableRest()
  .Run();
```

</li>
<li><b class="tab-title">Rust</b>

```rust
world
    .app()
    // Optional, gather statistics for explorer
    .enable_stats(true)
    .enable_rest(0)
    .run();
```
</li>
</ul>
</div>

When the application is running, navigate to this URL: https://flecs.dev/explorer . The explorer should connect automatically to the application.

## Usage
The following sections describe how the Flecs Remote API can be used by applications.

### JSON serialization
The Flecs JSON addon provides an easy API for serializing the world, queries and entities to JSON. The following example shows how to serialize the world to JSON:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_world_t *world = ecs_init();

char *json = ecs_world_to_json(world, NULL);
if (!json) {
    // error
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::world world;

char *json = world.to_json();
if (!json) {
    // error
}
```

</li>
</ul>
</div>

The resulting JSON can be deserialized back into the world:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
int res = ecs_world_from_json(world, json, NULL);
if (res) {
    // error
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
int res = world.from_json(json);
if (res) {
    // error
}
```

</li>
</ul>
</div>

Similarly, entities can be serialized to and from JSON:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_insert(...);

char *json = ecs_entity_to_json(world, e, NULL);
if (!json) {
    // error
}

int res = ecs_entity_from_json(world, e, json, NULL);
if (res) {
    // error
}

```
</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.entity().set(...);

char *json = e.to_json();
if (!json) {
    // error
}

int res = e.from_json(json);
if (res) {
    // error
}
```

</li>
</ul>
</div>

Queries can also be serialized to JSON:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
    .expr = "Position, Velocity"
});

ecs_iter_t it = ecs_query_iter(world, q);

char *json = ecs_iter_to_json(it, NULL);
if (!json) {
    // error
}
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto q = world.query<Position, Velocity>();

char *json = q.to_json();
if (!json) {
    // error
}
```

</li>
</ul>
</div>

For more details on the JSON format and the various serialization options, see the Remote API reference.

### REST API
The Flecs REST API enables remote access to running Flecs applications. The REST API can be used in various different ways:

- Using the embedded HTTP server
- Using a 3rd party HTTP server
- By calling `ecs_http_server_request` directly
- To communicate with a WASM image

The embedded HTTP server is a minimal implementation that only provides enough features to be able to serve up the REST API. It is sufficient for development purposes, but should not be used in production environments.

The Flecs REST API can be used in production environments to use Flecs as a backend datastore that can be dynamically queried. This requires forwarding requests to a 3rd party HTTP library (such as civetweb) using the `ecs_http_server_request` function.

The REST API can also be used to communicate with wasm images. By default when Flecs is built with `emscripten`, a function called `flecs_explorer_request` is exported as public symbol which enables doing REST requests. This symbol is used by the explorer to communicate with Flecs wasm images.

The following examples show how to enable the embedded REST API, which is required for using Flecs with the explorer:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
// Optional, gather statistics for explorer
ECS_IMPORT(world, FlecsStats);

// Creates REST server on default port (27750)
ecs_singleton_set(world, EcsRest, {0});

// Runs the system serving up REST requests
while (ecs_progress(world, 0)) { }
```

</li>
<li><b class="tab-title">C++</b>

```cpp
// Optional, gather statistics for explorer
world.import<flecs::stats>();

// Creates REST server on default port (27750)
world.set<flecs::Rest>({});

// Runs the system serving up REST requests
while (world.progress()) { }
```

</li>
<li><b class="tab-title">C#</b>

```cs
// Optional, gather statistics for explorer
world.Import<Ecs.Stats>();

// Creates REST server on default port (27750)
world.Set<flecs.EcsRest>(default);

// Runs the system serving up REST requests
while (world.Progress()) { }
```

</li>
<li><b class="tab-title">Rust</b>

```rust
// Optional, gather statistics for explorer
world.import::<stats::Stats>();

// Creates REST server on default port (27750)
world.set(flecs::rest::Rest::default());

// Runs the system serving up REST requests
while world.progress() {}
```
</li>
</ul>
</div>

Alternatively applications can also use the app addon to enable and run the REST API, which provides a portable way to run applications across desktop, mobile and web applications:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">C</b>

```c
ecs_world_t *world = ecs_init();

ecs_app_run(world, &(ecs_app_desc_t) {
    // Optional, gather statistics for explorer
    .enable_stats = true 
    .enable_rest = true,
});
```

</li>
<li><b class="tab-title">C++</b>

```cpp
world.app()
  // Optional, gather statistics for explorer
  .enable_stats()
  .enable_rest()
  .run();
```

</li>
</ul>
</div>

Alternatively the REST API can be called directly with the `ecs_http_server_request` C function. This makes it easy to use the REST API with a 3rd party HTTP server, or to run the API over a different protocol.

The following example shows how to use the `ecs_http_server_request` function:

```c
// Create a REST server. Don't specify connectivity 
// parameters which ensures no HTTP server is created.
ecs_http_server_t *srv = ecs_rest_server_init(
    world, NULL);

ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
int res = ecs_http_server_request(srv, 
    "GET", "/entity/my_entity", &reply);
if (res || reply.code != 200) {
    // error
}

char *body = ecs_strbuf_get(&reply.body);
// Use reply body
ecs_os_free(body);

// Free server
ecs_rest_server_fini(srv);
```

For more details on the REST API, see the API reference.

## JavaScript library
The JavaScript library is a client-side wrapper that makes it easy to build web applications with the Flecs REST API. To use the client library, include it like this:

```html
<script src="flecs.js" type="text/javascript" charset="utf-8"></script>
```

The latest version of the client library can be downloaded on this URL:
https://www.flecs.dev/explorer/flecs.js

The client can then create a connection to a running application like this:

```js
// Connect to localhost on the default port (27750)
let conn = flecs.connect("localhost");
```

Clients can also connect to a remote host:

```js
let conn = flecs.connect("https://my-game.com/api:9000");
```

Clients can also connect directly to a wasm image:

```js
let conn = flecs.connect("my-game.wasm");
```

Once the connection is created, the client can make requests:

```js
conn.query("Position, Velocity, (ChildOf, scene)", 
    {}, (reply) => {
        for (let entity of reply.results) {
            console.log(`entity ${entity.name} matched!`);
        }
    });
```

## Reference
The following sections describe the different REST endpoints of the Flecs Remote API. Where applicable the corresponding JSON serialization code will be mentioned. The endpoint options map one to one to JSON (de)serializer options, prefixed by `serialize_`. For example, REST API option `values` maps to JSON serializer option `serialize_values`.

### GET entity
Retrieve an entity and its tags, pairs and components.

```
GET entity/<path>
```

#### Options

| Option        | Type     | Description                                     |
|---------------|----------|-------------------------------------------------|
| entity_id     | bool     | Serialize numeric entity id.                    |
| doc           | bool     | Serialize flecs doc components                  |
| full_paths    | bool     | Serialize full tag/pair/component paths         |
| inherited     | bool     | Serialize inherited components                  |
| values        | bool     | Serialize component values                      |
| builtin       | bool     | Serialize builtin components for parent & name  |
| type_info     | bool     | Serialize type info for components              |
| matches       | bool     | Serialize matched with queries                  |
| alerts        | bool     | Serialize active alerts for entity & children   |
| refs          | entity   | Serialize relationship back references          |
| try           | bool     | Don't throw HTTP error on failure (REST only)   |

#### Examples

In this example, `Mass` is a component with reflection data, `Position` is a pair component without reflection data, and `(Description, Color)` is a pair component with reflection data.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /entity/Sun/Earth
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.entity("Sun.Earth", {}, (reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");

char *json = ecs_entity_to_json(world, e, NULL);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");

auto json = e.to_json();
// ...
```

</li>
</ul>
</div>

```json
{
  "parent": "Sun",
  "name": "Earth",
  "tags": [
    "Planet"
  ],
  "components": {
    "Mass": {
      "value": "5.9722e24"
    },
    "Position": null,
    "(flecs.doc.Description,flecs.doc.Color)": {
      "value": "#6b93d6"
    }
  }
}
```
---
This example shows how `type_info` can be used to obtain the component schema's. If no schema is available a `null` placeholder is sent. Note how the member name used in the `type_info` object is the same as used in the `components` object. The example also shows the `entity_id` option, which adds the `id` member.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /entity/Sun/Earth?id=true&type_info=true
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.entity("Sun.Earth", {id: true, type_info: true}, (reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");

ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
desc.serialize_entity_id = true;
desc.serialize_type_info = true;
char *json = ecs_entity_to_json(world, e, &desc);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");

ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
desc.serialize_entity_id = true;
desc.serialize_type_info = true;
auto json = e.to_json(&desc);
```

</li>
</ul>
</div>

```json
{
  "parent": "Sun",
  "name": "Earth",
  "id": 775,
  "type_info": {
    "Mass": {
      "value": [
        "float",
        {
          "unit": "flecs.units.Mass.KiloGrams",
          "symbol": "kg",
          "quantity": "flecs.units.Mass"
        }
      ]
    },
    "Position": 0,
    "(flecs.doc.Description,flecs.doc.Color)": {
      "value": [
        "text"
      ]
    }
  },
  "tags": [
    "Planet"
  ],
  "components": {
    "Mass": {
      "value": "5.9722e24"
    },
    "Position": null,
    "(flecs.doc.Description,flecs.doc.Color)": {
      "value": "#6b93d6"
    }
  }
}
```
---
This example demonstrates serializing inherited tags, pairs and components. Prefabs are added in the order they are encountered when traversing the `IsA` relationship. If multiple prefabs have the same component, the component will show up multiple times in the reply. A client must only consider the first occurrence of the component.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /entity/Sun/Earth?inherited=true
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.entity("Sun.Earth", {inherited: true}, (reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");

ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
desc.serialize_inherited = true;
char *json = ecs_entity_to_json(world, e, &desc);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");

ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
desc.serialize_inherited = true;
auto json = e.to_json(&desc);
```

</li>
</ul>
</div>

```json
{
  "parent": "Sun",
  "name": "Earth",
  "tags": [
    "Planet"
  ],
  "pairs": {
    "flecs.core.IsA": "HabitablePlanet"
  },
  "inherited": {
    "planets.HabitablePlanet": {
      "tags": [
        "HasWater",
        "HasBreathableAir"
      ]
    }
  },
  "components": {
    "Mass": {
      "value": "5.9722e24"
    }
  }
}
```
---
The following example shows the effect of disabling serialization of component values.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /entity/Sun/Earth?values=false
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.entity("Sun.Earth", {values: false}, (reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");

ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
desc.serialize_values = false;
char *json = ecs_entity_to_json(world, e, &desc);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");

ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
desc.serialize_values = false;
auto json = e.to_json(&desc);
```

</li>
</ul>
</div>

```json
{
  "parent": "Sun",
  "name": "Earth",
  "tags": [
    "Planet"
  ],
  "components": {
    "Mass": null,
  }
}
```

### PUT entity
Create an entity.

```
PUT /entity/<path>
```

#### Examples
The following example creates entity Sun.Earth.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
PUT /entity/Sun/Earth
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.create("Sun.Earth");
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_entity(world, { .name = "Sun.Earth" });
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.entity("Sun.Earth");
```

</li>
</ul>
</div>

### DELETE entity
Delete an entity.

```
DELETE /entity/<path>
```

#### Examples
The following example deletes entity Sun.Earth.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
DELETE /entity/Sun/Earth
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.delete("Sun.Earth");
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");
ecs_delete(world, e);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");
e.destruct();
```

</li>
</ul>
</div>


### GET component
Retrieve a single component from an entity.

```
GET /component/<path>?component=<component>
```

#### Examples

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /component/Sun/Earth?component=planets.Mass
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.get("Sun.Earth", {component: "planets.Mass"}, (reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");
ecs_entity_t c = ecs_lookup(world, "planets.Mass");
const void *ptr = ecs_get_id(world, e, c);
char *json = ecs_ptr_to_json(world, c, ptr);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");
flecs::entity c = world.lookup("planets::Mass");
const void *ptr = e.get(c);
auto json = world.to_json(c, ptr);
```

</li>
</ul>
</div>

```json
{ "value": "5.9722e24" }
```

### PUT component
Add or set a component.

```
PUT /component/<path>?component=<component>[&value=<value>]
```

#### Examples
The following example adds the component `Position` to `Sun.Earth`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
PUT /component/Sun/Earth?component=Position
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.add("Sun.Earth", "Position");
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");
ecs_entity_t c = ecs_lookup(world, "Position");
ecs_add_id(world, e, c);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");
flecs::entity c = world.lookup("planets::Mass");
e.add(c);
```

</li>
</ul>
</div>

---
The following example writes the value `{"x":10}` to the `Position` component of entity `Sun.Earth`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
PUT /component/Sun/Earth?component=Position&value=%7B%22x%22%3A%2210%22%7D
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.set("Sun.Earth", "Position", {x: 10});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");
ecs_entity_t c = ecs_lookup(world, "Position");
void *ptr = ecs_get_mut_id(world, e, c);
ecs_ptr_from_json(world, c, ptr, "{\"x\": 10}", NULL);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");
flecs::entity c = world.lookup("planets::Mass");
void *ptr = e.get_mut(c);
world.from_json(c, ptr, "{\"x\": 10}");
```

</li>
</ul>
</div>


### DELETE component
Remove a component from an entity.

```
DELETE /component/<path>?component=<component>
```

#### Examples
The following example removes the component `Position` from `Sun.Earth`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
DELETE /component/Sun/Earth?component=Position
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.remove("Sun.Earth", "Position");
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");
ecs_entity_t c = ecs_lookup(world, "Position");
ecs_remove_id(world, e, c);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");
flecs::entity c = world.lookup("planets::Mass");
e.remove(c);
```

</li>
</ul>
</div>

### PUT toggle
Toggle an entity or component.

```
PUT /toggle/<path>?enable=[true|false][&component=<component>]
```

When an entity is toggled, the `Disabled` tag is added or removed. When a component is toggled, the `ecs_enable_id` operation is invoked. A component toggle request will fail if the component does not have the `CanToggle` trait.

#### Examples
The following example disables entity `Sun.Earth`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
PUT /toggle/Sun/Earth?enable=false
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.disable("Sun.Earth");
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");
ecs_enable(world, e, false);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");
e.disable();
```

</li>
</ul>
</div>

---
The following example disables component `Position` for entity `Sun.Earth`:

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
PUT /toggle/Sun/Earth?enable=false&component=Position
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.disable("Sun.Earth", "Position");
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t e = ecs_lookup(world, "Sun.Earth");
ecs_entity_t c = ecs_lookup(world, "Position");
ecs_enable_id(world, e, c, false);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity e = world.lookup("Sun::Earth");
flecs::entity c = world.lookup("Position");
e.disable(c);
```

</li>
</ul>
</div>

### GET query
Retrieve matching results for a query.

```
GET /query/?expr=<query expression>
```

#### Options

| Option        | Type     | Description                                     |
|---------------|----------|-------------------------------------------------|
| name          | string   | Evaluate existing named query.                  |
| expr          | string   | Evaluate query expression.                      |
| entity_id     | bool     | Serialize numeric entity id.                    |
| doc           | bool     | Serialize flecs doc components                  |
| full_paths    | bool     | Serialize full tag/pair/component paths         |
| inherited     | bool     | Serialize inherited components                  |
| values        | bool     | Serialize component values                      |
| builtin       | bool     | Serialize builtin components for parent & name  |
| fields        | bool     | Serialize query fields                          |
| table         | bool     | Serialize table (all components for match)      |
| result        | bool     | Serialize results (disable for metadata-only request) |
| type_info     | bool     | Serialize type info for components              |
| field_info    | bool     | Serialize field info                            |
| query_info    | bool     | Serialize query info                            |
| query_plan    | bool     | Serialize query plan                            |
| query_profile | bool     | Serialize query profiling information           |
| try           | bool     | Don't throw HTTP error on failure (REST only)   |

#### Examples
The following example returns the results for query Position, Velocity. Note how each of the query terms results in a field in the reply. The mapping between query terms and fields is the same as in the regular flecs API.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /query?expr=Position%2CVelocity
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.query("Position, Velocity", {}, (reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
  .expr = "Position, Velocity"
});

ecs_iter_t it = ecs_query_iter(world, q);
char *json = ecs_iter_to_json(&it, NULL);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::query<> q = world.query_builder()
  .expr("Position, Velocity")
  .build();

auto json = q.iter().to_json();
```

</li>
</ul>
</div>

```json
{
  "results": [
    {
      "parent": "Earth.shipyard",
      "name": "USS Enterprise",
      "fields": {
        "values": [
          {
            "x": 10,
            "y": 20
          },
          {
            "x": 1,
            "y": 2
          }
        ]
      }
    },
    {
      "parent": "Earth.shipyard",
      "name": "USS Voyager",
      "fields": {
        "values": [
          {
            "x": 30,
            "y": 40
          },
          {
            "x": 3,
            "y": 4
          }
        ]
      }
    }
  ]
}
```
---
The following example shows the result of setting `table` to `true`, which serializes all components (the table) of each matched entity. The format of the individually returned results is the same as the `/entity` endpoint.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /query?expr=Position%2CVelocity?table=true
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.query("Position, Velocity", {table: true}, (reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
  .expr = "Position, Velocity"
});

ecs_iter_t it = ecs_query_iter(world, q);
ecs_iter_to_json_t desc = ECS_ITER_TO_JSON_INIT;
desc.serialize_table = true;
char *json = ecs_iter_to_json(&it, &desc);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::query<> q = world.query_builder()
  .expr("Position, Velocity")
  .build();

ecs_iter_to_json_t desc = ECS_ITER_TO_JSON_INIT;
desc.serialize_table = true;
auto json = q.iter().to_json(&desc);
```

</li>
</ul>
</div>

```json
{
  "results": [
    {
      "parent": "Earth.shipyard",
      "name": "USS Enterprise",
      "tags": [
        "Broken",
        "FtlEnabled"
      ],
      "pairs": {
        "flecs.core.IsA": "Freighter"
      },
      "components": {
        "Position": {
          "x": 10,
          "y": 20
        },
        "Velocity": {
          "x": 1,
          "y": 1
        }
      }
    },
    {
      "parent": "Earth.shipyard",
      "name": "USS Voyager",
      "tags": [
        "FtlEnabled"
      ],
      "pairs": {
        "IsA": "Freighter"
      },
      "components": {
        "Position": {
          "x": 30,
          "y": 40
        },
        "Velocity": {
          "x": 3,
          "y": 4
        }
      }
    }
  ]
}
```

---
The following example shows the result for query `Position, ?Position(up)`. This shows the information that gets added to the query result to describe what the source of a field is, and whether a field is set or not.

The `is_set` member is only added to the result if one of the fields in the result is not set. The `sources` member is only added to the result if one of the fields in the result is not matched on the `$this` query variable.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /query?expr=Position%2C%3FPosition(up)
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.query("Position, ?Position(up)", {}, (reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
  .expr = "Position, ?Position(up)"
});

ecs_iter_t it = ecs_query_iter(world, q);
char *json = ecs_iter_to_json(&it, NULL);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::query<> q = world.query_builder()
  .expr("Position, ?Position(up)")
  .build();

auto json = q.iter().to_json();
```

</li>
</ul>
</div>

```json
{
  "results": [
    {
      "name": "Earth.shipyard",
      "fields": {
        "is_set": [true, false],
        "sources": [0, 0],
        "values": [
          {
            "x": 100,
            "y": 200
          },
          {}
        ]
      }
    },
    {
      "parent": "Earth.shipyard",
      "name": "USS Voyager",
      "fields": {
        "sources": [0, "Earth.shipyard"],
        "values": [
          {
            "x": 30,
            "y": 40
          },
          {
            "x": 3,
            "y": 4
          }
        ]
      }
    },
    {
      "parent": "Earth.shipyard",
      "name": "USS Voyager",
      "fields": {
        "sources": [0, "Earth.shipyard"],
        "values": [
          {
            "x": 30,
            "y": 40
          },
          {
            "x": 3,
            "y": 4
          }
        ]
      }
    }
  ]
}
```
---
The following example shows the same query (`Position, ?Position(up)`) with `fields` set to `false` and `entity_ids` set to `true`.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /query?expr=Position%2C%3FPosition(up)&fields=false&entity_ids=true
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.query("Position, ?Position(up)", {fields: false, entity_ids: true}, (reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
  .expr = "Position, ?Position(up)"
});

ecs_iter_t it = ecs_query_iter(world, q);
ecs_iter_to_json_t desc = ECS_ITER_TO_JSON_INIT;
desc.serialize_fields = true;
desc.serialize_entity_ids = true;
char *json = ecs_iter_to_json(&it, &desc);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::query<> q = world.query_builder()
  .expr("Position, ?Position(up)")
  .build();

ecs_iter_to_json_t desc = ECS_ITER_TO_JSON_INIT;
desc.serialize_fields = true;
desc.serialize_entity_ids = true;
auto json = q.iter().to_json(&desc);
```

</li>
</ul>
</div>

```json
{
  "results": [
    {
      "name": "Earth.shipyard",
      "id": 700
    },
    {
      "parent": "Earth.shipyard",
      "name": "USS Voyager",
      "id": 701
    },
    {
      "parent": "Earth.shipyard",
      "name": "USS Voyager",
      "id": 702
    }
  ]
}
```

---
The following example shows how variables are serialized with the query `Position, (ChildOf, $p)`. The `ids` member is added to the serialized data, as the second term of this query has a variable (pair) id. Only fields with variable component ids are serialized to the `ids` array.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /query?expr=Position%2C(ChildOf%2C%24p)
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.query("Position, (ChildOf, $p)", {}, (reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
  .expr = "Position, (ChildOf, $p)"
});

ecs_iter_t it = ecs_query_iter(world, q);
char *json = ecs_iter_to_json(&it, NULL);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::query<> q = world.query_builder()
  .expr("Position, (ChildOf, $p)")
  .build();

auto json = q.iter().to_json();
```

</li>
</ul>
</div>

```json
{
  "results": [
    {
      "parent": "Earth",
      "name": "shipyard",
      "vars": {
        "p": "Earth"
      },
      "fields": {
        "ids": [0, ["flecs.core.ChildOf", "Earth"]],
        "values": [
          {
            "x": 100,
            "y": 200
          },
          0]
      }
    },
    {
      "parent": "Earth.shipyard",
      "name": "USS Enterprise",
      "vars": {
        "p": "shipyard"
      },
      "fields": {
        "ids": [0, ["flecs.core.ChildOf", "Earth.shipyard"]],
        "values": [
          {
            "x": 10,
            "y": 20
          },
          0]
      }
    },
    {
      "parent": "Earth.shipyard",
      "name": "USS Voyager",
      "vars": {
        "p": "shipyard"
      },
      "fields": {
        "ids": [0, ["flecs.core.ChildOf", "Earth.shipyard"]],
        "values": [
          {
            "x": 10,
            "y": 20
          },
          0]
      }
    }
  ]
}
```

---
The following example shows the result of setting `query_info` and `field_info` to true, and setting `results` to false. This can be used to obtain meta information about a query. `query_info` returns a result per term, whereas `field_info` returns a result per field. The query is `Position, ?Mass(up)`.

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /query?expr=Position%2C%3FMass(up)?query_info=true&field_info=true&results=false
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.query("Position, ?Mass(up)", {
    query_info: true,
    field_info: true,
    results: false
  }, (reply) => {
    // ...
  });
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_query_t *q = ecs_query(world, {
  .expr = "Position, ?Mass(up)"
});

ecs_iter_t it = ecs_query_iter(world, q);
ecs_iter_to_json_t desc = ECS_ITER_TO_JSON_INIT;
desc.serialize_query_info = true;
desc.serialize_field_info = true;
desc.serialize_results = true;
char *json = ecs_iter_to_json(&it, &desc);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::query<> q = world.query_builder()
  .expr("Position, ?Mass(up)")
  .build();

ecs_iter_to_json_t desc = ECS_ITER_TO_JSON_INIT;
desc.serialize_query_info = true;
desc.serialize_field_info = true;
desc.serialize_results = true;
auto json = q.iter().to_json(&desc);
```

</li>
</ul>
</div>

```json
{
  "field_info": [
    {
      "id": "Position",
      "type": "Position",
      "symbol": "Position",
      "schema": {
        "x": [
          "float"
        ],
        "y": [
          "float"
        ]
      }
    },
    {
      "id": "Mass",
      "optional": true,
      "type": "Mass",
      "symbol": "Mass",
      "schema": {
        "value": [
          "float"
        ]
      }
    }
  ],
  "query_info": {
    "vars": [
      "this"
    ],
    "terms": [
      {
        "inout": "default",
        "has_value": true,
        "oper": "and",
        "src": {
          "var": "this"
        },
        "first": {
          "entity": "Position",
          "symbol": "Position",
          "type": true
        },
        "flags": [
          "self"
        ]
      },
      {
        "inout": "default",
        "has_value": true,
        "oper": "optional",
        "src": {
          "var": "this"
        },
        "first": {
          "entity": "Mass",
          "symbol": "Mass",
          "type": true
        },
        "trav": {
          "entity": "flecs.core.ChildOf",
          "symbol": "EcsChildOf"
        },
        "flags": [
          "up"
        ]
      }
    ]
  }
}
```

### GET world
Retrieve all serializable data in the world.

```
GET /world
```

#### Example

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
GET /world
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.world((reply) => {
  // ...
});
```

</li>
<li><b class="tab-title">C</b>

```c
char *json = ecs_world_to_json(NULL);
// ...
ecs_os_free(json);
```

</li>
<li><b class="tab-title">C++</b>

```cpp
auto json = world.to_json();
```

</li>
</ul>
</div>

The output of this endpoint is equivalent to requesting the following query on the query endpoint with `table=true`:

```
!ChildOf(self|up, flecs), 
!Module(self|up), 
?Prefab, 
?Disabled
```

This excludes modules and builtin entities from the result, effectively just returning entities that are active in a scene.

### PUT script
Update code for Flecs script.

```
PUT /script/<path>?code=<code>
```

When the code failed to parse a response is sent back with the error:

```json
{
  "error": "58: unexpected end of script\n}\n ^"
}
```

#### Example

<div class="flecs-snippet-tabs">
<ul>
<li><b class="tab-title">HTTP</b>

```
PUT /script/main.flecs?code=SpaceShip{}
```

</li>
<li><b class="tab-title">JavaScript</b>

```js
const conn = flecs.connect("localhost");

conn.scriptUpdate("main\\.flecs", "SpaceShip{}");
```

</li>
<li><b class="tab-title">C</b>

```c
ecs_entity_t s = ecs_lookup(world, "main\\.flecs");
ecs_script_update(world, s, 0, "SpaceShip{}");
```

</li>
<li><b class="tab-title">C++</b>

```cpp
flecs::entity s = world.lookup("main.flecs");
ecs_script_update(world, s, 0, "SpaceShip{}");
```

</li>
</ul>
</div>
