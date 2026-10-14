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

static const struct {
    const char *spelling;
    const char *description;
} flecs_tokens[EcsTokLast] = {
    [EcsTokUnknown] = {"unknown token", "unknown token "},
    [EcsTokColon] = {":", ""},
    [EcsTokScopeOpen] = {"{", ""},
    [EcsTokScopeClose] = {"}", ""},
    [EcsTokParenOpen] = {"(", ""},
    [EcsTokParenClose] = {")", ""},
    [EcsTokBracketOpen] = {"[", ""},
    [EcsTokBracketClose] = {"]", ""},
    [EcsTokAnnotation] = {"@", ""},
    [EcsTokComma] = {",", ""},
    [EcsTokSemiColon] = {";", ""},
    [EcsTokAssign] = {"=", ""},
    [EcsTokAdd] = {"+", ""},
    [EcsTokSub] = {"-", ""},
    [EcsTokMul] = {"*", ""},
    [EcsTokDiv] = {"/", ""},
    [EcsTokMod] = {"%", ""},
    [EcsTokBitwiseOr] = {"|", ""},
    [EcsTokBitwiseAnd] = {"&", ""},
    [EcsTokNot] = {"!", ""},
    [EcsTokOptional] = {"?", ""},
    [EcsTokEq] = {"==", ""},
    [EcsTokNeq] = {"!=", ""},
    [EcsTokGt] = {">", ""},
    [EcsTokGtEq] = {">=", ""},
    [EcsTokLt] = {"<", ""},
    [EcsTokLtEq] = {"<=", ""},
    [EcsTokAnd] = {"&&", ""},
    [EcsTokOr] = {"||", ""},
    [EcsTokMatch] = {"~=", ""},
    [EcsTokRange] = {"..", ""},
    [EcsTokShiftLeft] = {"<<", ""},
    [EcsTokShiftRight] = {">>", ""},
    [EcsTokAddAssign] = {"+=", ""},
    [EcsTokMulAssign] = {"*=", ""},
    [EcsTokHasBracketOpen] = {"?[", ""},
    [EcsTokKeywordWith] = {"with", "keyword "},
    [EcsTokKeywordUsing] = {"using", "keyword "},
    [EcsTokKeywordProp] = {"prop", "keyword "},
    [EcsTokKeywordMut] = {"mut", "keyword "},
    [EcsTokKeywordConst] = {"const", "keyword "},
    [EcsTokKeywordMatch] = {"match", "keyword "},
    [EcsTokKeywordNew] = {"new", "keyword "},
    [EcsTokKeywordExport] = {"export", "keyword "},
    [EcsTokKeywordIf] = {"if", "keyword "},
    [EcsTokKeywordElse] = {"else", "keyword "},
    [EcsTokKeywordFor] = {"for", "keyword "},
    [EcsTokKeywordIn] = {"in", "keyword "},
    [EcsTokKeywordTemplate] = {"template", "keyword "},
    [EcsTokKeywordModule] = {"module", "keyword "},
    [EcsTokKeywordInclude] = {"include", "keyword "},
    [EcsTokKeywordFn] = {"fn", "keyword "},
    [EcsTokKeywordAwait] = {"await", "keyword "},
    [EcsTokKeywordTry] = {"try", "keyword "},
    [EcsTokKeywordCatch] = {"catch", "keyword "},
    [EcsTokKeywordContinue] = {"continue", "keyword "},
    [EcsTokKeywordAsync] = {"async", "keyword "},
    [EcsTokKeywordWhile] = {"while", "keyword "},
    [EcsTokKeywordScript] = {"script", "keyword "},
    [EcsTokArrow] = {"->", ""},
    [EcsTokIdentifier] = {"identifier", "identifier "},
    [EcsTokString] = {"string", "string "},
    [EcsTokChar] = {"char", "char "},
    [EcsTokNumber] = {"number", "number "},
    [EcsTokNewline] = {"newline", "newline"},
    [EcsTokMember] = {".", ""},
    [EcsTokEnd] = {"end of script", "end of script"},
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
    (void)parser;
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(!parser->token_end || dst < parser->token_end,
        ECS_INVALID_OPERATION, "out of parser token storage");

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

    while (isspace((unsigned char)pos[0]) &&
        (pos[0] != '\n' || !parser->significant_newline))
    {
        pos ++;
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

const char* flecs_scan_whitespace_and_comment(
    ecs_parser_t *parser,
    const char *pos,
    bool *newline)
{
    for (;;) {
        const char *start = pos;
        pos = flecs_scan_whitespace(parser, pos);
        if (newline && memchr(start, '\n', flecs_ito(size_t, pos - start))) {
            *newline = true;
        }

        if (!flecs_is_comment(pos)) {
            return pos;
        }

        if (pos[1] == '/') {
            pos = flecs_scan_line_comment(pos);
            continue;
        }

        const char *end = strstr(pos + 2, "*/");
        if (!end) {
            ecs_parser_error(parser->name, parser->code,
                flecs_parser_errpos(parser, pos),
                "missing */ for multiline comment");
            return NULL;
        }

        pos = end + 2;
    }
}

const char* flecs_parser_skip_newlines(
    ecs_parser_t *parser,
    const char *pos)
{
    bool significant_newline = parser->significant_newline;
    parser->significant_newline = false;
    pos = flecs_scan_whitespace_and_comment(parser, pos, NULL);
    parser->significant_newline = significant_newline;
    return pos;
}

static bool flecs_script_is_identifier(
    char c)
{
    return isalpha((unsigned char)c) || (c == '_') || (c == '$') || (c == '#');
}

const char* flecs_tokenizer_identifier(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out) 
{
    bool is_var = pos[0] == '$';
    char *outpos = NULL;
    const char *start = pos;
    if (parser) {
        outpos = parser->token_cur;
        if (parser->merge_variable_members) {
            is_var = false;
        }
    }

    if (out) {
        out->kind = EcsTokIdentifier;
        out->value = outpos;
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

        bool is_ident = flecs_script_is_identifier(c) || isdigit((unsigned char)c);

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
        parser->token_cur = flecs_tokenizer_write(parser, outpos, '\0');
    }

    return pos;
}

static char* flecs_tokenizer_copy(
    ecs_parser_t *parser,
    const char *start,
    const char *end)
{
    ecs_size_t length = flecs_ito(ecs_size_t, end - start);
    ecs_assert(!parser->token_end ||
        parser->token_cur + length < parser->token_end,
            ECS_INVALID_OPERATION, "out of parser token storage");

    char *result = parser->token_cur;
    ecs_os_memcpy(result, start, length);
    result[length] = '\0';
    parser->token_cur += length + 1;
    return result;
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
    if (base != 10 && *pos == '.' && isxdigit((unsigned char)pos[1])) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, start),
            "fractional part not allowed in hexadecimal or binary literal");
        return NULL;
    }

    if (*pos == '.' && isdigit((unsigned char)pos[1])) {
        pos = flecs_script_digits(pos + 1, 10);
    }

    if (base == 10 && (*pos == 'e' || *pos == 'E')) {
        digits = pos + 1;
        digits += *digits == '+' || *digits == '-';
        if (isdigit((unsigned char)*digits)) {
            pos = flecs_script_digits(digits, 10);
        }
    }

    if ((!has_digits && base != 10) || flecs_script_is_identifier(*pos) ||
        isdigit((unsigned char)*pos))
    {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, start), "invalid number literal");
        return NULL;
    }

    out->kind = EcsTokNumber;
    out->value = flecs_tokenizer_copy(parser, start, pos);
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

static const char* flecs_script_string(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out) 
{
    const char *end = flecs_script_skip_string(parser, pos + 1, pos[0]);
    if (!end) {
        return NULL;
    }

    ecs_assert(end[0] == pos[0], ECS_INTERNAL_ERROR, NULL);

    bool is_char = pos[0] == '\'';
    int32_t len = flecs_ito(int32_t, end - (pos + 1));
    if (is_char && len == 0) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos), "Empty char");
        return NULL;
    } else if (is_char && (len > 1) && !((len == 2) && (pos[1] == '\\'))) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos), "only one char allowed");
        return NULL;
    }

    out->kind = is_char ? EcsTokChar : EcsTokString;
    out->value = flecs_tokenizer_copy(parser, pos + 1, end);
    return end + 1;
}

static void flecs_tokenizer_set_pos(
    ecs_parser_t *parser,
    const char *pos)
{
    parser->pos = parser->fixed_pos ? parser->fixed_pos : pos;
    if (!parser->stmt_pos) {
        parser->stmt_pos = parser->pos;
    }
}

const char* flecs_tokenizer_line(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out,
    bool stmt_end)
{
    const char *start = pos = flecs_scan_whitespace(parser, pos);
    flecs_tokenizer_set_pos(parser, pos);

    for (; pos[0] && pos[0] != '\n'; pos ++) {
        if (stmt_end && (pos[0] == ';' || flecs_is_comment(pos))) {
            break;
        }
    }

    const char *end = pos;
    while (end != start && isspace((unsigned char)end[-1])) {
        end --;
    }

    out->kind = EcsTokString;
    out->value = flecs_tokenizer_copy(parser, start, end);
    parser->token_keep = parser->token_cur;

    return pos;
}

static const char* flecs_token_scan(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out,
    bool is_lookahead)
{
    if (!pos) {
        return NULL;
    }

    pos = flecs_scan_whitespace_and_comment(parser, pos, NULL);
    if (!pos) {
        return NULL;
    }

    if (!is_lookahead) {
        flecs_tokenizer_set_pos(parser, pos);
    }

    out->kind = EcsTokUnknown;
    out->value = NULL;

    if (pos[0] == '\0') {
        out->kind = EcsTokEnd;
        return pos;
    } else if (pos[0] == '\n') {
        out->kind = EcsTokNewline;

        do {
            pos = flecs_scan_whitespace_and_comment(parser, pos + 1, NULL);
        } while (pos && pos[0] == '\n');

        return pos;

    } else if (isdigit((unsigned char)pos[0])) {
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

        if (flecs_tokens[ch].spelling && flecs_tokens[ch].spelling[0] == ch) {
            out->kind = (ecs_token_kind_t)ch;
            out->value = flecs_tokens[ch].spelling;
            return pos + 1;
        }
    }

    if (pos[0] == '\'' || pos[0] == '"' || pos[0] == '`') {
        return flecs_script_string(parser, pos, out);

    } else if (flecs_script_is_identifier(pos[0])) {
        pos = flecs_tokenizer_identifier(parser, pos, out);
        for (ecs_token_kind_t k = EcsTokKeywordWith; pos && k < EcsTokLast; k ++) {
            if (!ecs_os_strcmp(out->value, flecs_tokens[k].spelling)) {
                parser->token_cur = ECS_CONST_CAST(char*, out->value);
                out->kind = k;
                out->value = flecs_tokens[k].spelling;
            }
        }

        return pos;
    }

    ecs_parser_error(parser->name, parser->code,
        flecs_parser_errpos(parser, pos), "unknown token '%c'", pos[0]);

    return NULL;
}

const char* flecs_token(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out,
    bool is_lookahead)
{
    const char *result = flecs_token_scan(parser, pos, out, is_lookahead);

    if (result && !is_lookahead) {
        parser->token_keep = parser->token_cur;
    }

    if (result && out->kind != EcsTokNewline && out->kind != EcsTokEnd) {
        parser->token_ends[parser->token_ends_i & 7] = result;
        parser->token_ends_i ++;
    }

    return result;
}

void flecs_token_restore(
    ecs_parser_t *parser,
    const char *token_cur)
{
    if (token_cur > parser->token_keep) {
        parser->token_cur = ECS_CONST_CAST(char*, token_cur);
    } else {
        parser->token_cur = parser->token_keep;
    }
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
