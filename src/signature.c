#include "private_api.h"

/* TODO: after new query parser is working & code is ported to new types for
 *       storing terms, this code needs a big cleanup */

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
#define TOK_PAREN_OPEN '('
#define TOK_PAREN_CLOSE ')'
#define TOK_AS_ENTITY '\\'

#define TOK_ANY "ANY"
#define TOK_OWNED "OWNED"
#define TOK_SHARED "SHARED"
#define TOK_SYSTEM "SYSTEM"
#define TOK_PARENT "PARENT"
#define TOK_CASCADE "CASCADE"

#define TOK_ROLE_CHILDOF "CHILDOF"
#define TOK_ROLE_INSTANCEOF "INSTANCEOF"
#define TOK_ROLE_TRAIT "TRAIT"
#define TOK_ROLE_PAIR "PAIR"
#define TOK_ROLE_AND "AND"
#define TOK_ROLE_OR "OR"
#define TOK_ROLE_XOR "XOR"
#define TOK_ROLE_NOT "NOT"
#define TOK_ROLE_SWITCH "SWITCH"
#define TOK_ROLE_CASE "CASE"
#define TOK_ROLE_DISABLED "DISABLED"

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
bool ecs_is_0(
    const char *id)
{
    return id[0] == '0' && !id[1];
}

static
bool ecs_is_var(
    const char *id)
{
    if (id[0] == '_') {
        return true;
    }

    if (isdigit(id[0])) {
        return false;
    }

    const char *ptr;
    char ch;
    for (ptr = id; (ch = *ptr); ptr ++) {
        if (!isupper(ch) && ch != '_' && !isdigit(ch)) {
            return false;
        }
    }

    return true;
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
void vec_add_id(
    ecs_vector_t **vec,
    ecs_id_t id)
{
    ecs_id_t *e = ecs_vector_add(vec, ecs_id_t);
    *e = id;

    /* Keep array sorted so that we can use it in type compare operations */
    ecs_vector_sort(*vec, ecs_id_t, entity_compare);
}


/* -- Private functions -- */

static
bool valid_token_start_char(
    char ch)
{
    if (ch && (isalpha(ch) || (ch == '.') || (ch == '_') || (ch == '*') ||
        (ch == '0') || (ch == TOK_AS_ENTITY) || isdigit(ch))) {
        return true;
    }

    return false;
}

static
bool valid_token_char(
    char ch)
{
    if (ch && (isalpha(ch) || isdigit(ch) || ch == '_' || ch == '.')) {
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
const char* parse_token(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr,
    char *token_out)
{
    ptr = skip_space(ptr);
    char *tptr = token_out, ch = ptr[0];

    if (!valid_token_start_char(ch)) {
        ecs_parser_error(name, sig, column, 
            "invalid start of identifier '%s'", ptr);
        return NULL;
    }

    tptr[0] = ch;
    tptr ++;
    ptr ++;

    for (; (ch = *ptr); ptr ++) {
        if (!valid_token_char(ch)) {
            break;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';

    return skip_space(ptr);
}

static
void parse_identifier(
    const char *token,
    ecs_term_id_t *out)
{
    const char *tptr = token;
    char ch = token[0];
    if (ch == TOK_AS_ENTITY) {
        tptr ++;
    }

    out->name = ecs_os_strdup(tptr);
    
    if (ch == TOK_AS_ENTITY) {
        out->var_kind = EcsVarIsEntity;
    } else if (ecs_is_var(tptr)) {
        out->var_kind = EcsVarIsVariable;
    }
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
    } else if (!ecs_os_strcmp(token, TOK_ROLE_TRAIT) || 
               !ecs_os_strcmp(token, TOK_ROLE_PAIR)) 
    {
        return ECS_PAIR;            
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
    } else if (!ecs_os_strcmp(token, TOK_ROLE_DISABLED)) {
        return ECS_DISABLED;        
    } else {
        ecs_parser_error(name, sig, column, "invalid role '%s'", token);
        return 0;
    }
}

static
ecs_from_kind_t parse_source(
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
ecs_oper_kind_t parse_operator(
    char ch)
{
    if (ch == TOK_OPTIONAL) {
        return EcsOptional;
    } else if (ch == TOK_NOT) {
        return EcsNot;
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
    ecs_inout_kind_t *inout_kind_out)
{
    char token[ECS_MAX_TOKEN_SIZE];

    ptr = parse_token(name, sig, column, ptr, token);
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

static
const char* parse_arguments(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr,
    char *token,
    ecs_term_t *elem)
{
    (void)column;

    int32_t arg = 0;

    do {
        if (valid_token_start_char(ptr[0])) {
            if (arg == 2) {
                ecs_parser_error(name, sig, (ptr - sig), 
                    "too many arguments in term");
                return NULL;
            }

            ptr = parse_token(name, sig, (ptr - sig), ptr, token);
            if (!ptr) {
                return NULL;
            }

            parse_identifier(token, &elem->args[arg]);

            if (ptr[0] == TOK_AND) {
                ptr = skip_space(ptr + 1);
            } else if (ptr[0] == TOK_PAREN_CLOSE) {
                ptr = skip_space(ptr + 1);
                break;
            } else {
                ecs_parser_error(name, sig, (ptr - sig), 
                    "expected ',' or ')'");
                return NULL;
            }

        } else {
            ecs_parser_error(name, sig, (ptr - sig), 
                "expected variable identifier");
            return NULL;
        }

        arg ++;

    } while (true);

    return ptr;
}

static
const char* parse_element(
    const char *name,
    const char *sig,
    ecs_term_t *elem_out)
{
    const char *ptr = sig;
    char token[ECS_MAX_TOKEN_SIZE] = {0};
    ecs_term_t elem = { 0 };

    ptr = skip_space(ptr);

    /* Inout specifiers always come first */
    if (ptr[0] == TOK_ANNOTATE_OPEN) {
        ptr = parse_annotation(name, sig, (ptr - sig), ptr + 1, &elem.inout);
        if (!ptr) {
            return NULL;
        }
        ptr = skip_space(ptr);
    }

    if (valid_operator_char(ptr[0])) {
        elem.oper = parse_operator(ptr[0]);
        ptr = skip_space(ptr + 1);
    }

    /* If next token is the start of an identifier, it could be either a type
     * role, source or component identifier */
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, sig, (ptr - sig), ptr, token);
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
            elem.pred.entity = ECS_PAIR;
            ptr += 3;
            goto parse_trait;
        }

        /* Is token a predicate? */
        if (ptr[0] == TOK_PAREN_OPEN) {
            goto parse_predicate;    
        }

        /* Next token must be a predicate */
        goto parse_predicate;

    /* If next token is the source token, this is an empty source */
    } else if (ptr[0] == TOK_SOURCE) {
        goto empty_source;

    /* If next token is a singleton, assign identifier to pred and subject */
    } else if (ptr[0] == TOK_SINGLETON) {
        ptr ++;
        if (valid_token_start_char(ptr[0])) {
            ptr = parse_token(name, sig, (ptr - sig), ptr, token);
            if (!ptr) {
                return NULL;
            }

            goto parse_singleton;
        
        /* Deprecated, but still supported: singleton entity */
        } else if (ptr[0] == TOK_SOURCE) {
            ecs_os_strcpy(token, "$");
            ptr ++;
            goto parse_source;

        } else {
            ecs_parser_error(name, sig, (ptr - sig), 
                "expected identifier after singleton operator");
            return NULL;
        }

    /* Pair with implicit subject */
    } else if (ptr[0] == TOK_PAREN_OPEN) {
        goto parse_pair;

    /* Nothing else expected here */
    } else {
        ecs_parser_error(name, sig, (ptr - sig), 
            "unexpected character '%c'", ptr[0]);
        return NULL;
    }

empty_source:
    elem.from_kind = EcsFromEmpty;
    ptr = skip_space(ptr + 1);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        goto parse_predicate;
    } else {
        ecs_parser_error(name, sig, (ptr - sig), 
            "expected identifier after source operator");
        return NULL;
    }

parse_source:
    elem.from_kind = parse_source(token);
    if (elem.from_kind == EcsFromEntity) {
        parse_identifier(token, &elem.args[0]);
    }

    ptr = skip_space(ptr);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, sig, (ptr - sig), ptr, token);
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
            elem.pred.entity = ECS_PAIR;
            ptr += 3;
            goto parse_trait;
        }        

        /* If not, it's a predicate */
        goto parse_predicate;
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
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* Is token a trait? */
        if (ptr[0] == TOK_TRAIT) {
            ptr ++;
            goto parse_trait;
        }

        /* If not, it's a predicate */
        goto parse_predicate;

    } else if (ptr[0] == TOK_PAREN_OPEN) {
        goto parse_pair;
    } else {
        ecs_parser_error(name, sig, (ptr - sig), 
            "expected identifier after role");
        return NULL;
    }

parse_predicate:
    parse_identifier(token, &elem.pred);

    ptr = skip_space(ptr);
    
    if (ptr[0] == TOK_PAREN_OPEN) {
        ptr ++;
        if (ptr[0] == TOK_PAREN_CLOSE) {
            elem.from_kind = EcsFromEmpty;
            ptr ++;
            ptr = skip_space(ptr);
        } else {
            ptr = parse_arguments(name, sig, (ptr - sig), ptr, token, &elem);
        }

        goto parse_done;

    } else if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        goto parse_name;
    }

    goto parse_done;

parse_pair:
    ptr = parse_token(name, sig, (ptr - sig), ptr + 1, token);
    if (!ptr) {
        return NULL;
    }

    if (ptr[0] == TOK_AND) {
        ptr ++;
        elem.args[0].name = ecs_os_strdup(".");
        goto parse_pair_predicate;
    } else {
        ecs_parser_error(name, sig, (ptr - sig), 
            "unexpected character '%c'", ptr[0]);
    }

parse_pair_predicate:
    parse_identifier(token, &elem.pred);
    ptr = skip_space(ptr);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        if (ptr[0] == TOK_PAREN_CLOSE) {
            ptr ++;
            goto parse_pair_object;
        } else {
            ecs_parser_error(name, sig, (ptr - sig), 
                "unexpected character '%c'", ptr[0]);
            return NULL;
        }
    } else if (ptr[0] == TOK_PAREN_CLOSE) {
        /* No object */
        goto parse_done;
    }

parse_pair_object:
    parse_identifier(token, &elem.args[1]);
    ptr = skip_space(ptr);
    goto parse_done; 

parse_trait:
    parse_identifier(token, &elem.pred);

    ptr = skip_space(ptr);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* Can only be an object */
        goto parse_trait_target;
    } else {
        ecs_parser_error(name, sig, (ptr - sig), 
            "expected identifier after trait");
        return NULL;
    }

parse_trait_target:
    parse_identifier(".", &elem.args[0]);
    parse_identifier(token, &elem.args[1]);

    ptr = skip_space(ptr);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* Can only be a name */
        goto parse_name;
    } else {
        /* If nothing else, parsing of this element is done */
        goto parse_done;
    }

parse_singleton:
    parse_identifier(token, &elem.pred);
    parse_identifier(token, &elem.args[0]);
    goto parse_done;

parse_name:
    elem.name = ecs_os_strdup(token);
    ptr = skip_space(ptr);

parse_done:
    if (ptr[0] != TOK_AND && ecs_os_strncmp(ptr, TOK_OR, 2) && ptr[0]) {
        ecs_parser_error(name, sig, (ptr - sig), 
            "expected end of expression or next element");
        return NULL;
    }

    if (!ecs_os_strcmp(elem.pred.name, "0")) {
        if (ptr[0]) {
            ecs_parser_error(name, sig, (ptr - sig), 
                "unexpected element after 0"); 
            return NULL;
        }

        elem.from_kind = EcsFromEmpty;
    }

    if (!elem.args[0].name && elem.from_kind != EcsFromEmpty) {
        elem.args[0].name = ecs_os_strdup(".");
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
    ecs_term_t elem;

    if (!sig) {
        return 0;
    }

    sig = skip_space(sig);

    if (!sig[0]) {
        return 0;
    }

    bool is_or = false;
    const char *ptr = sig;
    while ((ptr = parse_element(name, ptr, &elem))) {
        if (is_or) {
            ecs_assert(elem.oper == EcsAnd, ECS_INVALID_SIGNATURE, sig);
            elem.oper = EcsOr;
        }

        if (elem.oper == EcsOr && elem.from_kind == EcsFromEmpty) {
            ecs_parser_error(name, sig, (ptr - sig), 
                "invalid empty source in OR expression"); 
            return -1;
        }

        if (action(world, name, sig, ptr - sig, &elem, ctx)) {
            return -1;
        }

        is_or = false;
        if (!strncmp(ptr, TOK_OR, 2)) {
            is_or = true;
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
        return -1;
    }

    return 0;
}

static
int resolve_identifier(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    int64_t column,    
    ecs_term_id_t *identifier)
{
    if (!identifier->name) {
        return 0;
    }

    if (identifier->var_kind == EcsVarDefault) {
        if (ecs_is_var(identifier->name)) {
            identifier->var_kind = EcsVarIsVariable;
        }
    }

    if (identifier->var_kind != EcsVarIsVariable) {
        if (ecs_is_0(identifier->name)) {
            identifier->entity = 0;
        } else {
            ecs_entity_t e = ecs_lookup_fullpath(world, identifier->name);
            if (!e) {
                ecs_parser_error(name, expr, column, 
                    "unresolved identifier '%s'");
                return -1;
            }

            /* Use OR, as entity may have already been populated with role */
            identifier->entity = e;
        }
    }

    return 0;
}

void ecs_term_set_legacy(
    ecs_term_t *term)
{
    if (term->from_kind == EcsFromDefault) {
        if (term->args[0].entity == EcsThis) {
            term->from_kind = EcsFromOwned;
        } else if (!term->args[0].entity) {
            term->from_kind = EcsFromEmpty;
        } else {
            term->from_kind = EcsFromEntity;
            ecs_assert(term->args[0].entity != 0, ECS_INTERNAL_ERROR, NULL);
        }
    } else {
        if (term->from_kind == EcsFromDefault || 
            term->from_kind == EcsFromOwned || 
            term->from_kind == EcsFromAny || 
            term->from_kind == EcsFromShared) 
        {
            term->args[0].entity = EcsThis;
            term->args[0].name = ecs_os_strdup(".");
        }
    }

    if (term->args[1].name) {
        term->is.component = ecs_pair(
            term->pred.entity, term->args[1].entity);
    } else {
        term->is.component = term->pred.entity;
    }

    if (term->role == ECS_AND) {
        term->oper = EcsOperAll;
    } else if (term->role == ECS_OR) {
        term->oper = EcsOr;
    } else {
        term->is.component |= term->role;
    }
}

/** Parse callback that adds component to the components array for a system */
static
int ecs_parse_signature_action(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    int64_t column,
    ecs_term_t *term,
    void *data)
{
    (void)name;
    (void)expr;
    (void)column;
    return ecs_sig_add(world, data, term);
}

int ecs_term_resolve(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    int64_t column,
    ecs_term_t *term)
{
    if (resolve_identifier(world, name, expr, column, &term->pred)) {
        return -1;
    }
    if (resolve_identifier(world, name, expr, column, &term->args[0])) {
        return -1;
    }
    if (resolve_identifier(world, name, expr, column, &term->args[1])) {
        return -1;
    }

    return 0;
}

int ecs_sig_init(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_sig_t *sig)
{
    if (expr && ecs_os_strlen(expr) && strcmp(expr, "0")) {
        sig->expr = ecs_os_strdup(expr);
    } else {
        sig->expr = NULL;
    }

    sig->terms = NULL;
    sig->name = NULL;

    if (sig->expr) {
        return ecs_parse_expr(
            world, name, sig->expr, ecs_parse_signature_action, sig);
    } else {
        return 0;
    }
}

void ecs_sig_deinit(
    ecs_sig_t *sig)
{   
    ecs_vector_each(sig->terms, ecs_term_t, column, {
        if (column->oper == EcsOr) {
            ecs_vector_free(column->is.type);
        }
        
        ecs_os_free(column->pred.name);
        ecs_os_free(column->args[0].name);
        ecs_os_free(column->args[1].name);
        ecs_os_free(column->name);
    });

    ecs_vector_free(sig->terms);
    ecs_os_free(sig->expr);
}

int ecs_sig_add(
    ecs_world_t *world,
    ecs_sig_t *sig,
    ecs_term_t *term)
{
    /* Resolve identifiers in term */
    if (ecs_term_resolve(world, sig->name, sig->expr, 0, term)) {
        return -1;
    }

    /* Patch up legacy fields */
    ecs_term_set_legacy(term);

    ecs_term_t *elem;
    if (term->oper != EcsOr) {
        elem = ecs_vector_add(&sig->terms, ecs_term_t);
        *elem = *term;

    /* If term role is OR, component should be a type */
    } else if (term->role == ECS_OR) {
        const EcsType *type = ecs_get(world, term->is.component, EcsType);
        if (!type) {
            ecs_parser_error(sig->name, sig->expr, 0, 
                "entity used with OR must be a type");
            return -1;
        }

        elem = ecs_vector_add(&sig->terms, ecs_term_t);
        *elem = *term;
        elem->is.type = ecs_vector_copy(type->normalized, ecs_entity_t);

    } else {
        /* OR follows OR or AND, AND being the first elem of the expression */        
        elem = ecs_vector_last(sig->terms, ecs_term_t);
        ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);
        if (elem->oper != EcsAnd && elem->oper != EcsOr) {
            ecs_parser_error(sig->name, sig->expr, 0, 
                "unexpected || operator in expression");
            return -1;
        }

        if (elem->from_kind != term->from_kind) {
            ecs_parser_error(sig->name, sig->expr, 0, 
                "cannot mix subjects in OR expression");
            return -1;
        }

        if (term->inout == EcsInOutDefault) {
            term->inout = elem->inout;
        }

        if (elem->inout != term->inout) {
            ecs_parser_error(sig->name, sig->expr, 0, 
                "cannot mix in/out in OR expression");
            return -1;
        }

        if (elem->oper == EcsAnd) {
            ecs_entity_t last_component = elem->is.component;
            elem->is.type = NULL;
            vec_add_id(&elem->is.type, last_component);
            elem->oper = EcsOr;
        }

        vec_add_id(&elem->is.type, term->is.component);
    }

    return 0;
}

static
bool sig_id_set(
    ecs_term_id_t *id)
{
    return (id->name != NULL) || (id->entity != 0);
}

static
void sig_add_id(
    ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_term_id_t *id)
{
    if (id->name) {
        ecs_strbuf_appendstr(buf, id->name);
    } else if (id->entity) {
        char *path = ecs_get_fullpath(world, id->entity);
        ecs_strbuf_appendstr(buf, path);
        ecs_os_free(path);
    } else {
        ecs_strbuf_appendstr(buf, "0");
    }
}

char* ecs_sig_str(
    ecs_world_t *world,
    ecs_sig_t *sig)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    ecs_term_t *terms = ecs_vector_first(sig->terms, ecs_term_t);
    int32_t i, count = ecs_vector_count(sig->terms);

    for (i = 0; i < count; i ++) {
        if (i) {
            ecs_strbuf_appendstr(&buf, ", ");
        }

        ecs_term_t *term = &terms[i];
        if (term->inout == EcsIn) {
            ecs_strbuf_appendstr(&buf, "[in] ");
        } else if (term->inout == EcsInOut) {
            ecs_strbuf_appendstr(&buf, "[inout] ");
        } else if (term->inout == EcsInOut) {
            ecs_strbuf_appendstr(&buf, "[out] ");
        }

        if (term->role) {
            ecs_strbuf_appendstr(&buf, ecs_role_str(term->role));
            ecs_strbuf_appendstr(&buf, " ");
        }

        ecs_assert(term->oper == term->oper, ECS_INTERNAL_ERROR, NULL);

        if (term->oper != EcsOr) {
            if (term->oper == EcsNot) {
                ecs_strbuf_appendstr(&buf, "!");
            } else if (term->oper == EcsOptional) {
                ecs_strbuf_appendstr(&buf, "?");
            }

            if (term->args[0].entity == EcsThis && sig_id_set(&term->args[1])) {
                ecs_strbuf_appendstr(&buf, "(");
            }

            if (!sig_id_set(&term->args[1]) && 
                (term->pred.entity != term->args[0].entity)) 
            {
                sig_add_id(world, &buf, &term->pred);

                if (!sig_id_set(&term->args[0])) {
                    ecs_strbuf_appendstr(&buf, "()");
                } else if (term->args[0].entity != EcsThis) {
                    ecs_strbuf_appendstr(&buf, "(");
                    sig_add_id(world, &buf, &term->args[0]);
                }

                if (sig_id_set(&term->args[1])) {
                    ecs_strbuf_appendstr(&buf, ", ");
                    sig_add_id(world, &buf, &term->args[1]);
                    ecs_strbuf_appendstr(&buf, ")");
                }
            } else {
                ecs_strbuf_appendstr(&buf, "$");
                sig_add_id(world, &buf, &term->pred);
            }
        } else {
            int32_t j = 0, id_count = ecs_vector_count(term->is.type);
            ecs_entity_t *ids = ecs_vector_first(term->is.type, ecs_entity_t);
            char id_buf[256];
            for (j = 0; j < id_count; j ++) {
                ecs_entity_str(world, ids[j], id_buf, 256);
                ecs_strbuf_appendstr(&buf, id_buf);
                if (j < (id_count - 1)) {
                    ecs_strbuf_appendstr(&buf, " || ");
                }
            }
        }
    }

    return ecs_strbuf_get(&buf);
}


/* Check if system meets constraints of non-table columns */
bool ecs_sig_check_constraints(
    ecs_world_t *world,
    ecs_sig_t *sig)
{
    ecs_vector_each(sig->terms, ecs_term_t, elem, {
        ecs_from_kind_t from_kind = elem->from_kind;
        ecs_oper_kind_t oper = elem->oper;

        if (from_kind == EcsFromEntity) {
            ecs_type_t type = ecs_get_type(world, elem->args[0].entity);
            if (ecs_type_has_id(world, type, elem->is.component)) {
                if (oper == EcsNot) {
                    return false;
                }
            } else {
                if (oper != EcsNot) {
                    return false;
                }
            }
        }
    });

    return true;
}
