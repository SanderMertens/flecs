/**
 * @file addons/plecs.c
 * @brief Plecs addon.
 */

#include "flecs.h"
#include "script.h"

#define Keyword(keyword, _kind)\
    } else if (!ecs_os_strncmp(pos, keyword " ", ecs_os_strlen(keyword) + 1)) {\
        out->value = keyword;\
        out->kind = _kind;\
        return pos + ecs_os_strlen(keyword);

#define Operator(oper, _kind)\
    } else if (pos[0] == oper[0]) {\
        out->value = oper;\
        out->kind = _kind;\
        return pos + 1;

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
    case EcsTokBracketOpen:
    case EcsTokBracketClose:
    case EcsTokAnnotation:
    case EcsTokComma:
    case EcsTokSemiColon:
    case EcsTokAssign:
        return "";
    case EcsTokKeywordWith:
    case EcsTokKeywordUsing:
    case EcsTokKeywordAssembly:
    case EcsTokKeywordProp:
    case EcsTokKeywordConst:
    case EcsTokKeywordIf:
    case EcsTokKeywordModule:
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
    const char *pos,
    bool (*scanfunc)(char))
{
    char *out = parser->token_cur;
    do {
        if (!*pos || !scanfunc(*pos)) {
            *out = '\0';
            parser->token_cur = out + 1;
            return pos;
        }

        *out = *pos;
        out ++;
        pos ++;
    } while (true);
}

static
const char* flecs_scan_whitespace(
    ecs_script_parser_t *parser,
    const char *pos) 
{
    (void)parser;

    while (pos[0] && isspace(pos[0]) && pos[0] != '\n') {
        pos ++;
    }

    return pos;
}

static
const char* flecs_scan_whitespace_and_comment(
    ecs_script_parser_t *parser,
    const char *pos) 
{
repeat_skip_whitespace_comment:
    pos = flecs_scan_whitespace(parser, pos);
    if (pos[0] == '/' && pos[1] == '/') {
        for (pos = pos + 2; pos[0] && pos[0] != '\n'; pos ++) { }
        if (pos[0] == '\n') {
            pos ++;
            goto repeat_skip_whitespace_comment;
        }
    }

    return pos;
}

// Identifier token
static
bool flecs_script_is_identifier(
    char c)
{
    return isalpha(c) || (c == '_') || (c == '$');
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
    const char *pos,
    ecs_script_token_t *out) 
{
    out->kind = EcsTokIdentifier;
    out->value = parser->token_cur;
    return flecs_script_scan_token(parser, pos, 
        flecs_script_scan_identifier);
}

static
const char* flecs_script_skip_string(
    ecs_script_parser_t *parser,
    const char *pos, 
    char delim)
{
    char ch;
    for (; (ch = pos[0]) && pos[0] != delim; pos ++) {
        if (ch == '\\') {
            pos ++;
        }
    }

    if (!pos[0]) {
        ecs_parser_error(parser->script->name, parser->script->code,
            pos - parser->script->code, "unterminated string");
        return NULL;
    }

    return pos;
}

const char* flecs_script_expr(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_token_t *out,
    char until)
{
    parser->pos = pos;

    int32_t scope_depth = until == '}' ? 1 : 0;
    int32_t paren_depth = until == ')' ? 1 : 0;
    
    const char *start = pos = flecs_scan_whitespace(parser, pos);
    char ch;

    for (; (ch = pos[0]); pos ++) {
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
                    pos - parser->script->code, "mismatching { }");
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
                    pos - parser->script->code, "mismatching ( )");
                return NULL;
            }
        } else
        if (ch == '"') {
            pos = flecs_script_skip_string(parser, pos + 1, '"');
            if (!pos) {
                return NULL;
            }
        } else
        if (ch == '`') {
            pos = flecs_script_skip_string(parser, pos + 1, '`');
            if (!pos) {
                return NULL;
            }
        } else
        if (ch == until) {
            break;
        }
    }

    if (!pos[0]) {
        if (until == '\0') {
            ecs_parser_error(parser->script->name, parser->script->code,
                pos - parser->script->code, "expected end of script");
            return NULL;
        } else
        if (until == '\n') {
            ecs_parser_error(parser->script->name, parser->script->code,
                pos - parser->script->code, "expected newline");
            return NULL;
        } else {
            ecs_parser_error(parser->script->name, parser->script->code,
                pos - parser->script->code, "expected '%c'", until);
            return NULL;
        }
    }

    if (scope_depth) {
        ecs_parser_error(parser->script->name, parser->script->code,
            pos - parser->script->code, "mismatching { }");
        return NULL;
    }
    if (paren_depth) {
        ecs_parser_error(parser->script->name, parser->script->code,
            pos - parser->script->code, "mismatching ( )");
        return NULL;
    }

    if (until != ']') {
        parser->token_cur[0] = '{';
    } else {
        parser->token_cur[0] = '[';
    }
    
    int32_t len = pos - start;
    ecs_os_memcpy(parser->token_cur + 1, start, len);
    out->value = parser->token_cur;
    parser->token_cur += len + 1;

    while (isspace(parser->token_cur[-1])) {
        parser->token_cur --;
    }

    if (until != ']') {
        parser->token_cur[0] = '}';
    } else {
        parser->token_cur[0] = ']';
    }

    parser->token_cur ++;

    parser->token_cur[0] = '\0';
    parser->token_cur ++;

    return pos;
}

const char* flecs_script_until(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_token_t *out,
    char until)
{
    parser->pos = pos;
    
    const char *start = pos = flecs_scan_whitespace(parser, pos);
    char ch;

    for (; (ch = pos[0]); pos ++) {
        if (ch == until) {
            break;
        }
    }

    if (!pos[0]) {
        if (until == '\0') {
            ecs_parser_error(parser->script->name, parser->script->code,
                pos - parser->script->code, "expected end of script");
            return NULL;
        } else
        if (until == '\n') {
            ecs_parser_error(parser->script->name, parser->script->code,
                pos - parser->script->code, "expected newline");
            return NULL;
        } else {
            ecs_parser_error(parser->script->name, parser->script->code,
                pos - parser->script->code, "expected '%c'", until);
            return NULL;
        }
    }

    int32_t len = pos - start;
    ecs_os_memcpy(parser->token_cur, start, len);
    out->value = parser->token_cur;
    parser->token_cur += len;

    while (isspace(parser->token_cur[-1])) {
        parser->token_cur --;
    }

    parser->token_cur[0] = '\0';
    parser->token_cur ++;

    return pos;
}

const char* flecs_script_token(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_token_t *out,
    bool is_lookahead)
{
    parser->pos = pos;

    // Skip whitespace and comments
    pos = flecs_scan_whitespace_and_comment(parser, pos);

    out->kind = EcsTokUnknown;
    out->value = NULL;

    if (pos[0] == '\0') {
        out->kind = EcsTokEnd;
        return pos;
    } else if (pos[0] == '\n') {
        out->kind = EcsTokNewline;
        
        // Parse multiple newlines/whitespaces as a single token
        pos = flecs_scan_whitespace_and_comment(parser, pos + 1);
        if (pos[0] == '\n') {
            pos ++;
        }
        return pos;

    Operator(":",        EcsTokColon)
    Operator("{",        EcsTokScopeOpen)
    Operator("}",        EcsTokScopeClose)
    Operator("(",        EcsTokParenOpen)
    Operator(")",        EcsTokParenClose)
    Operator("[",        EcsTokBracketOpen)
    Operator("]",        EcsTokBracketClose)
    Operator("@",        EcsTokAnnotation)
    Operator(",",        EcsTokComma)
    Operator(";",        EcsTokSemiColon)
    Operator("=",        EcsTokAssign)

    Keyword ("with",     EcsTokKeywordWith)
    Keyword ("using",    EcsTokKeywordUsing)
    Keyword ("assembly", EcsTokKeywordAssembly)
    Keyword ("prop",     EcsTokKeywordProp)
    Keyword ("const",    EcsTokKeywordConst)
    Keyword ("if",       EcsTokKeywordIf)
    Keyword ("else",     EcsTokKeywordElse)
    Keyword ("module",   EcsTokKeywordModule)

    } else if (flecs_script_is_identifier(pos[0])) {
        pos = flecs_script_identifier(parser, pos, out);
        return pos;
    }

    if (!is_lookahead) {
        ecs_parser_error(parser->script->name, parser->script->code,
            pos - parser->script->code, "unknown token '%c'", pos[0]);
    }

    return NULL;
}
