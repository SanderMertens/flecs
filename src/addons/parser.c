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
#define TOK_NAME_SEP '.'
#define TOK_BRACKET_OPEN '['
#define TOK_BRACKET_CLOSE ']'
#define TOK_WILDCARD '*'
#define TOK_VARIABLE '$'
#define TOK_PAREN_OPEN '('
#define TOK_PAREN_CLOSE ')'

#define TOK_SELF "self"
#define TOK_SUPERSET "super"
#define TOK_SUBSET "sub"
#define TOK_CASCADE "cascade"
#define TOK_PARENT "parent"
#define TOK_ALL "all"

#define TOK_OVERRIDE "OVERRIDE"

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
#define TOK_INOUT_FILTER "filter"

#define ECS_MAX_TOKEN_SIZE (256)

typedef char ecs_token_t[ECS_MAX_TOKEN_SIZE];

const char* ecs_parse_eol_and_whitespace(
    const char *ptr)
{
    while (isspace(*ptr)) {
        ptr ++;
    }

    return ptr;    
}

/** Skip spaces when parsing signature */
const char* ecs_parse_whitespace(
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
        if (!isdigit(ch)) {
            break;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';
    
    return ptr;
}

static
bool is_newline_comment(
    const char *ptr)
{
    if (ptr[0] == '/' && ptr[1] == '/') {
        return true;
    }
    return false;
}

const char* ecs_parse_fluff(
    const char *ptr,
    char **last_comment)
{
    const char *last_comment_start = NULL;

    do {
        /* Skip whitespaces before checking for a comment */
        ptr = ecs_parse_whitespace(ptr);

        /* Newline comment, skip until newline character */
        if (is_newline_comment(ptr)) {
            ptr += 2;
            last_comment_start = ptr;

            while (ptr[0] && ptr[0] != TOK_NEWLINE) {
                ptr ++;
            }
        }

        /* If a newline character is found, skip it */
        if (ptr[0] == TOK_NEWLINE) {
            ptr ++;
        }

    } while (isspace(ptr[0]) || is_newline_comment(ptr));

    if (last_comment) {
        *last_comment = (char*)last_comment_start;
    }

    return ptr;
}

/* -- Private functions -- */

static
bool valid_identifier_start_char(
    char ch)
{
    if (ch && (isalpha(ch) || (ch == '.') || (ch == '_') || (ch == '*') ||
        (ch == '0') || (ch == TOK_VARIABLE) || isdigit(ch))) 
    {
        return true;
    }

    return false;
}

static
bool valid_token_start_char(
    char ch)
{
    if ((ch == '"') || (ch == '{') || (ch == '}') || (ch == ',') || (ch == '-')
        || (ch == '[') || (ch == ']') || valid_identifier_start_char(ch))
    {
        return true;
    }

    return false;
}

static
bool valid_token_char(
    char ch)
{
    if (ch && 
        (isalpha(ch) || isdigit(ch) || ch == '_' || ch == '.' || ch == '"')) 
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
const char* parse_digit(
    const char *ptr,
    char *token_out)
{
    ptr = ecs_parse_whitespace(ptr);
    ptr = ecs_parse_digit(ptr, token_out);
    return ecs_parse_whitespace(ptr);
}

const char* ecs_parse_token(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token_out)
{
    int64_t column = ptr - expr;

    ptr = ecs_parse_whitespace(ptr);
    char *tptr = token_out, ch = ptr[0];

    if (!valid_token_start_char(ch)) {
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

    if (ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ',') {
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
        if (!valid_token_char(ch) && !in_str) {
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

    const char *next_ptr = ecs_parse_whitespace(ptr);
    if (next_ptr[0] == ':' && next_ptr != ptr) {
        /* Whitespace between token and : is significant */
        ptr = next_ptr - 1;
    } else {
        ptr = next_ptr;
    }

    return ptr;
}

static
const char* ecs_parse_identifier(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token_out)
{
    if (!valid_identifier_start_char(ptr[0])) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected start of identifier");
        return NULL;
    }

    ptr = ecs_parse_token(name, expr, ptr, token_out);

    return ptr;
}

static
int parse_identifier(
    const char *token,
    ecs_term_id_t *out)
{
    const char *tptr = token;
    if (tptr[0] == TOK_VARIABLE && tptr[1]) {
        out->var = EcsVarIsVariable;
        tptr ++;
    }

    out->name = ecs_os_strdup(tptr);

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
    } else if (!ecs_os_strcmp(token, TOK_OVERRIDE)) {
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
    } else if (!ecs_os_strcmp(token, TOK_INOUT_FILTER)) {
        *inout_kind_out = EcsInOutFilter;
    }

    ptr = ecs_parse_whitespace(ptr);

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
    } else if (!ecs_os_strcmp(token, TOK_CASCADE)) {
        return EcsCascade;
    } else if (!ecs_os_strcmp(token, TOK_ALL)) {
        return EcsAll;
    } else if (!ecs_os_strcmp(token, TOK_PARENT)) {
        return EcsParent;
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
                "cannot mix super and sub", token);
            return NULL;            
        }
        
        id->set.mask |= tok;

        if (ptr[0] == TOK_PAREN_OPEN) {
            ptr ++;

            /* Relationship (overrides IsA default) */
            if (!isdigit(ptr[0]) && valid_token_start_char(ptr[0])) {
                ptr = ecs_parse_identifier(name, expr, ptr, token);
                if (!ptr) {
                    return NULL;
                }         

                id->set.relation = ecs_lookup_fullpath(world, token);
                if (!id->set.relation) {
                    ecs_parser_error(name, expr, column, 
                        "unresolved identifier '%s'", token);
                    return NULL;
                }

                if (ptr[0] == TOK_AND) {
                    ptr = ecs_parse_whitespace(ptr + 1);
                } else if (ptr[0] != TOK_PAREN_CLOSE) {
                    ecs_parser_error(name, expr, column, 
                        "expected ',' or ')'");
                    return NULL;
                }
            }

            /* Max depth of search */
            if (isdigit(ptr[0])) {
                ptr = parse_digit(ptr, token);
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
                    ptr = ecs_parse_whitespace(ptr + 1);
                }
            }

            /* If another digit is found, previous depth was min depth */
            if (isdigit(ptr[0])) {
                ptr = parse_digit(ptr, token);
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
                ecs_parser_error(name, expr, column, "expected ')', got '%c'",
                    ptr[0]);
                return NULL;                
            } else {
                ptr = ecs_parse_whitespace(ptr + 1);
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
            if (valid_token_start_char(ptr[0])) {
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

            ptr = ecs_parse_identifier(name, expr, ptr, token);
            if (!ptr) {
                return NULL;
            }

            ecs_term_id_t *term_id = NULL;

            if (arg == 0) {
                term_id = &term->subj;
            } else if (arg == 1) {
                term_id = &term->obj;
            }

            /* If token is a colon, the token is an identifier followed by a
             * set expression. */
            if (ptr[0] == TOK_COLON) {
                if (parse_identifier(token, term_id)) {
                    ecs_parser_error(name, expr, (ptr - expr), 
                        "invalid identifier '%s'", token);
                    return NULL;
                }

                ptr = ecs_parse_whitespace(ptr + 1);
                ptr = parse_set_expr(world, name, expr, (ptr - expr), ptr,
                    NULL, term_id, TOK_PAREN_CLOSE);
                if (!ptr) {
                    return NULL;
                }

            /* If token is a self, super or sub token, this is a set
             * expression */
            } else if (!ecs_os_strcmp(token, TOK_ALL) ||
                !ecs_os_strcmp(token, TOK_CASCADE) ||
                !ecs_os_strcmp(token, TOK_SELF) || 
                !ecs_os_strcmp(token, TOK_SUPERSET) || 
                !ecs_os_strcmp(token, TOK_SUBSET) ||
                !(ecs_os_strcmp(token, TOK_PARENT)))
            {
                ptr = parse_set_expr(world, name, expr, (ptr - expr), ptr, 
                    token, term_id, TOK_PAREN_CLOSE);
                if (!ptr) {
                    return NULL;
                }

            /* Regular identifier */
            } else if (parse_identifier(token, term_id)) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid identifier '%s'", token);
                return NULL;
            }

            if (ptr[0] == TOK_AND) {
                ptr = ecs_parse_whitespace(ptr + 1);

                term->role = ECS_PAIR;

            } else if (ptr[0] == TOK_PAREN_CLOSE) {
                ptr = ecs_parse_whitespace(ptr + 1);
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
void parser_unexpected_char(
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
const char* parse_term(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_term_t *term_out)
{
    const char *ptr = expr;
    char token[ECS_MAX_TOKEN_SIZE] = {0};
    ecs_term_t term = { .move = true /* parser never owns resources */ };

    ptr = ecs_parse_whitespace(ptr);

    /* Inout specifiers always come first */
    if (ptr[0] == TOK_BRACKET_OPEN) {
        ptr = parse_annotation(name, expr, (ptr - expr), ptr + 1, &term.inout);
        if (!ptr) {
            goto error;
        }
        ptr = ecs_parse_whitespace(ptr);
    }

    if (valid_operator_char(ptr[0])) {
        term.oper = parse_operator(ptr[0]);
        ptr = ecs_parse_whitespace(ptr + 1);
    }

    /* If next token is the start of an identifier, it could be either a type
     * role, source or component identifier */
    if (valid_token_start_char(ptr[0])) {
        ptr = ecs_parse_identifier(name, expr, ptr, token);
        if (!ptr) {
            goto error;
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

    /* Pair with implicit subject */
    } else if (ptr[0] == TOK_PAREN_OPEN) {
        goto parse_pair;

    /* Nothing else expected here */
    } else {
        parser_unexpected_char(name, expr, ptr, ptr[0]);
        goto error;
    }

parse_role:
    term.role = parse_role(name, expr, (ptr - expr), token);
    if (!term.role) {
        goto error;
    }

    ptr = ecs_parse_whitespace(ptr);

    /* If next token is the source token, this is an empty source */
    if (valid_token_start_char(ptr[0])) {
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
    if (parse_identifier(token, &term.pred)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        goto error;
    }

    /* Set expression */
    if (ptr[0] == TOK_COLON) {
        ptr = ecs_parse_whitespace(ptr + 1);
        ptr = parse_set_expr(world, name, expr, (ptr - expr), ptr, NULL, 
            &term.pred, TOK_COLON);
        if (!ptr) {
            goto error;
        }

        ptr = ecs_parse_whitespace(ptr);

        if (ptr[0] == TOK_AND || !ptr[0]) {
            goto parse_done;
        }

        if (ptr[0] != TOK_COLON) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unexpected token '%c' after predicate set expression", ptr[0]);
            goto error;
        }

        ptr = ecs_parse_whitespace(ptr + 1);
    } else {
        ptr = ecs_parse_whitespace(ptr);
    }
    
    if (ptr[0] == TOK_PAREN_OPEN) {
        ptr ++;
        if (ptr[0] == TOK_PAREN_CLOSE) {
            term.subj.set.mask = EcsNothing;
            ptr ++;
            ptr = ecs_parse_whitespace(ptr);
        } else {
            ptr = parse_arguments(
                world, name, expr, (ptr - expr), ptr, token, &term);
        }

        goto parse_done;
    }

    goto parse_done;

parse_pair:
    ptr = ecs_parse_identifier(name, expr, ptr + 1, token);
    if (!ptr) {
        goto error;
    }

    if (ptr[0] == TOK_AND) {
        ptr ++;
        term.subj.entity = EcsThis;
        goto parse_pair_predicate;
    } else if (ptr[0] == TOK_PAREN_CLOSE) {
        term.subj.entity = EcsThis;
        goto parse_pair_predicate;
    } else {
        parser_unexpected_char(name, expr, ptr, ptr[0]);
        goto error;
    }

parse_pair_predicate:
    if (parse_identifier(token, &term.pred)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        goto error;
    }

    ptr = ecs_parse_whitespace(ptr);
    if (valid_token_start_char(ptr[0])) {
        ptr = ecs_parse_identifier(name, expr, ptr, token);
        if (!ptr) {
            goto error;
        }

        if (ptr[0] == TOK_PAREN_CLOSE) {
            ptr ++;
            goto parse_pair_object;
        } else {
            parser_unexpected_char(name, expr, ptr, ptr[0]);
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
    if (parse_identifier(token, &term.obj)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        goto error;
    }

    if (term.role != 0) {
        if (term.role != ECS_PAIR) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid combination of role '%s' with pair", 
                    ecs_role_str(term.role));
            goto error;
        }
    } else {
        term.role = ECS_PAIR;
    }

    ptr = ecs_parse_whitespace(ptr);
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
bool is_valid_end_of_term(
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

    ecs_term_id_t *subj = &term->subj;

    bool prev_or = false;
    if (ptr != expr) {
        if (ptr[0]) {
            if (ptr[0] == ',') {
                ptr ++;
            } else if (ptr[0] == '|') {
                ptr += 2;
                prev_or = true;
            } else {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid preceding token");
            }
        }
    }
    
    ptr = ecs_parse_eol_and_whitespace(ptr);
    if (!ptr[0]) {
        *term = (ecs_term_t){0};
        return (char*)ptr;
    }

    if (ptr == expr && !strcmp(expr, "0")) {
        return (char*)&ptr[1];
    }

    int32_t prev_set = subj->set.mask;

    /* Parse next element */
    ptr = parse_term(world, name, ptr, term);
    if (!ptr) {
        goto error;
    }

    /* Check for $() notation */
    if (!ecs_os_strcmp(term->pred.name, "$")) {
        if (term->subj.name) {
            ecs_os_free(term->pred.name);
            
            term->pred = term->subj;

            if (term->obj.name) {
                term->subj = term->obj;       
            } else {
                term->subj.entity = EcsThis;
                term->subj.name = NULL;
                term->subj.var = EcsVarIsVariable;
            }

            term->obj.name = ecs_os_strdup(term->pred.name);
            term->obj.var = EcsVarIsVariable;
        }
    }

    /* Post-parse consistency checks */

    /* If next token is OR, term is part of an OR expression */
    if (!ecs_os_strncmp(ptr, TOK_OR, 2) || prev_or) {
        /* An OR operator must always follow an AND or another OR */
        if (term->oper != EcsAnd) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "cannot combine || with other operators");
            goto error;
        }

        term->oper = EcsOr;
    }

    /* Term must either end in end of expression, AND or OR token */
    if (!is_valid_end_of_term(ptr)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected end of expression or next term");
        goto error;
    }

    /* If the term just contained a 0, the expression has nothing. Ensure
     * that after the 0 nothing else follows */
    if (!ecs_os_strcmp(term->pred.name, "0")) {
        if (ptr[0]) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unexpected term after 0"); 
            goto error;
        }

        if (subj->set.mask != EcsDefaultSet || 
           (subj->entity && subj->entity != EcsThis) ||
           (subj->name && ecs_os_strcmp(subj->name, "This")))
        {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid combination of 0 with non-default subject");
            goto error;
        }

        subj->set.mask = EcsNothing;
        ecs_os_free(term->pred.name);
        term->pred.name = NULL;
    }

    /* Cannot combine EcsNothing with operators other than AND */
    if (term->oper != EcsAnd && subj->set.mask == EcsNothing) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid operator for empty source"); 
        goto error;
    }

    /* Verify consistency of OR expression */
    if (prev_or && term->oper == EcsOr) {
        /* Set expressions must be the same for all OR terms */
        if (subj->set.mask != prev_set) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "cannot combine different sources in OR expression");
            goto error;
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

    ptr = ecs_parse_whitespace(ptr);

    return (char*)ptr;
error:
    if (term) {
        ecs_term_fini(term);
    }
    return NULL;
}

#endif
