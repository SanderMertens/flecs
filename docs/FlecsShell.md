# Flecs Shell

## Introduction
Flecs Shell is a command line interface that provides a quick way to inspect and modify a running world. Flecs Shell has the look and feel of a Unix shell:

```ruby
$ cd game
"game"

game$ ls
["game.player", "game.enemy"]

game$ get player Position
{"Position": {"x": 10, "y": 20}}

$ set player Position {x: 15}
{"Position": {"x": 15, "y": 20}}

$ query "Position, Velocity"
[
  {"entity": "game.player", "fields": [
    {"id": ["Position"], "value": {"Position": {"x": 15, "y": 20}}},
    {"id": ["Velocity"], "value": {"Velocity": {"x": 1, "y": 0}}}]},
  {"entity": "game.enemy", "fields": [
    {"id": ["Position"], "value": {"Position": {"x": 50, "y": 30}}},
    {"id": ["Velocity"], "value": {"Velocity": {"x": 1, "y": 0}}}]}
]
```

Use cases for Flecs Shell are:
- a debug console for a game
- running quick repeatable scripts
- remote controlling one or more games

## The Basics
A command is a name followed by a number of arguments:

```ruby
set bob Position {10, 20}
```

Arguments are separated by whitespaces, unless they are surrounded by:
- double quotes: "foo bar"
- three backticks: \`\`\`hello world\`\``
- parentheses: (hello, world)
- curly braces: {hello: world}
- angular brackets: [hello, world]

To get a list of supported commands, run the help command:

```ruby
help
```

Arguments are parsed as flecs script expressions. The following command is valid:

```ruby
set bob Position {10, 5 + 15}
```

### Navigating
Commands are ran relative to a scope. The following examples show how to move the scope around:

```ruby
$ cd game
"game"

game$ pwd
"game"

game$ cd
"#0"

$
```

The `ls` command lists the children of an entity. `tree` lists all descendants with their depth:

```ruby
$ ls game
["game.player", "game.enemy"]

$ tree game
[{"entity": "game.player", "depth": 0},
 {"entity": "game.enemy", "depth": 0}]
```

Without an argument, `ls` and `tree` use the current scope. Entity arguments can also be relative paths, where `.` is the current scope and `..` its parent:

```ruby
$ cd game
"game"

$ cat ./player
...

$ ls ..
["flecs", "scene", "game", "Position", "Velocity", "SpaceShip"]
```

### Inspecting entities
The `cat` command shows an entity with its components and values:

```ruby
$ cat game.player
{"entity": "game.player", "id": 615,
 "components": [
   {"id": ["Position"], "value": {"Position": {"x": 10, "y": 20}}},
   {"id": ["Velocity"], "value": {"Velocity": {"x": 1, "y": 0}}},
   {"id": ["flecs.core.Identifier", "flecs.core.Name"], "value": {"EcsIdentifier": {"value": "player"}}}],
 "ids": [
   ["flecs.script.Script", "scene"],
   ["flecs.core.ChildOf", "game"],
   ["flecs.core.IsA", "SpaceShip"]]}
```

Use `get` to get the value of a single component:

```ruby
$ get game.player Position
{"Position": {"x": 10, "y": 20}}
```

The `schema` command returns the definition of a type as script code, and `schema_value` returns its default value:

```ruby
$ schema Position
"struct Position(x: f32, y: f32)\n"

$ schema_value Position
{"Position": {"x": 0, "y": 0}}
```

The `man` command returns the [documentation](/flecs/group__c__addons__doc.html) of an entity, such as its brief description and link.

### Modifying entities
The `set` command assigns a component value. The value is parsed into the current value of the component, which means that members that are not specified keep their value:

```ruby
$ set game.player Position {x: 15}
{"Position": {"x": 15, "y": 20}}
```

Components, tags and pairs are added with `add`, and removed with `rm`:

```ruby
$ add game.player Velocity
["Velocity"]

$ add game.player (ChildOf, game.enemy)
["flecs.core.ChildOf", "game.enemy"]

$ rm game.player Velocity
```

When `rm` is called without a component, it deletes the entity. New entities are created in the current scope with `new`, and entities are moved or renamed with `mv`:

```ruby
$ cd game
"game"

$ new asteroid
"game.asteroid"

$ mv asteroid game.rock
"game.rock"
```

To get or set a singleton, use `$` as entity:

```ruby
$ set $ TimeOfDay {t: 0.5}
{"TimeOfDay": {"t": 0.5}}
```

### Searching
The `find` command finds entities by name, and `grep` searches entity names and the names and values of components. Both search the descendants of the current scope:

```ruby
$ find play
["game.player"]

$ grep 50
[{"entity": "game.enemy", "id": ["Position"], "text": "{x: 50, y: 30}"}]
```

Use `grep_i` for a case insensitive search. The `query` command runs a query written in the [query language](FlecsQueryLanguage.md), and `eval` evaluates an [expression](FlecsScript.md#expressions):

```ruby
$ query "Position, !Velocity"
[]

$ eval "10 + 20"
{"i64": 30}
```

## Custom commands
Commands are [script functions](FlecsScript.md#functions). Applications can add commands by creating functions in a scope, and adding that scope to the `command_paths` member of the `EcsScriptShell` singleton:

```c
ecs_entity_t commands = ecs_entity(world, { .name = "game.commands" });

ecs_entity_t spawn = ecs_function(world, {
    .name = "spawn",
    .parent = commands,
    .return_type = ecs_id(ecs_entity_t),
    .params = {
        { .name = "prefab", .type = ecs_id(ecs_entity_t),
          .brief = "Prefab to instantiate" },
        { .name = "count", .type = ecs_id(ecs_i32_t), .optional = true,
          .brief = "Number of instances (default: 1)" }
    },
    .callback = spawn_command
});

ecs_doc_set_brief(world, spawn, "Spawn instances of a prefab");

EcsScriptShell *shell = ecs_singleton_ensure(world, EcsScriptShell);
ecs_vec_append_t(NULL, &shell->command_paths, ecs_entity_t)[0] = commands;
ecs_singleton_modified(world, EcsScriptShell);
```

The command can then be used like any other command:

```
> spawn SpaceShip
> spawn SpaceShip 10
```

The `help` command describes the command with the doc brief of the function and the briefs of its parameters. When a command runs, the scopes in `command_paths` are searched in order, and the first function with a matching name is used. A command can override a builtin command by inserting its scope before the builtin scopes.

The callback is a regular script function callback. The `argc` argument is the number of arguments that were provided, so optional parameters can be detected. A command gets the session it runs in with `ecs_shell_get_session()`, and reports errors by logging them with `ecs_err()`:

```c
void spawn_command(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_entity_t prefab = *(ecs_entity_t*)argv[0].ptr;
    int32_t count = argc > 1 ? *(int32_t*)argv[1].ptr : 1;

    if (!ecs_has_id(ctx->world, prefab, EcsPrefab)) {
        ecs_err("'%s' is not a prefab", ecs_get_name(ctx->world, prefab));
        return;
    }

    ecs_entity_t session = ecs_shell_get_session(ctx->world);
    const EcsCommandSession *s = ecs_get(ctx->world, session, EcsCommandSession);

    ecs_entity_t e = 0;
    for (int32_t i = 0; i < count; i ++) {
        e = ecs_new_w_pair(ctx->world, EcsIsA, prefab);
        if (s->scope) {
            ecs_add_pair(ctx->world, e, EcsChildOf, s->scope);
        }
    }

    *(ecs_entity_t*)result->ptr = e;
}
```

## Resources
Commands can return files, like images, by storing them as resources of the session. A client that receives a resource can fetch its content to show it to the user. For example, a `capture` command could return a PNG with a capture of the screen:

```
> capture
{"name": "1", "content_type": "image/png"}
```

A command creates a resource with `ecs_shell_resource_create()`, and signals that it returned a resource by returning the `flecs.script.shell.resource` type (`ecs_shell_resource_t`):

```c
void capture_command(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    void *png = NULL;
    ecs_size_t png_size = 0;
    if (capture_screen_png(&png, &png_size)) {
        ecs_err("failed to capture screen");
        return;
    }

    ecs_shell_resource_create(ctx->world,
        ecs_shell_get_session(ctx->world), "image/png", png, png_size,
        result->ptr);

    free(png);
}

ecs_function(world, {
    .name = "capture",
    .parent = commands,
    .return_type = ecs_id(ecs_shell_resource_t),
    .callback = capture_command
});
```

Resource names are unique within a session. The content of a resource is read with `ecs_shell_resource_read()`, or remotely with `GET /shell/session/<id>/resource/<name>`. Only resources that were created in the session can be read.

Resources are stored in `.flecs/shell/<session>/<name>`, relative to the current working directory. The directory of a session is created when the first resource is created, and is deleted together with the session. Files are written, read and removed with the [OS API](/flecs/group__c__os__api.html), which applications can override to store resources elsewhere.

## API
This section goes over how to use the shell in an application. The shell is implemented by the script shell addon, which is enabled by defining `FLECS_SCRIPT_SHELL`. To use the shell, import its module:

```c
ECS_IMPORT(world, FlecsScriptShell);
```

### Creating a session
A session is created with `ecs_shell_session_init()`. When no entity is provided, sessions are created in the `script.shell.sessions` scope:

```c
ecs_entity_t session = ecs_shell_session_init(world, NULL);
```

A session can also be created for an existing entity, with an initial scope and a custom history size:

```c
ecs_entity_t session = ecs_shell_session_init(world,
    &(ecs_shell_session_desc_t){
        .entity = my_entity,
        .scope = ecs_lookup(world, "game"),
        .history_size = 100
    });
```

A session is closed by deleting its entity, which also deletes its resources:

```c
ecs_delete(world, session);
```

### Running a command
A command is run with `ecs_shell_run()`, which returns the result of the command and the errors that were logged while it ran. The result must be freed with `ecs_shell_result_fini()`:

```c
ecs_shell_result_t r = ecs_shell_run(world, session, "ls game");

if (ecs_vec_count(&r.errors)) {
    char **errors = ecs_vec_first(&r.errors);
    for (int i = 0; i < ecs_vec_count(&r.errors); i ++) {
        printf("error: %s\n", errors[i]);
    }
} else if (r.result.type) {
    char *json = ecs_ptr_to_json(world, r.result.type, r.result.ptr);
    printf("%s\n", json);
    ecs_os_free(json);
}

ecs_shell_result_fini(world, &r);
```

The `result` member is empty when the command has no return value or failed. Script edit commands are the exception: when the edit was applied but reloading the script failed, the result is set together with the errors of the reload.