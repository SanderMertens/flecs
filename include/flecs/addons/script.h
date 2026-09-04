/**
 * @file addons/script.h
 * @brief Flecs script module.
 *
 * For script examples, see examples/script.
 */

#ifdef FLECS_SCRIPT

/**
 * @defgroup c_addons_script Flecs script
 * @ingroup c_addons
 * DSL for loading scenes, assets, and configuration.
 *
 * @{
 */

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_DOC
#define FLECS_DOC
#endif

#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif

#ifndef FLECS_SCRIPT_H
#define FLECS_SCRIPT_H

#ifdef __cplusplus
extern "C" {
#endif

#define FLECS_SCRIPT_FUNCTION_ARGS_MAX (16)

/* Must be the same as EcsPrimitiveKindLast */
#define FLECS_SCRIPT_VECTOR_FUNCTION_COUNT (18)

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScript);

/* Relationship added to entities that are created by a template body. The
 * target of the pair is the template that declares the statement that created
 * the entity, which for nested templates is the innermost one. The pair is
 * added to every entity created while instantiating a template body, at any
 * scope depth (`if`/`else`, `with`, pair scopes, `for` loops and nested entity
 * scopes), which makes it the way to tell entities that come from a template
 * apart from entities that are declared by a plain script statement. */
FLECS_API
extern ECS_DECLARE(EcsScriptTemplate);

/* Tag added to a managed script entity when the script failed to parse or
 * evaluate. The tag is removed when the script evaluates successfully. The
 * error message is stored in the error member of the EcsScript component. */
FLECS_API
extern ECS_DECLARE(EcsScriptError);

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScriptConstVar);

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScriptMutVar);

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScriptFunction);

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScriptMethod);

FLECS_API
extern ECS_DECLARE(EcsScriptVectorType);

/* Script template. */
typedef struct ecs_script_template_t ecs_script_template_t;

/** Script variable. */
typedef struct ecs_script_var_t {
    const char *name;                    /**< Variable name. */
    ecs_value_t value;                   /**< Variable value. */
    const ecs_type_info_t *type_info;    /**< Type information. */
    int32_t sp;                          /**< Stack pointer. */
    bool is_const;                       /**< Whether the variable is constant. */
    bool owned;                          /**< Whether the scope owns the value storage. */
} ecs_script_var_t;

/** Script variable scope. */
typedef struct ecs_script_vars_t {
    struct ecs_script_vars_t *parent;    /**< Parent variable scope. */
    int32_t sp;                          /**< Stack pointer for this scope. */

    ecs_hashmap_t var_index;             /**< Index for variable name lookups. */
    ecs_vec_t vars;                      /**< Vector of variables in this scope. */

    const ecs_world_t *world;            /**< The world. */
    struct ecs_stack_t *stack;           /**< Stack allocator for variable storage. */
    ecs_stack_cursor_t *cursor;          /**< Cursor into the stack allocator. */
    ecs_allocator_t *allocator;          /**< General purpose allocator. */
} ecs_script_vars_t;

/** Script object. */
typedef struct ecs_script_t {
    ecs_world_t *world;    /**< The world. */
    const char *name;      /**< Script name. */
    const char *code;      /**< Script source code. */
} ecs_script_t;

/** Runtime for executing scripts. */
typedef struct ecs_script_runtime_t ecs_script_runtime_t;

/** Script component. 
 * This component is added to the entities of managed scripts and templates.
 */
typedef struct EcsScript {
    char *filename;                     /**< Script filename. */
    char *code;                         /**< Script source code. */
    char *error;                        /**< Set if script evaluation had errors. */
    ecs_script_t *script;               /**< Parsed script object. */
    ecs_script_template_t *template_;   /**< Only set for template scripts. */
    ecs_vec_t observers;                /**< Observers for referenced components. */
    ecs_vec_t dyn_observers;            /**< Observers for refs resolved at runtime. */
    bool lenient;                       /**< Load script in lenient mode. */
    bool ir;                            /**< Evaluate script with IR runtime. */
} EcsScript;

/** Script function context. */
typedef struct ecs_function_ctx_t {
    ecs_world_t *world;       /**< The world. */
    ecs_entity_t function;    /**< The function entity. */
    ecs_entity_t entity;      /**< "this" entity (for methods and async functions). */
    void *ctx;                /**< User context. */
} ecs_function_ctx_t;

/** Script function callback. */
typedef void(*ecs_function_callback_t)(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result);

/** Script vector function callback. */
typedef void(*ecs_vector_function_callback_t)(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count);

/** Function argument type. */
typedef struct ecs_script_parameter_t {
    const char *name;       /**< Parameter name. */
    ecs_entity_t type;      /**< Parameter type. */
} ecs_script_parameter_t;

/** Used with ecs_script_parse() and ecs_script_eval(). */
typedef struct ecs_script_eval_desc_t {
    ecs_script_runtime_t *runtime; /**< Reusable runtime (optional). */

    /** Load script in lenient mode. Unknown components, members and functions
     * are tolerated instead of failing the script. See
     * ecs_script_set_lenient(). */
    bool lenient;

    /** Evaluate script with the IR runtime instead of the AST interpreter. */
    bool ir;
} ecs_script_eval_desc_t;

/** Used to capture error output from script evaluation. */
typedef struct ecs_script_eval_result_t {
    char *error;       /**< Error message, or NULL if no error. Must be freed by the application. */
    int32_t line;      /**< Line number (1-based) of first error, or 0 if not available. */
    int32_t column;    /**< Column number (1-based) of first error, or 0 if not available. */
} ecs_script_eval_result_t;

#ifdef FLECS_SCRIPT_ASYNC
#include "script_async.h"
#endif

/** Const component.
 * This component describes a const variable that can be used from scripts. The
 * value of a const variable is folded into expressions that use it.
 */
typedef struct EcsScriptConstVar {
    ecs_value_t value;
    const ecs_type_info_t *type_info;
} EcsScriptConstVar;

/** Mut component.
 * This component describes a mutable global variable that can be used from
 * scripts. Unlike a const variable, the value of a mut variable is never folded
 * into expressions, and scripts that use it are reevaluated when it changes.
 */
typedef struct EcsScriptMutVar {
    ecs_value_t value;
    const ecs_type_info_t *type_info;
} EcsScriptMutVar;

struct ecs_script_function_t {
    ecs_entity_t return_type;
    ecs_vec_t params; /* vec<ecs_script_parameter_t> */
    ecs_function_callback_t callback;
#ifdef FLECS_SCRIPT_ASYNC
    ecs_async_function_callback_t async_callback;
    ecs_async_function_cancel_t async_cancel;
#endif
    ecs_vector_function_callback_t vector_callbacks[FLECS_SCRIPT_VECTOR_FUNCTION_COUNT];
    void *ctx;
    void *binding_ctx;
    ecs_ctx_free_t binding_ctx_free;
};

/** Function component.
 * This component describes a function that can be called from a script.
 */
typedef struct ecs_script_function_t EcsScriptFunction;

/** Method component. 
 * This component describes a method that can be called from a script. Methods
 * are functions that can be called on instances of a type. A method entity is
 * stored in the scope of the type it belongs to.
 */
typedef struct ecs_script_function_t EcsScriptMethod;

/* Parsing and running scripts */

/** Parse script.
 * This operation parses a script and returns a script object upon success. To
 * run the script, call ecs_script_eval().
 *
 * When the result parameter is not NULL, the script will capture errors and
 * return them in the output struct. If result.error is set, it must be freed
 * by the application.
 * 
 * @param world The world.
 * @param name Name of the script (typically a file or module name).
 * @param code The script code.
 * @param desc Parameters for script runtime.
 * @param result Output of script evaluation.
 * @return Script object if success, NULL if failed.
*/
FLECS_API
ecs_script_t* ecs_script_parse(
    ecs_world_t *world,
    const char *name,
    const char *code,
    const ecs_script_eval_desc_t *desc,
    ecs_script_eval_result_t *result);

/** Evaluate script.
 * This operation evaluates (runs) a parsed script.
 *
 * When the result parameter is not NULL, the script will capture errors and
 * return them in the output struct. If result.error is set, it must be freed
 * by the application.
 * 
 * @param script The script.
 * @param desc Parameters for script runtime.
 * @param result Output of script evaluation (optional).
 * @return Zero if success, non-zero if failed.
*/
FLECS_API
int ecs_script_eval(
    const ecs_script_t *script,
    const ecs_script_eval_desc_t *desc,
    ecs_script_eval_result_t *result);

/** Free script.
 * This operation frees a script object.
 * 
 * Templates created by the script rely upon resources in the script object,
 * and for that reason keep the script alive until all templates created by the
 * script are deleted.
 *
 * @param script The script.
 */
FLECS_API
void ecs_script_free(
    ecs_script_t *script);

/** Parse script.
 * This parses a script and instantiates the entities in the world.
 * This operation is the equivalent to doing:
 * 
 * @code
 * ecs_script_t *script = ecs_script_parse(world, name, code);
 * ecs_script_eval(script);
 * ecs_script_free(script);
 * @endcode
 * 
 * @param world The world.
 * @param name The script name (typically the file).
 * @param code The script.
 * @param result Output of script evaluation (optional).
 * @return Zero if success, non-zero otherwise.
 */
FLECS_API
int ecs_script_run(
    ecs_world_t *world,
    const char *name,
    const char *code,
    ecs_script_eval_result_t *result);

/** Parse script and evaluate with options.
 * Same as ecs_script_run(), but accepts a descriptor that configures parsing
 * and evaluation, such as lenient mode or the runtime (AST or IR) to use.
 *
 * @param world The world.
 * @param name The script name (typically the file).
 * @param code The script.
 * @param desc Parse and evaluation options (optional).
 * @param result Output of script evaluation (optional).
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_script_run_w_desc(
    ecs_world_t *world,
    const char *name,
    const char *code,
    const ecs_script_eval_desc_t *desc,
    ecs_script_eval_result_t *result);

/** Parse script file.
 * This parses a script file and instantiates the entities in the world. This
 * operation is equivalent to loading the file contents and passing it to
 * ecs_script_run().
 *
 * @param world The world.
 * @param filename The script file name.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_script_run_file(
    ecs_world_t *world,
    const char *filename);

/** Parse script file and evaluate with options.
 * Same as ecs_script_run_file(), but accepts a descriptor that configures
 * parsing and evaluation.
 *
 * @param world The world.
 * @param filename The script file name.
 * @param desc Parse and evaluation options (optional).
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_script_run_file_w_desc(
    ecs_world_t *world,
    const char *filename,
    const ecs_script_eval_desc_t *desc);

/** Enable or disable lenient script loading for a world.
 * When lenient loading is enabled, scripts that reference unknown components,
 * component members or functions will still load. This makes it possible to
 * load scripts for an application without having to load the application code
 * that registers the components used by the script.
 *
 * In lenient mode:
 * - An unresolved identifier that is used as a component or tag creates a
 *   placeholder tag entity with that (scoped) name.
 * - A value assigned to a placeholder, to a component without reflection data,
 *   or to an unknown member is parsed and discarded.
 * - An expression that uses an unresolved function or identifier is discarded.
 *   This includes expressions that read an unknown component (`e[Unknown]`) or
 *   an unknown member (`e[Position].unknown`), and expressions that use a
 *   variable that was itself skipped, both directly and in an interpolated
 *   string. When the expression is the condition or collection of a statement
 *   (such as `for`), the statement is skipped.
 * - A `using` statement with an unresolved identifier is skipped.
 * - A function with an unresolved parameter or return type is skipped, which
 *   makes calls to that function unresolved expressions.
 * - Unresolved references in `IsA` expressions, template base types and
 *   template instantiations are still errors, as those are structural.
 *
 * Every skipped name is reported once with ecs_warn().
 *
 * Lenient mode can also be enabled per script with
 * ecs_script_eval_desc_t::lenient and ecs_script_desc_t::lenient.
 *
 * @param world The world.
 * @param lenient Whether to enable lenient script loading.
 */
FLECS_API
void ecs_script_set_lenient(
    ecs_world_t *world,
    bool lenient);

/** Return whether lenient script loading is enabled for a world.
 *
 * @param world The world.
 * @return Whether lenient script loading is enabled.
 */
FLECS_API
bool ecs_script_get_lenient(
    const ecs_world_t *world);

/** Convert script IR to string.
 * Compiles the script to IR if it hasn't been compiled yet, and returns a
 * human readable listing of the instructions.
 *
 * @param script The script.
 * @return The IR listing. Must be freed with ecs_os_free.
 */
FLECS_API
char* ecs_script_ir_to_str(
    const ecs_script_t *script);

#ifdef FLECS_SCRIPT_IR_PROFILE
/** Reset IR runtime profiling counters (only available when compiled with
 * FLECS_SCRIPT_IR_PROFILE). */
FLECS_API
void ecs_script_ir_profile_reset(void);

/** Get IR runtime profiling counters as string (only available when compiled
 * with FLECS_SCRIPT_IR_PROFILE). Must be freed with ecs_os_free. */
FLECS_API
char* ecs_script_ir_profile_str(void);
#endif

/** Create runtime for script.
 * A script runtime is a container for any data created during script 
 * evaluation. By default, calling ecs_script_run() or ecs_script_eval() will
 * create a runtime on the spot. A runtime can be created in advance and reused
 * across multiple script evaluations to improve performance.
 * 
 * When scripts are evaluated on multiple threads, each thread should have its
 * own script runtime.
 * 
 * A script runtime must be deleted with ecs_script_runtime_free().
 * 
 * @return A new script runtime.
 */
FLECS_API
ecs_script_runtime_t* ecs_script_runtime_new(void);

/** Free script runtime.
 * This operation frees a script runtime created by ecs_script_runtime_new().
 * 
 * @param runtime The runtime to free.
 */
FLECS_API
void ecs_script_runtime_free(
    ecs_script_runtime_t *runtime);

/** Convert script AST to string.
 * This operation converts the script abstract syntax tree to a string, which
 * can be used to debug a script.
 *
 * @param script The script.
 * @param buf The buffer to write to.
 * @param colors Whether to include ANSI color codes in the output.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_script_ast_to_buf(
    ecs_script_t *script,
    ecs_strbuf_t *buf,
    bool colors);

/** Convert script AST to string.
 * This operation converts the script abstract syntax tree to a string, which
 * can be used to debug a script.
 *
 * @param script The script.
 * @param colors Whether to include ANSI color codes in the output.
 * @return The string if success, NULL if failed.
 */
FLECS_API
char* ecs_script_ast_to_str(
    ecs_script_t *script,
    bool colors);


/* Source preserving script edits */

/** Source location of a script statement.
 * Returned by ecs_script_entity_source(). The offset and length are byte
 * offsets into ecs_script_t::code.
 */
typedef struct ecs_script_source_t {
    int32_t offset;     /**< Offset of the first character of the statement. */
    int32_t length;     /**< Number of bytes occupied by the statement. */
    int32_t line;       /**< Line of the first character (1 based). */
    int32_t column;     /**< Column of the first character (1 based). */
    bool has_scope;     /**< Whether the statement is followed by a { } scope. */

    /** Set when the statement is inside the body of a `template` statement.
     * When set, this is the template entity that the body belongs to, and the
     * statement is shared by every instance of that template: editing it
     * changes all instances. Zero for regular entity statements. */
    ecs_entity_t template_;
} ecs_script_source_t;

/** Find the statement that declares an entity.
 * This operation returns the source location of the entity statement that
 * declared an entity. The script must have been evaluated at least once, as
 * the mapping from entity to statement is established during evaluation.
 *
 * Entities that were created by instantiating a `template` declared by this
 * script are resolved to the statement in the template body that created them.
 * Because a template body statement is shared by all instances of the template,
 * the same statement is returned for every instance, and
 * ecs_script_source_t::template_ is set to the template entity. The template
 * must be declared by this script; when a script instantiates a template that
 * is declared by another script (for example a template from an included file),
 * the body entities are reported by the script that declares the template, not
 * by the script that instantiates it.
 *
 * The operation returns false when the entity was not created by an entity
 * statement of this script. This is the case for:
 *
 * - entities that were not created by a script
 * - entities created by a different script (an `include` statement evaluates
 *   the included file as a separate script object, so entities created by an
 *   included file are never reported by the including script)
 * - entities created inside a `for` loop or a function body, also when the
 *   `for` loop is part of a template body
 * - entities created by a template body statement with a computed name
 * - entities created by a "new" expression
 *
 * An entity that carries the (EcsScriptTemplate, *) pair is never reported as a
 * plain (non-template) statement, also not by the script that instantiates the
 * template.
 *
 * When an entity is declared by more than one statement, the location of the
 * first declaration is returned.
 *
 * The returned offsets stay valid until the script is freed or reparsed.
 *
 * @param script The script.
 * @param entity The entity to find.
 * @param source Out parameter with the source location (optional).
 * @return True if the entity is declared by this script, false if not.
 */
FLECS_API
bool ecs_script_entity_source(
    const ecs_script_t *script,
    ecs_entity_t entity,
    ecs_script_source_t *source);

/** Find the managed script that declares an entity.
 * This operation returns the entity of the managed script (see ecs_script())
 * whose source code contains the statement that created the entity. The
 * returned script entity is the one to use with ecs_script_entity_source() and
 * ecs_script_edits_new():
 *
 * @code
 * ecs_entity_t s = ecs_script_entity_owner(world, e);
 * const EcsScript *sc = ecs_get(world, s, EcsScript);
 * ecs_script_edits_t *edits = ecs_script_edits_new(sc->script);
 * @endcode
 *
 * For entities that were created by a regular entity statement this is the
 * script that created the entity. For entities that were created by a template
 * body this is the script that *declares* the template, which is not
 * necessarily the script that instantiates it.
 *
 * The operation returns 0 when the entity is not editable, which is the case
 * when the entity was not created by a script, when the script that created it
 * is not managed (see ecs_script_parse()), or when the statement that created
 * the entity cannot be attributed to the entity (see
 * ecs_script_entity_source()).
 *
 * @param world The world.
 * @param entity The entity to find.
 * @return The managed script entity, or 0 if the entity is not editable.
 */
FLECS_API
ecs_entity_t ecs_script_entity_owner(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Set of pending edits for a script.
 * See ecs_script_edits_new().
 */
typedef struct ecs_script_edits_t ecs_script_edits_t;

/** Create an edit set for a script.
 * An edit set collects changes that are keyed by entity id, and turns them into
 * new script source code with ecs_script_edits_apply(). Only the edited
 * statements change; all other text (comments, whitespace, layout, expressions,
 * include statements) is preserved byte for byte.
 *
 * The script must be evaluated before edits can be added, and must outlive the
 * edit set.
 *
 * An edit set must be deleted with ecs_script_edits_free().
 *
 * @param script The script to edit.
 * @return A new edit set, or NULL if the script is invalid.
 */
FLECS_API
ecs_script_edits_t* ecs_script_edits_new(
    ecs_script_t *script);

/** Free an edit set.
 *
 * @param edits The edit set.
 */
FLECS_API
void ecs_script_edits_free(
    ecs_script_edits_t *edits);

/** Set a component value on an entity.
 * If the entity scope already contains a statement for the component, only the
 * value expression of that statement is replaced. The style of the existing
 * initializer is preserved:
 *
 * - a named initializer ("{x: 10, y: 20}") stays named
 * - a positional initializer ("{10, 20}") stays positional
 * - an empty initializer ("{}") becomes positional
 * - any other value form (a plain expression, a collection initializer, a match
 *   expression) is replaced with the default serialized form
 *
 * All members of the component are written, in the order in which they are
 * defined by the type. Floating point members are written as the shortest
 * decimal string that parses back to the same value ("1.2345" for a float with
 * value 1.2345f, "1" for a float with value 1.0f), so values roundtrip without
 * accumulating digits.
 *
 * When the entity was created by a template body statement (see
 * ecs_script_entity_source()) the statement in the template body is edited,
 * which changes the value for every instance of the template. Any expression
 * that the body used for the value (such as a prop or const reference) is
 * replaced by the literal value that is passed to this operation.
 *
 * If the entity scope does not contain a statement for the component, a new
 * "Component: {...}" statement is appended to the end of the entity scope,
 * using the indentation of the other statements in the scope (or the
 * indentation of the entity statement plus four spaces when the scope is
 * empty). When the entity statement has no scope, a scope is added.
 *
 * If the entity scope contains more than one statement for the component, the
 * last statement is replaced. If the entity is declared by more than one
 * statement, the first declaration is edited.
 *
 * Only statements in the scope of the entity itself are considered. A value
 * that the entity inherits from an enclosing `with` statement is not modified;
 * setting such a component adds a statement to the entity scope that overrides
 * the `with` value.
 *
 * The component is written with the shortest name that resolves to the same
 * component given the `using` and `module` statements of the script.
 *
 * @param edits The edit set.
 * @param entity The entity to edit.
 * @param component The component (or pair) to set.
 * @param value Pointer to the component value.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_script_edits_set(
    ecs_script_edits_t *edits,
    ecs_entity_t entity,
    ecs_id_t component,
    const void *value);

/** Set a component value on an entity from an expression string.
 * Same as ecs_script_edits_set(), but instead of serializing a value, the
 * provided expression is written to the script verbatim. The expression is not
 * validated.
 *
 * @param edits The edit set.
 * @param entity The entity to edit.
 * @param component The component (or pair) to set.
 * @param expr The value expression (for example "{10, 20}").
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_script_edits_set_expr(
    ecs_script_edits_t *edits,
    ecs_entity_t entity,
    ecs_id_t component,
    const char *expr);

/** Remove a component from an entity.
 * This removes the statement that adds the component (or tag) to the entity,
 * including the line(s) the statement occupies. If the entity scope contains
 * more than one statement for the component, the last statement is removed.
 *
 * The operation returns zero when the entity does not have a statement for the
 * component, as the resulting source has the requested state.
 *
 * @param edits The edit set.
 * @param entity The entity to edit.
 * @param component The component (or pair) to remove.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_script_edits_remove(
    ecs_script_edits_t *edits,
    ecs_entity_t entity,
    ecs_id_t component);

/** Delete an entity.
 * This removes the entity statement, its scope, and the line(s) the statement
 * occupies. Comments that precede the statement are preserved. A trailing
 * comment on the last line of the statement is removed together with the line.
 *
 * When the removed lines are surrounded by blank lines (where the start of the
 * file, the opening brace of the enclosing scope, the closing brace of the
 * enclosing scope and the end of the file count as blank), one of the blank
 * lines is removed as well, so that the statements around the deleted statement
 * stay separated by exactly one blank line.
 *
 * Other edits that fall inside the span of a deleted entity (such as edits to
 * child entities) are absorbed by the deletion.
 *
 * When the entity was created by a template body statement (see
 * ecs_script_entity_source()) the statement in the template body is deleted,
 * which removes the entity from every instance of the template.
 *
 * @param edits The edit set.
 * @param entity The entity to delete.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_script_edits_delete(
    ecs_script_edits_t *edits,
    ecs_entity_t entity);

/** Apply an edit set.
 * This operation returns new script source code with the edits applied. The
 * operation does not modify the script or the edit set, which means that
 * applying the same edit set twice returns the same text.
 *
 * The operation returns NULL when two edits overlap in a way that cannot be
 * resolved.
 *
 * @param edits The edit set.
 * @return The new source code, must be freed with ecs_os_free(). NULL if
 *   failed.
 */
FLECS_API
char* ecs_script_edits_apply(
    const ecs_script_edits_t *edits);


/* Managed scripts (script associated with entity that outlives the function) */

/** Used with ecs_script_init(). */
typedef struct ecs_script_desc_t {
    ecs_entity_t entity;   /**< Set to customize entity handle associated with script. */
    const char *filename;  /**< Set to load script from file. */
    const char *code;      /**< Set to parse script from string. */
    bool lenient;          /**< Load script in lenient mode (see ecs_script_set_lenient()). */
    bool ir;               /**< Evaluate script with IR runtime. */
} ecs_script_desc_t;

/** Load managed script.
 * A managed script tracks which entities it creates, and keeps those entities
 * synchronized when the contents of the script are updated. When the script is
 * updated, entities that are no longer in the new version will be deleted.
 *
 * This feature is experimental.
 *
 * @param world The world.
 * @param desc Script descriptor.
 * @return The script entity.
 */
FLECS_API
ecs_entity_t ecs_script_init(
    ecs_world_t *world,
    const ecs_script_desc_t *desc);

#define ecs_script(world, ...)\
    ecs_script_init(world, &(ecs_script_desc_t) __VA_ARGS__)

/** Update script with new code.
 *
 * @param world The world.
 * @param script The script entity.
 * @param instance A template instance (optional).
 * @param code The script code.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_script_update(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    const char *code);

/** Clear all entities associated with script.
 *
 * @param world The world.
 * @param script The script entity.
 * @param instance The script instance.
 */
FLECS_API
void ecs_script_clear(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance);


/* Script variables */

/** Create new variable scope.
 * Create root variable scope. A variable scope contains one or more variables. 
 * Scopes can be nested, which allows variables in different scopes to have the 
 * same name. Variables from parent scopes will be shadowed by variables in 
 * child scopes with the same name.
 * 
 * Use the `ecs_script_vars_push()` and `ecs_script_vars_pop()` functions to
 * push and pop variable scopes.
 * 
 * When a variable contains allocated resources (e.g., a string), its resources
 * will be freed when `ecs_script_vars_pop()` is called on the scope, the
 * ecs_script_vars_t::type_info field is initialized for the variable, and 
 * `ecs_type_info_t::hooks::dtor` is set.
 * 
 * @param world The world.
 * @return The new root variable scope.
 */
FLECS_API
ecs_script_vars_t* ecs_script_vars_init(
    ecs_world_t *world);

/** Free variable scope.
 * Free root variable scope. The provided scope should not have a parent. This
 * operation calls `ecs_script_vars_pop()` on the scope.
 *
 * @param vars The variable scope.
 */
FLECS_API
void ecs_script_vars_fini(
    ecs_script_vars_t *vars);

/** Push new variable scope.
 * 
 * Scopes created with ecs_script_vars_push() must be cleaned up with
 * ecs_script_vars_pop().
 * 
 * If the stack and allocator arguments are left to NULL, their values will be
 * copied from the parent.
 *
 * @param parent The parent scope (provide NULL for root scope).
 * @return The new variable scope.
 */
FLECS_API
ecs_script_vars_t* ecs_script_vars_push(
    ecs_script_vars_t *parent);

/** Pop variable scope.
 * This frees up the resources for a variable scope. The scope must be at the
 * top of a vars stack. Calling ecs_script_vars_pop() on a scope that is not the
 * last scope causes undefined behavior.
 *
 * @param vars The scope to free.
 * @return The parent scope.
 */
FLECS_API
ecs_script_vars_t* ecs_script_vars_pop(
    ecs_script_vars_t *vars);

/** Declare a variable.
 * This operation declares a new variable in the current scope. If a variable
 * with the specified name already exists, the operation will fail.
 * 
 * This operation does not allocate storage for the variable. This is done to
 * allow for variables that point to existing storage, which prevents having
 * to copy existing values to a variable scope.
 * 
 * @param vars The variable scope.
 * @param name The variable name.
 * @return The new variable, or NULL if the operation failed.
 */
FLECS_API
ecs_script_var_t* ecs_script_vars_declare(
    ecs_script_vars_t *vars,
    const char *name);

/** Define a variable.
 * This operation calls `ecs_script_vars_declare()` and allocates storage for
 * the variable. If the type has a ctor, it will be called on the new storage.
 * 
 * The scope's stack allocator will be used to allocate the storage. After 
 * `ecs_script_vars_pop()` is called on the scope, the variable storage will no
 * longer be valid.
 * 
 * The operation will fail if the type argument is not a type.
 * 
 * @param vars The variable scope.
 * @param name The variable name.
 * @param type The variable type.
 * @return The new variable, or NULL if the operation failed.
 */
FLECS_API
ecs_script_var_t* ecs_script_vars_define_id(
    ecs_script_vars_t *vars,
    const char *name,
    ecs_entity_t type);

#define ecs_script_vars_define(vars, name, type)\
    ecs_script_vars_define_id(vars, name, ecs_id(type))

/** Lookup a variable.
 * This operation looks up a variable in the current scope. If the variable 
 * can't be found in the current scope, the operation will recursively search
 * the parent scopes.
 * 
 * @param vars The variable scope.
 * @param name The variable name.
 * @return The variable, or NULL if one with the provided name does not exist.
 */
FLECS_API
ecs_script_var_t* ecs_script_vars_lookup(
    const ecs_script_vars_t *vars,
    const char *name);

/** Lookup a variable by stack pointer.
 * This operation provides a faster way to lookup variables that are always 
 * declared in the same order in a ecs_script_vars_t scope.
 * 
 * The stack pointer of a variable can be obtained from the ecs_script_var_t 
 * type. The provided frame offset must be valid for the provided variable  
 * stack. If the frame offset is not valid, this operation will panic.
 * 
 * @param vars The variable scope.
 * @param sp The stack pointer to the variable.
 * @return The variable.
 */
FLECS_API
ecs_script_var_t* ecs_script_vars_from_sp(
    const ecs_script_vars_t *vars,
    int32_t sp);

/** Print variables.
 * This operation prints all variables in the vars scope and parent scopes.
 * 
 * @param vars The variable scope.
 */
FLECS_API
void ecs_script_vars_print(
    const ecs_script_vars_t *vars);

/** Preallocate space for variables.
 * This operation preallocates space for the specified number of variables. This
 * is a performance optimization only, and is not necessary before declaring
 * variables in a scope.
 * 
 * @param vars The variable scope.
 * @param count The number of variables to preallocate space for.
 */
FLECS_API
void ecs_script_vars_set_size(
    ecs_script_vars_t *vars,
    int32_t count);

/** Convert iterator to vars.
 * This operation converts an iterator to a variable array. This allows for
 * using iterator results in expressions. The operation only converts a
 * single result at a time, and does not progress the iterator.
 *
 * Iterator fields with data will be made available as variables with as name
 * the field index (e.g., "$1"). The operation does not check if reflection data
 * is registered for a field type. If no reflection data is registered for the
 * type, using the field variable in expressions will fail.
 *
 * Field variables will only contain single elements, even if the iterator
 * returns component arrays. The offset parameter can be used to specify which
 * element in the component arrays to return. The offset parameter must be
 * smaller than it->count.
 *
 * The operation will create a variable for query variables that contain a
 * single entity.
 *
 * The operation will attempt to use existing variables. If a variable does not
 * yet exist, the operation will create it. If an existing variable exists with
 * a mismatching type, the operation will fail.
 *
 * Accessing variables after progressing the iterator or after the iterator is
 * destroyed will result in undefined behavior.
 *
 * If vars contains a variable that is not present in the iterator, the variable
 * will not be modified.
 *
 * @param it The iterator to convert to variables.
 * @param vars The variables to write to.
 * @param offset The offset to the current element.
 */
FLECS_API
void ecs_script_vars_from_iter(
    const ecs_iter_t *it,
    ecs_script_vars_t *vars,
    int offset);


/* Standalone expression evaluation */

/** Used with ecs_expr_run(). */
typedef struct ecs_expr_eval_desc_t {
    const char *name;                /**< Script name. */
    const char *expr;                /**< Full expression string. */
    const ecs_script_vars_t *vars;   /**< Variables accessible in expression. */
    ecs_entity_t type;               /**< Type of parsed value (optional). */
    ecs_entity_t (*lookup_action)(   /**< Function for resolving entity identifiers. */
        const ecs_world_t*,
        const char *value,
        void *ctx);
    void *lookup_ctx;                /**< Context passed to lookup function. */

    /** Disable constant folding (slower evaluation, faster parsing). */
    bool disable_folding;

    /** This option instructs the expression runtime to lookup variables by 
     * stack pointer instead of by name, which improves performance. Only enable 
     * when provided variables are always declared in the same order. */
    bool disable_dynamic_variable_binding;

    /** Allow for unresolved identifiers when parsing. Useful when entities can
     * be created in between parsing and evaluating. */
    bool allow_unresolved_identifiers;

    ecs_script_runtime_t *runtime;   /**< Reusable runtime (optional). */

    void *script_visitor;            /**< For internal usage. */
} ecs_expr_eval_desc_t;

/** Run expression.
 * This operation runs an expression and stores the result in the provided 
 * value. If the value contains a type that is different from the type of the
 * expression, the expression will be cast to the value.
 *
 * If the provided value for value.ptr is NULL, the value must be freed with 
 * ecs_ptr_free() afterwards.
 *
 * @param world The world.
 * @param ptr The pointer to the expression to parse.
 * @param value The value containing type and pointer to write to.
 * @param desc Configuration parameters for the parser.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_expr_run(
    ecs_world_t *world,
    const char *ptr,
    ecs_value_t *value,
    const ecs_expr_eval_desc_t *desc);

/** Parse expression.
 * This operation parses an expression and returns an object that can be 
 * evaluated multiple times with ecs_expr_eval().
 * 
 * @param world The world.
 * @param expr The expression string.
 * @param desc Configuration parameters for the parser.
 * @return A script object if parsing is successful, NULL if parsing failed.
 */
FLECS_API
ecs_script_t* ecs_expr_parse(
    ecs_world_t *world,
    const char *expr,
    const ecs_expr_eval_desc_t *desc);

/** Evaluate expression.
 * This operation evaluates an expression parsed with ecs_expr_parse() 
 * and stores the result in the provided value. If the value contains a type 
 * that is different from the type of the expression, the expression will be 
 * cast to the value.
 * 
 * If the provided value for value.ptr is NULL, the value must be freed with 
 * ecs_ptr_free() afterwards.
 * 
 * @param script The script containing the expression.
 * @param value The value in which to store the expression result.
 * @param desc Configuration parameters for the parser.
 * @return Zero if successful, non-zero if failed.
 */
FLECS_API
int ecs_expr_eval(
    const ecs_script_t *script,
    ecs_value_t *value,
    const ecs_expr_eval_desc_t *desc);

/** Evaluate interpolated expressions in string.
 * This operation evaluates expressions in a string, and replaces them with
 * their evaluated result. Supported expression formats are:
 *  - $variable_name
 *  - {expression}
 *
 * The $, { and } characters can be escaped with a backslash (\).
 *
 * @param world The world.
 * @param str The string to evaluate.
 * @param vars The variables to use for evaluation.
 * @return String with interpolated expressions, or NULL if failed.
 */
FLECS_API
char* ecs_script_string_interpolate(
    ecs_world_t *world,
    const char *str,
    const ecs_script_vars_t *vars);


/* Global const variables */

/** Used with ecs_const_var_init(). */
typedef struct ecs_const_var_desc_t {
    /** Variable name. */
    const char *name;

    /** Variable parent (namespace). */
    ecs_entity_t parent;

    /** Variable type. */
    ecs_entity_t type;

    /** Pointer to value of variable. The value will be copied to an internal
     * storage and does not need to be kept alive. */
    void *value;
} ecs_const_var_desc_t;

/** Create a const variable that can be accessed by scripts.
 * 
 * @param world The world.
 * @param desc Const var parameters.
 * @return The const var, or 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_const_var_init(
    ecs_world_t *world,
    ecs_const_var_desc_t *desc);

#define ecs_const_var(world, ...)\
    ecs_const_var_init(world, &(ecs_const_var_desc_t)__VA_ARGS__)


/** Return the value for a const variable.
 * This returns the value for a const variable that is created either with
 * ecs_const_var_init(), or in a script with "export const v = ...".
 *
 * @param world The world.
 * @param var The const variable.
 * @return The value of the const variable.
 */
FLECS_API
ecs_value_t ecs_const_var_get(
    const ecs_world_t *world,
    ecs_entity_t var);

/** Return pointer to the value of a const variable.
 * This operation returns the value of a const variable, casted to the specified
 * type. If the type is equal to that of the const variable, no cast is 
 * performed. If the variable cannot be casted to the specified type, the 
 * operation will throw an error.
 * 
 * The returned value is owned by the caller. If the returned value contains
 * allocated memory, this needs to be freed by the caller.
 *
 * This operation is intended to be used by the ecs_const_var_get_t macro.
 *
 * @param world The world.
 * @param name The name of the const variable.
 * @param type The requested type.
 * @param size The size of the requested type.
 * @param out Storage for the value of the const variable.
 * @return Pointer to the value of the const variable.
 */
FLECS_API
void* ecs_const_var_get_w_type(
    const ecs_world_t *world,
    const char *name,
    ecs_entity_t type,
    ecs_size_t size,
    void *out);

/** Return pointer to the value of a const variable.
 * This operation returns the value of a const variable, casted to the specified
 * type. If the type is equal to that of the const variable, no cast is 
 * performed. If the variable cannot be casted to the specified type, the 
 * operation will throw an error.
 * 
 * The returned value is owned by the caller. If the returned value contains
 * allocated memory, this needs to be freed by the caller.
 *
 * When the operation fails, a zero initialized value is returned.
 *
 * @param world The world.
 * @param name The name of the const variable.
 * @param T The requested type.
 * @return The value of the const variable.
 */
#define ecs_const_var_get_t(world, name, T)\
    (*ECS_CAST(T*, ecs_const_var_get_w_type(\
        world, name, ecs_id(T), ECS_SIZEOF(T), &(T){0})))


/* Global mut variables */

/** Used with ecs_mut_var_init(). */
typedef struct ecs_mut_var_desc_t {
    /** Variable name. */
    const char *name;

    /** Variable parent (namespace). */
    ecs_entity_t parent;

    /** Variable type. */
    ecs_entity_t type;

    /** Pointer to value of variable. The value will be copied to an internal
     * storage and does not need to be kept alive. */
    void *value;
} ecs_mut_var_desc_t;

/** Create a mut variable that can be accessed by scripts.
 * Unlike a const variable the value of a mut variable is never folded into
 * expressions, which means scripts that use it are reevaluated when the value
 * changes.
 *
 * @param world The world.
 * @param desc Mut var parameters.
 * @return The mut var, or 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_mut_var_init(
    ecs_world_t *world,
    ecs_mut_var_desc_t *desc);

#define ecs_mut_var(world, ...)\
    ecs_mut_var_init(world, &(ecs_mut_var_desc_t)__VA_ARGS__)


/** Return the value for a mut variable.
 * This returns the value for a mut variable that is created either with
 * ecs_mut_var_init(), or in a script with "export mut v = ...".
 *
 * @param world The world.
 * @param var The mut variable.
 * @return The value of the mut variable.
 */
FLECS_API
ecs_value_t ecs_mut_var_get(
    const ecs_world_t *world,
    ecs_entity_t var);

/** Return pointer to the value of a mut variable.
 * This operation returns the value of a mut variable, casted to the specified
 * type. If the type is equal to that of the mut variable, no cast is
 * performed. If the variable cannot be casted to the specified type, the
 * operation will throw an error.
 *
 * The returned value is owned by the caller. If the returned value contains
 * allocated memory, this needs to be freed by the caller.
 *
 * This operation is intended to be used by the ecs_mut_var_get_t macro.
 *
 * @param world The world.
 * @param name The name of the mut variable.
 * @param type The requested type.
 * @param size The size of the requested type.
 * @param out Storage for the value of the mut variable.
 * @return Pointer to the value of the mut variable.
 */
FLECS_API
void* ecs_mut_var_get_w_type(
    const ecs_world_t *world,
    const char *name,
    ecs_entity_t type,
    ecs_size_t size,
    void *out);

/** Return pointer to the value of a mut variable.
 * This operation returns the value of a mut variable, casted to the specified
 * type. If the type is equal to that of the mut variable, no cast is
 * performed. If the variable cannot be casted to the specified type, the
 * operation will throw an error.
 *
 * The returned value is owned by the caller. If the returned value contains
 * allocated memory, this needs to be freed by the caller.
 *
 * When the operation fails, a zero initialized value is returned.
 *
 * @param world The world.
 * @param name The name of the mut variable.
 * @param T The requested type.
 * @return The value of the mut variable.
 */
#define ecs_mut_var_get_t(world, name, T)\
    (*ECS_CAST(T*, ecs_mut_var_get_w_type(\
        world, name, ecs_id(T), ECS_SIZEOF(T), &(T){0})))

/** Set the value of a mut variable.
 * This operation sets the value of a mut variable from a value of the
 * specified type. If the type is equal to that of the mut variable, no cast is
 * performed. If the value cannot be casted to the type of the mut variable,
 * the operation will throw an error.
 *
 * The provided value is copied into the storage of the mut variable and does
 * not need to be kept alive.
 *
 * On success, OnSet observers for the mut variable are notified, which causes
 * scripts that use the variable to be reevaluated.
 *
 * This operation is intended to be used by the ecs_mut_var_set_t macro.
 *
 * @param world The world.
 * @param name The name of the mut variable.
 * @param type The type of the provided value.
 * @param size The size of the provided type.
 * @param value Pointer to the value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_mut_var_set_w_type(
    ecs_world_t *world,
    const char *name,
    ecs_entity_t type,
    ecs_size_t size,
    const void *value);

/** Set the value of a mut variable.
 * This operation sets the value of a mut variable from a value of the
 * specified type. If the type is equal to that of the mut variable, no cast is
 * performed. If the value cannot be casted to the type of the mut variable,
 * the operation will throw an error.
 *
 * The provided value is copied into the storage of the mut variable and does
 * not need to be kept alive.
 *
 * On success, OnSet observers for the mut variable are notified, which causes
 * scripts that use the variable to be reevaluated.
 *
 * @param world The world.
 * @param name The name of the mut variable.
 * @param T The type of the provided value.
 * @return Zero if success, non-zero if failed.
 */
#define ecs_mut_var_set_t(world, name, T, ...)\
    ecs_mut_var_set_w_type(\
        world, name, ecs_id(T), ECS_SIZEOF(T), &(T)__VA_ARGS__)

/** Mark mut var as modified.
 * This will notify OnSet observers.
 *
 * @param world The world.
 * @param var The mut variable.
 */
FLECS_API
void ecs_mut_var_modified(
    ecs_world_t *world,
    ecs_entity_t var);

/* Functions */

/** Vector function callbacks for different element types. */
typedef struct ecs_vector_fn_callbacks_t {
    ecs_vector_function_callback_t i8;   /**< Callback for i8 element type. */
    ecs_vector_function_callback_t i32;  /**< Callback for i32 element type. */
} ecs_vector_fn_callbacks_t;

/** Used with ecs_function_init() and ecs_method_init(). */
typedef struct ecs_function_desc_t {
    /** Function name. */
    const char *name;
    
    /** Parent of function. For methods the parent is the type for which the 
     * method will be registered. */
    ecs_entity_t parent;

    /** Function parameters. */
    ecs_script_parameter_t params[FLECS_SCRIPT_FUNCTION_ARGS_MAX];

    /** Function return type. */
    ecs_entity_t return_type;

    /** Function implementation. */
    ecs_function_callback_t callback;

    /** Vector function implementations.
     * Set these callbacks if a function has one or more arguments of type
     * flecs.script.vector, and optionally a return type of flecs.script.vector.
     * 
     * The flecs.script.vector type allows a function to be called with types
     * that meet the following constraints:
     * - The same type is provided for all arguments of type flecs.script.vector
     * - The provided type has one or more members of the same type
     * - The member type must be a primitive type
     * - The vector_callbacks array has an implementation for the primitive type.
     * 
     * This allows for statements like:
     * @code
     * const a: Rgb = {100, 150, 250}
     * const b: Rgb = {10, 10, 10}
     * const r = lerp(a, b, 0.1)
     * @endcode
     * 
     * which would otherwise have to be written out as:
     * 
     * @code
     * const r: Rgb = {
     *   lerp(a.r, b.r, 0.1),
     *   lerp(a.g, b.g, 0.1),
     *   lerp(a.b, b.b, 0.1)
     * }
     * @endcode
     * 
     * To register vector functions, do:
     * 
     * @code
     * ecs_function(world, {
     *     .name = "lerp",
     *     .return_type = EcsScriptVectorType,
     *     .params = {
     *         { .name = "a", .type = EcsScriptVectorType },
     *         { .name = "b", .type = EcsScriptVectorType },
     *         { .name = "t", .type = ecs_id(ecs_f64_t) }
     *     },
     *     .vector_callbacks = {
     *       [EcsF32] = flecs_lerp32,
     *       [EcsF64] = flecs_lerp64
     *     }
     * });
     * @endcode
     * 
     */
    ecs_vector_function_callback_t vector_callbacks[FLECS_SCRIPT_VECTOR_FUNCTION_COUNT];

    /** Context passed to function implementation. */
    void *ctx;
} ecs_function_desc_t;

/** Create new function.
 * This operation creates a new function that can be called from a script.
 * 
 * @param world The world.
 * @param desc Function init parameters.
 * @return The function, or 0 if failed.
*/
FLECS_API
ecs_entity_t ecs_function_init(
    ecs_world_t *world,
    const ecs_function_desc_t *desc);

#define ecs_function(world, ...)\
    ecs_function_init(world, &(ecs_function_desc_t)__VA_ARGS__)

FLECS_API
int ecs_function_call(
    ecs_world_t *world,
    ecs_entity_t function,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result);

/** Create new method. 
 * This operation creates a new method that can be called from a script. A 
 * method is like a function, except that it can be called on every instance of
 * a type.
 * 
 * Methods automatically receive the instance on which the method is invoked as
 * first argument.
 * 
 * @param world The world.
 * @param desc Method init parameters.
 * @return The method, or 0 if failed.
*/
FLECS_API
ecs_entity_t ecs_method_init(
    ecs_world_t *world,
    const ecs_function_desc_t *desc);

#define ecs_method(world, ...)\
    ecs_method_init(world, &(ecs_function_desc_t)__VA_ARGS__)

FLECS_API
int ecs_method_call(
    ecs_world_t *world,
    ecs_entity_t method,
    const ecs_value_t *instance,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result);


/* Value serialization */

/** Serialize value into expression string.
 * This operation serializes a value of the provided type to a string. The
 * memory pointed to must be large enough to contain a value of the used type.
 *
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @return String with expression, or NULL if failed.
 */
FLECS_API
char* ecs_ptr_to_expr(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data);

/** Serialize value into expression buffer.
 * Same as ecs_ptr_to_expr(), but serializes to an ecs_strbuf_t instance.
 *
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param buf The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_ptr_to_expr_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    ecs_strbuf_t *buf);

/** Similar to ecs_ptr_to_expr(), but serializes values to string.
 * Whereas the output of ecs_ptr_to_expr() is a valid expression, the output of
 * ecs_ptr_to_str() is a string representation of the value. In most cases the
 * output of the two operations is the same, but there are some differences:
 * - Strings are not quoted
 *
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @return String with result, or NULL if failed.
 */
FLECS_API
char* ecs_ptr_to_str(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data);

/** Serialize value into string buffer.
 * Same as ecs_ptr_to_str(), but serializes to an ecs_strbuf_t instance.
 *
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param buf The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_ptr_to_str_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    ecs_strbuf_t *buf);

typedef struct ecs_expr_node_t ecs_expr_node_t; 

/** Script module import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsScript)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsScriptImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
