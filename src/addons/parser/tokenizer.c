/**
 * @file addons/script/tokenizer.c
 * @brief Script tokenizer.
 */

#include "flecs.h"

#ifdef FLECS_PARSER

#include "parser.h"

#define Keyword(keyword, _kind)\
    } else if (!ecs_os_strncmp(pos, keyword " ", ecs_os_strlen(keyword) + 1)) {\
        out->value = keyword;\
        out->kind = _kind;\
        return pos + ecs_os_strlen(keyword);\
    } else if (!ecs_os_strncmp(pos, keyword "\n", ecs_os_strlen(keyword) + 1)) {\
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

const char* flecs_token_kind_str(
    ecs_token_kind_t kind)
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
    case EcsTokAdd:
    case EcsTokSub:
    case EcsTokMul:
    case EcsTokDiv:
    case EcsTokMod:
    case EcsTokBitwiseOr:
    case EcsTokBitwiseAnd:
    case EcsTokNot:
    case EcsTokOptional:
    case EcsTokEq:
    case EcsTokNeq:
    case EcsTokGt:
    case EcsTokGtEq:
    case EcsTokLt:
    case EcsTokLtEq:
    case EcsTokAnd:
    case EcsTokOr:
    case EcsTokMatch:
    case EcsTokRange:
    case EcsTokShiftLeft:
    case EcsTokShiftRight:
    case EcsTokAddAssign:
    case EcsTokMulAssign:
        return "";
    case EcsTokKeywordWith:
    case EcsTokKeywordUsing:
    case EcsTokKeywordProp:
    case EcsTokKeywordConst:
    case EcsTokKeywordIf:
    case EcsTokKeywordElse:
    case EcsTokKeywordFor:
    case EcsTokKeywordIn:
    case EcsTokKeywordTemplate:
    case EcsTokKeywordModule:
    case EcsTokKeywordMatch:
        return "keyword ";
    case EcsTokIdentifier:
        return "identifier ";
    case EcsTokString:
        return "string ";
    case EcsTokNumber:
        return "number ";
    case EcsTokNewline:
        return "newline";
    case EcsTokMember:
        return "member";
    case EcsTokEnd:
        return "end of script";
    default:
        return "<corrupt>";
    }
}

const char* flecs_token_str(
    ecs_token_kind_t kind)
{
    switch(kind) {
    case EcsTokUnknown: return "unknown token";
    case EcsTokColon: return ":";
    case EcsTokScopeOpen: return "{";
    case EcsTokScopeClose: return "}";
    case EcsTokParenOpen: return "(";
    case EcsTokParenClose: return ")";
    case EcsTokBracketOpen: return "[";
    case EcsTokBracketClose: return "]";
    case EcsTokAnnotation: return "@";
    case EcsTokComma: return ",";
    case EcsTokSemiColon: return ";";
    case EcsTokAssign: return "=";
    case EcsTokAdd: return "+";
    case EcsTokSub: return "-";
    case EcsTokMul: return "*";
    case EcsTokDiv: return "/";
    case EcsTokMod: return "%%";
    case EcsTokBitwiseOr: return "|";
    case EcsTokBitwiseAnd: return "&";
    case EcsTokNot: return "!";
    case EcsTokOptional: return "?";
    case EcsTokEq: return "==";
    case EcsTokNeq: return "!=";
    case EcsTokGt: return ">";
    case EcsTokGtEq: return ">=";
    case EcsTokLt: return "<";
    case EcsTokLtEq: return "<=";
    case EcsTokAnd: return "&&";
    case EcsTokOr: return "||";
    case EcsTokMatch: return "~=";
    case EcsTokRange: return "..";
    case EcsTokShiftLeft: return "<<";
    case EcsTokShiftRight: return ">>";
    case EcsTokAddAssign: return "+=";
    case EcsTokMulAssign: return "*=";
    case EcsTokKeywordWith: return "with";
    case EcsTokKeywordUsing: return "using";
    case EcsTokKeywordProp: return "prop";
    case EcsTokKeywordConst: return "const";
    case EcsTokKeywordMatch: return "match";
    case EcsTokKeywordIf: return "if";
    case EcsTokKeywordElse: return "else";
    case EcsTokKeywordFor: return "for";
    case EcsTokKeywordIn: return "in";
    case EcsTokKeywordTemplate: return "template";
    case EcsTokKeywordModule: return "module";
    case EcsTokIdentifier: return "identifier";
    case EcsTokString: return "string";
    case EcsTokNumber: return "number";
    case EcsTokNewline: return "newline";
    case EcsTokMember: return "member";
    case EcsTokEnd: return "end of script";
    default:
        return "<corrupt>";
    }
}

const char* flecs_scan_whitespace(
    ecs_parser_t *parser,
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
    ecs_parser_t *parser,
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

            ecs_parser_error(parser->name, parser->code, 
                pos - parser->code, "missing */ for multiline comment");
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

const char* flecs_tokenizer_identifier(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out) 
{
    if (out) {
        out->kind = EcsTokIdentifier;
        out->value = parser->token_cur;
    }

    bool is_var = pos[0] == '$';
    char *outpos = NULL;
    const char *start = pos;
    if (parser) {
        outpos = parser->token_cur;
        if (parser->merge_variable_members) {
            is_var = false;
        }
    }

    const char *name = parser ? parser->name : NULL;
    const char *code = parser ? parser->code : pos;

    if (!flecs_script_is_identifier(pos[0])) {
        ecs_parser_error(name, code, pos - code,
            "invalid start of identifier '%c'",
                pos[0]);
        return NULL;
    }

    do {
        char c = pos[0];
        bool is_ident = flecs_script_is_identifier(c) || isdigit(c);

        if (!is_var) {
            is_ident = is_ident || (c == '.');
        }

        /* Retain \. for name lookup operation */
        if (!is_ident && c == '\\' && pos[1] == '.') {
            is_ident = true;
        }

        /* Retain .* for using wildcard expressions */
        if (!is_ident && c == '*') {
            if (pos != start && pos[-1] == '.') {
                is_ident = true;
            }
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
                        ecs_parser_error(name, code, pos - code, 
                                    "< without > in identifier");
                        return NULL;
                    }

                    if (outpos) {
                        *outpos = c;
                        outpos ++;
                    }
                    pos ++;

                    if (!indent) {
                        break;
                    }
                } while (true);

                if (outpos && parser) {
                    *outpos = '\0';
                    parser->token_cur = outpos + 1;
                }
                return pos;
            } else if (c == '>') {
                ecs_parser_error(name, code, pos - code, 
                    "> without < in identifier");
                return NULL;
            } else {
                if (outpos && parser) {
                    *outpos = '\0';
                    parser->token_cur = outpos + 1;
                }
                return pos;
            }
        }

        if (outpos) {
            *outpos = *pos;
            outpos ++;
        }

        pos ++;
    } while (true);
}

// Number token static
static
bool flecs_script_is_number(
    const char *c)
{
    return isdigit(c[0]) || ((c[0] == '-') && isdigit(c[1]));
}

static
const char* flecs_script_number(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out) 
{
    out->kind = EcsTokNumber;
    out->value = parser->token_cur;
    
    bool dot_parsed = false;
    bool e_parsed = false;
    int base = 10;

    ecs_assert(flecs_script_is_number(pos), ECS_INTERNAL_ERROR, NULL);
    char *outpos = parser->token_cur;

    if (pos[0] == '-') {
        outpos[0] = pos[0];
        pos ++;
        outpos ++;
    }

    if (pos[0] == '0' && (pos[1] == 'x' || pos[1] == 'X')) {
        base = 16;
        outpos[0] = pos[0];
        outpos[1] = pos[1];
        outpos += 2;
        pos += 2;
    } else if (pos[0] == '0' && (pos[1] == 'b' || pos[1] == 'B')) {
        base = 2;
        outpos[0] = pos[0];
        outpos[1] = pos[1];
        outpos += 2;
        pos += 2;
    }

    do {
        char c = pos[0];
        bool valid_number = false;

        if (c == '.') {
            if (!dot_parsed && !e_parsed) {
                if (isdigit(pos[1])) {
                    dot_parsed = true;
                    valid_number = true;
                }
            }
        } else if (c == 'e') {
            if (!e_parsed) {
                if (isdigit(pos[1])) {
                    e_parsed = true;
                    valid_number = true;
                }
            }
        } else if ((base == 10) && isdigit(c)) {
            valid_number = true;
        } else if ((base == 16) && isxdigit(c)) {
            valid_number = true;
        }  else if ((base == 2) && (c == '0' || c == '1')) {
            valid_number = true;
        }

        if (!valid_number) {
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
    ecs_parser_t *parser,
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
        ecs_parser_error(parser->name, parser->code,
            pos - parser->code, "unterminated string");
        return NULL;
    }

    return pos;
}

static
const char* flecs_script_string(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out) 
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

static
const char* flecs_script_multiline_string(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out) 
{
    char ch;
    const char *end = pos + 1;
    while ((ch = end[0]) && (ch != '`')) {
        if (ch == '\\' && end[1] == '`') {
            end ++;
        }
        end ++;
    }

    if (ch != '`') {
        return NULL;
    }

    end --;

    int32_t len = flecs_ito(int32_t, end - pos);
    ecs_os_memcpy(parser->token_cur, pos + 1, len);
    parser->token_cur[len] = '\0';

    out->kind = EcsTokString;
    out->value = parser->token_cur;
    parser->token_cur += len + 1;
    return end + 2;
}

const char* flecs_tokenizer_until(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out,
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
            ecs_parser_error(parser->name, parser->code,
                pos - parser->code, "expected end of script");
            return NULL;
        } else
        if (until == '\n') {
            ecs_parser_error(parser->name, parser->code,
                pos - parser->code, "expected newline");
            return NULL;
        } else {
            ecs_parser_error(parser->name, parser->code,
                pos - parser->code, "expected '%c'", until);
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

const char* flecs_token(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out,
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

    } else if (flecs_script_is_number(pos)) {
        return flecs_script_number(parser, pos, out);

    OperatorMultiChar ("+=",       EcsTokAddAssign)
    OperatorMultiChar ("*=",       EcsTokMulAssign)
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
    Operator          ("+",        EcsTokAdd)
    Operator          ("-",        EcsTokSub)
    Operator          ("*",        EcsTokMul)
    Operator          ("/",        EcsTokDiv)
    Operator          ("%%",       EcsTokMod)
    Operator          ("?",        EcsTokOptional)
    
    OperatorMultiChar ("..",       EcsTokRange)
    Operator          (".",        EcsTokMember)

    OperatorMultiChar ("==",       EcsTokEq)
    OperatorMultiChar ("!=",       EcsTokNeq)
    OperatorMultiChar ("<<",       EcsTokShiftLeft)
    OperatorMultiChar (">>",       EcsTokShiftRight)
    OperatorMultiChar (">=",       EcsTokGtEq)
    OperatorMultiChar ("<=",       EcsTokLtEq)
    
    OperatorMultiChar ("&&",       EcsTokAnd)
    OperatorMultiChar ("||",       EcsTokOr)
    OperatorMultiChar ("~=",       EcsTokMatch)

    Operator          ("!",        EcsTokNot)
    Operator          ("=",        EcsTokAssign)
    Operator          ("&",        EcsTokBitwiseAnd)
    Operator          ("|",        EcsTokBitwiseOr)
    Operator          (">",        EcsTokGt)
    Operator          ("<",        EcsTokLt)

    Keyword           ("with",     EcsTokKeywordWith)
    Keyword           ("using",    EcsTokKeywordUsing)
    Keyword           ("template", EcsTokKeywordTemplate)
    Keyword           ("prop",     EcsTokKeywordProp)
    Keyword           ("const",    EcsTokKeywordConst)
    Keyword           ("if",       EcsTokKeywordIf)
    Keyword           ("else",     EcsTokKeywordElse)
    Keyword           ("for",      EcsTokKeywordFor)
    Keyword           ("in",       EcsTokKeywordIn)
    Keyword           ("match",    EcsTokKeywordMatch)
    Keyword           ("module",   EcsTokKeywordModule)

    } else if (pos[0] == '"') {
        return flecs_script_string(parser, pos, out);

    } else if (pos[0] == '`') {
        return flecs_script_multiline_string(parser, pos, out);

    } else if (flecs_script_is_identifier(pos[0])) {
        return flecs_tokenizer_identifier(parser, pos, out);
    }

    if (!is_lookahead) {
        ecs_parser_error(parser->name, parser->code,
            pos - parser->code, "unknown token '%c'", pos[0]);
    }

    return NULL;
}

#endif
