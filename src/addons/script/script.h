
#include "../../private_api.h"
#include <ctype.h>

typedef struct ecs_script_scope_t ecs_script_scope_t;
typedef struct ecs_script_entity_t ecs_script_entity_t;

struct ecs_script_t {
    const char *name;
    const char *code;
    ecs_allocator_t allocator;
    ecs_script_scope_t *root;
    char *token_buffer;
};

typedef struct ecs_script_parser_t {
    ecs_script_t *script;
    ecs_script_scope_t *scope;
    char *token_cur;
} ecs_script_parser_t;

/* Tokenizer */
typedef enum ecs_script_token_kind_t {
    EcsTokEnd = '\0',
    EcsTokUnknown,
    EcsTokIdentifier,
    EcsTokKeywordWith,
    EcsTokKeywordUsing,
    EcsTokKeywordAssembly,
    EcsTokKeywordProp,
    EcsTokKeywordConst,
    EcsTokColon = ':',
    EcsTokScopeOpen = '{',
    EcsTokScopeClose = '}',
    EcsTokParenOpen = '(',
    EcsTokParenClose = ')',
    EcsTokComma = ',',
    EcsTokVariable = '$',
    EcsTokAnnotation = '@',
    EcsTokAssign = '=',
    EcsTokNewline = '\n',
} ecs_script_token_kind_t;

typedef struct ecs_script_token_t {
    const char *value;
    ecs_script_token_kind_t kind;
} ecs_script_token_t;

typedef struct ecs_script_tokens_t {
    int32_t count;
    ecs_script_token_t tokens[256];
} ecs_script_tokens_t;

const char* flecs_script_expr(
    ecs_script_parser_t *parser,
    const char *ptr,
    ecs_script_token_t *out,
    char until);

const char* flecs_script_token_kind_str(
    ecs_script_token_kind_t kind);

const char* flecs_script_token(
    ecs_script_parser_t *parser,
    const char *ptr,
    ecs_script_token_t *out,
    bool is_lookahead);

/* AST */
typedef enum ecs_script_node_kind_t {
    EcsAstScope,
    EcsAstTag,
    EcsAstComponent,
    EcsAstDefaultComponent,
    EcsAstVariableComponent,
    EcsAstWith,
    EcsAstUsing,
    EcsAstAnnotation,
    EcsAstAssembly,
    EcsAstProp,
    EcsAstConst,
    EcsAstEntity,
    EcsAstPairScope,
} ecs_script_node_kind_t;

typedef struct ecs_script_node_t {
    ecs_script_node_kind_t kind;
} ecs_script_node_t;

struct ecs_script_scope_t {
    ecs_script_entity_t *parent;
    ecs_vec_t stmts;
};

typedef struct ecs_script_id_t {
    const char *first;
    const char *second;
} ecs_script_id_t;

typedef struct ecs_script_tag_t {
    ecs_script_node_t node;
    ecs_script_id_t id;
} ecs_script_tag_t;

typedef struct ecs_script_component_t {
    ecs_script_node_t node;
    ecs_script_id_t id;
    const char *expr;
} ecs_script_component_t;

typedef struct ecs_script_default_component_t {
    ecs_script_node_t node;
    const char *expr;
} ecs_script_default_component_t;

typedef struct ecs_script_var_component_t {
    ecs_script_node_t node;
    const char *var_name;
} ecs_script_var_component_t;

struct ecs_script_entity_t {
    ecs_script_node_t node;
    const char *name;
    bool name_is_var;
    ecs_script_scope_t *parent;
    ecs_script_scope_t *scope;
};

typedef struct ecs_script_with_t {
    ecs_script_node_t node;
    ecs_script_scope_t *parent;
    ecs_script_scope_t *expressions;
    ecs_script_scope_t *scope;
} ecs_script_with_t;

typedef struct ecs_script_pair_scope_t {
    ecs_script_node_t node;
    ecs_script_scope_t *parent;
    ecs_script_id_t id;
    ecs_script_scope_t *scope;
} ecs_script_pair_scope_t;

typedef struct ecs_script_using_t {
    ecs_script_node_t node;
    const char *name;
} ecs_script_using_t;

typedef struct ecs_script_annot_t {
    ecs_script_node_t node;
    const char *name;
    const char *expr;
} ecs_script_annot_t;

typedef struct ecs_script_assembly_t {
    ecs_script_node_t node;
    const char *name;
    ecs_script_scope_t* scope;
} ecs_script_assembly_t;

typedef struct ecs_script_prop_t {
    ecs_script_node_t node;
    const char *name;
    const char *type;
    const char *expr;
} ecs_script_prop_t;

typedef struct ecs_script_const_t {
    ecs_script_node_t node;
    const char *name;
    const char *type;
    const char *expr;
} ecs_script_const_t;

ecs_script_t* flecs_script_new(void);

ecs_script_entity_t* flecs_script_insert_entity(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_pair_scope_t* flecs_script_insert_pair_scope(
    ecs_script_parser_t *parser,
    const char *first,
    const char *second);

ecs_script_with_t* flecs_script_insert_with(
    ecs_script_parser_t *parser);

ecs_script_using_t* flecs_script_insert_using(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_assembly_t* flecs_script_insert_assembly(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_annot_t* flecs_script_insert_annot(
    ecs_script_parser_t *parser,
    const char *name,
    const char *expr);

ecs_script_prop_t* flecs_script_insert_prop(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_const_t* flecs_script_insert_const(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_tag_t* flecs_script_insert_tag(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_tag_t* flecs_script_insert_pair_tag(
    ecs_script_parser_t *parser,
    const char *first,
    const char *second);

ecs_script_component_t* flecs_script_insert_component(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_component_t* flecs_script_insert_pair_component(
    ecs_script_parser_t *parser,
    const char *first,
    const char *second);

ecs_script_default_component_t* flecs_script_insert_default_component(
    ecs_script_parser_t *parser);

ecs_script_var_component_t* flecs_script_insert_var_component(
    ecs_script_parser_t *parser,
    const char *name);
