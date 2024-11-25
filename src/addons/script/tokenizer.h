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
    EcsTokEq,
    EcsTokNeq,
    EcsTokGt,
    EcsTokGtEq,
    EcsTokLt,
    EcsTokLtEq,
    EcsTokAnd,
    EcsTokOr,
    EcsTokMatch,
    EcsTokShiftLeft,
    EcsTokShiftRight,
    EcsTokIdentifier,
    EcsTokString,
    EcsTokNumber,
    EcsTokKeywordModule,
    EcsTokKeywordUsing,
    EcsTokKeywordWith,
    EcsTokKeywordIf,
    EcsTokKeywordElse,
    EcsTokKeywordTemplate,
    EcsTokKeywordProp,
    EcsTokKeywordConst,
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

const char* flecs_script_expr(
    ecs_script_parser_t *parser,
    const char *ptr,
    ecs_script_token_t *out,
    char until);

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
