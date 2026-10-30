/**
 * @file addons/script_shell.h
 * @brief Command line interface for interacting with a world.
 */

#ifdef FLECS_SCRIPT_SHELL

#ifndef FLECS_SCRIPT
#define FLECS_SCRIPT
#endif

#ifndef FLECS_QUERY_DSL
#define FLECS_QUERY_DSL
#endif

#ifndef FLECS_FRAME
#define FLECS_FRAME
#endif

#ifndef FLECS_JSON
#define FLECS_JSON
#endif

/**
 * @defgroup c_addons_script_shell Script Shell
 * @ingroup c_addons
 * Command line interface for interacting with a world.
 *
 * Commands are script functions. The command_paths member of the
 * EcsScriptShell singleton lists the scopes that contain commands. When
 * running a command, the scopes are searched in order and the first function
 * with a matching name is used. The builtin commands are stored in:
 *
 * - flecs.script.shell.commands.core
 * - flecs.script.shell.commands.input
 * - flecs.script.shell.commands.script
 *
 * Applications can add commands by appending their own scopes to
 * command_paths.
 *
 * Each command has typed parameters and a typed return value. A command line
 * is parsed into a command name and arguments, where arguments are separated
 * by whitespace:
 *
 * - A token that starts with `"` ends at the next unescaped `"`. The quotes are
 *   removed and escape sequences (`\n`, `\t`, ...) are processed.
 * - A token that starts with three backticks ends at the next three backticks.
 *   The backticks are removed, the text in between is not processed.
 * - `()`, `{}` and `[]` group a token and can be nested. They are passed to
 *   the command unchanged.
 *
 * Arguments for string parameters are passed as is. Arguments for other
 * parameters are parsed as expressions of the parameter type. Identifiers are
 * resolved relative to the scope of the session. For entity parameters `$`
 * means the singleton, which uses the component argument as entity.
 *
 * Commands with optional parameters can be called with fewer arguments. Use
 * the help command to list the commands and their parameters.
 *
 * A command line can contain multiple commands separated by `;`. Commands run
 * in order, and each command sees the session state left by the previous one.
 * When a command fails the remaining commands do not run. A `;` in a quoted
 * string, between backticks or in a group is not a separator.
 *
 * Commands can store files, like images, as resources of the session with
 * ecs_shell_resource_create(). A command signals that it returned a resource
 * by returning the flecs.script.shell.resource type (ecs_shell_resource_t).
 * Clients can then fetch the content of the resource with the REST API:
 *
 * @code
 * GET /shell/session/<session>/resource/<name>
 * @endcode
 *
 * Resources are stored in CWD/.flecs/shell/<session>/<name>. The directory is
 * created when the first resource of a session is created, and is deleted
 * together with the session.
 * @{
 */

#ifndef FLECS_SCRIPT_SHELL_H
#define FLECS_SCRIPT_SHELL_H

#ifdef __cplusplus
extern "C" {
#endif

/** Default number of commands kept in the history of a session. */
#ifndef FLECS_SCRIPT_SHELL_HISTORY_SIZE
#define FLECS_SCRIPT_SHELL_HISTORY_SIZE (256)
#endif

/** Result of running a command line. */
typedef struct ecs_shell_result_t {
    /** Values returned by the commands that ran, one entry per command in the
     * order of the command line (vec<ecs_value_t>). The last entry belongs to
     * the command that failed, if any. An entry is empty when the command has
     * no return value or failed. Commands that edit script code are the
     * exception: when the edit was applied but reloading the script failed,
     * the entry is set together with the errors of the reload. */
    ecs_vec_t results;

    /** Errors logged while the commands ran, one entry per error (vec<char*>). */
    ecs_vec_t errors;
} ecs_shell_result_t;

/** Resource created by a command, see ecs_shell_resource_create(). */
typedef struct ecs_shell_resource_t {
    char *name;              /**< Name of the resource, unique within the session. */
    char *content_type;      /**< Content type (for example "image/png"). */
} ecs_shell_resource_t;

/** Command session.
 * A session stores the scope relative to which commands resolve identifiers,
 * and a bounded history of the commands that ran in the session together with
 * their results.
 *
 * The results are stored as JSON objects with the scope in which the command
 * ran, and the type, result and errors of the command:
 *
 * @code
 * {"scope":"parent", "type":"flecs.meta.string", "result":"...", "errors":[]}
 * @endcode
 *
 * The type and result members are omitted when the command did not return a
 * value. The results and resources members are not reflected.
 */
typedef struct EcsCommandSession {
    ecs_entity_t scope;      /**< Current scope, 0 for the root. */
    ecs_vec_t history;       /**< Commands that ran, oldest first (vec<char*>). */
    int32_t history_size;    /**< Maximum number of commands in the history. */
    ecs_vec_t results;       /**< JSON results of the commands in the history (vec<char*>). */
    ecs_vec_t resources;     /**< Resources created by commands (vec<ecs_shell_resource_t>). */
    char *resource_path;     /**< Directory with the resource files, NULL if no resources were created. */
} EcsCommandSession;

/** Shell singleton. */
typedef struct EcsScriptShell {
    ecs_vec_t command_paths; /**< Scopes that contain commands, searched in order (vec<ecs_entity_t>). */
} EcsScriptShell;

/** Used with ecs_shell_session_init(). */
typedef struct ecs_shell_session_desc_t {
    ecs_entity_t entity;     /**< Entity to use for the session (optional). */
    ecs_entity_t scope;      /**< Initial scope (optional, default is root). */
    int32_t history_size;    /**< History size (optional, default is FLECS_SCRIPT_SHELL_HISTORY_SIZE). */
} ecs_shell_session_desc_t;

/** Parameter of a command, returned by help. */
typedef struct ecs_shell_param_info_t {
    char *name;              /**< Parameter name. */
    ecs_entity_t type;       /**< Parameter type. */
    bool optional;           /**< Whether the parameter is optional. */
    char *brief;             /**< Parameter description. */
} ecs_shell_param_info_t;

/** Command, returned by help. */
typedef struct ecs_shell_info_t {
    char *name;              /**< Command name. */
    char *brief;             /**< Command description. */
    ecs_vec_t params;        /**< Parameters (vec<ecs_shell_param_info_t>). */
    ecs_entity_t return_type; /**< Return type, 0 if the command has no result. */
} ecs_shell_info_t;

/** Entity in a tree, returned by tree. */
typedef struct ecs_shell_tree_node_t {
    ecs_entity_t entity;     /**< The entity. */
    int32_t depth;           /**< Depth relative to the scope, 0 for children. */
} ecs_shell_tree_node_t;

/** Component and value. */
typedef struct ecs_shell_component_value_t {
    ecs_id_t id;             /**< Component id. */
    ecs_value_t value;       /**< Component value. */
} ecs_shell_component_value_t;

/** Entity information, returned by cat. */
typedef struct ecs_shell_entity_info_t {
    ecs_entity_t entity;     /**< The entity. */
    uint64_t id;             /**< Numeric entity id. */
    ecs_vec_t components;    /**< Components with values (vec<ecs_shell_component_value_t>). */
    ecs_vec_t ids;           /**< Ids without values (vec<ecs_id_t>). */
} ecs_shell_entity_info_t;

/** Documentation, returned by man. */
typedef struct ecs_shell_doc_info_t {
    ecs_entity_t entity;     /**< The entity. */
    char *name;              /**< Doc name. */
    char *brief;             /**< Brief description. */
    char *detail;            /**< Detailed description. */
    char *link;              /**< Link to external documentation. */
    char *color;             /**< Color. */
} ecs_shell_doc_info_t;

/** Query result row, returned by query. */
typedef struct ecs_shell_query_row_t {
    ecs_entity_t entity;     /**< Matched entity, 0 if the query has no $this. */
    ecs_vec_t fields;        /**< Fields with a value (vec<ecs_shell_component_value_t>). */
} ecs_shell_query_row_t;

/** Match returned by grep. */
typedef struct ecs_shell_grep_match_t {
    ecs_entity_t entity;     /**< Entity that matched. */
    ecs_id_t id;             /**< Component that matched, 0 for the name. */
    char *text;              /**< Text that matched (name, id or value). */
} ecs_shell_grep_match_t;

/** Script, returned by scripts. */
typedef struct ecs_shell_script_info_t {
    ecs_entity_t script;     /**< Script entity. */
    char *filename;          /**< Filename, NULL if not loaded from a file. */
    int32_t lines;           /**< Number of lines. */
    bool unsaved;            /**< Whether the code differs from the saved code. */
    char *error;             /**< Error of last evaluation, NULL if none. */
} ecs_shell_script_info_t;

/** Line of script code. */
typedef struct ecs_shell_script_line_t {
    int32_t line;            /**< Line number (1 based). */
    char *text;              /**< Text of the line. */
} ecs_shell_script_line_t;

/** Match returned by script_grep. */
typedef struct ecs_shell_script_match_t {
    ecs_entity_t script;     /**< Script entity. */
    int32_t line;            /**< Line number (1 based). */
    char *text;              /**< Text of the line. */
} ecs_shell_script_match_t;

/** Location of an entity statement, returned by script_where. */
typedef struct ecs_shell_script_location_t {
    ecs_entity_t script;     /**< Script that declares the entity. */
    int32_t line;            /**< Line of the statement (1 based). */
    int32_t column;          /**< Column of the statement (1 based). */
    ecs_entity_t template_;  /**< Template that declares the statement, if any. */
    char *statement;         /**< First line of the statement. */
} ecs_shell_script_location_t;

/** Script error, returned by script_errors. */
typedef struct ecs_shell_script_error_t {
    ecs_entity_t script;     /**< Script entity. */
    char *error;             /**< Error message. */
} ecs_shell_script_error_t;

#ifdef FLECS_SCRIPT_INPUT
/** Pointer state, returned by pointer. */
typedef struct ecs_shell_pointer_state_t {
    EcsScriptMouseEvent event; /**< Last mouse event. */
    ecs_entity_t pressed;    /**< Entity pressed by held buttons, 0 if none. */
} ecs_shell_pointer_state_t;
#endif

/** Shell component. */
FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScriptShell);

/** Session component. */
FLECS_API
extern ECS_COMPONENT_DECLARE(EcsCommandSession);

/** Resource type (flecs.script.shell.resource). */
FLECS_API
extern ECS_COMPONENT_DECLARE(ecs_shell_resource_t);

/** Create a command session.
 * When no entity is provided, the session is created in the
 * script.shell.sessions scope.
 *
 * @param world The world.
 * @param desc Session parameters (optional).
 * @return The session entity, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_shell_session_init(
    ecs_world_t *world,
    const ecs_shell_session_desc_t *desc);

/** Run a command line.
 * Parses the command line, runs its commands and returns their results and the
 * errors that were logged while they ran. Commands are separated by `;`. When a
 * command fails the remaining commands do not run. Each command that ran and a
 * JSON snapshot of its result are added to the history of the session. The
 * result must be freed with ecs_shell_result_fini().
 *
 * @param world The world.
 * @param session The session.
 * @param cmd The command line.
 * @return The results of the commands.
 */
FLECS_API
ecs_shell_result_t ecs_shell_run(
    ecs_world_t *world,
    ecs_entity_t session,
    const char *cmd);

/** Free the resources of a command result.
 *
 * @param world The world.
 * @param result The result.
 */
FLECS_API
void ecs_shell_result_fini(
    ecs_world_t *world,
    ecs_shell_result_t *result);

/** Get the session of the running command.
 * Commands use this to access the session that they run in.
 *
 * @param world The world.
 * @return The session, or 0 when called outside of ecs_shell_run().
 */
FLECS_API
ecs_entity_t ecs_shell_get_session(
    const ecs_world_t *world);

/** Create a resource in a session.
 * Writes the data to a new file in the resource directory of the session, and
 * adds the resource to the session. The resource directory is created when the
 * first resource is created. Files are written with the OS API.
 *
 * A command that returns a resource typically does:
 *
 * @code
 * ecs_shell_resource_create(ctx->world, ecs_shell_get_session(ctx->world),
 *     "image/png", data, size, result->ptr);
 * @endcode
 *
 * @param world The world.
 * @param session The session.
 * @param content_type The content type of the data (optional).
 * @param data The data to store.
 * @param size The size of the data.
 * @param resource Out parameter for the created resource (optional). The
 *        members must be freed by the caller.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_shell_resource_create(
    ecs_world_t *world,
    ecs_entity_t session,
    const char *content_type,
    const void *data,
    ecs_size_t size,
    ecs_shell_resource_t *resource);

/** Read a resource of a session.
 * Only resources that were created in the session can be read.
 *
 * @param world The world.
 * @param session The session.
 * @param name The name of the resource.
 * @param size Out parameter for the size of the data.
 * @param content_type Out parameter for the content type (optional). The
 *        string is owned by the session.
 * @return The data, NULL if the resource does not exist or could not be read.
 *         The data must be freed with ecs_os_free().
 */
FLECS_API
void* ecs_shell_resource_read(
    const ecs_world_t *world,
    ecs_entity_t session,
    const char *name,
    ecs_size_t *size,
    const char **content_type);

/** Script shell import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsScriptShell)
 * @endcode
 *
 * @param world The world.
 */
FLECS_API
void FlecsScriptShellImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
