/**
 * @file addons/parser.c
 * @brief Parser addon.
 */

#include "flecs.h"

#include "../private_api.h"
#include <ctype.h>

#define TOK_VARIABLE '$'

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
