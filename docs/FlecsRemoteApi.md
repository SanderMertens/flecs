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
The following sections describe the different REST endpoints of the Flecs Remote API. Where applicable the corresponding JSON serialization code will be mentioned. The endpoind options map one to one to JSON (de)serializer options, prefixed by `serialize_`. For example, REST API option `values` maps to JSON serializer option `serialize_values`.

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
| type_info     | bool     | Serialize type info for components              |
| matches       | bool     | Serialize matched with queries                  |
| alerts        | bool     | Serialize active alerts for entity & children   |
| refs          | entity   | Serialize relationship backrefs                 |
| try           | bool     | Don't throw HTTP error on failure (REST only)   |

#### Examples

In this example, `Mass` is a component with reflection data, `(Identifier, Name)` is a pair component without reflection data, and `(Description, Color)` is a pair component with reflection data.
```
GET /entity/Sun/Earth
```
```json
{
  "parent": "Sun",
  "name": "Earth",
  "tags": [
    "Planet"
  ],
  "pairs": {
    "ChildOf": "Sun"
  },
  "components": {
    "Mass": {
      "value": "5.9722e24"
    },
    "(Identifier,Name)": null,
    "(Description,Color)": {
      "value": "#6b93d6"
    }
  }
}
```
---
This example shows how `type_info` can be used to obtain the component schema's. If no schema is available a `null` placeholder is sent. The example also shows the `entity_id` option, which adds the `id` member.
```
GET /entity/Sun/Earth?id=true&type_info=true
```
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
    "(Identifier,Name)": 0,
    "(Description,Color)": {
      "value": [
        "text"
      ]
    }
  },
  "tags": [
    "Planet"
  ],
  "pairs": {
    "ChildOf": "Sun"
  },
  "components": {
    "Mass": {
      "value": "5.9722e24"
    },
    "(Identifier,Name)": null,
    "(Description,Color)": {
      "value": "#6b93d6"
    }
  }
}
```
---
This example shows the effect of setting `full_paths`. This can be useful when tag, pair or component names are used in follow up requests, which often require full paths. It can also be used to disambiguate between names in different namespaces.
```
GET /entity/Sun/Earth?full_paths=true
```
```json
{
  "parent": "Sun",
  "name": "Earth",
  "tags": [
    "planets.Planet"
  ],
  "pairs": {
    "flecs.core.ChildOf": "Sun"
  },
  "components": {
    "planets.Mass": {
      "value": "5.9722e24"
    },
    "(flecs.core.Identifier,flecs.core.Name)": null,
    "(flecs.doc.Description,flecs.doc.Color)": {
      "value": "#6b93d6"
    }
  }
}
```
---
This example demonstrates serializing inherited tags, pairs and components. Prefabs are added in the order they are encountered when traversing the `IsA` relationship. If multiple prefabs have the same component, the component will show up multiple times in the reply. A client must only consider the first occurrence of the component.
```
GET /entity/Sun/Earth?inherited=true
```
```json
{
  "parent": "Sun",
  "name": "Earth",
  "tags": [
    "Planet"
  ],
  "pairs": {
    "ChildOf": "Sun",
    "IsA": "HabitablePlanet"
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
    },
    "(Identifier,Name)": null,
    "(Description,Color)": {
      "value": "#6b93d6"
    }
  }
}
```
---
The following example shows the effect of disabling serialization of component values.
```
GET /entity/Sun/Earth?values=false
```
```json
{
  "parent": "Sun",
  "name": "Earth",
  "tags": [
    "Planet"
  ],
  "pairs": {
    "ChildOf": "Sun"
  },
  "components": {
    "Mass": null,
    "(Identifier,Name)": null,
    "(Description,Color)": null
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

```
PUT /entity/Sun/Earth
```

### DELETE entity
Delete an entity.

```
DELETE /entity/<path>
```

#### Examples
The following example deletes entity Sun.Earth.

```
DELETE /entity/Sun/Earth
```

### GET component
Retrieve a single component from an entity.

```
GET /component/<path>?component=<component>
```

#### Examples
```
GET /component/Sun/Earth?component=planets.Mass
```
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

```
PUT /component/Sun/Earth?component=Position
```

---
The following example writes the value `{"x":10}` to the `Position` component of entity `Sun.Earth`:

```
PUT /component/Sun/Earth?component=Position&value=%7B%22x%22%3A%2210%22%7D
```

### DELETE component
Remove a component from an entity.

```
DELETE /component/<path>?component=<component>
```

#### Examples
The following example removes the component `Position` from `Sun.Earth`:

```
DELETE /component/Sun/Earth?component=Position
```

### PUT toggle
Toggle an entity or component.

```
PUT /toggle/<path>?enable=[true|false][&component=<component>]
```

When an entity is toggled, the `Disabled` tag is added or removed. When a component is toggled, the `ecs_enable_id` operation is invoked. A component toggle request will fail if the component does not have the `CanToggle` trait.

#### Examples
The following example disables entity `Sun.Earth`:

```
PUT /toggle/Sun/Earth?enable=false
```

---
The following example disables component `Position` for entity `Sun.Earth`:

```
PUT /toggle/Sun/Earth?enable=false&component=Position
```

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

```
GET /query?expr=Position%2CVelocity
```
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
```
GET /query?expr=Position%2CVelocity?table=true
```
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
        "ChildOf": "shipyard",
        "IsA": "Freighter"
      },
      "components": {
        "Position": {
          "x": 10,
          "y": 20
        },
        "Velocity": {
          "x": 1,
          "y": 1
        },
        "(Identifier,Name)": null
      }
    },
    {
      "parent": "Earth.shipyard",
      "name": "USS Voyager",
      "tags": [
        "FtlEnabled"
      ],
      "pairs": {
        "ChildOf": "shipyard",
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
        },
        "(Identifier,Name)": null
      }
    }
  ]
}
```
---
The following example shows the result for query `Position, ?Position(up)`. This shows the information that gets added to the query result to describe what the source of a field is, and whether a field is set or not.

The `is_set` member is only added to the result if one of the fields in the result is not set. The `sources` member is only added to the result if one of the fields in the result is not matched on the `$this` query variable.
```
GET /query?expr=Position%2C%3FPosition(up)
```
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

```
GET /query?expr=Position%2C%3FPosition(up)&fields=false&entity_ids=true
```
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

```
GET /query?expr=Position%2C(ChildOf%2C%24p)
```

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
The following example shows the result of setting `query_info` and `field_info` to true, and setting `results` to false. This can be used to obtain meta information about a query. `query_info` returns a result per term, whereas `field_info` returns a result per field.

```
GET /query?expr=Position%2C%3FMass(up)?query_info=true&field_info=true&results=false
```
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
