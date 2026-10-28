/**
 * @file addons/script_command.h
 * @brief Command line interface for interacting with a world.
 */

#ifdef FLECS_SCRIPT_COMMAND

#ifndef FLECS_SCRIPT
#define FLECS_SCRIPT
#endif

#ifndef FLECS_QUERY_DSL
#define FLECS_QUERY_DSL
#endif

#ifndef FLECS_FRAME
#define FLECS_FRAME
#endif

/**
 * @defgroup c_addons_script_command Script Command
 * @ingroup c_addons
 * Command line interface for interacting with a world.
 *
 * Commands are script functions in the flecs.script.command scope. Each
 * command has typed parameters and a typed return value. A command line is
 * parsed into a command name and arguments, where arguments are separated by
 * whitespace:
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
 * @{
 */

#ifndef FLECS_SCRIPT_COMMAND_H
#define FLECS_SCRIPT_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

/** Default number of commands kept in the history of a session. */
#ifndef FLECS_SCRIPT_COMMAND_HISTORY_SIZE
#define FLECS_SCRIPT_COMMAND_HISTORY_SIZE (256)
#endif

/** Result of running a command. */
typedef struct ecs_command_result_t {
    /** Value returned by the command. Empty when the command has no return
     * value or failed. Commands that edit script code are the exception: when
     * the edit was applied but reloading the script failed, the result is set
     * together with the errors of the reload. */
    ecs_value_t result;

    /** Errors logged while the command ran, one entry per error (vec<char*>). */
    ecs_vec_t errors;
} ecs_command_result_t;

/** Command session.
 * A session stores the scope relative to which commands resolve identifiers,
 * and a bounded history of the commands that ran in the session.
 */
typedef struct EcsCommandSession {
    ecs_entity_t scope;      /**< Current scope, 0 for the root. */
    ecs_vec_t history;       /**< Commands that ran, oldest first (vec<char*>). */
    int32_t history_size;    /**< Maximum number of commands in the history. */
} EcsCommandSession;

/** Used with ecs_command_session_init(). */
typedef struct ecs_command_session_desc_t {
    ecs_entity_t entity;     /**< Entity to use for the session (optional). */
    ecs_entity_t scope;      /**< Initial scope (optional, default is root). */
    int32_t history_size;    /**< History size (optional, default is FLECS_SCRIPT_COMMAND_HISTORY_SIZE). */
} ecs_command_session_desc_t;

/** Parameter of a command, returned by help. */
typedef struct ecs_command_param_info_t {
    char *name;              /**< Parameter name. */
    ecs_entity_t type;       /**< Parameter type. */
    bool optional;           /**< Whether the parameter is optional. */
    char *brief;             /**< Parameter description. */
} ecs_command_param_info_t;

/** Command, returned by help. */
typedef struct ecs_command_info_t {
    char *name;              /**< Command name. */
    char *brief;             /**< Command description. */
    ecs_vec_t params;        /**< Parameters (vec<ecs_command_param_info_t>). */
    ecs_entity_t return_type; /**< Return type, 0 if the command has no result. */
} ecs_command_info_t;

/** Entity in a tree, returned by tree. */
typedef struct ecs_command_tree_node_t {
    ecs_entity_t entity;     /**< The entity. */
    int32_t depth;           /**< Depth relative to the scope, 0 for children. */
} ecs_command_tree_node_t;

/** Component and value. */
typedef struct ecs_command_component_value_t {
    ecs_id_t id;             /**< Component id. */
    ecs_value_t value;       /**< Component value. */
} ecs_command_component_value_t;

/** Entity information, returned by cat. */
typedef struct ecs_command_entity_info_t {
    ecs_entity_t entity;     /**< The entity. */
    uint64_t id;             /**< Numeric entity id. */
    ecs_vec_t components;    /**< Components with values (vec<ecs_command_component_value_t>). */
    ecs_vec_t ids;           /**< Ids without values (vec<ecs_id_t>). */
} ecs_command_entity_info_t;

/** Documentation, returned by man. */
typedef struct ecs_command_doc_info_t {
    ecs_entity_t entity;     /**< The entity. */
    char *name;              /**< Doc name. */
    char *brief;             /**< Brief description. */
    char *detail;            /**< Detailed description. */
    char *link;              /**< Link to external documentation. */
    char *color;             /**< Color. */
} ecs_command_doc_info_t;

/** Query result row, returned by query. */
typedef struct ecs_command_query_row_t {
    ecs_entity_t entity;     /**< Matched entity, 0 if the query has no $this. */
    ecs_vec_t fields;        /**< Fields with a value (vec<ecs_command_component_value_t>). */
} ecs_command_query_row_t;

/** Match returned by grep. */
typedef struct ecs_command_grep_match_t {
    ecs_entity_t entity;     /**< Entity that matched. */
    ecs_id_t id;             /**< Component that matched, 0 for the name. */
    char *text;              /**< Text that matched (name, id or value). */
} ecs_command_grep_match_t;

/** Script, returned by scripts. */
typedef struct ecs_command_script_info_t {
    ecs_entity_t script;     /**< Script entity. */
    char *filename;          /**< Filename, NULL if not loaded from a file. */
    int32_t lines;           /**< Number of lines. */
    bool unsaved;            /**< Whether the code differs from the saved code. */
    char *error;             /**< Error of last evaluation, NULL if none. */
} ecs_command_script_info_t;

/** Line of script code. */
typedef struct ecs_command_script_line_t {
    int32_t line;            /**< Line number (1 based). */
    char *text;              /**< Text of the line. */
} ecs_command_script_line_t;

/** Match returned by script_grep. */
typedef struct ecs_command_script_match_t {
    ecs_entity_t script;     /**< Script entity. */
    int32_t line;            /**< Line number (1 based). */
    char *text;              /**< Text of the line. */
} ecs_command_script_match_t;

/** Location of an entity statement, returned by script_where. */
typedef struct ecs_command_script_location_t {
    ecs_entity_t script;     /**< Script that declares the entity. */
    int32_t line;            /**< Line of the statement (1 based). */
    int32_t column;          /**< Column of the statement (1 based). */
    ecs_entity_t template_;  /**< Template that declares the statement, if any. */
    char *statement;         /**< First line of the statement. */
} ecs_command_script_location_t;

/** Script error, returned by script_errors. */
typedef struct ecs_command_script_error_t {
    ecs_entity_t script;     /**< Script entity. */
    char *error;             /**< Error message. */
} ecs_command_script_error_t;

#ifdef FLECS_SCRIPT_INPUT
/** Pointer state, returned by pointer. */
typedef struct ecs_command_pointer_state_t {
    EcsScriptMouseEvent event; /**< Last mouse event. */
    ecs_entity_t pressed;    /**< Entity pressed by held buttons, 0 if none. */
} ecs_command_pointer_state_t;
#endif

/** Session component. */
FLECS_API
extern ECS_COMPONENT_DECLARE(EcsCommandSession);

/** Create a command session.
 * When no entity is provided, the session is created in the
 * script.command.sessions scope.
 *
 * @param world The world.
 * @param desc Session parameters (optional).
 * @return The session entity, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_command_session_init(
    ecs_world_t *world,
    const ecs_command_session_desc_t *desc);

/** Run a command.
 * Parses the command line, runs the command and returns its result and the
 * errors that were logged while it ran. The command is added to the history
 * of the session. The result must be freed with ecs_command_result_fini().
 *
 * @param world The world.
 * @param session The session.
 * @param cmd The command line.
 * @return The result of the command.
 */
FLECS_API
ecs_command_result_t ecs_command_run(
    ecs_world_t *world,
    ecs_entity_t session,
    const char *cmd);

/** Free the resources of a command result.
 *
 * @param world The world.
 * @param result The result.
 */
FLECS_API
void ecs_command_result_fini(
    ecs_world_t *world,
    ecs_command_result_t *result);

/** Get the session of the running command.
 * Commands use this to access the session that they run in.
 *
 * @param world The world.
 * @return The session, or 0 when called outside of ecs_command_run().
 */
FLECS_API
ecs_entity_t ecs_command_get_session(
    const ecs_world_t *world);

/** Script command import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsScriptCommand)
 * @endcode
 *
 * @param world The world.
 */
FLECS_API
void FlecsScriptCommandImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
