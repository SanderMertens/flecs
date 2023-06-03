/**
 * @file addons/parser.c
 * @brief Parser addon.
 */

#include "flecs.h"

#ifdef FLECS_PARSER

#include "../private_api.h"
#include <ctype.h>

#define ECS_ANNOTATION_LENGTH_MAX (16)

#define TOK_NEWLINE '\n'
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
#define TOK_WILDCARD '*'
#define TOK_VARIABLE '$'
#define TOK_PAREN_OPEN '('
#define TOK_PAREN_CLOSE ')'
#define TOK_EQ "=="
#define TOK_NEQ "!="
#define TOK_MATCH "~="
#define TOK_EXPR_STRING '"'

#define TOK_SELF "self"
#define TOK_UP "up"
#define TOK_DOWN "down"
#define TOK_CASCADE "cascade"
#define TOK_PARENT "parent"

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
const ecs_id_t ECS_OR =                                            (1ull << 59);

static
const ecs_id_t ECS_NOT =                                           (1ull << 58);

#define ECS_MAX_TOKEN_SIZE (256)

typedef char ecs_token_t[ECS_MAX_TOKEN_SIZE];

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
        } else
        if (!flecs_valid_token_char(ch) && !in_str) {
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
    const char *token,
    ecs_term_id_t *out)
{
    const char *tptr = token;
    if (tptr[0] == TOK_VARIABLE && tptr[1]) {
        out->flags |= EcsIsVariable;
        tptr ++;
    }
    if (tptr[0] == TOK_EXPR_STRING && tptr[1]) {
        out->flags |= EcsIsName;
        tptr ++;
        if (tptr[0] == TOK_NOT) {
            /* Already parsed */
            tptr ++;
        }
    }

    out->name = ecs_os_strdup(tptr);

    ecs_size_t len = ecs_os_strlen(out->name);
    if (out->flags & EcsIsName) {
        if (out->name[len - 1] != TOK_EXPR_STRING) {
            ecs_parser_error(NULL, token, 0, "missing '\"' at end of string");
            return -1;
        } else {
            out->name[len - 1] = '\0';
        }
    }

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
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
    return 0;
}

static
const char* flecs_parse_annotation(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr, 
    ecs_inout_kind_t *inout_kind_out)
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
uint8_t flecs_parse_set_token(
    const char *token)
{
    if (!ecs_os_strcmp(token, TOK_SELF)) {
        return EcsSelf;
    } else if (!ecs_os_strcmp(token, TOK_UP)) {
        return EcsUp;
    } else if (!ecs_os_strcmp(token, TOK_DOWN)) {
        return EcsDown;
    } else if (!ecs_os_strcmp(token, TOK_CASCADE)) {
        return EcsCascade;
    } else if (!ecs_os_strcmp(token, TOK_PARENT)) {
        return EcsParent;
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
    ecs_term_id_t *id,
    char tok_end)
{
    char token_buf[ECS_MAX_TOKEN_SIZE] = {0};
    if (!token) {
        token = token_buf;
        ptr = ecs_parse_identifier(name, expr, ptr, token);
        if (!ptr) {
            return NULL;
        }
    }

    do {
        uint8_t tok = flecs_parse_set_token(token);
        if (!tok) {
            ecs_parser_error(name, expr, column, 
                "invalid set token '%s'", token);
            return NULL;
        }

        if (id->flags & tok) {
            ecs_parser_error(name, expr, column, 
                "duplicate set token '%s'", token);
            return NULL;            
        }
        
        id->flags |= tok;

        if (ptr[0] == TOK_PAREN_OPEN) {
            ptr ++;

            /* Relationship (overrides IsA default) */
            if (!isdigit(ptr[0]) && flecs_valid_token_start_char(ptr[0])) {
                ptr = ecs_parse_identifier(name, expr, ptr, token);
                if (!ptr) {
                    return NULL;
                }         

                id->trav = ecs_lookup_fullpath(world, token);
                if (!id->trav) {
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
    char *token,
    ecs_term_t *term)
{
    (void)column;

    int32_t arg = 0;

    do {
        if (flecs_valid_token_start_char(ptr[0])) {
            if (arg == 2) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "too many arguments in term");
                return NULL;
            }

            ptr = ecs_parse_identifier(name, expr, ptr, token);
            if (!ptr) {
                return NULL;
            }

            ecs_term_id_t *term_id = NULL;

            if (arg == 0) {
                term_id = &term->src;
            } else if (arg == 1) {
                term_id = &term->second;
            }

            /* If token is a colon, the token is an identifier followed by a
             * set expression. */
            if (ptr[0] == TOK_COLON) {
                if (flecs_parse_identifier(token, term_id)) {
                    ecs_parser_error(name, expr, (ptr - expr), 
                        "invalid identifier '%s'", token);
                    return NULL;
                }

                ptr = ecs_parse_ws(ptr + 1);
                ptr = flecs_parse_term_flags(world, name, expr, (ptr - expr), ptr,
                    NULL, term_id, TOK_PAREN_CLOSE);
                if (!ptr) {
                    return NULL;
                }

            /* Check for term flags */
            } else if (!ecs_os_strcmp(token, TOK_CASCADE) ||
                !ecs_os_strcmp(token, TOK_SELF) || 
                !ecs_os_strcmp(token, TOK_UP) || 
                !ecs_os_strcmp(token, TOK_DOWN) || 
                !(ecs_os_strcmp(token, TOK_PARENT)))
            {
                ptr = flecs_parse_term_flags(world, name, expr, (ptr - expr), ptr, 
                    token, term_id, TOK_PAREN_CLOSE);
                if (!ptr) {
                    return NULL;
                }

            /* Regular identifier */
            } else if (flecs_parse_identifier(token, term_id)) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid identifier '%s'", token);
                return NULL;
            }

            if (ptr[0] == TOK_AND) {
                ptr = ecs_parse_ws(ptr + 1);

                term->id_flags = ECS_PAIR;

            } else if (ptr[0] == TOK_PAREN_CLOSE) {
                ptr = ecs_parse_ws(ptr + 1);
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
    const char *name,
    const char *expr,
    ecs_term_t *term_out)
{
    const char *ptr = expr;
    char token[ECS_MAX_TOKEN_SIZE] = {0};
    ecs_term_t term = { .move = true /* parser never owns resources */ };

    ptr = ecs_parse_ws(ptr);

    /* Inout specifiers always come first */
    if (ptr[0] == TOK_BRACKET_OPEN) {
        ptr = flecs_parse_annotation(name, expr, (ptr - expr), ptr + 1, &term.inout);
        if (!ptr) {
            goto error;
        }
        ptr = ecs_parse_ws(ptr);
    }

    if (flecs_valid_operator_char(ptr[0])) {
        term.oper = flecs_parse_operator(ptr[0]);
        ptr = ecs_parse_ws(ptr + 1);
    }

    /* If next token is the start of an identifier, it could be either a type
     * role, source or component identifier */
    if (flecs_valid_identifier_start_char(ptr[0])) {
        ptr = ecs_parse_identifier(name, expr, ptr, token);
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
        term.src.id = 0;
        term.src.flags = EcsIsEntity;
        term.inout = EcsInOutNone;
        goto parse_done;

    /* Close query scope */
    } else if (ptr[0] == TOK_SCOPE_CLOSE) {
        term.first.id = EcsScopeClose;
        term.src.id = 0;
        term.src.flags = EcsIsEntity;
        term.inout = EcsInOutNone;
        ptr = ecs_parse_ws(ptr + 1);
        goto parse_done;

    /* Nothing else expected here */
    } else {
        flecs_parser_unexpected_char(name, expr, ptr, ptr[0]);
        goto error;
    }

flecs_parse_role:
    term.id_flags = flecs_parse_role(name, expr, (ptr - expr), token);
    if (!term.id_flags) {
        goto error;
    }

    ptr = ecs_parse_ws(ptr);

    /* If next token is the source token, this is an empty source */
    if (flecs_valid_token_start_char(ptr[0])) {
        ptr = ecs_parse_identifier(name, expr, ptr, token);
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
    if (flecs_parse_identifier(token, &term.first)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        goto error;
    }

    /* Set expression */
    if (ptr[0] == TOK_COLON) {
        ptr = ecs_parse_ws(ptr + 1);
        ptr = flecs_parse_term_flags(world, name, expr, (ptr - expr), ptr, NULL, 
            &term.first, TOK_COLON);
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
        ptr ++;
        if (ptr[0] == TOK_PAREN_CLOSE) {
            term.src.flags = EcsIsEntity;
            term.src.id = 0;
            ptr ++;
            ptr = ecs_parse_ws(ptr);
        } else {
            ptr = flecs_parse_arguments(
                world, name, expr, (ptr - expr), ptr, token, &term);
        }

        goto parse_done;
    }

    goto parse_done;

parse_eq:
    term.src = term.first;
    term.first = (ecs_term_id_t){0};
    term.first.id = EcsPredEq;
    goto parse_right_operand;

parse_neq:
    term.src = term.first;
    term.first = (ecs_term_id_t){0};
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
    term.first = (ecs_term_id_t){0};
    term.first.id = EcsPredMatch;
    goto parse_right_operand;

parse_right_operand:
    if (flecs_valid_token_start_char(ptr[0])) {
        ptr = ecs_parse_identifier(name, expr, ptr, token);
        if (!ptr) {
            goto error;
        }

        if (term.first.id == EcsPredMatch) {
            if (token[0] == '"' && token[1] == '!') {
                term.oper = EcsNot;
            }
        }

        if (flecs_parse_identifier(token, &term.second)) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid identifier '%s'", token); 
            goto error;
        }

        term.src.flags &= ~EcsTraverseFlags;
        term.src.flags |= EcsSelf;
        term.inout = EcsInOutNone;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected identifier");
        goto error;
    }
    goto parse_done;
parse_pair:
    ptr = ecs_parse_identifier(name, expr, ptr + 1, token);
    if (!ptr) {
        goto error;
    }

    if (ptr[0] == TOK_COLON) {
        ptr = ecs_parse_ws(ptr + 1);
        ptr = flecs_parse_term_flags(world, name, expr, (ptr - expr), ptr,
            NULL, &term.first, TOK_PAREN_CLOSE);
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
        term.src.flags |= EcsIsVariable;
        goto parse_pair_predicate;
    } else if (ptr[0] == TOK_PAREN_CLOSE) {
        term.src.id = EcsThis;
        term.src.flags |= EcsIsVariable;
        goto parse_pair_predicate;
    } else {
        flecs_parser_unexpected_char(name, expr, ptr, ptr[0]);
        goto error;
    }

parse_pair_predicate:
    if (flecs_parse_identifier(token, &term.first)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        goto error;
    }

    ptr = ecs_parse_ws(ptr);
    if (flecs_valid_token_start_char(ptr[0])) {
        ptr = ecs_parse_identifier(name, expr, ptr, token);
        if (!ptr) {
            goto error;
        }

        if (ptr[0] == TOK_COLON) {
            ptr = ecs_parse_ws(ptr + 1);
            ptr = flecs_parse_term_flags(world, name, expr, (ptr - expr), ptr,
                NULL, &term.second, TOK_PAREN_CLOSE);
            if (!ptr) {
                goto error;
            }
        }

        if (ptr[0] == TOK_PAREN_CLOSE) {
            ptr ++;
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
    if (flecs_parse_identifier(token, &term.second)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        goto error;
    }

    if (term.id_flags != 0) {
        if (!ECS_HAS_ID_FLAG(term.id_flags, PAIR)) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid combination of role '%s' with pair", 
                    ecs_id_flag_str(term.id_flags));
            goto error;
        }
    } else {
        term.id_flags = ECS_PAIR;
    }

    ptr = ecs_parse_ws(ptr);
    goto parse_done;

parse_done:
    *term_out = term;
    return ptr;

error:
    ecs_term_fini(&term);
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
        (ptr[0] == ':') ||        /* inheritance (in plecs) */
        (ptr[0] == '='))          /* assignment (in plecs) */
    {
        return true;
    }
    return false;
}

char* ecs_parse_term(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_term_t *term)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(term != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_term_id_t *src = &term->src;

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
    }
    
    ptr = ecs_parse_ws_eol(ptr);
    if (!ptr[0]) {
        *term = (ecs_term_t){0};
        return (char*)ptr;
    }

    if (ptr == expr && !strcmp(expr, "0")) {
        return (char*)&ptr[1];
    }

    /* Parse next element */
    ptr = flecs_parse_term(world, name, ptr, term);
    if (!ptr) {
        goto error;
    }

    /* Check for $() notation */
    if (term->first.name && !ecs_os_strcmp(term->first.name, "$")) {
        if (term->src.name) {
            ecs_os_free(term->first.name);
            
            term->first = term->src;

            if (term->second.name) {
                term->src = term->second;       
            } else {
                term->src.id = EcsThis;
                term->src.name = NULL;
                term->src.flags |= EcsIsVariable;
            }

            term->second.name = ecs_os_strdup(term->first.name);
            term->second.flags |= EcsIsVariable;
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

        if (src->flags != 0) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid combination of 0 with non-default subject");
            goto error;
        }

        src->flags = EcsIsEntity;
        src->id = 0;
        ecs_os_free(term->first.name);
        term->first.name = NULL;
    }

    /* Cannot combine EcsIsEntity/0 with operators other than AND */
    if (term->oper != EcsAnd && ecs_term_match_0(term)) {
        if (term->first.id != EcsScopeOpen && term->first.id != EcsScopeClose) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid operator for empty source"); 
            goto error;
        }
    }

    /* Automatically assign This if entity is not assigned and the set is
     * nothing */
    if (!(src->flags & EcsIsEntity)) {
        if (!src->name) {
            if (!src->id) {
                src->id = EcsThis;
                src->flags |= EcsIsVariable;
            }
        }
    }

    if (src->name && !ecs_os_strcmp(src->name, "0")) {
        src->id = 0;
        src->flags = EcsIsEntity;
    }

    /* Process role */
    if (term->id_flags == ECS_AND) {
        term->oper = EcsAndFrom;
        term->id_flags = 0;
    } else if (term->id_flags == ECS_OR) {
        term->oper = EcsOrFrom;
        term->id_flags = 0;
    } else if (term->id_flags == ECS_NOT) {
        term->oper = EcsNotFrom;
        term->id_flags = 0;
    }

    ptr = ecs_parse_ws(ptr);

    return (char*)ptr;
error:
    if (term) {
        ecs_term_fini(term);
    }
    return NULL;
}

#endif
