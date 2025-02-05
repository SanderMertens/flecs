/**
 * @file addons/script.h
 * @brief Flecs script module.
 *
 * For script, see examples/script.
 */

#ifdef FLECS_SCRIPT

/**
 * @defgroup c_addons_script Flecs script
 * @ingroup c_addons
 * DSL for loading scenes, assets and configuration.
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

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScript);

FLECS_API
extern ECS_DECLARE(EcsScriptTemplate);

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScriptConstVar);

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScriptFunction);

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScriptMethod);

/* Script template. */
typedef struct ecs_script_template_t ecs_script_template_t;

/** Script variable. */
typedef struct ecs_script_var_t {
    const char *name;
    ecs_value_t value;
    const ecs_type_info_t *type_info;
    int32_t sp;
    bool is_const;
} ecs_script_var_t;

/** Script variable scope. */
typedef struct ecs_script_vars_t {
    struct ecs_script_vars_t *parent;
    int32_t sp;

    ecs_hashmap_t var_index;
    ecs_vec_t vars;

    const ecs_world_t *world;
    struct ecs_stack_t *stack;
    ecs_stack_cursor_t *cursor;
    ecs_allocator_t *allocator;
} ecs_script_vars_t;

/** Script object. */
typedef struct ecs_script_t {
    ecs_world_t *world;
    const char *name;
    const char *code;
} ecs_script_t;

/* Runtime for executing scripts */
typedef struct ecs_script_runtime_t ecs_script_runtime_t;

/** Script component. 
 * This component is added to the entities of managed scripts and templates.
 */
typedef struct EcsScript {
    ecs_script_t *script;
    ecs_script_template_t *template_; /* Only set for template scripts */
} EcsScript;

/** Script function context. */
typedef struct ecs_function_ctx_t {
    ecs_world_t *world;
    ecs_entity_t function;
    void *ctx;
} ecs_function_ctx_t;

/** Script function callback. */
typedef void(*ecs_function_callback_t)(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result);

/** Function argument type. */
typedef struct ecs_script_parameter_t {
    const char *name;
    ecs_entity_t type;
} ecs_script_parameter_t;

/** Const component.
 * This component describes a const variable that can be used from scripts.
 */
typedef struct EcsScriptConstVar {
    ecs_value_t value;
    const ecs_type_info_t *type_info;
} EcsScriptConstVar;

/** Function component.
 * This component describes a function that can be called from a script.
 */
typedef struct EcsScriptFunction {
    ecs_entity_t return_type;
    ecs_vec_t params; /* vec<ecs_script_parameter_t> */
    ecs_function_callback_t callback;
    void *ctx;
} EcsScriptFunction;

/** Method component. 
 * This component describes a method that can be called from a script. Methods
 * are functions that can be called on instances of a type. A method entity is
 * stored in the scope of the type it belongs to.
 */
typedef struct EcsScriptMethod {
    ecs_entity_t return_type;
    ecs_vec_t params; /* vec<ecs_script_parameter_t> */
    ecs_function_callback_t callback;
    void *ctx;
} EcsScriptMethod;

/* Parsing & running scripts */

/** Used with ecs_script_parse() and ecs_script_eval() */
typedef struct ecs_script_eval_desc_t {
    ecs_script_vars_t *vars;       /**< Variables used by script */
    ecs_script_runtime_t *runtime; /**< Reusable runtime (optional) */
} ecs_script_eval_desc_t;

/** Parse script.
 * This operation parses a script and returns a script object upon success. To
 * run the script, call ecs_script_eval().
 * 
 * If the script uses outside variables, an ecs_script_vars_t object must be
 * provided in the vars member of the desc object that defines all variables 
 * with the correct types.
 * 
 * @param world The world.
 * @param name Name of the script (typically a file/module name).
 * @param code The script code.
 * @param desc Parameters for script runtime.
 * @return Script object if success, NULL if failed.
*/
FLECS_API
ecs_script_t* ecs_script_parse(
    ecs_world_t *world,
    const char *name,
    const char *code,
    const ecs_script_eval_desc_t *desc);

/** Evaluate script.
 * This operation evaluates (runs) a parsed script.
 * 
 * If variables were provided to ecs_script_parse(), an application may pass
 * a different ecs_script_vars_t object to ecs_script_eval(), as long as the
 * object has all referenced variables and they are of the same type.
 * 
 * @param script The script.
 * @param desc Parameters for script runtime.
 * @return Zero if success, non-zero if failed.
*/
FLECS_API
int ecs_script_eval(
    const ecs_script_t *script,
    const ecs_script_eval_desc_t *desc);

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
 * @return Zero if success, non-zero otherwise.
 */
FLECS_API
int ecs_script_run(
    ecs_world_t *world,
    const char *name,
    const char *code);

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

/** Create runtime for script.
 * A script runtime is a container for any data created during script 
 * evaluation. By default calling ecs_script_run() or ecs_script_eval() will
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
 * @return The string if success, NULL if failed.
 */
FLECS_API
char* ecs_script_ast_to_str(
    ecs_script_t *script,
    bool colors);


/* Managed scripts (script associated with entity that outlives the function) */

/** Used with ecs_script_init() */
typedef struct ecs_script_desc_t {
    ecs_entity_t entity;   /* Set to customize entity handle associated with script */
    const char *filename;  /* Set to load script from file */
    const char *code;      /* Set to parse script from string */
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
 * @param instance An template instance (optional).
 * @param code The script code.
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
 * When a variable contains allocated resources (e.g. a string), its resources
 * will be freed when `ecs_script_vars_pop()` is called on the scope, the
 * ecs_script_vars_t::type_info field is initialized for the variable, and 
 * `ecs_type_info_t::hooks::dtor` is set.
 * 
 * @param world The world.
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
 * This operation prints all variables in the vars scope and parent scopes.asm
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

/** Convert iterator to vars
 * This operation converts an iterator to a variable array. This allows for
 * using iterator results in expressions. The operation only converts a
 * single result at a time, and does not progress the iterator.
 *
 * Iterator fields with data will be made available as variables with as name
 * the field index (e.g. "$1"). The operation does not check if reflection data
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
    const char *name;                /**< Script name */
    const char *expr;                /**< Full expression string */
    const ecs_script_vars_t *vars;   /**< Variables accessible in expression */
    ecs_entity_t type;               /**< Type of parsed value (optional) */
    ecs_entity_t (*lookup_action)(   /**< Function for resolving entity identifiers */
        const ecs_world_t*,
        const char *value,
        void *ctx);
    void *lookup_ctx;                /**< Context passed to lookup function */

    /** Disable constant folding (slower evaluation, faster parsing) */
    bool disable_folding;

    /** This option instructs the expression runtime to lookup variables by 
     * stack pointer instead of by name, which improves performance. Only enable 
     * when provided variables are always declared in the same order. */
    bool disable_dynamic_variable_binding;

    /** Allow for unresolved identifiers when parsing. Useful when entities can
     * be created in between parsing & evaluating. */
    bool allow_unresolved_identifiers;

    ecs_script_runtime_t *runtime;   /**< Reusable runtime (optional) */
} ecs_expr_eval_desc_t;

/** Run expression.
 * This operation runs an expression and stores the result in the provided 
 * value. If the value contains a type that is different from the type of the
 * expression, the expression will be cast to the value.
 *
 * If the provided value for value.ptr is NULL, the value must be freed with 
 * ecs_value_free() afterwards.
 *
 * @param world The world.
 * @param ptr The pointer to the expression to parse.
 * @param value The value containing type & pointer to write to.
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
 * ecs_value_free() afterwards.
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
 */
FLECS_API
char* ecs_script_string_interpolate(
    ecs_world_t *world,
    const char *str,
    const ecs_script_vars_t *vars);


/* Global const variables */

/** Used with ecs_const_var_init */
typedef struct ecs_const_var_desc_t {
    /* Variable name. */
    const char *name;

    /* Variable parent (namespace). */
    ecs_entity_t parent;

    /* Variable type. */
    ecs_entity_t type;

    /* Pointer to value of variable. The value will be copied to an internal
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

/* Functions */

/** Used with ecs_function_init and ecs_method_init */
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

/** Create new method. 
 * This operation creates a new method that can be called from a script. A 
 * method is like a function, except that it can be called on every instance of
 * a type.
 * 
 * Methods automatically receive the instance on which the method is invoked as
 * first argument.
 * 
 * @param world Method The world.
 * @param desc Method init parameters.
 * @return The function, or 0 if failed.
*/
FLECS_API
ecs_entity_t ecs_method_init(
    ecs_world_t *world,
    const ecs_function_desc_t *desc);

#define ecs_method(world, ...)\
    ecs_method_init(world, &(ecs_function_desc_t)__VA_ARGS__)


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

/** Similar as ecs_ptr_to_expr(), but serializes values to string.
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
