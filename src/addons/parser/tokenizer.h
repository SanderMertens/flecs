/**
 * @file addons/parser/tokenizer.h
 * @brief Parser tokenizer.
 */

#ifndef FLECS_PARSER_TOKENIZER_H
#define FLECS_PARSER_TOKENIZER_H

/* Tokenizer */
typedef enum ecs_token_kind_t {
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
    EcsTokChar = '\'',
    EcsTokGt = '>',
    EcsTokLt = '<',
    EcsTokEq = 128,
    EcsTokNeq,
    EcsTokGtEq,
    EcsTokLtEq,
    EcsTokAnd,
    EcsTokOr,
    EcsTokMatch,
    EcsTokRange,
    EcsTokShiftLeft,
    EcsTokShiftRight,
    EcsTokAddAssign,
    EcsTokMulAssign,
    EcsTokHasBracketOpen,
    EcsTokArrow,
    EcsTokIdentifier,
    EcsTokFunction,
    EcsTokString,
    EcsTokNumber,
    EcsTokKeywordWith,
    EcsTokKeywordUsing,
    EcsTokKeywordProp,
    EcsTokKeywordMut,
    EcsTokKeywordConst,
    EcsTokKeywordMatch,
    EcsTokKeywordNew,
    EcsTokKeywordExport,
    EcsTokKeywordIf,
    EcsTokKeywordElse,
    EcsTokKeywordFor,
    EcsTokKeywordIn,
    EcsTokKeywordTemplate,
    EcsTokKeywordModule,
    EcsTokKeywordInclude,
    EcsTokKeywordFn,
    EcsTokKeywordAwait,
    EcsTokKeywordTry,
    EcsTokKeywordCatch,
    EcsTokKeywordContinue,
    EcsTokKeywordAsync,
    EcsTokKeywordWhile,
    EcsTokKeywordScript,
    EcsTokLast
} ecs_token_kind_t;

typedef struct ecs_token_t {
    const char *value;
    ecs_token_kind_t kind;
} ecs_token_t;

typedef struct ecs_tokens_t {
    int32_t count;
    ecs_token_t tokens[256];
} ecs_tokens_t;

typedef struct ecs_tokenizer_t {
    ecs_tokens_t stack;
    ecs_token_t *tokens;
} ecs_tokenizer_t;

const char* flecs_tokenizer_until(
    ecs_parser_t *parser,
    const char *ptr,
    ecs_token_t *out,
    char until);

const char* flecs_token_kind_str(
    ecs_token_kind_t kind);

const char* flecs_token_str(
    ecs_token_kind_t kind);

const char* flecs_token(
    ecs_parser_t *parser,
    const char *ptr,
    ecs_token_t *out,
    bool is_lookahead);

const char* flecs_scan_whitespace(
    ecs_parser_t *parser,
    const char *pos);

const char* flecs_tokenizer_identifier(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_t *out);

int64_t flecs_parser_errpos(
    const ecs_parser_t *parser,
    const char *pos);

const char* flecs_parser_stmt_end(
    const ecs_parser_t *parser,
    const char *pos);

#endif
