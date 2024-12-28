/**
 * @file addons/script/tokenizer.h
 * @brief Script tokenizer.
 */

#ifndef FLECS_SCRIPT_TOKENIZER_H
#define FLECS_SCRIPT_TOKENIZER_H

/* Tokenizer */
typedef enum ecs_script_token_kind_t {
    EcsTokEnd = '\0',
    EcsTokUnknown,
    EcsTokScopeOpen = '{',
    EcsTokScopeClose = '}',
    EcsTokParenOpen = '(',
    EcsTokParenClose = ')',
    EcsTokBracketOpen = '[',
    EcsTokBracketClose = ']',
    EcsTokMember = '.',
    EcsTokComma = ',',
    EcsTokSemiColon = ';',
    EcsTokColon = ':',
    EcsTokAssign = '=',
    EcsTokAdd = '+',
    EcsTokSub = '-',
    EcsTokMul = '*',
    EcsTokDiv = '/',
    EcsTokMod = '%',
    EcsTokBitwiseOr = '|',
    EcsTokBitwiseAnd = '&',
    EcsTokNot = '!',
    EcsTokOptional = '?',
    EcsTokAnnotation = '@',
    EcsTokNewline = '\n',
    EcsTokEq = 100,
    EcsTokNeq = 101,
    EcsTokGt = 102,
    EcsTokGtEq = 103,
    EcsTokLt = 104,
    EcsTokLtEq = 105,
    EcsTokAnd = 106,
    EcsTokOr = 107,
    EcsTokMatch = 108,
    EcsTokRange = 109,
    EcsTokShiftLeft = 110,
    EcsTokShiftRight = 111,
    EcsTokIdentifier = 112,
    EcsTokString = 113,
    EcsTokNumber = 114,
    EcsTokKeywordModule = 115,
    EcsTokKeywordUsing = 116,
    EcsTokKeywordWith = 117,
    EcsTokKeywordIf = 118,
    EcsTokKeywordFor = 119,
    EcsTokKeywordIn = 120,
    EcsTokKeywordElse = 121,
    EcsTokKeywordTemplate = 122,
    EcsTokKeywordProp = 130,
    EcsTokKeywordConst = 131,
    EcsTokKeywordMatch = 132,
    EcsTokAddAssign = 133,
    EcsTokMulAssign = 134,
} ecs_script_token_kind_t;

typedef struct ecs_script_token_t {
    const char *value;
    ecs_script_token_kind_t kind;
} ecs_script_token_t;

typedef struct ecs_script_tokens_t {
    int32_t count;
    ecs_script_token_t tokens[256];
} ecs_script_tokens_t;

typedef struct ecs_script_tokenizer_t {
    ecs_script_tokens_t stack;
    ecs_script_token_t *tokens;
} ecs_script_tokenizer_t;

const char* flecs_script_until(
    ecs_script_parser_t *parser,
    const char *ptr,
    ecs_script_token_t *out,
    char until);

const char* flecs_script_token_kind_str(
    ecs_script_token_kind_t kind);

const char* flecs_script_token_str(
    ecs_script_token_kind_t kind);

const char* flecs_script_token(
    ecs_script_parser_t *parser,
    const char *ptr,
    ecs_script_token_t *out,
    bool is_lookahead);

const char* flecs_scan_whitespace(
    ecs_script_parser_t *parser,
    const char *pos);

const char* flecs_script_identifier(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_token_t *out);

#endif
