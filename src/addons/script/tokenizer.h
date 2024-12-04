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
    EcsTokShiftLeft = 109,
    EcsTokShiftRight = 110,
    EcsTokIdentifier = 111,
    EcsTokString = 112,
    EcsTokNumber = 113,
    EcsTokKeywordModule = 114,
    EcsTokKeywordUsing = 115,
    EcsTokKeywordWith = 116,
    EcsTokKeywordIf = 117,
    EcsTokKeywordElse = 118,
    EcsTokKeywordTemplate = 119,
    EcsTokKeywordProp = 120,
    EcsTokKeywordConst = 121,
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

#endif
