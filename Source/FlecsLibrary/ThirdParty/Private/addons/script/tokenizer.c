/**
 * @file addons/script/tokenizer.c
 * @brief Script tokenizer.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

#define Keyword(keyword, _kind)\
    } else if (!ecs_os_strncmp(pos, keyword " ", ecs_os_strlen(keyword) + 1)) {\
        out->value = keyword;\
        out->kind = _kind;\
        return pos + ecs_os_strlen(keyword);

#define OperatorMultiChar(oper, _kind)\
    } else if (!ecs_os_strncmp(pos, oper, ecs_os_strlen(oper))) {\
        out->value = oper;\
        out->kind = _kind;\
        return pos + ecs_os_strlen(oper);

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
    case EcsTokMul:
    case EcsTokAssign:
    case EcsTokBitwiseOr:
    case EcsTokNot:
    case EcsTokOptional:
    case EcsTokEq:
    case EcsTokNeq:
    case EcsTokMatch:
    case EcsTokOr:
        return "";
    case EcsTokKeywordWith:
    case EcsTokKeywordUsing:
    case EcsTokKeywordTemplate:
    case EcsTokKeywordProp:
    case EcsTokKeywordConst:
    case EcsTokKeywordIf:
    case EcsTokKeywordElse:
    case EcsTokKeywordModule:
        return "keyword ";
    case EcsTokIdentifier:
        return "identifier ";
    case EcsTokString:
        return "string ";
    case EcsTokNumber:
        return "number ";
    case EcsTokNewline:
        return "newline";
    case EcsTokEnd:
        return "end of script";
    default:
        return "<corrupt>";
    }
}

const char* flecs_scan_whitespace(
    ecs_script_parser_t *parser,
    const char *pos) 
{
    (void)parser;

    if (parser->significant_newline) {
        while (pos[0] && isspace(pos[0]) && pos[0] != '\n') {
            pos ++;
        }
    } else {
        while (pos[0] && isspace(pos[0])) {
            pos ++;
        }
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
    if (pos[0] == '/') {
        if (pos[1] == '/') {
            for (pos = pos + 2; pos[0] && pos[0] != '\n'; pos ++) { }
            if (pos[0] == '\n') {
                pos ++;
                goto repeat_skip_whitespace_comment;
            }
        } else if (pos[1] == '*') {
            for (pos = &pos[2]; pos[0] != 0; pos ++) {
                if (pos[0] == '*' && pos[1] == '/') {
                    pos += 2;
                    goto repeat_skip_whitespace_comment;
                }
            }

            ecs_parser_error(parser->script->pub.name, parser->script->pub.code, 
                pos - parser->script->pub.code, "missing */ for multiline comment");
        }
    }

    return pos;
}

// Identifier token
static
bool flecs_script_is_identifier(
    char c)
{
    return isalpha(c) || (c == '_') || (c == '$') || (c == '#');
}

static
const char* flecs_script_identifier(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_token_t *out) 
{
    out->kind = EcsTokIdentifier;
    out->value = parser->token_cur;

    ecs_assert(flecs_script_is_identifier(pos[0]), ECS_INTERNAL_ERROR, NULL);
    char *outpos = parser->token_cur;
    do {
        char c = pos[0];
        bool is_ident = flecs_script_is_identifier(c) || 
            isdigit(c) || (c == '.') || (c == '*');

        /* Retain \. for name lookup operation */
        if (!is_ident && c == '\\' && pos[1] == '.') {
            is_ident = true;
        }

        if (!is_ident) {
            if (c == '\\') {
                pos ++;
            } else if (c == '<') {
                int32_t indent = 0;
                do {
                    c = *pos;

                    if (c == '<') {
                        indent ++;
                    } else if (c == '>') {
                        indent --;
                    } else if (!c) {
                        ecs_parser_error(parser->script->pub.name, 
                            parser->script->pub.code, pos - parser->script->pub.code, 
                                "< without > in identifier");
                        return NULL;
                    }

                    *outpos = c;
                    outpos ++;
                    pos ++;

                    if (!indent) {
                        break;
                    }
                } while (true);

                *outpos = '\0';
                parser->token_cur = outpos + 1;
                return pos;
            } else if (c == '>') {
                ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
                    pos - parser->script->pub.code, "> without < in identifier");
                return NULL;
            } else {
                *outpos = '\0';
                parser->token_cur = outpos + 1;
                return pos;
            }
        }

        *outpos = *pos;
        outpos ++;
        pos ++;
    } while (true);
}

// Number token static
static
bool flecs_script_is_number(
    char c)
{
    return isdigit(c) || (c == '-');
}

static
const char* flecs_script_number(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_token_t *out) 
{
    out->kind = EcsTokNumber;
    out->value = parser->token_cur;

    ecs_assert(flecs_script_is_number(pos[0]), ECS_INTERNAL_ERROR, NULL);
    char *outpos = parser->token_cur;
    do {
        char c = pos[0];
        if (!isdigit(c)) {
            *outpos = '\0';
            parser->token_cur = outpos + 1;
            break;
        }

        outpos[0] = pos[0];
        outpos ++;
        pos ++;
    } while (true);

    return pos;
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
        ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
            pos - parser->script->pub.code, "unterminated string");
        return NULL;
    }

    return pos;
}

static
const char* flecs_script_string(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_token_t *out) 
{
    const char *end = flecs_script_skip_string(parser, pos + 1, '"');
    if (!end) {
        return NULL;
    }

    ecs_assert(end[0] == '"', ECS_INTERNAL_ERROR, NULL);
    end --;

    int32_t len = flecs_ito(int32_t, end - pos);
    ecs_os_memcpy(parser->token_cur, pos + 1, len);
    parser->token_cur[len] = '\0';

    out->kind = EcsTokString;
    out->value = parser->token_cur;
    parser->token_cur += len + 1;
    return end + 2;
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
                ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
                    pos - parser->script->pub.code, "mismatching { }");
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
                ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
                    pos - parser->script->pub.code, "mismatching ( )");
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
            ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
                pos - parser->script->pub.code, "expected end of script");
            return NULL;
        } else
        if (until == '\n') {
            ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
                pos - parser->script->pub.code, "expected newline");
            return NULL;
        } else {
            ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
                pos - parser->script->pub.code, "expected '%c'", until);
            return NULL;
        }
    }

    if (scope_depth) {
        ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
            pos - parser->script->pub.code, "mismatching { }");
        return NULL;
    }
    if (paren_depth) {
        ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
            pos - parser->script->pub.code, "mismatching ( )");
        return NULL;
    }

    if (until != ']') {
        parser->token_cur[0] = '{';
    } else {
        parser->token_cur[0] = '[';
    }
    
    int32_t len = flecs_ito(int32_t, pos - start);
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
            ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
                pos - parser->script->pub.code, "expected end of script");
            return NULL;
        } else
        if (until == '\n') {
            ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
                pos - parser->script->pub.code, "expected newline");
            return NULL;
        } else {
            ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
                pos - parser->script->pub.code, "expected '%c'", until);
            return NULL;
        }
    }

    int32_t len = flecs_ito(int32_t, pos - start);
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

    Operator          (":",        EcsTokColon)
    Operator          ("{",        EcsTokScopeOpen)
    Operator          ("}",        EcsTokScopeClose)
    Operator          ("(",        EcsTokParenOpen)
    Operator          (")",        EcsTokParenClose)
    Operator          ("[",        EcsTokBracketOpen)
    Operator          ("]",        EcsTokBracketClose)
    Operator          ("@",        EcsTokAnnotation)
    Operator          (",",        EcsTokComma)
    Operator          (";",        EcsTokSemiColon)
    Operator          ("*",        EcsTokMul)
    Operator          ("?",        EcsTokOptional)

    OperatorMultiChar ("==",       EcsTokEq)
    OperatorMultiChar ("!=",       EcsTokNeq)
    OperatorMultiChar ("~=",       EcsTokMatch)
    OperatorMultiChar ("||",       EcsTokOr)

    OperatorMultiChar ("!",        EcsTokNot)
    OperatorMultiChar ("=",        EcsTokAssign)
    OperatorMultiChar ("|",        EcsTokBitwiseOr)

    Keyword           ("with",     EcsTokKeywordWith)
    Keyword           ("using",    EcsTokKeywordUsing)
    Keyword           ("template", EcsTokKeywordTemplate)
    Keyword           ("prop",     EcsTokKeywordProp)
    Keyword           ("const",    EcsTokKeywordConst)
    Keyword           ("if",       EcsTokKeywordIf)
    Keyword           ("else",     EcsTokKeywordElse)
    Keyword           ("module",   EcsTokKeywordModule)

    } else if (pos[0] == '"') {
        return flecs_script_string(parser, pos, out);

    } else if (flecs_script_is_number(pos[0])) {
        return flecs_script_number(parser, pos, out);

    } else if (flecs_script_is_identifier(pos[0])) {
        return flecs_script_identifier(parser, pos, out);
    }

    if (!is_lookahead) {
        ecs_parser_error(parser->script->pub.name, parser->script->pub.code,
            pos - parser->script->pub.code, "unknown token '%c'", pos[0]);
    }

    return NULL;
}

#endif
