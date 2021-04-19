#include "private_api.h"

/* TODO: after new query parser is working & code is ported to new types for
 *       storing terms, this code needs a big cleanup */

#define ECS_ANNOTATION_LENGTH_MAX (16)

#define TOK_COLON ':'
#define TOK_AND ','
#define TOK_OR "||"
#define TOK_NOT '!'
#define TOK_OPTIONAL '?'
#define TOK_BITWISE_OR '|'
#define TOK_TRAIT '>'
#define TOK_FOR "FOR"
#define TOK_NAME_SEP '.'
#define TOK_BRACKET_OPEN '['
#define TOK_BRACKET_CLOSE ']'
#define TOK_WILDCARD '*'
#define TOK_SINGLETON '$'
#define TOK_PAREN_OPEN '('
#define TOK_PAREN_CLOSE ')'
#define TOK_AS_ENTITY '\\'

#define TOK_SELF "self"
#define TOK_SUPERSET "superset"
#define TOK_SUBSET "subset"
#define TOK_ALL "all"

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

/* -- Private functions -- */

static
bool valid_token_start_char(
    char ch)
{
    if (ch && (isalpha(ch) || (ch == '.') || (ch == '_') || (ch == '*') ||
        (ch == '0') || (ch == TOK_AS_ENTITY) || isdigit(ch))) 
    {
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
const char* parse_digit(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr,
    char *token_out)
{
    ptr = skip_space(ptr);
    char *tptr = token_out, ch = ptr[0];

    if (!isdigit(ch)) {
        ecs_parser_error(name, sig, column, 
            "invalid start of number '%s'", ptr);
        return NULL;
    }

    tptr[0] = ch;
    tptr ++;
    ptr ++;

    for (; (ch = *ptr); ptr ++) {
        if (!isdigit(ch)) {
            break;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';

    return skip_space(ptr);
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
int parse_identifier(
    const char *token,
    ecs_term_id_t *out)
{
    char ch = token[0];

    const char *tptr = token;
    if (ch == TOK_AS_ENTITY) {
        tptr ++;
    }

    out->name = ecs_os_strdup(tptr);

    if (ch == TOK_AS_ENTITY) {
        out->var_kind = EcsVarIsEntity;
    } else if (ecs_is_var(tptr)) {
        out->var_kind = EcsVarIsVariable;
    }

    return 0;
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

    if (ptr[0] != TOK_BRACKET_CLOSE) {
        ecs_parser_error(name, sig, column, "expected ]");
        return NULL;
    }

    return ptr + 1;
}

static
uint8_t parse_set_token(
    const char *token)
{
    if (!ecs_os_strcmp(token, TOK_SELF)) {
        return EcsSelf;
    } else if (!ecs_os_strcmp(token, TOK_SUPERSET)) {
        return EcsSuperSet;
    } else if (!ecs_os_strcmp(token, TOK_SUBSET)) {
        return EcsSubSet;
    } else if (!ecs_os_strcmp(token, TOK_ALL)) {
        return EcsAll;
    } else {
        return 0;
    }
}

static
const char* parse_set_expr(
    ecs_world_t *world,
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr,
    char *token,
    ecs_term_id_t *id)
{
    do {
        uint8_t tok = parse_set_token(token);
        if (!tok) {
            ecs_parser_error(name, sig, column, 
                "invalid set token '%s'", token);
            return NULL;
        }

        if (id->set & tok) {
            ecs_parser_error(name, sig, column, 
                "duplicate set token '%s'", token);
            return NULL;            
        }

        if ((tok == EcsSubSet && id->set & EcsSuperSet) ||
            (tok == EcsSuperSet && id->set & EcsSubSet))
        {
            ecs_parser_error(name, sig, column, 
                "cannot mix superset and subset", token);
            return NULL;            
        }    

        id->set |= tok;

        if (ptr[0] == TOK_PAREN_OPEN) {
            ptr ++;

            /* Relationship (overrides IsA default) */
            if (!isdigit(ptr[0]) && valid_token_start_char(ptr[0])) {
                ptr = parse_token(name, sig, (ptr - sig), ptr, token);
                if (!ptr) {
                    return NULL;
                }         

                ecs_entity_t rel = ecs_lookup_fullpath(world, token);
                if (!rel) {
                    ecs_parser_error(name, sig, column, 
                        "unresolved identifier '%s'", token);
                    return NULL;
                }

                id->relation = rel;

                if (ptr[0] == TOK_AND) {
                    ptr = skip_space(ptr + 1);
                } else if (ptr[0] != TOK_PAREN_CLOSE) {
                    ecs_parser_error(name, sig, column, 
                        "expected ',' or ')'");
                    return NULL;
                }
            }

            /* Max depth of search */
            if (isdigit(ptr[0])) {
                ptr = parse_digit(name, sig, (ptr - sig), ptr, token);
                if (!ptr) {
                    return NULL;
                }

                id->max_depth = atoi(token);
                if (id->max_depth < 0) {
                    ecs_parser_error(name, sig, column, 
                        "invalid negative depth");
                    return NULL;  
                }

                if (ptr[0] == ',') {
                    ptr = skip_space(ptr + 1);
                }
            }

            /* If another digit is found, previous depth was min depth */
            if (isdigit(ptr[0])) {
                ptr = parse_digit(name, sig, (ptr - sig), ptr, token);
                if (!ptr) {
                    return NULL;
                }

                id->min_depth = id->max_depth;
                id->max_depth = atoi(token);
                if (id->max_depth < 0) {
                    ecs_parser_error(name, sig, column, 
                        "invalid negative depth");
                    return NULL;  
                }
            }

            if (ptr[0] != TOK_PAREN_CLOSE) {
                ecs_parser_error(name, sig, column, "expected ')'");
                return NULL;                
            } else {
                ptr = skip_space(ptr + 1);
                if (ptr[0] != TOK_PAREN_CLOSE && ptr[0] != TOK_AND) { 
                    ecs_parser_error(name, sig, column, 
                        "expected end of set expr");
                    return NULL;
                }
            }
        }

        /* Next token in set expression */
        if (ptr[0] == TOK_BITWISE_OR) {
            ptr ++;
            if (valid_token_start_char(ptr[0])) {
                ptr = parse_token(name, sig, (ptr - sig), ptr, token);
                if (!ptr) {
                    return NULL;
                }
            }

        /* End of set expression */
        } else if (ptr[0] == TOK_PAREN_CLOSE || ptr[0] == TOK_AND) {
            break;
        }
    } while (true);

    if (id->set & EcsAll && !(id->set & EcsSuperSet) && !(id->set & EcsSubSet)){
        ecs_parser_error(name, sig, column, 
            "invalid 'all' token without superset or subset");
        return NULL;
    }

    if (id->set & EcsSelf && id->min_depth != 0) {
        ecs_parser_error(name, sig, column, 
            "min_depth must be zero for set expression with 'self'");
        return NULL;        
    }

    return ptr;
}

static
const char* parse_arguments(
    ecs_world_t *world,
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

            /* If token is a self, superset or subset token, this is a set
             * expression */
            if (!ecs_os_strcmp(token, TOK_ALL) ||
                !ecs_os_strcmp(token, TOK_SELF) || 
                !ecs_os_strcmp(token, TOK_SUPERSET) || 
                !ecs_os_strcmp(token, TOK_SUBSET))
            {
                ptr = parse_set_expr(world, name, sig, ptr - sig, ptr, 
                    token, &elem->args[arg]);

            /* Regular identifier */
            } else if (parse_identifier(token, &elem->args[arg])) {
                ecs_parser_error(name, sig, (ptr - sig), 
                    "invalid identifier '%s'", token);
                return NULL;
            }

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
                "expected identifier or set expression");
            return NULL;
        }

        arg ++;

    } while (true);

    return ptr;
}

static
const char* parse_element(
    ecs_world_t *world,
    const char *name,
    const char *sig,
    ecs_term_t *elem_out)
{
    const char *ptr = sig;
    char token[ECS_MAX_TOKEN_SIZE] = {0};
    ecs_term_t elem = { 0 };

    ptr = skip_space(ptr);

    /* Inout specifiers always come first */
    if (ptr[0] == TOK_BRACKET_OPEN) {
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
        if (ptr[0] == TOK_COLON) {
            ptr ++;
            goto parse_source;
        }

        /* Is token a type role? */
        if (ptr[0] == TOK_BITWISE_OR && ptr[1] != TOK_BITWISE_OR) {
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
    } else if (ptr[0] == TOK_COLON) {
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
        } else if (ptr[0] == TOK_COLON) {
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
    switch(elem.from_kind) {
    case EcsFromAny:
        elem.args[0].set = EcsSelf | EcsSuperSet;
        break;
    case EcsFromOwned:
        elem.args[0].set = EcsSelf;
        break;
    case EcsFromShared:
        elem.args[0].set = EcsSuperSet;
        break;
    case EcsFromParent:
        elem.args[0].set = EcsSuperSet;
        elem.args[0].relation = EcsChildOf;
        elem.args[0].max_depth = 1;
        break;
    case EcsCascade:
        elem.args[0].set = EcsSuperSet | EcsAll;
        elem.args[0].relation = EcsChildOf;
        break;
    case EcsFromEntity:
        if (parse_identifier(token, &elem.args[0])) {
            ecs_parser_error(name, sig, (ptr - sig), 
                "invalid identifier '%s'", token); 
            return NULL;           
        }
        break;
    case EcsFromSystem:
        elem.args[0].name = ecs_os_strdup(name);
        break;  
    default:
        break;
    }

    ptr = skip_space(ptr);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, sig, (ptr - sig), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* Is the next token a role? */
        if (ptr[0] == TOK_BITWISE_OR && ptr[1] != TOK_BITWISE_OR) {
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
    if (parse_identifier(token, &elem.pred)) {
        ecs_parser_error(name, sig, (ptr - sig), 
            "invalid identifier '%s'", token); 
        return NULL;        
    }

    ptr = skip_space(ptr);
    
    if (ptr[0] == TOK_PAREN_OPEN) {
        ptr ++;
        if (ptr[0] == TOK_PAREN_CLOSE) {
            elem.from_kind = EcsFromEmpty;
            ptr ++;
            ptr = skip_space(ptr);
        } else {
            ptr = parse_arguments(
                world, name, sig, (ptr - sig), ptr, token, &elem);
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
    if (parse_identifier(token, &elem.pred)) {
        ecs_parser_error(name, sig, (ptr - sig), 
            "invalid identifier '%s'", token); 
        return NULL;            
    }

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
    if (parse_identifier(token, &elem.args[1])) {
        ecs_parser_error(name, sig, (ptr - sig), 
            "invalid identifier '%s'", token); 
        return NULL;
    }

    ptr = skip_space(ptr);
    goto parse_done; 

parse_trait:
    if (parse_identifier(token, &elem.pred)) {
        ecs_parser_error(name, sig, (ptr - sig), 
            "invalid identifier '%s'", token); 
        return NULL;        
    }

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
    if (parse_identifier(token, &elem.args[1])) {
        ecs_parser_error(name, sig, (ptr - sig), 
            "invalid identifier '%s'", token); 
        return NULL;        
    }

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
    if (parse_identifier(token, &elem.pred)) {
        ecs_parser_error(name, sig, (ptr - sig), 
            "invalid identifier '%s'", token); 
        return NULL;        
    }

    parse_identifier(token, &elem.args[0]);
    goto parse_done;

parse_name:
    elem.name = ecs_os_strdup(token);
    ptr = skip_space(ptr);

parse_done:
    if (!ecs_os_strncmp(ptr, TOK_OR, 2)) {
        if (elem.oper != EcsAnd) {
            ecs_parser_error(name, sig, (ptr - sig), 
                "cannot combine || with other operators");
            return NULL;
        }

        elem.oper = EcsOr;
    }

    if (ptr[0] != TOK_AND && elem.oper != EcsOr && ptr[0]) {
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
        ecs_assert(elem.args[0].name == NULL, ECS_INTERNAL_ERROR, NULL);
        elem.args[0].name = ecs_os_strdup(".");
        elem.args[0].entity = 0;
    }

    *elem_out = elem;

    return ptr;
}

int ecs_parse_expr(
    ecs_world_t *world,
    ecs_sig_t *sig,
    ecs_parse_action_t action,
    void *ctx)
{
    ecs_term_t elem;
    ecs_assert(sig != NULL, ECS_INVALID_PARAMETER, NULL);
    const char *expr = sig->expr;

    /* Don't parse empty expressions */
    if (!expr) {
        return 0;
    }

    expr = skip_space(expr);
    if (!expr[0]) {
        return 0;
    }

    bool prev_or = false;
    ecs_from_kind_t prev_from = 0;

    const char *ptr = expr;
    while ((ptr = parse_element(world, sig->name, ptr, &elem))) {
        if (elem.oper == EcsOr && elem.from_kind == EcsFromEmpty) {
            ecs_parser_error(sig->name, expr, (ptr - expr), 
                "invalid empty source in OR expression"); 
            ecs_term_free(&elem);    
            return -1;
        }

        if (prev_or) {
            if (elem.from_kind != prev_from) {
                ecs_parser_error(sig->name, expr, (ptr - expr), 
                    "cannot combine different sources in OR expression");
                ecs_term_free(&elem);
                return -1;
            }

            prev_or = elem.oper == EcsOr;
            prev_from = elem.from_kind;

            if (elem.oper != EcsAnd && elem.oper != EcsOr) {
                ecs_parser_error(sig->name, expr, (ptr - expr), 
                    "cannot combine || with other operators");
                ecs_term_free(&elem);
                return -1;
            }

            elem.oper = EcsOr;
        } else {
            prev_or = elem.oper == EcsOr;
            prev_from = elem.from_kind;
        }

        if (action(world, sig, ptr - expr, &elem, ctx)) {
            return -1;
        }

        if (ptr[0]) {
            ptr ++;
            if (prev_or) {
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
            ecs_assert(!term->args[0].name || !strcmp(term->args[0].name, "."), 
                ECS_INTERNAL_ERROR, NULL);

            if (!term->args[0].name) {
                term->args[0].name = ecs_os_strdup(".");
            }

            term->args[0].entity = EcsThis;

            if (term->from_kind == EcsFromAny) {
                term->args[0].set = EcsSelf | EcsSuperSet;
                term->args[0].relation = EcsIsA;
            } else if (term->from_kind == EcsFromShared) {
                term->args[0].set = EcsSuperSet;
                term->args[0].relation = EcsIsA;
            } else if (term->from_kind == EcsFromOwned) {
                term->args[0].set = EcsSelf;
            }
        } else if (term->from_kind == EcsFromParent) {
            term->args[0].set = EcsSuperSet;
            term->args[0].relation = EcsChildOf;
            term->args[0].max_depth = 1;
        } else if (term->from_kind == EcsCascade) {
            term->args[0].set = EcsAll | EcsSuperSet;
            term->args[0].relation = EcsChildOf;
            term->oper = EcsOptional;
        }
    }

    if (term->args[0].set == EcsSelf) {
        term->from_kind = EcsFromOwned;
    } else if (!term->args[0].relation || (term->args[0].relation == EcsIsA)) {
        if (term->args[0].set == EcsSuperSet) {
            term->from_kind = EcsFromShared;
        } else if (term->args[0].set & EcsSuperSet && term->args[0].set & EcsSelf) {
            term->from_kind = EcsFromAny;
        }
    } else if (term->args[0].relation == EcsChildOf) {
        if (term->args[0].set & EcsSuperSet) {
            if (term->args[0].max_depth == 1) {
                term->from_kind = EcsFromParent;
            } else if (term->args[0].set & EcsAll && term->oper == EcsOptional) {
                term->from_kind = EcsCascade;
            }
        }
    }

    if (term->args[1].name) {
        term->id = ecs_pair(
            term->pred.entity, term->args[1].entity);
    } else {
        term->id = term->pred.entity;
    }

    if (term->role == ECS_AND) {
        term->oper = EcsOperAll;
    } else if (term->role == ECS_OR) {
        term->oper = EcsOr;
    } else {
        term->id |= term->role;
    }
}

/** Parse callback that adds component to the components array for a system */
static
int ecs_parse_signature_action(
    ecs_world_t *world,
    ecs_sig_t *sig,
    int64_t column,
    ecs_term_t *term,
    void *data)
{
    (void)column;
    (void)data;
    return ecs_sig_add(world, sig, term);
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

void ecs_term_free(
    ecs_term_t *term)
{
    ecs_os_free(term->pred.name);
    ecs_os_free(term->args[0].name);
    ecs_os_free(term->args[1].name);
    ecs_os_free(term->name);
}

int ecs_sig_init(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_sig_t *sig)
{
    if (expr && ecs_os_strlen(expr) && strcmp(expr, "0")) {
        /* Dispell const, but only temporary. Only strdup the expression if the
         * signature was successfully parsed */
        sig->expr = (char*)expr;
    } else {
        sig->expr = NULL;
    }

    sig->terms = NULL;
    sig->name = (char*)name; /* Dispell const- same as above */

    if (sig->expr) {
        int result = ecs_parse_expr(world, sig, ecs_parse_signature_action, NULL);
        if (!result) {
            sig->name = ecs_os_strdup(name);
            sig->expr = ecs_os_strdup(expr);
        }

        return result;
    } else {
        return 0;
    }
}

void ecs_sig_deinit(
    ecs_sig_t *sig)
{   
    ecs_term_t *terms = ecs_vector_first(sig->terms, ecs_term_t);
    int32_t i, count = ecs_vector_count(sig->terms);

    for (i = 0; i < count; i ++) {
        ecs_term_free(&terms[i]);
    }

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

    /* If term role is OR, component should be a type */
    if (term->role == ECS_OR) {
        const EcsType *type = ecs_get(world, term->id, EcsType);
        if (!type) {
            ecs_parser_error(sig->name, sig->expr, 0, 
                "entity used with OR must be a type");
            return -1;
        }

        int32_t i, count = ecs_vector_count(type->normalized);
        ecs_entity_t *ids = ecs_vector_first(type->normalized, ecs_entity_t);

        for (i = 0; i < count; i ++) {
            elem = ecs_vector_add(&sig->terms, ecs_term_t);
            *elem = *term;
            elem->pred.entity = ids[i];
            elem->pred.name = NULL;
            elem->args[0].name = NULL;
            elem->args[1].name = NULL;
            elem->oper = EcsOr;
            elem->id = ids[i];
            elem->role = 0;
        }

        ecs_term_free(term);

    } else {
        elem = ecs_vector_add(&sig->terms, ecs_term_t);
        *elem = *term;
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
    int32_t or_count = 0;

    for (i = 0; i < count; i ++) {
        if (i) {
            if (or_count >= 1) {
                ecs_strbuf_appendstr(&buf, " || ");
            } else {
                ecs_strbuf_appendstr(&buf, ", ");
            }
        }

        ecs_term_t *term = &terms[i];

        if (or_count < 1) {
            if (term->inout == EcsIn) {
                ecs_strbuf_appendstr(&buf, "[in] ");
            } else if (term->inout == EcsInOut) {
                ecs_strbuf_appendstr(&buf, "[inout] ");
            } else if (term->inout == EcsInOut) {
                ecs_strbuf_appendstr(&buf, "[out] ");
            }
        }

        if (term->role) {
            ecs_strbuf_appendstr(&buf, ecs_role_str(term->role));
            ecs_strbuf_appendstr(&buf, " ");
        }

        if (term->oper == EcsOr) {
            or_count ++;
        } else {
            or_count = 0;
        }

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
            if (ecs_type_has_id(world, type, elem->id)) {
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
