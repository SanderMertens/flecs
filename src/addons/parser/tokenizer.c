/**
 * @file addons/parser/tokenizer.c
 * @brief Parser tokenizer.
 */

#include "flecs.h"

#ifdef FLECS_PARSER

#include "parser.h"

int64_t flecs_parser_errpos(
    const ecs_parser_t *parser,
    const char *pos)
{
    if (parser->fixed_pos) {
        return parser->fixed_pos - parser->code;
    }
    return pos - parser->code;
}

static bool flecs_is_comment(
    const char *pos)
{
    return pos[0] == '/' && (pos[1] == '/' || pos[1] == '*');
}

static bool flecs_keyword_boundary(
    const char *pos)
{
    if (!pos[0]) {
        return true;
    }

    if (isspace(pos[0])) {
        return true;
    }

    if (flecs_is_comment(pos)) {
        return true;
    }

    return false;
}

static const struct {
    const char *spelling;
    const char *description;
    uint8_t length;
} flecs_tokens[EcsTokLast] = {
    [EcsTokUnknown] = {"unknown token", "unknown token ", 0},
    [EcsTokColon] = {":", "", 1},
    [EcsTokScopeOpen] = {"{", "", 1},
    [EcsTokScopeClose] = {"}", "", 1},
    [EcsTokParenOpen] = {"(", "", 1},
    [EcsTokParenClose] = {")", "", 1},
    [EcsTokBracketOpen] = {"[", "", 1},
    [EcsTokBracketClose] = {"]", "", 1},
    [EcsTokAnnotation] = {"@", "", 1},
    [EcsTokComma] = {",", "", 1},
    [EcsTokSemiColon] = {";", "", 1},
    [EcsTokAssign] = {"=", "", 1},
    [EcsTokAdd] = {"+", "", 1},
    [EcsTokSub] = {"-", "", 1},
    [EcsTokMul] = {"*", "", 1},
    [EcsTokDiv] = {"/", "", 1},
    [EcsTokMod] = {"%", "", 1},
    [EcsTokBitwiseOr] = {"|", "", 1},
    [EcsTokBitwiseAnd] = {"&", "", 1},
    [EcsTokNot] = {"!", "", 1},
    [EcsTokOptional] = {"?", "", 1},
    [EcsTokEq] = {"==", "", 2},
    [EcsTokNeq] = {"!=", "", 2},
    [EcsTokGt] = {">", "", 1},
    [EcsTokGtEq] = {">=", "", 2},
    [EcsTokLt] = {"<", "", 1},
    [EcsTokLtEq] = {"<=", "", 2},
    [EcsTokAnd] = {"&&", "", 2},
    [EcsTokOr] = {"||", "", 2},
    [EcsTokMatch] = {"~=", "", 2},
    [EcsTokRange] = {"..", "", 2},
    [EcsTokShiftLeft] = {"<<", "", 2},
    [EcsTokShiftRight] = {">>", "", 2},
    [EcsTokAddAssign] = {"+=", "", 2},
    [EcsTokMulAssign] = {"*=", "", 2},
    [EcsTokHasBracketOpen] = {"?[", "", 2},
    [EcsTokKeywordWith] = {"with", "keyword ", 4},
    [EcsTokKeywordUsing] = {"using", "keyword ", 5},
    [EcsTokKeywordProp] = {"prop", "keyword ", 4},
    [EcsTokKeywordMut] = {"mut", "keyword ", 3},
    [EcsTokKeywordConst] = {"const", "keyword ", 5},
    [EcsTokKeywordMatch] = {"match", "keyword ", 5},
    [EcsTokKeywordNew] = {"new", "keyword ", 3},
    [EcsTokKeywordExport] = {"export", "keyword ", 6},
    [EcsTokKeywordIf] = {"if", "keyword ", 2},
    [EcsTokKeywordElse] = {"else", "keyword ", 4},
    [EcsTokKeywordFor] = {"for", "keyword ", 3},
    [EcsTokKeywordIn] = {"in", "keyword ", 2},
    [EcsTokKeywordTemplate] = {"template", "keyword ", 8},
    [EcsTokKeywordModule] = {"module", "keyword ", 6},
    [EcsTokKeywordInclude] = {"include", "keyword ", 7},
    [EcsTokKeywordFn] = {"fn", "keyword ", 2},
    [EcsTokKeywordAwait] = {"await", "keyword ", 5},
    [EcsTokKeywordTry] = {"try", "keyword ", 3},
    [EcsTokKeywordCatch] = {"catch", "keyword ", 5},
    [EcsTokKeywordContinue] = {"continue", "keyword ", 8},
    [EcsTokKeywordAsync] = {"async", "keyword ", 5},
    [EcsTokKeywordWhile] = {"while", "keyword ", 5},
    [EcsTokKeywordScript] = {"script", "keyword ", 6},
    [EcsTokArrow] = {"->", "", 2},
    [EcsTokIdentifier] = {"identifier", "identifier ", 0},
    [EcsTokFunction] = {"function", "function ", 0},
    [EcsTokString] = {"string", "string ", 0},
    [EcsTokChar] = {"char", "char ", 0},
    [EcsTokNumber] = {"number", "number ", 0},
    [EcsTokNewline] = {"newline", "newline", 0},
    [EcsTokMember] = {".", "member", 0},
    [EcsTokEnd] = {"end of script", "end of script", 0},
};

static const ecs_token_kind_t flecs_token_pairs[128][2] = {
    ['+'] = {EcsTokAddAssign},
    ['*'] = {EcsTokMulAssign},
    ['-'] = {EcsTokArrow},
    ['?'] = {EcsTokHasBracketOpen},
    ['.'] = {EcsTokRange},
    ['='] = {EcsTokEq},
    ['!'] = {EcsTokNeq},
    ['<'] = {EcsTokLtEq, EcsTokShiftLeft},
    ['>'] = {EcsTokGtEq, EcsTokShiftRight},
    ['&'] = {EcsTokAnd},
    ['|'] = {EcsTokOr},
    ['~'] = {EcsTokMatch}
};

static char* flecs_tokenizer_write(
    ecs_parser_t *parser,
    char *dst,
    char ch)
{
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);

    if (parser && parser->token_end) {
        ecs_assert(dst < parser->token_end, ECS_INVALID_OPERATION,
            "out of parser token storage");
    }

    dst[0] = ch;
    return dst + 1;
}

const char* flecs_token_kind_str(
    ecs_token_kind_t kind)
{
    if ((unsigned)kind < EcsTokLast && flecs_tokens[kind].description) {
        return flecs_tokens[kind].description;
    }
    return "<corrupt>";
}

const char* flecs_token_str(
    ecs_token_kind_t kind)
{
    if (kind == EcsTokMod) {
        return "%%";
    }
    if (kind == EcsTokMember) {
        return "member";
    }
    if ((unsigned)kind < EcsTokLast && flecs_tokens[kind].spelling) {
        return flecs_tokens[kind].spelling;
    }
    return "<corrupt>";
}

const char* flecs_scan_whitespace(
    ecs_parser_t *parser,
    const char *pos) 
{
    ecs_assert(pos != NULL, ECS_INTERNAL_ERROR, NULL);

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

static const char* flecs_scan_line_comment(
    const char *pos)
{
    ecs_assert(pos[0] == '/' && pos[1] == '/', ECS_INTERNAL_ERROR, NULL);

    for (pos = pos + 2; pos[0] && pos[0] != '\n'; pos ++) { }
    return pos;
}

static const char* flecs_scan_whitespace_and_comment(
    ecs_parser_t *parser,
    const char *pos)
{
    if (!pos) {
        return NULL;
    }
    const char *newline = NULL;
    bool collapse = false;
    for (;;) {
        pos = flecs_scan_whitespace(parser, pos);
        if (pos[0] == '\n') {
            if (!collapse) {
                return pos;
            }
            newline = pos ++;
            continue;
        }
        if (!flecs_is_comment(pos)) {
            return newline ? newline : pos;
        }
        if (pos[1] == '/') {
            pos = flecs_scan_line_comment(pos);
            if (parser->significant_newline && pos[0] == '\n') {
                collapse = true;
                newline = pos ++;
            }
        } else {
            const char *end = strstr(pos + 2, "*/");
            if (!end) {
                if (newline) {
                    return newline;
                }
                ecs_parser_error(parser->name, parser->code,
                    flecs_parser_errpos(parser, pos),
                    "missing */ for multiline comment");
                return NULL;
            }
            pos = end + 2;
            const char *next = pos;
            if (next[0] == '\r' && next[1] == '\n') {
                next ++;
            }
            if (parser->significant_newline && next[0] == '\n' &&
                flecs_is_comment(flecs_scan_whitespace(parser, next + 1)))
            {
                collapse = true;
                newline = next;
                pos = next + 1;
            }
        }
    }
}

static bool flecs_script_is_identifier(
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

        if ((c == '.') && (pos[1] == '.')) {
            break;
        }

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
                if (!pos[0]) {
                    goto done;
                }
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
                        outpos = flecs_tokenizer_write(parser, outpos, c);
                    }
                    pos ++;

                    if (!indent) {
                        break;
                    }
                } while (true);

                goto done;
            } else if (c == '>') {
                ecs_parser_error(name, code, pos - code, 
                    "> without < in identifier");
                return NULL;
            } else {
                goto done;
            }
        }

        if (outpos) {
            outpos = flecs_tokenizer_write(parser, outpos, *pos);
        }

        pos ++;
    } while (true);

done:
    if (outpos) {
        outpos = flecs_tokenizer_write(parser, outpos, '\0');
        if (parser) {
            parser->token_cur = outpos;
        }
    }

    if (out && parser && parser->function_token) {
        if (pos[0] == '(') {
            out->kind = EcsTokFunction;
            pos ++;
        }
    }

    return pos;
}

static bool flecs_script_is_number(
    const char *c)
{
    return isdigit(c[0]) || ((c[0] == '-') && isdigit(c[1]));
}

static const char* flecs_script_digits(
    const char *pos,
    int base)
{
    while (base == 16 ? isxdigit((unsigned char)*pos) :
        base == 2 ? (*pos == '0' || *pos == '1') : isdigit((unsigned char)*pos))
    {
        pos ++;
    }
    return pos;
}

static const char* flecs_script_number(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out)
{
    const char *start = pos;
    int base = 10;
    if (*pos == '-') {
        pos ++;
    }
    if (pos[0] == '0') {
        if (pos[1] == 'x' || pos[1] == 'X') {
            base = 16;
        } else if (pos[1] == 'b' || pos[1] == 'B') {
            base = 2;
        }
        pos += base != 10 ? 2 : 0;
    }
    const char *digits = pos;
    pos = flecs_script_digits(pos, base);
    bool has_digits = pos != digits;
    if (*pos == '.' && isdigit((unsigned char)pos[1])) {
        digits = ++ pos;
        pos = flecs_script_digits(pos, base);
        has_digits |= pos != digits;
    }
    if (base == 10 && (*pos == 'e' || *pos == 'E')) {
        digits = pos + 1;
        digits += *digits == '+' || *digits == '-';
        if (isdigit((unsigned char)*digits)) {
            pos = flecs_script_digits(digits, base);
        }
    }
    if (!has_digits && base != 10) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos), "missing digits in number literal");
        return NULL;
    }
    ecs_size_t length = flecs_ito(ecs_size_t, pos - start);
    ecs_assert(!parser->token_end || parser->token_cur + length < parser->token_end,
        ECS_INVALID_OPERATION, "out of parser token storage");
    out->kind = EcsTokNumber;
    out->value = parser->token_cur;
    ecs_os_memcpy(parser->token_cur, start, length);
    parser->token_cur[length] = '\0';
    parser->token_cur += length + 1;
    return pos;
}

static const char* flecs_script_skip_string(
    ecs_parser_t *parser,
    const char *pos, 
    char delim)
{
    char ch;
    const char *start = pos;
    for (; (ch = pos[0]) && pos[0] != delim; pos ++) {
        if (ch == '\\' && pos[1]) {
            pos ++;
        }
    }

    if (!pos[0]) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, start), "unterminated string");
        return NULL;
    }

    return pos;
}

static const char* flecs_tokenizer_emit(
    ecs_parser_t *parser,
    const char *pos,
    const char *end,
    ecs_token_kind_t kind,
    ecs_token_t *out)
{
    int32_t len = flecs_ito(int32_t, end - pos);
    char *outpos = parser->token_cur;
    int32_t i;
    for (i = 0; i < len; i ++) {
        outpos = flecs_tokenizer_write(parser, outpos, pos[i + 1]);
    }
    outpos = flecs_tokenizer_write(parser, outpos, '\0');

    out->kind = kind;
    out->value = parser->token_cur;
    parser->token_cur = outpos;
    return end + 2;
}

static const char* flecs_script_char(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out)
{
    const char *end = flecs_script_skip_string(parser, pos + 1, '\'');
    if (!end) {
        return NULL;
    }

    ecs_assert(end[0] == '\'', ECS_INTERNAL_ERROR, NULL);
    end --;

    int32_t len = flecs_ito(int32_t, end - pos);
    if(len == 0) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos), "Empty char");
        return NULL;
    } else if ((len > 1) && !((len == 2) && (pos[1] == '\\'))) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos), "only one char allowed");
        return NULL;
    }

    return flecs_tokenizer_emit(parser, pos, end, EcsTokChar, out);
}

static const char* flecs_script_string(
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

    return flecs_tokenizer_emit(parser, pos, end, EcsTokString, out);
}

static const char* flecs_script_multiline_string(
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
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos), "unterminated string");
        return NULL;
    }

    end --;

    return flecs_tokenizer_emit(parser, pos, end, EcsTokString, out);
}

const char* flecs_tokenizer_until(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out,
    char until)
{
    const char *start = pos = flecs_scan_whitespace(parser, pos);
    char ch;

    parser->pos = parser->fixed_pos ? parser->fixed_pos : pos;
    if (!parser->stmt_pos) {
        parser->stmt_pos = parser->pos;
    }

    for (; (ch = pos[0]); pos ++) {
        if (ch == until) {
            break;
        }
    }

    if (!pos[0]) {
        if (until == '\0') {
            ecs_parser_error(parser->name, parser->code,
                flecs_parser_errpos(parser, pos), "expected end of script");
            return NULL;
        } else
        if (until == '\n') {
            ecs_parser_error(parser->name, parser->code,
                flecs_parser_errpos(parser, pos), "expected newline");
            return NULL;
        } else {
            ecs_parser_error(parser->name, parser->code,
                flecs_parser_errpos(parser, pos), "expected '%c'", until);
            return NULL;
        }
    }

    int32_t len = flecs_ito(int32_t, pos - start);
    char *token_start = parser->token_cur;
    char *outpos = token_start;
    int32_t i;
    for (i = 0; i < len; i ++) {
        outpos = flecs_tokenizer_write(parser, outpos, start[i]);
    }
    out->value = parser->token_cur;
    parser->token_cur = outpos;

    while (parser->token_cur != token_start && isspace(parser->token_cur[-1])) {
        parser->token_cur --;
    }

    parser->token_cur = flecs_tokenizer_write(parser, parser->token_cur, '\0');

    return pos;
}

static const char* flecs_token_scan(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out,
    bool is_lookahead)
{
    if (!pos) {
        if (!is_lookahead) {
            ecs_parser_error(parser->name, parser->code, 0,
                "unexpected end of parser state");
        }
        return NULL;
    }

    pos = flecs_scan_whitespace_and_comment(parser, pos);
    if (!pos) {
        return NULL;
    }

    if (!is_lookahead) {
        parser->pos = parser->fixed_pos ? parser->fixed_pos : pos;
        if (!parser->stmt_pos) {
            parser->stmt_pos = parser->pos;
        }
    }

    out->kind = EcsTokUnknown;
    out->value = NULL;

    if (pos[0] == '\0') {
        out->kind = EcsTokEnd;
        return pos;
    } else if (pos[0] == '\n') {
        out->kind = EcsTokNewline;

        pos = flecs_scan_whitespace_and_comment(parser, pos + 1);
        if (!pos) {
            return NULL;
        }

        if (pos[0] == '\n') {
            pos ++;
        }

        return pos;

    } else if (flecs_script_is_number(pos)) {
        return flecs_script_number(parser, pos, out);

    }

    unsigned char ch = (unsigned char)pos[0];
    if (ch < 128) {
        for (int32_t i = 0; i < 2; i ++) {
            ecs_token_kind_t kind = flecs_token_pairs[ch][i];
            if (kind && pos[1] == flecs_tokens[kind].spelling[1]) {
                out->kind = kind;
                out->value = flecs_tokens[kind].spelling;
                return pos + 2;
            }
        }
        if (flecs_tokens[ch].spelling &&
            (!flecs_tokens[ch].description[0] || ch == '.'))
        {
            out->kind = (ecs_token_kind_t)ch;
            out->value = ch == '%' ? "%%" : flecs_tokens[ch].spelling;
            return pos + 1;
        }
    }

    for (ecs_token_kind_t kind = EcsTokKeywordWith; kind < EcsTokLast; kind ++) {
        const char *word = flecs_tokens[kind].spelling;
        int32_t len = flecs_tokens[kind].length;
        if (word[0] == pos[0] && !ecs_os_strncmp(pos, word, len) &&
            flecs_keyword_boundary(pos + len))
        {
            out->kind = kind;
            out->value = word;
            return pos + len;
        }
    }

    if (pos[0] == '\'') {
        return flecs_script_char(parser, pos, out);

    } else if (pos[0] == '"') {
        return flecs_script_string(parser, pos, out);

    } else if (pos[0] == '`') {
        return flecs_script_multiline_string(parser, pos, out);

    } else if (flecs_script_is_identifier(pos[0])) {
        return flecs_tokenizer_identifier(parser, pos, out);
    }

    if (!is_lookahead) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos), "unknown token '%c'", pos[0]);
    }

    return NULL;
}

const char* flecs_token(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out,
    bool is_lookahead)
{
    const char *result = flecs_token_scan(parser, pos, out, is_lookahead);

    (void)is_lookahead;

    if (result && out->kind != EcsTokNewline && out->kind != EcsTokEnd) {
        parser->token_ends[parser->token_ends_i & 7] = result;
        parser->token_ends_i ++;
    }

    return result;
}

const char* flecs_parser_stmt_end(
    const ecs_parser_t *parser,
    const char *pos)
{
    const char *end = NULL;
    int32_t i;

    for (i = 0; i < 8; i ++) {
        const char *cur = parser->token_ends[i];
        if (cur && cur <= pos && (!end || cur > end)) {
            end = cur;
        }
    }

    if (!end) {
        end = pos;
    }

    return end;
}

#endif
