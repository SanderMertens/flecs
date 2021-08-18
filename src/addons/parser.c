#include "flecs.h"

#ifdef FLECS_PARSER

#include "../private_api.h"

/* TODO: after new query parser is working & code is ported to new types for
 *       storing terms, this code needs a big cleanup */

#define ECS_ANNOTATION_LENGTH_MAX (16)

#define TOK_COLON ':'
#define TOK_AND ','
#define TOK_OR "||"
#define TOK_NOT '!'
#define TOK_OPTIONAL '?'
#define TOK_BITWISE_OR '|'
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
#define TOK_CASCADE_SET "cascade"
#define TOK_ALL "all"

#define TOK_ANY "ANY"
#define TOK_OWNED "OVERRIDE"
#define TOK_SHARED "SHARED"
#define TOK_SYSTEM "SYSTEM"
#define TOK_PARENT "PARENT"
#define TOK_CASCADE "CASCADE"

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

static
const char *skip_newline_and_space(
    const char *ptr)
{
    while (isspace(*ptr)) {
        ptr ++;
    }

    return ptr;    
}

/** Skip spaces when parsing signature */
static
const char *skip_space(
    const char *ptr)
{
    while ((*ptr != '\n') && isspace(*ptr)) {
        ptr ++;
    }

    return ptr;
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

    int tmpl_nesting = 0;

    for (; (ch = *ptr); ptr ++) {
        if (ch == '<') {
            tmpl_nesting ++;
        } else if (ch == '>') {
            if (!tmpl_nesting) {
                break;
            }
            tmpl_nesting --;
        } else
        if (!valid_token_char(ch)) {
            break;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';

    if (tmpl_nesting != 0) {
        ecs_parser_error(name, sig, column, 
            "identifier '%s' has mismatching < > pairs", ptr);
        return NULL;
    }

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
        out->var = EcsVarIsEntity;
    } else if (ecs_identifier_is_var(tptr)) {
        out->var = EcsVarIsVariable;
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
    if        (!ecs_os_strcmp(token, TOK_ROLE_PAIR)) 
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
        return ECS_OVERRIDE;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_DISABLED)) {
        return ECS_DISABLED;        
    } else {
        ecs_parser_error(name, sig, column, "invalid role '%s'", token);
        return 0;
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
    } else if (!ecs_os_strcmp(token, TOK_CASCADE_SET)) {
        return EcsCascade;
    } else if (!ecs_os_strcmp(token, TOK_ALL)) {
        return EcsAll;
    } else {
        return 0;
    }
}

static
const char* parse_set_expr(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    int64_t column,
    const char *ptr,
    char *token,
    ecs_term_id_t *id)
{
    do {
        uint8_t tok = parse_set_token(token);
        if (!tok) {
            ecs_parser_error(name, expr, column, 
                "invalid set token '%s'", token);
            return NULL;
        }

        if (id->set.mask & tok) {
            ecs_parser_error(name, expr, column, 
                "duplicate set token '%s'", token);
            return NULL;            
        }

        if ((tok == EcsSubSet && id->set.mask & EcsSuperSet) ||
            (tok == EcsSuperSet && id->set.mask & EcsSubSet))
        {
            ecs_parser_error(name, expr, column, 
                "cannot mix superset and subset", token);
            return NULL;            
        }    

        id->set.mask |= tok;

        if (ptr[0] == TOK_PAREN_OPEN) {
            ptr ++;

            /* Relationship (overrides IsA default) */
            if (!isdigit(ptr[0]) && valid_token_start_char(ptr[0])) {
                ptr = parse_token(name, expr, (ptr - expr), ptr, token);
                if (!ptr) {
                    return NULL;
                }         

                ecs_entity_t rel = ecs_lookup_fullpath(world, token);
                if (!rel) {
                    ecs_parser_error(name, expr, column, 
                        "unresolved identifier '%s'", token);
                    return NULL;
                }

                id->set.relation = rel;

                if (ptr[0] == TOK_AND) {
                    ptr = skip_space(ptr + 1);
                } else if (ptr[0] != TOK_PAREN_CLOSE) {
                    ecs_parser_error(name, expr, column, 
                        "expected ',' or ')'");
                    return NULL;
                }
            }

            /* Max depth of search */
            if (isdigit(ptr[0])) {
                ptr = parse_digit(name, expr, (ptr - expr), ptr, token);
                if (!ptr) {
                    return NULL;
                }

                id->set.max_depth = atoi(token);
                if (id->set.max_depth < 0) {
                    ecs_parser_error(name, expr, column, 
                        "invalid negative depth");
                    return NULL;  
                }

                if (ptr[0] == ',') {
                    ptr = skip_space(ptr + 1);
                }
            }

            /* If another digit is found, previous depth was min depth */
            if (isdigit(ptr[0])) {
                ptr = parse_digit(name, expr, (ptr - expr), ptr, token);
                if (!ptr) {
                    return NULL;
                }

                id->set.min_depth = id->set.max_depth;
                id->set.max_depth = atoi(token);
                if (id->set.max_depth < 0) {
                    ecs_parser_error(name, expr, column, 
                        "invalid negative depth");
                    return NULL;  
                }
            }

            if (ptr[0] != TOK_PAREN_CLOSE) {
                ecs_parser_error(name, expr, column, "expected ')'");
                return NULL;                
            } else {
                ptr = skip_space(ptr + 1);
                if (ptr[0] != TOK_PAREN_CLOSE && ptr[0] != TOK_AND) { 
                    ecs_parser_error(name, expr, column, 
                        "expected end of set expr");
                    return NULL;
                }
            }
        }

        /* Next token in set expression */
        if (ptr[0] == TOK_BITWISE_OR) {
            ptr ++;
            if (valid_token_start_char(ptr[0])) {
                ptr = parse_token(name, expr, (ptr - expr), ptr, token);
                if (!ptr) {
                    return NULL;
                }
            }

        /* End of set expression */
        } else if (ptr[0] == TOK_PAREN_CLOSE || ptr[0] == TOK_AND) {
            break;
        }
    } while (true);

    if (id->set.mask & EcsCascade && !(id->set.mask & EcsSuperSet) && 
        !(id->set.mask & EcsSubSet))
    {
        /* If cascade is used without specifying superset or subset, assume
         * superset */
        id->set.mask |= EcsSuperSet;
    }

    if (id->set.mask & EcsSelf && id->set.min_depth != 0) {
        ecs_parser_error(name, expr, column, 
            "min_depth must be zero for set expression with 'self'");
        return NULL;        
    }

    return ptr;
}

static
const char* parse_arguments(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    int64_t column,
    const char *ptr,
    char *token,
    ecs_term_t *term)
{
    (void)column;

    int32_t arg = 0;

    do {
        if (valid_token_start_char(ptr[0])) {
            if (arg == 2) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "too many arguments in term");
                return NULL;
            }

            ptr = parse_token(name, expr, (ptr - expr), ptr, token);
            if (!ptr) {
                return NULL;
            }

            /* If token is a self, superset or subset token, this is a set
             * expression */
            if (!ecs_os_strcmp(token, TOK_ALL) ||
                !ecs_os_strcmp(token, TOK_CASCADE_SET) ||
                !ecs_os_strcmp(token, TOK_SELF) || 
                !ecs_os_strcmp(token, TOK_SUPERSET) || 
                !ecs_os_strcmp(token, TOK_SUBSET))
            {
                ptr = parse_set_expr(world, name, expr, (ptr - expr), ptr, 
                    token, &term->args[arg]);
                if (!ptr) {
                    return NULL;
                }

            /* Regular identifier */
            } else if (parse_identifier(token, &term->args[arg])) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid identifier '%s'", token);
                return NULL;
            }

            if (ptr[0] == TOK_AND) {
                ptr = skip_space(ptr + 1);

            } else if (ptr[0] == TOK_PAREN_CLOSE) {
                ptr = skip_space(ptr + 1);
                break;

            } else {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "expected ',' or ')'");
                return NULL;
            }

        } else {
            ecs_parser_error(name, expr, (ptr - expr), 
                "expected identifier or set expression");
            return NULL;
        }

        arg ++;

    } while (true);

    return ptr;
}

static
const char* parse_term(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_term_t *term_out)
{
    const char *ptr = expr;
    char token[ECS_MAX_TOKEN_SIZE] = {0};
    ecs_term_t term = { .move = true /* parser never owns resources */ };

    ptr = skip_space(ptr);

    /* Inout specifiers always come first */
    if (ptr[0] == TOK_BRACKET_OPEN) {
        ptr = parse_annotation(name, expr, (ptr - expr), ptr + 1, &term.inout);
        if (!ptr) {
            return NULL;
        }
        ptr = skip_space(ptr);
    }

    if (valid_operator_char(ptr[0])) {
        term.oper = parse_operator(ptr[0]);
        ptr = skip_space(ptr + 1);
    }

    /* If next token is the start of an identifier, it could be either a type
     * role, source or component identifier */
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
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
            ptr = parse_token(name, expr, (ptr - expr), ptr, token);
            if (!ptr) {
                return NULL;
            }

            goto parse_singleton;

        } else {
            ecs_parser_error(name, expr, (ptr - expr), 
                "expected identifier after singleton operator");
            return NULL;
        }

    /* Pair with implicit subject */
    } else if (ptr[0] == TOK_PAREN_OPEN) {
        goto parse_pair;

    /* Nothing else expected here */
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "unexpected character '%c'", ptr[0]);
        return NULL;
    }

empty_source:
    term.args[0].set.mask = EcsNothing;
    ptr = skip_space(ptr + 1);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }

        goto parse_predicate;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected identifier after source operator");
        return NULL;
    }

parse_source:
    if (!ecs_os_strcmp(token, TOK_PARENT)) {
        term.args[0].set.mask = EcsSuperSet;
        term.args[0].set.relation = EcsChildOf;
        term.args[0].set.max_depth = 1;
    } else if (!ecs_os_strcmp(token, TOK_SYSTEM)) {
        term.args[0].name = ecs_os_strdup(name);
    } else if (!ecs_os_strcmp(token, TOK_ANY)) {
        term.args[0].set.mask = EcsSelf | EcsSuperSet;
        term.args[0].set.relation = EcsIsA;
        term.args[0].entity = EcsThis;
    } else if (!ecs_os_strcmp(token, TOK_OWNED)) {
        term.args[0].set.mask = EcsSelf;
        term.args[0].entity = EcsThis;
    } else if (!ecs_os_strcmp(token, TOK_SHARED)) {
        term.args[0].set.mask = EcsSuperSet;
        term.args[0].set.relation = EcsIsA;
        term.args[0].entity = EcsThis;
    } else if (!ecs_os_strcmp(token, TOK_CASCADE)) {
        term.args[0].set.mask = EcsSuperSet | EcsCascade;
        term.args[0].set.relation = EcsChildOf;
        term.args[0].entity = EcsThis;
        term.oper = EcsOptional;
    } else {
         if (parse_identifier(token, &term.args[0])) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid identifier '%s'", token); 
            return NULL;           
        }
    }

    ptr = skip_space(ptr);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* Is the next token a role? */
        if (ptr[0] == TOK_BITWISE_OR && ptr[1] != TOK_BITWISE_OR) {
            ptr++;
            goto parse_role;
        }      

        /* If not, it's a predicate */
        goto parse_predicate;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected identifier after source");
        return NULL;
    }

parse_role:
    term.role = parse_role(name, expr, (ptr - expr), token);
    if (!term.role) {
        return NULL;
    }

    ptr = skip_space(ptr);

    /* If next token is the source token, this is an empty source */
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* If not, it's a predicate */
        goto parse_predicate;

    } else if (ptr[0] == TOK_PAREN_OPEN) {
        goto parse_pair;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected identifier after role");
        return NULL;
    }

parse_predicate:
    if (parse_identifier(token, &term.pred)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        return NULL;        
    }

    ptr = skip_space(ptr);
    
    if (ptr[0] == TOK_PAREN_OPEN) {
        ptr ++;
        if (ptr[0] == TOK_PAREN_CLOSE) {
            term.args[0].set.mask = EcsNothing;
            ptr ++;
            ptr = skip_space(ptr);
        } else {
            ptr = parse_arguments(
                world, name, expr, (ptr - expr), ptr, token, &term);
        }

        goto parse_done;

    } else if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }

        goto parse_name;
    }

    goto parse_done;

parse_pair:
    ptr = parse_token(name, expr, (ptr - expr), ptr + 1, token);
    if (!ptr) {
        return NULL;
    }

    if (ptr[0] == TOK_AND) {
        ptr ++;
        term.args[0].entity = EcsThis;
        goto parse_pair_predicate;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "unexpected character '%c'", ptr[0]);
    }

parse_pair_predicate:
    if (parse_identifier(token, &term.pred)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        return NULL;            
    }

    ptr = skip_space(ptr);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }

        if (ptr[0] == TOK_PAREN_CLOSE) {
            ptr ++;
            goto parse_pair_object;
        } else {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unexpected character '%c'", ptr[0]);
            return NULL;
        }
    } else if (ptr[0] == TOK_PAREN_CLOSE) {
        /* No object */
        goto parse_done;
    }

parse_pair_object:
    if (parse_identifier(token, &term.args[1])) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        return NULL;
    }

    ptr = skip_space(ptr);
    goto parse_done; 

parse_singleton:
    if (parse_identifier(token, &term.pred)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        return NULL;        
    }

    parse_identifier(token, &term.args[0]);
    goto parse_done;

parse_name:
    term.name = ecs_os_strdup(token);
    ptr = skip_space(ptr);

parse_done:
    *term_out = term;

    return ptr;
}

char* ecs_parse_term(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_term_t *term)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_term_id_t *subj = &term->args[0];

    bool prev_or = false;
    if (ptr != expr) {
        /* If this is not the start of the expression, scan back to check if
         * previous token was an OR */
        const char *bptr = ptr - 1;
        do {
            char ch = bptr[0];
            if (isspace(ch)) {
                bptr --;
                continue;
            }

            /* Previous token was not an OR */
            if (ch == TOK_AND) {
                break;
            }

            /* Previous token was an OR */
            if (ch == TOK_OR[0]) {
                prev_or = true;
                break;
            }

            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid preceding token");
            return NULL;
        } while (true);
    }

    ptr = skip_newline_and_space(ptr);
    if (!ptr[0]) {
        return (char*)ptr;
    }

    if (ptr == expr && !strcmp(expr, "0")) {
        return (char*)&ptr[1];
    }

    int32_t prev_set = subj->set.mask;

    /* Parse next element */
    ptr = parse_term(world, name, ptr, term);
    if (!ptr) {
        ecs_term_fini(term);
        return NULL;
    }

    /* Post-parse consistency checks */

    /* If next token is OR, term is part of an OR expression */
    if (!ecs_os_strncmp(ptr, TOK_OR, 2) || prev_or) {
        /* An OR operator must always follow an AND or another OR */
        if (term->oper != EcsAnd) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "cannot combine || with other operators");
            ecs_term_fini(term);
            return NULL;
        }

        term->oper = EcsOr;
    }

    /* Term must either end in end of expression, AND or OR token */
    if (ptr[0] != TOK_AND && (ptr[0] != TOK_OR[0]) && (ptr[0] != '\n') && ptr[0]) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected end of expression or next term");
        ecs_term_fini(term);
        return NULL;
    }

    /* If the term just contained a 0, the expression has nothing. Ensure
     * that after the 0 nothing else follows */
    if (!ecs_os_strcmp(term->pred.name, "0")) {
        if (ptr[0]) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unexpected term after 0"); 
            ecs_term_fini(term);
            return NULL;
        }

        if (subj->set.mask != EcsDefaultSet || 
           (subj->entity && subj->entity != EcsThis) ||
           (subj->name && ecs_os_strcmp(subj->name, "This")))
        {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid combination of 0 with non-default subject");
            ecs_term_fini(term);
            return NULL;
        }

        subj->set.mask = EcsNothing;
        ecs_os_free(term->pred.name);
        term->pred.name = NULL;
    }

    /* Cannot combine EcsNothing with operators other than AND */
    if (term->oper != EcsAnd && subj->set.mask == EcsNothing) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid operator for empty source"); 
        ecs_term_fini(term);    
        return NULL;
    }

    /* Verify consistency of OR expression */
    if (prev_or && term->oper == EcsOr) {
        /* Set expressions must be the same for all OR terms */
        if (subj->set.mask != prev_set) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "cannot combine different sources in OR expression");
            ecs_term_fini(term);
            return NULL;
        }

        term->oper = EcsOr;
    }

    /* Automatically assign This if entity is not assigned and the set is
     * nothing */
    if (subj->set.mask != EcsNothing) {
        if (!subj->name) {
            if (!subj->entity) {
                subj->entity = EcsThis;
            }
        }
    }

    if (subj->name && !ecs_os_strcmp(subj->name, "0")) {
        subj->entity = 0;
        subj->set.mask = EcsNothing;
    }

    /* Process role */
    if (term->role == ECS_AND) {
        term->oper = EcsAndFrom;
        term->role = 0;
    } else if (term->role == ECS_OR) {
        term->oper = EcsOrFrom;
        term->role = 0;
    } else if (term->role == ECS_NOT) {
        term->oper = EcsNotFrom;
        term->role = 0;
    }

    if (ptr[0]) {
        if (ptr[0] == ',') {
            ptr ++;
        } else if (ptr[0] == '|') {
            ptr += 2;
        }
    }

    ptr = skip_space(ptr);

    return (char*)ptr;
}

#endif
