/**
 * @file addons/plecs.h
 * @brief Flecs script module.
 *
 * For script, see examples/plecs.
 */

#ifdef FLECS_SCRIPT

/**
 * @defgroup c_addons_plecs Flecs script
 * @ingroup c_addons
 * Data definition format for loading entity data.
 *
 * @{
 */

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_EXPR
#define FLECS_EXPR
#endif

#ifndef FLECS_SCRIPT_H
#define FLECS_SCRIPT_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScript);

typedef struct ecs_script_t ecs_script_t;
typedef struct ecs_script_assembly_t ecs_script_assembly_t;

/* Script component */
typedef struct EcsScript {
    ecs_script_t *script;
    ecs_script_assembly_t *assembly; /* Only set for assemblies */
} EcsScript;

/** Parse plecs string.
 * This parses a plecs string and instantiates the entities in the world.
 *
 * @param world The world.
 * @param name The script name (typically the file).
 * @param str The plecs string.
 * @return Zero if success, non-zero otherwise.
 */
FLECS_API
int ecs_script_run(
    ecs_world_t *world,
    const char *name,
    const char *str);

/** Parse plecs file.
 * This parses a plecs file and instantiates the entities in the world. This
 * operation is equivalent to loading the file contents and passing it to
 * ecs_script_run().
 *
 * @param world The world.
 * @param filename The plecs file name.
 * @return Zero if success, non-zero otherwise.
 */
FLECS_API
int ecs_script_run_file(
    ecs_world_t *world,
    const char *filename);

/** Used with ecs_script_init() */
typedef struct ecs_script_desc_t {
    ecs_entity_t entity;   /* Set to customize entity handle associated with script */
    const char *filename;  /* Set to load script from file */
    const char *str;       /* Set to parse script from string */
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
 * @param instance An assembly instance (optional).
 * @param str The script code.
 * @param vars Optional preset variables for script parameterization.
 */
FLECS_API
int ecs_script_update(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    const char *str,
    ecs_vars_t *vars);

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

FLECS_API
ecs_script_t* ecs_script_parse(
    ecs_world_t *world,
    const char *name,
    const char *code);

FLECS_API
ecs_script_t* ecs_script_parse_file(
    ecs_world_t *world,
    const char *filename);

FLECS_API
int ecs_script_to_buf(
    ecs_script_t *script,
    ecs_strbuf_t *buf);

FLECS_API
char* ecs_script_to_str(
    ecs_script_t *script);

FLECS_API
int ecs_script_eval(
    ecs_script_t *script);


typedef struct ecs_script_var_t {
    const char *name;
    ecs_value_t value;
} ecs_script_var_t;

typedef struct ecs_script_vars_t {
    struct ecs_script_vars_t *parent;
    ecs_hashmap_t var_index;
    ecs_vec_t vars;

    struct ecs_stack_t *stack;
    ecs_stack_cursor_t *cursor;
    ecs_allocator_t *allocator;
} ecs_script_vars_t;

ecs_script_vars_t* ecs_script_vars_push(
    ecs_script_vars_t *parent,
    struct ecs_stack_t *stack,
    ecs_allocator_t *allocator);

ecs_script_vars_t* ecs_script_vars_pop(
    ecs_script_vars_t *vars);

ecs_script_var_t* ecs_script_vars_declare(
    ecs_script_vars_t *vars,
    const char *name);

ecs_script_var_t* ecs_script_vars_lookup(
    const ecs_script_vars_t *vars,
    const char *name);


/** Used with ecs_parse_expr(). */
typedef struct ecs_parse_expr_desc_t {
    const char *name;
    const char *expr;
    ecs_entity_t (*lookup_action)(
        const ecs_world_t*,
        const char *value,
        void *ctx);
    void *lookup_ctx;
    ecs_script_vars_t *vars;
} ecs_parse_expr_desc_t;

/** Parse expression into value.
 * This operation parses a flecs expression into the provided pointer. The
 * memory pointed to must be large enough to contain a value of the used type.
 *
 * If no type and pointer are provided for the value argument, the operation
 * will discover the type from the expression and allocate storage for the
 * value. The allocated value must be freed with ecs_value_free().
 *
 * @param world The world.
 * @param ptr The pointer to the expression to parse.
 * @param value The value containing type & pointer to write to.
 * @param desc Configuration parameters for deserializer.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_parse_expr(
    ecs_world_t *world,
    const char *ptr,
    ecs_value_t *value,
    const ecs_parse_expr_desc_t *desc);






/* Module import */
FLECS_API
void FlecsScriptImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
