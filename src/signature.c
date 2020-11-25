#include "private_api.h"

#define ECS_ANNOTATION_LENGTH_MAX (16)

#define TOK_SOURCE ':'
#define TOK_AND ','
#define TOK_OR "||"
#define TOK_NOT '!'
#define TOK_OPTIONAL '?'
#define TOK_ROLE '|'
#define TOK_TRAIT '>'
#define TOK_FOR "FOR"
#define TOK_NAME_SEP '.'
#define TOK_ANNOTATE_OPEN '['
#define TOK_ANNOTATE_CLOSE ']'
#define TOK_WILDCARD '*'
#define TOK_SINGLETON '$'

#define TOK_ANY "ANY"
#define TOK_OWNED "OWNED"
#define TOK_SHARED "SHARED"
#define TOK_SYSTEM "SYSTEM"
#define TOK_PARENT "PARENT"
#define TOK_CASCADE "CASCADE"

#define TOK_ROLE_CHILDOF "CHILDOF"
#define TOK_ROLE_INSTANCEOF "INSTANCEOF"
#define TOK_ROLE_TRAIT "TRAIT"
#define TOK_ROLE_AND "AND"
#define TOK_ROLE_OR "OR"
#define TOK_ROLE_XOR "XOR"
#define TOK_ROLE_NOT "NOT"
#define TOK_ROLE_SWITCH "SWITCH"
#define TOK_ROLE_CASE "CASE"

#define TOK_IN "in"
#define TOK_OUT "out"
#define TOK_INOUT "inout"

#define ECS_MAX_TOKEN_SIZE (256)

typedef char ecs_token_t[ECS_MAX_TOKEN_SIZE];

/** Skip spaces when parsing signature */
static
const char *skip_space(
    const char *ptr)
{
    while (isspace(*ptr)) {
        ptr ++;
    }
    return ptr;
}

static
int entity_compare(
    const void *ptr1,
    const void *ptr2)
{
    ecs_entity_t e1 = *(ecs_entity_t*)ptr1;
    ecs_entity_t e2 = *(ecs_entity_t*)ptr2;
    return (e1 > e2) - (e1 < e2);
}

static
void vec_add_entity(
    ecs_vector_t **vec,
    ecs_entity_t entity)
{
    ecs_entity_t *e = ecs_vector_add(vec, ecs_entity_t);
    *e = entity;

    /* Keep array sorted so that we can use it in type compare operations */
    ecs_vector_sort(*vec, ecs_entity_t, entity_compare);
}


/* -- Private functions -- */

static
bool valid_identifier_char(
    char ch)
{
    if (ch && (isalpha(ch) || isdigit(ch) || ch == '_' || ch == '.' || 
        ch == TOK_SINGLETON || ch == TOK_WILDCARD)) 
    {
        return true;
    }

    return false;
}

static
bool valid_operator_char(
    char ch)
{
    if (ch == TOK_OPTIONAL || ch == TOK_NOT) {
        return true;
    }

    return false;
}

static
const char* parse_identifier(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr,
    char *token_out)
{
    ptr = skip_space(ptr);

    char *tptr = token_out, ch = ptr[0];

    if (!valid_identifier_char(ch)) {
        ecs_parser_error(name, sig, column, "invalid identifier", ptr);
        return NULL;
    }

    for (; (ch = *ptr); ptr ++) {
        if (!valid_identifier_char(ch)) {
            break;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';

    return skip_space(ptr);
}

static
ecs_entity_t parse_role(
    const char *name,
    const char *sig,
    int64_t column,
    const char *token)
{
    if        (!ecs_os_strcmp(token, TOK_ROLE_CHILDOF)) {
        return ECS_CHILDOF;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_INSTANCEOF)) {
        return ECS_INSTANCEOF;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_TRAIT)) {
        return ECS_TRAIT;            
    } else if (!ecs_os_strcmp(token, TOK_ROLE_AND)) {
        return ECS_AND;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_OR)) {
        return ECS_OR;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_XOR)) {
        return ECS_XOR;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_NOT)) {
        return ECS_NOT;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_SWITCH)) {
        return ECS_SWITCH;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_CASE)) {
        return ECS_CASE;
    } else if (!ecs_os_strcmp(token, TOK_OWNED)) {
        return ECS_OWNED;
    } else {
        ecs_parser_error(name, sig, column, "invalid role '%s'", token);
        return 0;
    }
}

static
ecs_sig_from_kind_t parse_source(
    const char *token)
{
    if        (!ecs_os_strcmp(token, TOK_PARENT)) {
        return EcsFromParent;
    } else if (!ecs_os_strcmp(token, TOK_SYSTEM)) {
        return EcsFromSystem;
    } else if (!ecs_os_strcmp(token, TOK_ANY)) {
        return EcsFromAny;
    } else if (!ecs_os_strcmp(token, TOK_OWNED)) {
        return EcsFromOwned;
    } else if (!ecs_os_strcmp(token, TOK_SHARED)) {
        return EcsFromShared;
    } else if (!ecs_os_strcmp(token, TOK_CASCADE)) {
        return EcsCascade;  
    } else {
        return EcsFromEntity;
    }
} 

static
ecs_sig_oper_kind_t parse_operator(
    char ch)
{
    if (ch == TOK_OPTIONAL) {
        return EcsOperOptional;
    } else if (ch == TOK_NOT) {
        return EcsOperNot;
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

static
const char* parse_annotation(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr, 
    ecs_sig_inout_kind_t *inout_kind_out)
{
    char token[ECS_MAX_TOKEN_SIZE];

    ptr = parse_identifier(name, sig, column, ptr, token);
    if (!ptr) {
        return NULL;
    }

    if (!strcmp(token, "in")) {
        *inout_kind_out = EcsIn;
    } else
    if (!strcmp(token, "out")) {
        *inout_kind_out = EcsOut;
    } else
    if (!strcmp(token, "inout")) {
        *inout_kind_out = EcsInOut;
    }

    ptr = skip_space(ptr);

    if (ptr[0] != TOK_ANNOTATE_CLOSE) {
        ecs_parser_error(name, sig, column, "expected ]");
        return NULL;
    }

    return ptr + 1;
}

typedef struct sig_element_t {
    ecs_entity_t role;
    ecs_sig_inout_kind_t inout_kind;
    ecs_sig_from_kind_t from_kind;
    ecs_sig_oper_kind_t oper_kind;
    char *trait;
    char *source;
    char *component;
    char *name;
} sig_element_t;

static
const char* parse_element(
    const char *name,
    const char *sig,
    sig_element_t *elem_out)
{
    bool explicit_inout = false;
    const char *ptr = sig;
    char token[ECS_MAX_TOKEN_SIZE] = {0};
    sig_element_t elem = {
        .inout_kind = EcsInOut,
        .from_kind = EcsFromOwned,
        .oper_kind = EcsOperAnd
    };

    ptr = skip_space(ptr);

    /* Inout specifiers always come first */
    if (ptr[0] == TOK_ANNOTATE_OPEN) {
        explicit_inout = true;
        ptr = parse_annotation(name, sig, (ptr - sig), ptr + 1, &elem.inout_kind);
        if (!ptr) {
            return NULL;
        }
        ptr = skip_space(ptr);
    }

    if (valid_operator_char(ptr[0])) {
        elem.oper_kind = parse_operator(ptr[0]);
        ptr = skip_space(ptr + 1);
    }

    /* If next token is the start of an identifier, it could be either a type
     * role, source or component identifier */
    if (valid_identifier_char(ptr[0])) {
        ptr = parse_identifier(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }
        
        /* Is token a source identifier? */
        if (ptr[0] == TOK_SOURCE) {
            ptr ++;
            goto parse_source;
        }

        /* Is token a type role? */
        if (ptr[0] == TOK_ROLE && ptr[1] != TOK_ROLE) {
            ptr ++;
            goto parse_role;
        }

        /* Is token a trait? (using shorthand notation) */
        if (!ecs_os_strncmp(ptr, TOK_FOR, 3)) {
            elem.role = ECS_TRAIT;
            ptr += 3;
            goto parse_trait;
        }

        /* If it is neither, the next token must be a component */
        goto parse_component;

    /* If next token is the source token, this is an empty source */
    } else if (ptr[0] == TOK_SOURCE) {
        goto empty_source;

    /* Nothing else expected here */
    } else {
        ecs_parser_error(name, sig, (ptr - sig), 
            "unexpected character '%c'", ptr[0]);
        return NULL;
    }

empty_source:
    elem.from_kind = EcsFromEmpty;
    ptr = skip_space(ptr + 1);
    if (valid_identifier_char(ptr[0])) {
        ptr = parse_identifier(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        goto parse_component;
    } else {
        ecs_parser_error(name, sig, (ptr - sig), 
            "expected identifier after source operator");
        return NULL;
    }

parse_source:
    elem.from_kind = parse_source(token);
    if (elem.from_kind == EcsFromEntity) {
        elem.source = ecs_os_strdup(token);
    }

    ptr = skip_space(ptr);
    if (valid_identifier_char(ptr[0])) {
        ptr = parse_identifier(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* Is the next token a role? */
        if (ptr[0] == TOK_ROLE && ptr[1] != TOK_ROLE) {
            ptr++;
            goto parse_role;
        }

        /* Is token a trait? (using shorthand notation) */
        if (!ecs_os_strncmp(ptr, TOK_FOR, 3)) {
            elem.role = ECS_TRAIT;
            ptr += 3;
            goto parse_trait;
        }        

        /* If not, it's a component */
        goto parse_component;
    } else {
        ecs_parser_error(name, sig, (ptr - sig), 
            "expected identifier after source");
        return NULL;
    }

parse_role:
    elem.role = parse_role(name, sig, (ptr - sig), token);
    if (!elem.role) {
        return NULL;
    }

    ptr = skip_space(ptr);

    /* If next token is the source token, this is an empty source */
    if (valid_identifier_char(ptr[0])) {
        ptr = parse_identifier(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* Is token a trait? */
        if (ptr[0] == TOK_TRAIT) {
            ptr ++;
            goto parse_trait;
        }

        /* If not, it's a component */
        goto parse_component;
    } else {
        ecs_parser_error(name, sig, (ptr - sig), 
            "expected identifier after role");
        return NULL;
    }

parse_trait:
    elem.trait = ecs_os_strdup(token);

    ptr = skip_space(ptr);
    if (valid_identifier_char(ptr[0])) {
        ptr = parse_identifier(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* Can only be a component */
        goto parse_component;
    } else {
        ecs_parser_error(name, sig, (ptr - sig), 
            "expected identifier after trait");
        return NULL;
    }

parse_component:
    elem.component = ecs_os_strdup(token);

    ptr = skip_space(ptr);
    if (valid_identifier_char(ptr[0])) {
        ptr = parse_identifier(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* Can only be a name */
        goto parse_name;
    } else {
        /* If nothing else, parsing of this element is done */
        goto parse_done;
    }

parse_name:
    elem.name = ecs_os_strdup(token);
    ptr = skip_space(ptr);

parse_done:
    if (ptr[0] != TOK_AND && ecs_os_strncmp(ptr, TOK_OR, 2) && ptr[0]) {
        ecs_parser_error(name, sig, (ptr - sig), 
            "expected end of expression or next element");
        return NULL;
    }

    if (!ecs_os_strcmp(elem.component, "0")) {
        if (ptr[0]) {
            ecs_parser_error(name, sig, (ptr - sig), 
                "unexpected element after 0"); 
            return NULL;
        }

        if (elem.from_kind != EcsFromOwned) {
            ecs_parser_error(name, sig, (ptr - sig), 
                "invalid source modifier for 0"); 
            return NULL;
        }
    }

    if (!explicit_inout) {
        if (elem.from_kind != EcsFromOwned) {
            elem.inout_kind = EcsIn;
        }
    }

    *elem_out = elem;

    return ptr;
}

int ecs_parse_expr(
    ecs_world_t *world,
    const char *name,
    const char *sig,
    ecs_parse_action_t action,
    void *ctx)
{
    sig_element_t elem;

    bool is_or = false;
    const char *ptr = sig;
    while ((ptr = parse_element(name, ptr, &elem))) {
        if (is_or) {
            ecs_assert(elem.oper_kind == EcsOperAnd, ECS_INVALID_SIGNATURE, sig);
            elem.oper_kind = EcsOperOr;
        }

        if (action(world, name, sig, ptr - sig, 
            elem.from_kind, elem.oper_kind, elem.inout_kind, elem.role,
            elem.component, elem.source, elem.trait, elem.name, ctx))
        {
            if (!name) {
                return -1;
            }

            ecs_abort(ECS_INVALID_SIGNATURE, sig);
        }

        ecs_os_free(elem.component);
        ecs_os_free(elem.source);
        ecs_os_free(elem.trait);
        ecs_os_free(elem.name);

        is_or = false;
        if (!strncmp(ptr, TOK_OR, 2)) {
            is_or = true;
            if (elem.from_kind == EcsFromEmpty) {
                ecs_parser_error(name, sig, ptr - sig, 
                    "invalid empty source in or expression");
                return -1;
            }

            if (elem.from_kind == EcsFromSystem) {
                ecs_parser_error(name, sig, ptr - sig, 
                    "invalid system source in or expression");
                return -1;
            }
        }

        if (ptr[0]) {
            ptr ++;
            if (is_or) {
                ptr ++;
            }
        }
        
        ptr = skip_space(ptr);

        if (!ptr[0]) {
            break;
        }
    }

    if (!ptr) {
        if (!name) {
            return -1;
        }
        
        ecs_abort(ECS_INVALID_SIGNATURE, sig);
    }

    return 0;
}

/** Parse callback that adds component to the components array for a system */
static
int ecs_parse_signature_action(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    int64_t column,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t role,
    const char *entity_id,
    const char *source_id,
    const char *trait_id,
    const char *arg_name,
    void *data)
{
    ecs_sig_t *sig = data;
    bool is_singleton = false;

    ecs_assert(sig != NULL, ECS_INTERNAL_ERROR, NULL);

    if (entity_id[0] == '$') {
        if (from_kind ==  EcsFromEntity) {
            ecs_parser_error(name, expr, column, 
                "singleton component '%s' cannot have a source", entity_id);
        }

        from_kind = EcsFromEntity;
        is_singleton = true;
        entity_id ++;
    }

    /* Lookup component handle by string identifier */
    ecs_entity_t source = 0, component = ecs_lookup_fullpath(world, entity_id);
    if (!component) {
        /* "0" is a valid expression used to indicate that a system matches no
         * components */
        if (!strcmp(entity_id, "0")) {
            /* No need to add 0 component to signature */
            return 0;
        } else {
            ecs_parser_error(name, expr, column, 
                "unresolved component identifier '%s'", entity_id);
        }
    }

    if (is_singleton) {
        source = component;
    }

    /* Lookup trait handle by string identifier */
    if (trait_id) {
        ecs_entity_t trait = ecs_lookup_fullpath(world, trait_id);
        if (!trait) {
            ecs_parser_error(name, expr, column, 
                "unresolved trait identifier '%s'", trait_id);
        } else {
            component = ecs_entity_t_comb(component, trait);
        }
    }

    component |= role;

    if (!source && from_kind == EcsFromEntity) {
        source = ecs_lookup_fullpath(world, source_id);
        if (!source) {
            ecs_parser_error(name, expr, column, 
                "unresolved source identifier '%s'", source_id);
        }
    }

    return ecs_sig_add(
        world, sig, from_kind, oper_kind, inout_kind, component, source, 
        arg_name);
}

void ecs_sig_init(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_sig_t *sig)
{
    if (expr && ecs_os_strlen(expr)) {
        sig->expr = ecs_os_strdup(expr);
    } else {
        sig->expr = ecs_os_strdup("0");
    }

    ecs_parse_expr(world, name, sig->expr, ecs_parse_signature_action, sig);
}

void ecs_sig_deinit(
    ecs_sig_t *sig)
{   
    ecs_vector_each(sig->columns, ecs_sig_column_t, column, {
        if (column->oper_kind == EcsOperOr) {
            ecs_vector_free(column->is.type);
        }
        ecs_os_free(column->name);
    });

    ecs_vector_free(sig->columns);
    ecs_os_free(sig->expr);
}

int ecs_sig_add(
    ecs_world_t *world,
    ecs_sig_t *sig,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t component,
    ecs_entity_t source,
    const char *arg_name)
{
    ecs_sig_column_t *elem;

    /* If component has AND role, all components of specified type must match */
    if (ECS_HAS_ROLE(component, AND)) {
        elem = ecs_vector_add(&sig->columns, ecs_sig_column_t);
        component &= ECS_ENTITY_MASK;
        const EcsType *type = ecs_get(world, component, EcsType);
        if (!type) {
            ecs_parser_error(sig->name, sig->expr, 0, 
                "AND flag can only be applied to types");
        }

        elem->is.component = component;
        elem->from_kind = from_kind;
        elem->oper_kind = EcsOperAll;
        elem->inout_kind = inout_kind;
        elem->source = source;

    } else 

    /* If component has OR role, add type as OR column */
    if (ECS_HAS_ROLE(component, OR)) {
        elem = ecs_vector_add(&sig->columns, ecs_sig_column_t);
        component &= ECS_ENTITY_MASK;
        const EcsType *type = ecs_get(world, component, EcsType);
        if (!type) {
            ecs_parser_error(sig->name, sig->expr, 0, 
                "OR flag can only be applied to types");
        }

        elem->is.type = ecs_vector_copy(type->normalized, ecs_entity_t);
        elem->from_kind = from_kind;
        elem->oper_kind = EcsOperOr;
        elem->inout_kind = inout_kind;
        elem->source = source;
    } else

    /* AND (default) and optional columns are stored the same way */
    if (oper_kind != EcsOperOr) {
        elem = ecs_vector_add(&sig->columns, ecs_sig_column_t);
        elem->from_kind = from_kind;
        elem->oper_kind = oper_kind;
        elem->inout_kind = inout_kind;
        elem->is.component = component;
        elem->source = source;

    /* OR columns store a type id instead of a single component */
    } else {
        ecs_assert(inout_kind != EcsOut, ECS_INVALID_SIGNATURE, NULL);
        elem = ecs_vector_last(sig->columns, ecs_sig_column_t);

        if (elem->from_kind != from_kind) {
            /* Cannot mix FromEntity and FromComponent in OR */
            ecs_parser_error(sig->name, sig->expr, 0, 
                "cannot mix source kinds in || expression");
            goto error;
        }

        if (elem->oper_kind != EcsOperAnd && elem->oper_kind != EcsOperOr) {
            ecs_parser_error(sig->name, sig->expr, 0, 
                "cannot mix operators in || expression");
            goto error;         
        }

        if (elem->oper_kind == EcsOperAnd) {
            ecs_entity_t prev = elem->is.component;
            elem->is.type = NULL;
            vec_add_entity(&elem->is.type, prev);
            vec_add_entity(&elem->is.type, component);
        } else {
            vec_add_entity(&elem->is.type, component);
        }      

        elem->from_kind = from_kind;
        elem->oper_kind = oper_kind;
    }

    if (arg_name) {
        elem->name = ecs_os_strdup(arg_name);
    } else {
        elem->name = NULL;
    }

    return 0;
error:
    return -1;
}

/* Check if system meets constraints of non-table columns */
bool ecs_sig_check_constraints(
    ecs_world_t *world,
    ecs_sig_t *sig)
{
    ecs_vector_each(sig->columns, ecs_sig_column_t, elem, {
        ecs_sig_from_kind_t from_kind = elem->from_kind;
        ecs_sig_oper_kind_t oper_kind = elem->oper_kind;

        if (from_kind == EcsFromEntity) {
            ecs_type_t type = ecs_get_type(world, elem->source);
            if (ecs_type_has_entity(world, type, elem->is.component)) {
                if (oper_kind == EcsOperNot) {
                    return false;
                }
            } else {
                if (oper_kind != EcsOperNot) {
                    return false;
                }
            }
        }
    });

    return true;
}
