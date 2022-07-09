# JSON format
This document provides an overview of the JSON serializer format. For an overview of how to use the JSON serializer API, [see the JSON addon documentation](https://flecs.docsforge.com/master/api-json/).

## Value kinds
This section describes value kinds that are used by the JSON serializers.

### **Path**
A path field returns the path identifier of an entity as returned by `ecs_get_fullpath` or `flecs::entity::path`. The path contains the entity's name and the names of its parent and grandparents separated by a dot (`.`).

**Example**:
```json
"SolarSystem.Sun.Earth"
```

### **Label**
A label field returns the doc name of the entity as returned by `ecs_doc_get_name` or `flecs::entity::doc_name`. The doc name is a user-friendly name for the entity that does not need to be unique and may contain any character.

Serializing labels requires the [doc](https://flecs.docsforge.com/master/api-doc/) addon.

**Example**:
```json
"The Earth"
```

### **Id**
An id field returns a component id or pair. It is formatted as an array of at least one and at most three elements where the elements have the following meaning:

- Component _or_ First element of pair
- Second element of pair
- Type flag

Each element is formatted as a [Path](#path).

**Example**:
```json
["transform.Position"]
["Likes", "Apples"]
["Movement", 0, "SWITCH"]
```

### **Id label**
Same as [Id](#id) but with [Label](#label)'s instead of paths.

**Example**:
```json
["Entity position"]
["Located in", "San Francisco"]
```

### **Value**
A JSON object or scalar representing the value of a component. Component values can only be serialized if the component is described in the reflection framework (see the [meta addon](https://flecs.docsforge.com/master/api-meta/)).

When a component has no value (e.g. a tag) or is not described by the reflection framework, `0` will be used as placeholder.

**Example**:
```json
{"x": 10, "y": 20}
```

### **Type info**
A JSON object that represents the structure of a component type.

**Example**:
```json
{
    "type_info": [{
        "x": ["float"],
        "y": ["float"]
    }, 0, 0]
}
```

### **Entity**
The entity kind is an object that contains metadata and data of a single serialized entity, and is returned by the [Entity serializer](#entity-serializer).

The following sections describe the fields that an object of the entity kind may contain:

#### **"path"**
The entity path.

Type: [Path](#path), optional

#### **"label"**
The entity doc name.

Type: [Label](#label), optional

#### **"brief"**
Brief description (as returned by `ecs_doc_get_brief`).

Type: string, optional

#### **"link"**
Link to URL (as returned by `ecs_doc_get_link`).

Type: string, optional

#### **"is_a"**
Base entities.

Type: Array([Entity](#entity)), optional

#### **"ids"**
Component ids.

Type: Array([Id](#id))

#### **"id_labels"**
Component labels.

Type: Array([Id label](#id-label)), optional

#### **"hidden"**
Array indicating whether a component is hidden. Only applies to components of base entities (in the ["is_a"](is_a) array).

A base component is hidden when it is overridden by an entity higher in the inheritance hierarchy.

Type: Array(bool), optional

#### **"values"**
Component values.

Type: Array([Value](#value)), optional

#### **"type_info"**
Array with objects that describe the component types of the terms.

Type: Array([Type info](#type-info)), optional

#### **Example**
Default serializer options:
```json
{
    "path":"Sun.Earth",
    "ids":[
        ["Position"],
        ["flecs.core.ChildOf", "Sun"],
        ["flecs.core.Identifier", "flecs.core.Name"]
    ]
}
```

This example shows an entity with a base (an `IsA` relationship) with default serializer options:
```json
{
    "path":"Sun.Earth",
    "is_a": [{
        "path":"planets.RockyPlanet",
        "ids": [
            ["planets.HasRocks"]
        ]
    }],
    "ids":[
        ["Position"],
        ["flecs.core.ChildOf", "Sun"],
        ["flecs.core.Identifier", "flecs.core.Name"]
    ]
}
```

This example shows an entity with a base with all serializer options enabled:
```json
{
    "path":"Sun.Earth",
    "label": "The Earth",
    "brief": "The planet you call home",
    "link": "www.earth.com",
    "is_a": [{
        "path":"planets.RockyPlanet",
        "label":"A rocky planet",
        "ids": [
            ["Position"],
            ["planets.HasRocks"]
        ],
        "id_labels": [
            ["Position"],
            ["HasRocks"]
        ],
        "values":[0, 0],
        "hidden":[true, false]
    }],
    "ids":[
        ["Position"],
        ["flecs.core.ChildOf", "Sun"],
        ["flecs.core.Identifier", "flecs.core.Name"]
    ],
    "ids_labels":[
        ["Position"],
        ["ChildOf", "Sun"],
        ["Identifier", "Name"]
    ],
    "values":[
        {"x": 10, "y": 20},
        0, 0
    ],
    "type_info": [{
        "x": ["float"],
        "y": ["float"]
    }, 0, 0]
}
```

### **Result**
The result kind is an object that contains the metadata and data of a single result returned by an iterator (see [Iterator](#iterator)).

The following sections describe the fields that an object of the entity kind may contain:

#### **"entities"**
Array with paths of the returned entities.

Type: Array([Path](#path)), optional

#### **"entity_labels"**
Same as [entities](#entities), but with [Label](#label)'s instead of paths.

Type: Array([Label](#label)), optional

#### **"entity_ids"**
Same as [entities](#entities), but with numerical ids instead of paths.

Type: Array(Number), optional

#### **"subjects"**
Array with paths of subjects for each term. A subject indicates the actual entity on which a component is stored. If this is the matched entity (default) the array will contain a `0` element.

Type: Array([Path](#path)), optional

#### **"variables"**
Array with variable values for current result (see [query variables](https://flecs.docsforge.com/master/query-manual/#variables)).

Type: Array([Path](#path)), optional

#### **"variable_labels"**
Same as [variables](#variables), but with [Label](#label)'s instead of paths.

Type: Array([Label](#label)), optional

#### **"variable_ids"**
Same as [variables](#variables), but with numerical ids instead of paths.

Type: Array(Number), optional

#### **"ids"**
Array with component ids. This list is more specific than the ids array provided by the top-level iterator object. The arrays can be different in the case of terms with an `Or` operator, or terms with wildcard ids.

Type: Array(string), optional

#### **"is_set"**
Array with booleans that can be used to determine whether terms with the `Optional` operator are set.

Type: Array(bool), optional

#### **"values"**
Array with component values. The array contains an element for each term. If a component has no value, or no value could be serialized for the component a `[]` element is added.

Each element in the array can be either an array with component values when the component is from the matched entity, or a single component value when the component is from another entity (like a parent, prefab, singleton).

Type: Array(Array([Value](#value))), optional

### **Iterator**
The iterator kind is an object that contains metadata and data of all the entities yielded by an iterator.

#### **"ids"**
Array with ids for each term.

Type: Array(string), optional

#### **"variables"**
Array with variable names (see [query variables](https://flecs.docsforge.com/master/query-manual/#variables)).

Type: Array(string), optional

#### **"results"**
Array with elements for each returned result.

Type: Array([Result](#result))

#### **"eval_duration"**
Time it took to serialize the iterator.

Type: Number

#### **"type_info"**
Array with objects that describe the component types of the terms.

Type: Array([Type info](#type-info)), optional

#### **Example**

```json
{
  "ids": ["Position", "Jedi"],
  "results": [{
    "is_set": [true, true],
    "entities": ["Luke", "Yoda"],
    "values": [
      [{ "x": 10, "y": 20 },
       { "x": 20, "y": 30 }],
      0
    ]
  }]
}
```

## Entity serializer
The entity serializer returns a JSON string of the [Entity](#entity) type. This format is returned by `ecs_entity_to_json` and `flecs::entity::to_json`.

### Serializer options
The following options (found in `ecs_entity_to_json_desc_t`) customize the output of the entity serializer:

#### **serialize_path**
Serialize the ["path"](#path-1) member.

**Default**: `true`

**Example**:
```json
{"path": "SolarSystem.Sun.Earth"}
```

#### **serialize_label**
Serialize the ["label"](#label-1) member.

**Default**: `false`

**Example**:
```json
{"label": "Rocky planet"}
```

#### **serialize_brief**
Serialize the ["brief"](#brief) member.

**Default**: `false`

**Example**:
```json
{"brief": "A rocky planet"}
```

#### **serialize_link**
Serialize the ["link"](#link) member.

**Default**: `false`

**Example**:
```json
{"brief": "www.rocky-planet.com"}
```

#### **serialize_id_labels**
Serializes the ["id_labels"](#id_labels) member.

**Default**: `false`

**Example**:
```json
{"id_labels": [["A Rocky Planet"], ["Has surface water"]]}
```

#### **serialize_base**
Serializes the ["is_a"](#is_a) member.

**Default**: `true`

**Example**:
```json
{"is_a": [{
    "path":"planets.RockyPlanet",
    "ids": [
        ["planets.HasRocks"]
    ]
}]}
```

#### **serialize_private**
Serialize private components. Private components are regular components with the `EcsPrivate` (or `flecs::Private`) tag. When `serialize_private` is false, private components will be hidden from all arrays.

**Example**:
```json
{"ids": ["Position", "PrivateComponent"]}
```

**Default**: `false`

#### **serialize_hidden**
Serializes the ["hidden"](#hidden) member.

**Example**:
```json
{"hidden":[true, false]}
```

Full example:
```json
{
    "path":"Sun.Earth",
    "is_a": [{
        "path":"planets.RockyPlanet",
        "ids": [
            ["Position"],
            ["planets.HasRocks"]
        ],
        "hidden":[true, false]
    }],
    "ids":[
        ["Position"],
        ["flecs.core.ChildOf", "Sun"],
        ["flecs.core.Identifier", "flecs.core.Name"]
    ]
}
```
Note that `hidden[0]` is `true` in this example, because the `Position` component from `planets.RockyPlanet` is overridden by the entity.

**Default**: `false`

#### **serialize_values**
Serializes the ["values"](#values) member.

**Example**:
```json
{"values":[{"x":10, "y": 20}, 0]}
```

**Default**: `false`

#### **serialize_type_info**
Serialize the ["type_info"](#type_info) member.

**Example**:
```json
{
    "type_info": [{
        "x": ["float"],
        "y": ["float"]
    }, 0, 0]
}
```

## Iterator serializer
The entity serializer returns a JSON string of the [Iterator](#iterator) type. This format is returned by `ecs_iter_to_json`.

### Serializer options

#### **serialize_term_ids**
Serialize the top level ["ids"](#term_ids) member.

**Default**: `true`

**Example**:

Example result for query `Position, Jedi`
```json
{
  "ids": ["Position", "Jedi"],
  "results": [ ]
}
```

#### **serialize_ids**
Serialize the result specific ["ids"](#ids-1) member.

If the iterated query does not contain variable ids (either an `Or` term or a term with a wildcard id) the result specific `ids` member will exactly match the top-level `ids` member.

**Default**: `true`

**Example**:

Example result for query `Position, (Likes, *)`
```json
{
  "ids": ["Position", "(Likes,*)"],
  "results": [{
    "ids": ["Position", "(Likes,Apples)"]
  }]
}
```

#### **serialize_subjects**
Serialize the ["subjects"](#subjects) member.

**Default**: `true`

**Example**:

Example result for query `Position, Position(parent)`
```json
{
  "ids": ["Position", "Position"],
  "results": [{
    "entities": ["Parent.A", "Parent.B"],
        "subjects": [0, "Parent"]
  }]
}
```

#### **serialize_variables**
Serialize the ["variables"](#variables) member.

**Default**: `true`

**Example**:

Example result for query `Position, (Likes, _Food)`
```json
{
  "variables": ["Food"],
  "results": [{
    "ids": ["Position", "(Likes,Apples)"],
        "variables": ["Apples"],
  }]
}
```

#### **serialize_is_set**
Serialize the ["is_set"](#is_set) member.

**Default**: `true`

**Example**:

Example result for query `Position, ?Velocity`
```json
{
    "ids": ["Position", "Velocity"],
  "results": [{
        "is_set": [true, false]
  }]
}
```

#### **serialize_values**
Serialize the ["values"](#values) member.

**Default**: `true`

**Example**:

Example result for query `Position`
```json
{
  "ids": ["Position"],
  "results": [{
    "values": [
      [{
        "x": 10,
        "y": 20
      }]
    ]
  }]
}
```

#### **serialize_entities**
Serialize the ["entities"](#entities) member.

**Default**: `true`

**Example**:
```json
{
  "results": [{
    "entities": ["MyEntity"]
  }]
}
```

#### **serialize_entity_labels**
Serialize the ["entity_labels"](#entity_labels) member.

**Default**: `false`

**Example**:
```json
{
  "results": [{
    "entities": ["Parent.MyEntity"],
        "entity_labels": ["My entity"]
  }]
}
```

#### **serialize_variable_labels**
Serialize the ["variable_labels"](#variable_labels) member.

**Default**: `false`

**Example**:
```json
{
  "results": [{
    "variables": ["GrannySmith"],
    "variable_labels": ["Granny smith"]
  }]
}
```

#### **measure_eval_duration**
Serialize the ["eval_duration"](#eval_duration) member.

**Default**: `false`

**Example**:
```json
{
  "eval_duration": 0.001
}
```

#### **serialize_type_info**
Serialize the ["type_info"](#type_info_1) member.

**Default**: `false`

**Example**:
```json
{
  "ids": ["Position", "Jedi"],
  "type_info": [{
      "x": ["float"],
      "y": ["float"]
  }, 0],
  "results": [{
  }]
}
```
