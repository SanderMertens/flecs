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
    EcsTokColon = ':',
    EcsTokScopeOpen = '{',
    EcsTokScopeClose = '}',
    EcsTokParenOpen = '(',
    EcsTokParenClose = ')',
    EcsTokBracketOpen = '[',
    EcsTokBracketClose = ']',
    EcsTokComma = ',',
    EcsTokSemiColon = ';',
    EcsTokAnnotation = '@',
    EcsTokAssign = '=',
    EcsTokNewline = '\n',
    EcsTokIdentifier,
    EcsTokKeywordModule,
    EcsTokKeywordUsing,
    EcsTokKeywordWith,
    EcsTokKeywordIf,
    EcsTokKeywordElse,
    EcsTokKeywordAssembly,
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

const char* flecs_script_token(
    ecs_script_parser_t *parser,
    const char *ptr,
    ecs_script_token_t *out,
    bool is_lookahead);

#endif
