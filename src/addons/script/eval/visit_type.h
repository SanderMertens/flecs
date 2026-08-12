/**
 * @file addons/script/eval/visit_type.h
 * @brief Script visitor for looking up symbols. Must be ran after include 
 *        visitor and before eval visitor.
 */

#ifndef FLECS_SCRIPT_VISIT_TYPE_H
#define FLECS_SCRIPT_VISIT_TYPE_H

typedef struct ecs_script_type_scope_t {
    struct ecs_script_type_scope_t *parent;
    ecs_hashmap_t symbols;  /* Symbols table. name_index<string, index_into_symbols> */
    int32_t variable_count; /* Number of variables in scope */
} ecs_script_type_scope_t;

typedef enum ecs_script_symbol_kind_t {
    EcsScriptSymbolConst,           /* const variable. */
    EcsScriptSymbolProp,            /* prop variable. */
    EcsScriptSymbolMut,             /* mut variable. */
    EcsScriptSymbolArgument,        /* function argument. */
    EcsScriptSymbolGlobalConst,     /* global const variable (export const). */
    EcsScriptSymbolGlobalMut,       /* global mut variable (export mut). */
    EcsScriptSymbolEntity,          /* reference to entity defined in script. */
    EcsScriptSymbolUnresolved
} ecs_script_symbol_kind_t;

typedef struct ecs_script_symbol_t {
    char *name;                      /* Name of the symbol (owned, used as key in symbols table). */
    ecs_script_symbol_kind_t kind;   /* Kind of symbol. */
    ecs_script_type_scope_t *parent; /* Scope in which symbol is defined (internal only). */
    ecs_script_type_scope_t *scope;  /* Child scope (for entity scopes, internal only). */
    bool external;                   /* If true, symbol is not defined by script (not stored in symbols, synthesized by flecs_script_lookup_symbol). */

    union {
        ecs_entity_t external;       /* External reference to entity. Only set if external is true. */
        int32_t entity;              /* Slot in internal entity reference table. Only set if external is false. */
        int32_t variable;            /* Variable stack slot. Only set if external is false. */
    } is;
} ecs_script_symbol_t;

typedef struct ecs_script_type_visitor_t {
    ecs_script_visit_t base;
    ecs_world_t *world;
    struct ecs_script_eval_visitor_t *eval;
    struct ecs_script_type_scope_t *root;
    int32_t no_eval;
    int32_t no_typing;
    int32_t template_body;

    /* Current scope. Pushed/popped by the AST visitor for lexical scopes. A script may reopen the same scope multiple times:
     * a {
     *   b {}
     * }
     *
     * a {
     *   // can refer to a.b
     * }
     */
    ecs_script_type_scope_t *cur;

    ecs_entity_t module;    /* Script module. Created/populated by include visitor. */
    ecs_vec_t using;        /* Using statements for script. Only apply to external entities. vec<ecs_entity_t> */
    ecs_vec_t symbols;      /* Symbols for script. Only contains symbols defined by script. vec<ecs_script_symbol_t> */
    ecs_vec_t scopes;       /* Vector which stores scopes. Used so we can free all scopes after visiting is done. */

    int32_t entity_count;   /* Number of entities created in script. Used to compute is.entity. */
    int32_t variable_count; /* Number of currently declared variables. Used to populate is.variable. */

    /* Note: when entering template/function bodies, the entity_count/variable_count members are set to 0, and restored when
     * leaving the template/function bodes (ordinary variables from outside a template/function scope won't be hoisted by
     * templates, and instead will have to be declared as extern). */
} ecs_script_type_visitor_t;

/* Initialize type visitor. Creates the root scope, which stays current after
 * visiting so symbols can be resolved once visiting is done. */
void flecs_script_type_visit_init(
    ecs_script_type_visitor_t *v,
    ecs_world_t *world,
    ecs_script_impl_t *script);

/* Free resources of type visitor (scopes, symbols, using statements). */
void flecs_script_type_visit_fini(
    ecs_script_type_visitor_t *v);

/* Visitor for resolving symbols in script. Walks the AST and populates the
 * visitor with the state necessary to resolve the symbols defined by the
 * script. */
int flecs_script_visit_type(
    ecs_script_type_visitor_t *visitor,
    ecs_script_node_t *node);

int flecs_script_type_visit_new_expr(
    ecs_script_t *script,
    ecs_script_entity_t *entity,
    const ecs_expr_eval_desc_t *desc);

/* Looks up a symbol based on the current state of the visitor. If the string 
 * cannot be resolved to any of the symbols defined in the script, the function
 * will lookup the symbol in the world following these rules:
 * - lookup in flecs.meta
 * - lookup in using paths
 * - lookup from module (if set)
 * - lookup from world root scope (using ecs_lookup)
 * 
 * If the name cannot be resolved, the function will return a value with the kind
 * set to EcsScriptSymbolUnresolved.
 * 
 * Note that the lookup function does not take into account the current scope for
 * world lookups. This is because the type visitor assumes that any scope in the script
 * is owned by the script. This allows the resolver to statically resolve all
 * references before eval time.
 * 
 * Name expressions ("e_{i}") are not supported by this operation.
 */
ecs_script_symbol_t flecs_script_lookup_symbol(
    const ecs_script_type_visitor_t *v,
    const char *name);

#endif
