/**
 * @file addons/parser.c
 * @brief Parser addon.
 */

#include "flecs.h"

#include "../private_api.h"
#include <ctype.h>

#define TOK_COLON ':'
#define TOK_AND ','
#define TOK_OR "||"
#define TOK_NOT '!'
#define TOK_OPTIONAL '?'
#define TOK_BITWISE_OR '|'
#define TOK_BRACKET_OPEN '['
#define TOK_BRACKET_CLOSE ']'
#define TOK_SCOPE_OPEN '{'
#define TOK_SCOPE_CLOSE '}'
#define TOK_VARIABLE '$'
#define TOK_PAREN_OPEN '('
#define TOK_PAREN_CLOSE ')'
#define TOK_EQ "=="
#define TOK_NEQ "!="
#define TOK_MATCH "~="
#define TOK_EXPR_STRING '"'

#define TOK_SELF "self"
#define TOK_UP "up"
#define TOK_CASCADE "cascade"
#define TOK_DESC "desc"

#define TOK_OVERRIDE "OVERRIDE"
#define TOK_ROLE_AND "AND"
#define TOK_ROLE_OR "OR"
#define TOK_ROLE_NOT "NOT"
#define TOK_ROLE_TOGGLE "TOGGLE"

#define TOK_IN "in"
#define TOK_OUT "out"
#define TOK_INOUT "inout"
#define TOK_INOUT_NONE "none"

static
const ecs_id_t ECS_AND =                                           (1ull << 60);

static
const ecs_id_t ECS_OR =                                            (1ull << 59);

static
const ecs_id_t ECS_NOT =                                           (1ull << 58);

#define ECS_MAX_TOKEN_SIZE (256)

typedef char ecs_token_t[ECS_MAX_TOKEN_SIZE];


/* -- Private functions -- */

bool flecs_isident(
    char ch)
{
    return isalpha(ch) || (ch == '_');
}

static
bool flecs_valid_identifier_start_char(
    char ch)
{
    if (ch && (flecs_isident(ch) || (ch == '*') ||
        (ch == '0') || (ch == TOK_VARIABLE) || isdigit(ch))) 
    {
        return true;
    }

    return false;
}

static
bool flecs_valid_token_start_char(
    char ch)
{
    if ((ch == '"') || (ch == '{') || (ch == '}') || (ch == ',') || (ch == '-')
        || (ch == '[') || (ch == ']') || (ch == '`') || 
        flecs_valid_identifier_start_char(ch))
    {
        return true;
    }

    return false;
}

static
bool flecs_valid_token_char(
    char ch)
{
    if (ch && (flecs_isident(ch) || isdigit(ch) || ch == '.' || ch == '"')) {
        return true;
    }

    return false;
}

static
bool flecs_valid_operator_char(
    char ch)
{
    if (ch == TOK_OPTIONAL || ch == TOK_NOT) {
        return true;
    }

    return false;
}

const char* ecs_parse_token(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token_out,
    char delim)
{
    int64_t column = ptr - expr;

    ptr = ecs_parse_ws(ptr);
    char *tptr = token_out, ch = ptr[0];

    if (!flecs_valid_token_start_char(ch)) {
        if (ch == '\0' || ch == '\n') {
            ecs_parser_error(name, expr, column, 
                "unexpected end of expression");
        } else {
            ecs_parser_error(name, expr, column, 
                "invalid start of token '%s'", ptr);
        }
        return NULL;
    }

    tptr[0] = ch;
    tptr ++;
    ptr ++;

    if (ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ',' || ch == '`') {
        tptr[0] = 0;
        return ptr;
    }

    int tmpl_nesting = 0;
    bool in_str = ch == '"';

    for (; (ch = *ptr); ptr ++) {
        if (ch == '<') {
            tmpl_nesting ++;
        } else if (ch == '>') {
            if (!tmpl_nesting) {
                break;
            }
            tmpl_nesting --;
        } else if (ch == '"') {
            in_str = !in_str;
        } else if (ch == '\\') {
            ptr ++;
            tptr[0] = ptr[0];
            tptr ++;
            continue;
        } else if (!flecs_valid_token_char(ch) && !in_str) {
            break;
        }

        if (delim && (ch == delim)) {
            break;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';

    if (tmpl_nesting != 0) {
        ecs_parser_error(name, expr, column, 
            "identifier '%s' has mismatching < > pairs", ptr);
        return NULL;
    }

    const char *next_ptr = ecs_parse_ws(ptr);
    if (next_ptr[0] == ':' && next_ptr != ptr) {
        /* Whitespace between token and : is significant */
        ptr = next_ptr - 1;
    } else {
        ptr = next_ptr;
    }

    return ptr;
}

const char* ecs_parse_identifier(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token_out)
{
    if (!flecs_valid_identifier_start_char(ptr[0]) && (ptr[0] != '"')) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected start of identifier");
        return NULL;
    }

    ptr = ecs_parse_token(name, expr, ptr, token_out, 0);

    return ptr;
}

static
int flecs_parse_identifier(
    char **token,
    ecs_term_ref_t *out)
{
    char *tptr = *token;
    if (tptr[0] == TOK_VARIABLE && tptr[1]) {
        out->id |= EcsIsVariable;
        tptr ++;
    }
    if (tptr[0] == TOK_EXPR_STRING && tptr[1]) {
        out->id |= EcsIsName;
        tptr ++;
        if (tptr[0] == TOK_NOT) {
            /* Already parsed */
            tptr ++;
        }
    }

    char *name = tptr;
    out->name = name;

    ecs_size_t len = ecs_os_strlen(name);
    if (out->id & EcsIsName) {
        if (name[len - 1] != TOK_EXPR_STRING) {
            ecs_parser_error(NULL, *token, 0, "missing '\"' at end of string");
            return -1;
        } else {
            name[len - 1] = '\0';
        }
    }

    (*token) += (name - *token) + len + 1;

    return 0;
}

static
ecs_entity_t flecs_parse_role(
    const char *name,
    const char *sig,
    int64_t column,
    const char *token)
{
    if (!ecs_os_strcmp(token, TOK_ROLE_AND)) {
        return ECS_AND;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_OR)) {
        return ECS_OR;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_NOT)) {
        return ECS_NOT;
    } else if (!ecs_os_strcmp(token, TOK_OVERRIDE)) {
        return ECS_OVERRIDE;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_TOGGLE)) {
        return ECS_TOGGLE;        
    } else {
        ecs_parser_error(name, sig, column, "invalid role '%s'", token);
        return 0;
    }
}

static
ecs_oper_kind_t flecs_parse_operator(
    char ch)
{
    if (ch == TOK_OPTIONAL) {
        return EcsOptional;
    } else if (ch == TOK_NOT) {
        return EcsNot;
    } else {
        ecs_throw(ECS_INTERNAL_ERROR, NULL);
    }
error:
    return 0;
}

static
const char* flecs_parse_annotation(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr, 
    int16_t *inout_kind_out)
{
    char token[ECS_MAX_TOKEN_SIZE];

    ptr = ecs_parse_identifier(name, sig, ptr, token);
    if (!ptr) {
        return NULL;
    }

    if (!ecs_os_strcmp(token, TOK_IN)) {
        *inout_kind_out = EcsIn;
    } else
    if (!ecs_os_strcmp(token, TOK_OUT)) {
        *inout_kind_out = EcsOut;
    } else
    if (!ecs_os_strcmp(token, TOK_INOUT)) {
        *inout_kind_out = EcsInOut;
    } else if (!ecs_os_strcmp(token, TOK_INOUT_NONE)) {
        *inout_kind_out = EcsInOutNone;
    }

    ptr = ecs_parse_ws(ptr);

    if (ptr[0] != TOK_BRACKET_CLOSE) {
        ecs_parser_error(name, sig, column, "expected ]");
        return NULL;
    }

    return ptr + 1;
}

static
ecs_flags64_t flecs_parse_set_token(
    const char *token)
{
    if (!ecs_os_strcmp(token, TOK_SELF)) {
        return EcsSelf;
    } else if (!ecs_os_strcmp(token, TOK_UP)) {
        return EcsUp;
    } else if (!ecs_os_strcmp(token, TOK_CASCADE)) {
        return EcsCascade;
    } else if (!ecs_os_strcmp(token, TOK_DESC)) {
        return EcsDesc;
    } else {
        return 0;
    }
}

static
const char* flecs_parse_term_flags(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    int64_t column,
    const char *ptr,
    char *token,
    ecs_term_t *term,
    ecs_term_ref_t *ref,
    char tok_end)
{
    ecs_assert(term != NULL, ECS_INTERNAL_ERROR, NULL);

    char token_buf[ECS_MAX_TOKEN_SIZE] = {0};
    if (!token) {
        token = token_buf;
        ptr = ecs_parse_identifier(name, expr, ptr, token);
        if (!ptr) {
            return NULL;
        }
    }

    do {
        ecs_flags64_t tok = flecs_parse_set_token(token);
        if (!tok) {
            ecs_parser_error(name, expr, column, 
                "invalid set token '%s'", token);
            return NULL;
        }

        if (ref->id & tok) {
            ecs_parser_error(name, expr, column, 
                "duplicate set token '%s'", token);
            return NULL;            
        }
        
        ref->id |= tok;

        if (ptr[0] == TOK_PAREN_OPEN) {
            ptr ++;

            /* Relationship (overrides IsA default) */
            if (!isdigit(ptr[0]) && flecs_valid_token_start_char(ptr[0])) {
                ptr = ecs_parse_identifier(name, expr, ptr, token);
                if (!ptr) {
                    return NULL;
                }         

                term->trav = ecs_lookup(world, token);
                if (!term->trav) {
                    ecs_parser_error(name, expr, column, 
                        "unresolved identifier '%s'", token);
                    return NULL;
                }

                if (ptr[0] == TOK_AND) {
                    ptr = ecs_parse_ws(ptr + 1);
                } else if (ptr[0] != TOK_PAREN_CLOSE) {
                    ecs_parser_error(name, expr, column, 
                        "expected ',' or ')'");
                    return NULL;
                }
            }

            if (ptr[0] != TOK_PAREN_CLOSE) {
                ecs_parser_error(name, expr, column, "expected ')', got '%c'",
                    ptr[0]);
                return NULL;                
            } else {
                ptr = ecs_parse_ws(ptr + 1);
                if (ptr[0] != tok_end && ptr[0] != TOK_AND && ptr[0] != 0) {
                    ecs_parser_error(name, expr, column, 
                        "expected end of set expr");
                    return NULL;
                }
            }
        }

        /* Next token in set expression */
        if (ptr[0] == TOK_BITWISE_OR) {
            ptr ++;
            if (flecs_valid_token_start_char(ptr[0])) {
                ptr = ecs_parse_identifier(name, expr, ptr, token);
                if (!ptr) {
                    return NULL;
                }
            }

        /* End of set expression */
        } else if (ptr[0] == tok_end || ptr[0] == TOK_AND || !ptr[0]) {
            break;
        }
    } while (true);

    return ptr;
}

static
const char* flecs_parse_arguments(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    int64_t column,
    const char *ptr,
    char **token,
    ecs_term_t *term,
    ecs_oper_kind_t *extra_oper,
    ecs_term_ref_t *extra_args)
{
    (void)column;

    int32_t arg = 0;

    if (extra_args) {
        ecs_os_memset_n(extra_args, 0, ecs_term_ref_t, FLECS_TERM_ARG_COUNT_MAX);
    }

    if (!term) {
        arg = 2;
    }

    do {
        if (flecs_valid_token_start_char(ptr[0])) {
            if ((arg == FLECS_TERM_ARG_COUNT_MAX) || (!extra_args && arg == 2)) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "too many arguments in term");
                goto error;
            }

            ptr = ecs_parse_identifier(name, expr, ptr, *token);
            if (!ptr) {
                goto error;
            }

            ecs_term_ref_t *term_ref = NULL;
            ptr = ecs_parse_ws_eol(ptr);

            if (arg == 0) {
                term_ref = &term->src;
            } else if (arg == 1) {
                term_ref = &term->second;
            } else {
                term_ref = &extra_args[arg - 2];
            }

            /* If token is a colon, the token is an identifier followed by a
             * set expression. */
            if (ptr[0] == TOK_COLON) {
                if (flecs_parse_identifier(token, term_ref)) {
                    ecs_parser_error(name, expr, (ptr - expr), 
                        "invalid identifier '%s'", *token);
                    goto error;
                }

                ptr = ecs_parse_ws_eol(ptr + 1);
                ptr = flecs_parse_term_flags(world, name, expr, (ptr - expr), ptr,
                    NULL, term, term_ref, TOK_PAREN_CLOSE);
                if (!ptr) {
                    goto error;
                }

            /* Check for term flags */
            } else if (!ecs_os_strcmp(*token, TOK_CASCADE) ||
                !ecs_os_strcmp(*token, TOK_DESC) ||
                !ecs_os_strcmp(*token, TOK_SELF) || 
                !ecs_os_strcmp(*token, TOK_UP))
            {
                ptr = flecs_parse_term_flags(world, name, expr, (ptr - expr), ptr, 
                    *token, term, term_ref, TOK_PAREN_CLOSE);
                if (!ptr) {
                    goto error;
                }

            /* Regular identifier */
            } else if (flecs_parse_identifier(token, term_ref)) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid identifier '%s'", *token);
                goto error;
            }

            if (ptr[0] == TOK_AND) {
                if (extra_oper && *extra_oper != EcsAnd) {
                    ecs_parser_error(name, expr, (ptr - expr), 
                        "cannot mix ',' and '||' in term arguments");
                    goto error;
                }
                ptr = ecs_parse_ws_eol(ptr + 1);

            } else if (ptr[0] == TOK_PAREN_CLOSE) {
                ptr = ecs_parse_ws(ptr + 1);
                break;

            } else if (extra_oper && ptr[0] == TOK_OR[0] && ptr[1] == TOK_OR[1]){
                if (arg >= 2 && *extra_oper != EcsOr) {
                    ecs_parser_error(name, expr, (ptr - expr), 
                        "cannot mix ',' and '||' in term arguments");
                    goto error;
                }

                *extra_oper = EcsOr;
                ptr = ecs_parse_ws_eol(ptr + 2);

            } else {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "expected ',' or ')'");
                goto error;
            }

        } else {
            ecs_parser_error(name, expr, (ptr - expr), 
                "expected identifier or set expression");
            goto error;
        }

        arg ++;

    } while (true);

    return ptr;
error:
    return NULL;
}

static
void flecs_parser_unexpected_char(
    const char *name,
    const char *expr,
    const char *ptr,
    char ch)
{
    if (ch && (ch != '\n')) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "unexpected character '%c'", ch);
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "unexpected end of term");
    }
}

static
const char* flecs_parse_term(
    const ecs_world_t *world,
    ecs_stage_t *stage,
    const char *name,
    const char *expr,
    ecs_term_t *term_out,
    ecs_oper_kind_t *extra_oper,
    ecs_term_ref_t *extra_args)
{
    const char *ptr = expr;
    ecs_term_t term = {0};

    ptr = ecs_parse_ws(ptr);

    /* Inout specifiers always come first */
    if (ptr[0] == TOK_BRACKET_OPEN) {
        ptr = flecs_parse_annotation(
            name, expr, (ptr - expr), ptr + 1, &term.inout);
        if (!ptr) {
            goto error;
        }
        ptr = ecs_parse_ws_eol(ptr);
    }

    if (flecs_valid_operator_char(ptr[0])) {
        term.oper = flecs_ito(int16_t, flecs_parse_operator(ptr[0]));
        ptr = ecs_parse_ws(ptr + 1);
    }

    /* If next token is the start of an identifier, it could be either a type
     * role, source or component identifier */
    if (flecs_valid_identifier_start_char(ptr[0])) {
        ptr = ecs_parse_identifier(name, expr, ptr, stage->parser_token);
        if (!ptr) {
            goto error;
        }

        /* Is token a type role? */
        if (ptr[0] == TOK_BITWISE_OR && ptr[1] != TOK_BITWISE_OR) {
            ptr ++;
            goto flecs_parse_role;
        }

        /* Is token a predicate? */
        if (ptr[0] == TOK_PAREN_OPEN) {
            goto parse_predicate;    
        }

        /* Next token must be a predicate */
        goto parse_predicate;

    /* Pair with implicit subject */
    } else if (ptr[0] == TOK_PAREN_OPEN) {
        goto parse_pair;

    /* Open query scope */
    } else if (ptr[0] == TOK_SCOPE_OPEN) {
        term.first.id = EcsScopeOpen;
        term.src.id = EcsIsEntity;
        term.inout = EcsInOutNone;
        goto parse_done;

    /* Close query scope */
    } else if (ptr[0] == TOK_SCOPE_CLOSE) {
        term.first.id = EcsScopeClose;
        term.src.id = EcsIsEntity;
        term.inout = EcsInOutNone;
        ptr = ecs_parse_ws(ptr + 1);
        goto parse_done;

    /* Nothing else expected here */
    } else {
        flecs_parser_unexpected_char(name, expr, ptr, ptr[0]);
        goto error;
    }

flecs_parse_role:
    term.id = flecs_parse_role(name, expr, (ptr - expr), stage->parser_token);
    if (!term.id) {
        goto error;
    }

    ptr = ecs_parse_ws(ptr);

    /* If next token is the source token, this is an empty source */
    if (flecs_valid_token_start_char(ptr[0])) {
        ptr = ecs_parse_identifier(name, expr, ptr, stage->parser_token);
        if (!ptr) {
            goto error;
        }

        /* If not, it's a predicate */
        goto parse_predicate;

    } else if (ptr[0] == TOK_PAREN_OPEN) {
        goto parse_pair;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected identifier after role");
        goto error;
    }

parse_predicate:
    if (flecs_parse_identifier(&stage->parser_token, &term.first)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", stage->parser_token); 
        goto error;
    }

    /* Set expression */
    if (ptr[0] == TOK_COLON) {
        ptr = ecs_parse_ws(ptr + 1);
        ptr = flecs_parse_term_flags(world, name, expr, (ptr - expr), ptr, NULL, 
            &term, &term.first, TOK_COLON);
        if (!ptr) {
            goto error;
        }

        ptr = ecs_parse_ws(ptr);

        if (ptr[0] == TOK_AND || !ptr[0]) {
            goto parse_done;
        }

        if (ptr[0] != TOK_COLON) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unexpected token '%c' after predicate set expression", ptr[0]);
            goto error;
        }

        ptr = ecs_parse_ws(ptr + 1);
    } else if (!ecs_os_strncmp(ptr, TOK_EQ, 2)) {
        ptr = ecs_parse_ws(ptr + 2);
        goto parse_eq;
    } else if (!ecs_os_strncmp(ptr, TOK_NEQ, 2)) {
        ptr = ecs_parse_ws(ptr + 2);
        goto parse_neq;
    } else if (!ecs_os_strncmp(ptr, TOK_MATCH, 2)) {
        ptr = ecs_parse_ws(ptr + 2);
        goto parse_match;
    } else {
        ptr = ecs_parse_ws(ptr);
    }

    if (ptr[0] == TOK_PAREN_OPEN) {
        ptr = ecs_parse_ws_eol(ptr + 1);
        if (ptr[0] == TOK_PAREN_CLOSE) {
            term.src.id = EcsIsEntity;
            ptr ++;
            ptr = ecs_parse_ws(ptr);
        } else {
            ptr = flecs_parse_arguments(
                world, name, expr, (ptr - expr), ptr, &stage->parser_token, 
                &term, extra_oper, extra_args);
        }

        goto parse_done;
    }

    goto parse_done;

parse_eq:
    term.src = term.first;
    term.first = (ecs_term_ref_t){0};
    term.first.id = EcsPredEq;
    goto parse_right_operand;

parse_neq:
    term.src = term.first;
    term.first = (ecs_term_ref_t){0};
    term.first.id = EcsPredEq;
    if (term.oper != EcsAnd) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid operator combination");
        goto error;
    }
    term.oper = EcsNot;
    goto parse_right_operand;
    
parse_match:
    term.src = term.first;
    term.first = (ecs_term_ref_t){0};
    term.first.id = EcsPredMatch;
    goto parse_right_operand;

parse_right_operand:
    if (flecs_valid_token_start_char(ptr[0])) {
        ptr = ecs_parse_identifier(name, expr, ptr, stage->parser_token);
        if (!ptr) {
            goto error;
        }

        if (ECS_TERM_REF_ID(&term.first) == EcsPredMatch) {
            if (stage->parser_token[0] == '"' && stage->parser_token[1] == '!') {
                term.oper = EcsNot;
            }
        }

        if (flecs_parse_identifier(&stage->parser_token, &term.second)) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid identifier '%s'", stage->parser_token); 
            goto error;
        }

        term.src.id &= ~EcsTraverseFlags;
        term.src.id |= EcsSelf;
        term.inout = EcsInOutNone;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected identifier");
        goto error;
    }
    goto parse_done;
parse_pair:
    ptr = ecs_parse_identifier(name, expr, ptr + 1, stage->parser_token);
    if (!ptr) {
        goto error;
    }

    if (ptr[0] == TOK_COLON) {
        ptr = ecs_parse_ws(ptr + 1);
        ptr = flecs_parse_term_flags(world, name, expr, (ptr - expr), ptr,
            NULL, &term, &term.first, TOK_PAREN_CLOSE);
        if (!ptr) {
            goto error;
        }
    }

    if (ptr[0] == TOK_AND) {
        ptr = ecs_parse_ws(ptr + 1);
        if (ptr[0] == TOK_PAREN_CLOSE) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "expected identifier for second element of pair"); 
            goto error;
        }
        
        term.src.id = EcsThis;
        term.src.id |= EcsIsVariable;
        goto parse_pair_predicate;
    } else if (ptr[0] == TOK_PAREN_CLOSE) {
        term.src.id = EcsThis;
        term.src.id |= EcsIsVariable;
        goto parse_pair_predicate;
    } else {
        flecs_parser_unexpected_char(name, expr, ptr, ptr[0]);
        goto error;
    }

parse_pair_predicate:
    if (flecs_parse_identifier(&stage->parser_token, &term.first)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", stage->parser_token); 
        goto error;
    }

    ptr = ecs_parse_ws(ptr);
    if (flecs_valid_token_start_char(ptr[0])) {
        ptr = ecs_parse_identifier(name, expr, ptr, stage->parser_token);
        if (!ptr) {
            goto error;
        }

        if (ptr[0] == TOK_COLON) {
            ptr = ecs_parse_ws(ptr + 1);
            ptr = flecs_parse_term_flags(world, name, expr, (ptr - expr), ptr,
                NULL, &term, &term.second, TOK_PAREN_CLOSE);
            if (!ptr) {
                goto error;
            }
        }

        if (ptr[0] == TOK_PAREN_CLOSE || ptr[0] == TOK_AND || ptr[0] == TOK_OR[0]) {
            goto parse_pair_object;
        } else {
            flecs_parser_unexpected_char(name, expr, ptr, ptr[0]);
            goto error;
        }
    } else if (ptr[0] == TOK_PAREN_CLOSE) {
        /* No object */
        ptr ++;
        goto parse_done;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected pair object or ')'");
        goto error;
    }

parse_pair_object:
    if (flecs_parse_identifier(&stage->parser_token, &term.second)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", stage->parser_token); 
        goto error;
    }

    if (term.id == 0) {
        term.id = ECS_PAIR;
    }

    if (ptr[0] == TOK_AND) {
        ptr = ecs_parse_ws(ptr + 1);
        ptr = flecs_parse_arguments(world, name, expr, (ptr - expr), ptr, 
            &stage->parser_token, NULL, extra_oper, extra_args);
        if (!ptr) {
            goto error;
        }
    } else if (extra_oper && ptr[0] == TOK_OR[0] && ptr[1] == TOK_OR[1]) {
        ptr = ecs_parse_ws_eol(ptr + 2);
        *extra_oper = EcsOr;
        ptr = flecs_parse_arguments(world, name, expr, (ptr - expr), ptr, 
            &stage->parser_token, NULL, extra_oper, extra_args);
        if (!ptr) {
            goto error;
        }
    } else {
        ptr ++;
    }

    ptr = ecs_parse_ws(ptr);
    goto parse_done;

parse_done:
    *term_out = term;
    return ptr;

error:
    *term_out = (ecs_term_t){0};
    return NULL;
}

static
bool flecs_is_valid_end_of_term(
    const char *ptr)
{
    if ((ptr[0] == TOK_AND) ||    /* another term with And operator */
        (ptr[0] == TOK_OR[0]) ||  /* another term with Or operator */
        (ptr[0] == '\n') ||       /* newlines are valid */
        (ptr[0] == '\0') ||       /* end of string */
        (ptr[0] == '/') ||        /* comment (in plecs) */
        (ptr[0] == '{') ||        /* scope (in plecs) */
        (ptr[0] == '}') ||
        (ptr[0] == '[') ||        /* collection scope (in plecs) */
        (ptr[0] == ']') ||
        (ptr[0] == ':') ||        /* inheritance (in plecs) */
        (ptr[0] == '='))          /* assignment (in plecs) */
    {
        return true;
    }
    return false;
}

char* ecs_parse_term(
    const ecs_world_t *world,
    ecs_stage_t *stage,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_term_t *term,
    ecs_oper_kind_t *extra_oper,
    ecs_term_ref_t *extra_args,
    bool allow_newline)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(term != NULL, ECS_INVALID_PARAMETER, NULL);

    if (extra_oper) {
        *extra_oper = EcsAnd;
    }

    ecs_term_ref_t *src = &term->src;

    if (ptr != expr) {
        if (ptr[0]) {
            if (ptr[0] == ',') {
                ptr ++;
            } else if (ptr[0] == '|') {
                ptr += 2;
            } else if (ptr[0] == '{') {
                ptr ++;
            } else if (ptr[0] == '}') {
                /* nothing to be done */
            } else {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid preceding token");
            }
        }
    } else {
        /* If this is first term in expression, reset token buffer */
        stage->parser_token = stage->parser_tokens;
    }

    ptr = ecs_parse_ws_eol(ptr);
    if (!ptr[0]) {
        *term = (ecs_term_t){0};
        return ECS_CONST_CAST(char*, ptr);
    }

    if (extra_oper) {
        *extra_oper = EcsAnd;
    }

    if (ptr == expr && !strcmp(expr, "0")) {
        return ECS_CONST_CAST(char*, &ptr[1]);
    }

    /* Parse next element */
    ptr = flecs_parse_term(world, stage, name, ptr, term, extra_oper, extra_args);
    if (!ptr) {
        goto error;
    }

    /* Check for $() notation */
    if (term->first.name && !ecs_os_strcmp(term->first.name, "$")) {
        if (term->src.name) {            
            term->first = term->src;

            if (term->second.name) {
                term->src = term->second;       
            } else {
                term->src.id = EcsThis | (term->src.id & EcsTermRefFlags);
                term->src.id |= EcsIsVariable;
                term->src.name = NULL;
            }

            const char *var_name = strrchr(term->first.name, '.');
            if (var_name) {
                var_name ++;
            } else {
                var_name = term->first.name;
            }

            term->second.name = var_name;
            term->second.id |= EcsIsVariable;
        }
    }

    /* Post-parse consistency checks */

    /* If next token is OR, term is part of an OR expression */
    if (!ecs_os_strncmp(ptr, TOK_OR, 2)) {
        /* An OR operator must always follow an AND or another OR */
        if (term->oper != EcsAnd) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "cannot combine || with other operators");
            goto error;
        }

        term->oper = EcsOr;
    }

    /* Term must either end in end of expression, AND or OR token */
    if (!flecs_is_valid_end_of_term(ptr)) {
        if (!flecs_isident(ptr[0]) || ((ptr != expr) && (ptr[-1] != ' '))) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "expected end of expression or next term");
            goto error;
        }
    }

    /* If the term just contained a 0, the expression has nothing. Ensure
     * that after the 0 nothing else follows */
    if (term->first.name && !ecs_os_strcmp(term->first.name, "0")) {
        if (ptr[0]) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unexpected term after 0"); 
            goto error;
        }

        if ((src->id & EcsTermRefFlags) != 0) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid combination of 0 with non-default subject");
            goto error;
        }

        src->id = EcsIsEntity;
        term->first.name = NULL;
    }

    /* Cannot combine EcsIsEntity/0 with operators other than AND */
    if (term->oper != EcsAnd && ecs_term_match_0(term)) {
        if ((ECS_TERM_REF_ID(&term->first) != EcsScopeOpen) && 
            (ECS_TERM_REF_ID(&term->first) != EcsScopeClose)) 
        {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid operator for empty source"); 
            goto error;
        }
    }

    /* Automatically assign This if entity is not assigned and the set is
     * nothing */
    if (!(src->id & EcsIsEntity)) {
        if (!src->name) {
            if (!src->id) {
                src->id = EcsThis | (src->id & EcsTermRefFlags);
                src->id |= EcsIsVariable;
            }
        }
    }

    if (src->name && !ecs_os_strcmp(src->name, "0")) {
        src->id = EcsIsEntity;
    }

    /* Process role */
    if (term->id == ECS_AND) {
        term->oper = EcsAndFrom;
        term->id = 0;
    } else if (term->id == ECS_OR) {
        term->oper = EcsOrFrom;
        term->id = 0;
    } else if (term->id == ECS_NOT) {
        term->oper = EcsNotFrom;
        term->id = 0;
    }

    if (allow_newline) {
        ptr = ecs_parse_ws_eol(ptr);
    } else {
        ptr = ecs_parse_ws(ptr);
    }

    return ECS_CONST_CAST(char*, ptr);
error:
    return NULL;
}

const char* ecs_parse_ws_eol(
    const char *ptr)
{
    while (isspace(*ptr)) {
        ptr ++;
    }

    return ptr;
}

const char* ecs_parse_ws(
    const char *ptr)
{
    while ((*ptr != '\n') && isspace(*ptr)) {
        ptr ++;
    }

    return ptr;
}

const char* ecs_parse_digit(
    const char *ptr,
    char *token)
{
    char *tptr = token;
    char ch = ptr[0];

    if (!isdigit(ch) && ch != '-') {
        ecs_parser_error(NULL, NULL, 0, "invalid start of number '%s'", ptr);
        return NULL;
    }

    tptr[0] = ch;
    tptr ++;
    ptr ++;

    for (; (ch = *ptr); ptr ++) {
        if (!isdigit(ch) && (ch != '.') && (ch != 'e')) {
            break;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';
    
    return ptr;
}