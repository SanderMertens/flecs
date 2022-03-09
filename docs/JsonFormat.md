# JSON format
This document provides an overview of the JSON serializer format. For an overview of how to use the JSON serializer API, [see the JSON addon documentation](https://flecs.docsforge.com/master/api-json/).

## Value kinds
This section describes value kinds that are reused across JSON serializers.

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

Each element is formatted as a [Path](#Path).

**Example**: 
```json
["transform.Position"]
["Likes", "Apples"]
["Movement", 0, "SWITCH"]
```

### **Id label**
Same as [Id](#Id) but with [Label](Label)'s instead of paths.

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

### **Entity**
An entity is the top-level object returned by the entity serializer. It can have the following properties:

#### **"path"**
The entity path.

Type: [Path](#Path), optional

#### **"label"**
The entity doc name.

Type: [Label](#Label), optional

#### **"is_a"**
Base entities.

Type: Array([Entity](#Entity))

#### **"ids"**
Component ids.

Type: Array([Id](#Id))

#### **"id_labels"**
Component labels.

Type: Array([Id label](#Id_label))

#### **"hidden"**
Array indicating whether a component is hidden. Only applies to components of base entities (in the ["is_a"](_is_a_) array).

Type: Array(bool)

#### **"values"**
Component values.

#### **Example**
See [Entity format](#Entity_format).

Type: Array([Value](#Value))

## Entity format
The entity format serializes the data and metadata of a single entity to JSON. This format is returned by functions that serialize entities to JSON, like `ecs_entity_to_json` and `flecs::entity::to_json`.

### Example
This example uses the default serializer options:
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

### Serializer options
The following options (found in `ecs_entity_to_json_desc_t`) customize the output of the entity serializer:

#### **serialize_path**
Serialize the ["path"](#_path_) member.

**Default**: `true`

**Example**:
```json
{"path": "SolarSystem.Sun.Earth"}
```

#### **serialize_label**
Serialize the ["label"](#_label_) member.

**Default**: `false`

**Example**:
```json
{"label": "A rocky planet"}
```

#### **serialize_id_labels**
Serializes the ["id_labels"](#_id_labels_) member.

**Default**: `false`

**Example**: 
```json
{"id_labels": [["A Rocky Planet"], ["Has surface water"]]}
```

#### **serialize_base**
Serializes the ["is_a"](#_is_a_) member.

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
Serializes the ["hidden"](#_hidden_) member.

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
Serializes the ["values"](#_values_) member.

**Example**: 
```json
{"values":[{"x":10, "y": 20}, 0]}
```

**Default**: `false`

#### **serialize_type_info**
Serialize the ["type_info"](#_type_info_) member.

**Example**: 
```json
{
    "type_info": [{
        "x": ["float"],
        "y": ["float"]
    }, 0, 0]
}
```
