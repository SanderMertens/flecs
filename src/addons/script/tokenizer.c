/**
 * @file addons/plecs.c
 * @brief Plecs addon.
 */

#include "flecs.h"
#include "script.h"

#define Keyword(keyword, _kind)\
    } else if (!ecs_os_strncmp(ptr, keyword " ", ecs_os_strlen(keyword) + 1)) {\
        out->value = keyword;\
        out->kind = _kind;\
        return ptr + ecs_os_strlen(keyword);

#define Operator(oper, _kind)\
    } else if (ptr[0] == oper[0]) {\
        out->value = oper;\
        out->kind = _kind;\
        return ptr + 1;

const char* flecs_script_token_kind_str(
    ecs_script_token_kind_t kind)
{
    switch(kind) {
    case EcsTokUnknown:
        return "unknown token ";
    case EcsTokColon:
    case EcsTokScopeOpen:
    case EcsTokScopeClose:
    case EcsTokParenOpen:
    case EcsTokParenClose:
    case EcsTokVariable:
    case EcsTokAnnotation:
    case EcsTokComma:
    case EcsTokAssign:
        return "";
    case EcsTokKeywordWith:
    case EcsTokKeywordUsing:
    case EcsTokKeywordAssembly:
    case EcsTokKeywordProp:
    case EcsTokKeywordConst:
    case EcsTokKeywordIf:
        return "keyword ";
    case EcsTokIdentifier:
        return "identifier ";
    case EcsTokNewline:
        return "newline";
    case EcsTokEnd:
        return "end of script";
    default:
        return "<corrupt>";
    }
}

static
const char* flecs_script_scan_token(
    ecs_script_parser_t *parser,
    const char *ptr,
    bool (*scanfunc)(char))
{
    char *out = parser->token_cur;
    do {
        if (!*ptr || !scanfunc(*ptr)) {
            *out = '\0';
            parser->token_cur = out + 1;
            return ptr;
        }

        *out = *ptr;
        out ++;
        ptr ++;
    } while (true);
}

static
const char* flecs_scan_whitespace(
    ecs_script_parser_t *parser,
    const char *ptr) 
{
    (void)parser;

    while (ptr[0] && isspace(ptr[0]) && ptr[0] != '\n') {
        ptr ++;
    }

    return ptr;
}

// Identifier token
static
bool flecs_script_is_identifier(
    char c)
{
    return isalpha(c) || (c == '_');
}

static
bool flecs_script_scan_identifier(
    char c)
{
    return flecs_script_is_identifier(c) || isdigit(c) || (c == '.') || (c == '*');
}

static
const char* flecs_script_identifier(
    ecs_script_parser_t *parser,
    const char *ptr,
    ecs_script_token_t *out) 
{
    out->kind = EcsTokIdentifier;
    out->value = parser->token_cur;
    return flecs_script_scan_token(parser, ptr, 
        flecs_script_scan_identifier);
}

static
const char* flecs_script_skip_string(
    ecs_script_parser_t *parser,
    const char *ptr, 
    char delim)
{
    char ch;
    for (; (ch = ptr[0]) && ptr[0] != delim; ptr ++) {
        if (ch == '\\') {
            ptr ++;
        }
    }

    if (!ptr[0]) {
        ecs_parser_error(parser->script->name, parser->script->code,
            ptr - parser->script->code, "unterminated string");
        return NULL;
    }

    return ptr;
}

const char* flecs_script_expr(
    ecs_script_parser_t *parser,
    const char *ptr,
    ecs_script_token_t *out,
    char until)
{
    int32_t scope_depth = until == '}' ? 1 : 0;
    int32_t paren_depth = until == ')' ? 1 : 0;
    
    const char *start = ptr = flecs_scan_whitespace(parser, ptr);
    char ch;

    for (; (ch = ptr[0]); ptr ++) {
        if (ch == '{') {
            if (ch == until) {
                break;
            }
            scope_depth ++;
        } else 
        if (ch == '}') {
            scope_depth --;
            if (!scope_depth && until == '}') {
                break;
            }
            if (scope_depth < 0) {
                ecs_parser_error(parser->script->name, parser->script->code,
                    ptr - parser->script->code, "mismatching { }");
                return NULL;
            }
        } else
        if (ch == '(') {
            paren_depth ++;
        } else
        if (ch == ')') {
            paren_depth --;
            if (!paren_depth && until == ')') {
                break;
            }
            if (paren_depth < 0) {
                ecs_parser_error(parser->script->name, parser->script->code,
                    ptr - parser->script->code, "mismatching ( )");
                return NULL;
            }
        } else
        if (ch == '"') {
            ptr = flecs_script_skip_string(parser, ptr + 1, '"');
            if (!ptr) {
                return NULL;
            }
        } else
        if (ch == '`') {
            ptr = flecs_script_skip_string(parser, ptr + 1, '`');
            if (!ptr) {
                return NULL;
            }
        } else
        if (ch == until) {
            break;
        }
    }

    if (!ptr[0]) {
        if (until == '\0') {
            ecs_parser_error(parser->script->name, parser->script->code,
                ptr - parser->script->code, "expected end of script");
            return NULL;
        } else
        if (until == '\n') {
            ecs_parser_error(parser->script->name, parser->script->code,
                ptr - parser->script->code, "expected newline");
            return NULL;
        } else {
            ecs_parser_error(parser->script->name, parser->script->code,
                ptr - parser->script->code, "expected '%c'", until);
            return NULL;
        }
    }

    if (scope_depth) {
        ecs_parser_error(parser->script->name, parser->script->code,
            ptr - parser->script->code, "mismatching { }");
        return NULL;
    }
    if (paren_depth) {
        ecs_parser_error(parser->script->name, parser->script->code,
            ptr - parser->script->code, "mismatching ( )");
        return NULL;
    }

    int32_t len = ptr - start;
    ecs_os_memcpy(parser->token_cur, start, len);
    out->value = parser->token_cur;
    parser->token_cur += len;
    
    while (isspace(parser->token_cur[-1])) {
        parser->token_cur --;
    }

    parser->token_cur[0] = '\0';
    parser->token_cur ++;

    return ptr;
}

const char* flecs_script_token(
    ecs_script_parser_t *parser,
    const char *ptr,
    ecs_script_token_t *out,
    bool is_lookahead)
{
    // Skip whitespace and comments
    do {
        ptr = flecs_scan_whitespace(parser, ptr);
        if (ptr[0] == '/' && ptr[1] == '/') {
            for (ptr = ptr + 2; ptr[0] && ptr[0] != '\n'; ptr ++) { }
        }
    } while (isspace(ptr[0]) && ptr[0] != '\n');

    out->kind = EcsTokUnknown;
    out->value = NULL;

    if (ptr[0] == '\0') {
        out->kind = EcsTokEnd;
        return ptr;
    } else if (ptr[0] == '\n') {
        out->kind = EcsTokNewline;
        return ptr + 1;

    Operator(":",        EcsTokColon)
    Operator("{",        EcsTokScopeOpen)
    Operator("}",        EcsTokScopeClose)
    Operator("(",        EcsTokParenOpen)
    Operator(")",        EcsTokParenClose)
    Operator("$",        EcsTokVariable)
    Operator("@",        EcsTokAnnotation)
    Operator(",",        EcsTokComma)
    Operator("=",        EcsTokAssign)

    Keyword ("with",     EcsTokKeywordWith)
    Keyword ("using",    EcsTokKeywordUsing)
    Keyword ("assembly", EcsTokKeywordAssembly)
    Keyword ("prop",     EcsTokKeywordProp)
    Keyword ("const",    EcsTokKeywordConst)
    Keyword ("if",       EcsTokKeywordIf)
    Keyword ("else",     EcsTokKeywordElse)

    } else if (flecs_script_is_identifier(ptr[0])) {
        ptr = flecs_script_identifier(parser, ptr, out);
        return ptr;
    }

    if (!is_lookahead) {
        ecs_parser_error(parser->script->name, parser->script->code,
            ptr - parser->script->code, "unknown token '%c'", ptr[0]);
    }

    return NULL;
}
