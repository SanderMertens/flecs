/**
 * @file addons/parser/grammar.h
 * @brief Grammar parser.
 * 
 * Macro utilities that facilitate a simple recursive descent parser.
 */

#ifndef FLECS_PARSER_GRAMMAR_H
#define FLECS_PARSER_GRAMMAR_H

#include "parser.h"

#if defined(ECS_TARGET_CLANG)
/* Ignore unused enum constants in switch as it would blow up the parser code */
#pragma clang diagnostic ignored "-Wswitch-enum"
/* To allow for nested Parse statements */
#pragma clang diagnostic ignored "-Wshadow"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wshadow"
#elif defined(ECS_TARGET_MSVC)
/* Allow for variable shadowing */
#pragma warning(disable : 4456)
#endif

#define EcsTokKeyword\
    case EcsTokKeywordWith: case EcsTokKeywordUsing: case EcsTokKeywordProp:\
    case EcsTokKeywordMut: case EcsTokKeywordConst: case EcsTokKeywordMatch:\
    case EcsTokKeywordNew: case EcsTokKeywordExport: case EcsTokKeywordIf:\
    case EcsTokKeywordElse: case EcsTokKeywordFor: case EcsTokKeywordIn:\
    case EcsTokKeywordTemplate: case EcsTokKeywordModule:\
    case EcsTokKeywordInclude: case EcsTokKeywordFn: case EcsTokKeywordAwait:\
    case EcsTokKeywordTry: case EcsTokKeywordCatch: case EcsTokKeywordAsync:\
    case EcsTokKeywordContinue: case EcsTokKeywordWhile: case EcsTokKeywordScript

/* Definitions for parser functions */
#define ParserBegin\
    ecs_tokenizer_t _tokenizer;\
    ecs_os_zeromem(&_tokenizer);\
    _tokenizer.tokens = _tokenizer.stack;\
    ecs_tokenizer_t *tokenizer = &_tokenizer;

#define ParserEnd\
        Error("unexpected end of rule (parser error)");\
    error:\
        return NULL

/* Get token */
#define Token(n) (tokenizer->tokens[n].value)

/* Push/pop token frame (allows token stack reuse in recursive functions) */
#define TokenFramePush() \
    int32_t _token_frame = tokenizer->count;\
    ecs_token_t *_token_frame_base = tokenizer->tokens;\
    tokenizer->tokens = &tokenizer->stack[_token_frame];

#define TokenFramePop() \
    tokenizer->count = _token_frame;\
    tokenizer->tokens = _token_frame_base;

/* Error */
#define Error(...)\
    ecs_parser_error(parser->name, parser->code,\
        flecs_parser_errpos(parser, pos - 1), __VA_ARGS__);\
    goto error

/* Warning */
#define Warning(...)\
    ecs_parser_warning(parser->name, parser->code,\
        flecs_parser_errpos(parser, pos - 1), __VA_ARGS__);\

/* Parse expression */
#define Expr(...)\
    {\
        ecs_expr_node_t *EXPR = NULL;\
        bool expr_significant_newline = parser->significant_newline;\
        parser->significant_newline = false;\
        parser->expr_pos = pos;\
        if (!(pos = flecs_script_parse_expr(parser, pos, 0, &EXPR))) {\
            goto error;\
        }\
        parser->significant_newline = expr_significant_newline;\
        parser->expr_end = pos;\
        __VA_ARGS__\
    }

/* Parse initializer */
#define Initializer(until, ...)\
    {\
        ecs_expr_node_t *INITIALIZER = NULL;\
        bool init_significant_newline = parser->significant_newline;\
        if (until != '\n') {\
            parser->significant_newline = false;\
        }\
        parser->expr_pos = (until == '\n') ? pos : pos - 1;\
        if (!(pos = flecs_script_parse_initializer(\
            parser, pos, until, &INITIALIZER))) \
        {\
            goto error;\
        }\
        parser->significant_newline = init_significant_newline;\
        parser->expr_end = (until == '\n')\
            ? flecs_parser_stmt_end(parser, pos)\
            : pos;\
        if (INITIALIZER && !INITIALIZER->end) {\
            INITIALIZER->end = parser->expr_end;\
        }\
        __VA_ARGS__\
    }

/* Parse next token */
#define Parse(...)\
    {\
        ecs_assert(tokenizer->count < ECS_PARSER_MAX_TOKENS,\
            ECS_INTERNAL_ERROR, NULL);\
        ecs_token_t *t = &tokenizer->stack[tokenizer->count ++];\
        if (!(pos = flecs_token(parser, pos, t, false))) {\
            goto error;\
        }\
        switch(t->kind) {\
            __VA_ARGS__\
        default:\
            if (t->value) {\
                Error("unexpected %s'%s'", \
                    flecs_token_kind_str(t->kind), t->value);\
            } else {\
                Error("unexpected %s", \
                    flecs_token_kind_str(t->kind));\
            }\
        }\
    }

/* Parse N consecutive tokens */
#define Parse_1(tok, ...)\
    Parse(\
        case tok: {\
            __VA_ARGS__\
        }\
        break;\
    )

#define Parse_2(tok1, tok2, ...)\
    Parse_1(tok1, Parse_1(tok2, __VA_ARGS__))

#define Parse_3(tok1, tok2, tok3, ...)\
    Parse_2(tok1, tok2, Parse_1(tok3, __VA_ARGS__))

#define Consume() \
    pos = lookahead;\
    parser->token_keep = parser->token_cur

/* Same as Parse, but doesn't error out if token is not in handled cases */
#define LookAhead(...)\
    {\
        const char *lookahead;\
        const char *lookahead_pos = pos;\
        int32_t lookahead_count = tokenizer->count;\
        ecs_token_t lookahead_token;\
        const char *old_lh_token_cur = parser->token_cur;\
        if ((lookahead = flecs_token(parser, pos, &lookahead_token, true))) {\
            ecs_assert(tokenizer->count < ECS_PARSER_MAX_TOKENS,\
                ECS_INTERNAL_ERROR, NULL);\
            tokenizer->stack[tokenizer->count ++] = lookahead_token;\
            switch(lookahead_token.kind) {\
                __VA_ARGS__\
            default:\
                break;\
            }\
            if (pos <= lookahead_pos) {\
                tokenizer->count = lookahead_count;\
            }\
            flecs_token_restore(parser, old_lh_token_cur);\
        } else {\
            goto error;\
        }\
    }

/* Lookahead N consecutive tokens */
#define LookAhead_1(tok, ...)\
    LookAhead(\
        case tok: {\
            __VA_ARGS__\
        }\
    )

#define LookAheadAny_2(cases1, cases2, ...)\
    LookAhead(cases1 {\
        const char *lookahead_start = pos;\
        pos = lookahead;\
        LookAhead(cases2 {\
            pos = lookahead_start;\
            __VA_ARGS__\
        })\
        if (pos == lookahead) {\
            pos = lookahead_start;\
        }\
    })

#define LookAhead_2(tok1, tok2, ...)\
    LookAheadAny_2(case tok1:, case tok2:, __VA_ARGS__)

#define LookAheadSkipNewlines_1(tok, ...)\
    {\
        const char *stmt_end = pos;\
        pos = flecs_parser_skip_newlines(parser, pos);\
        LookAhead_1(tok, __VA_ARGS__)\
        pos = stmt_end;\
    }

/* Open scope */
#define Scope(s, ...) {\
        ecs_script_scope_t *old_scope = parser->scope;\
        parser->scope = s;\
        __VA_ARGS__\
        parser->scope = old_scope;\
    }

/* Parser loop */
#define Loop(...)\
    int32_t token_stack_count = tokenizer->count;\
    do {\
        tokenizer->count = token_stack_count;\
        __VA_ARGS__\
    } while (true);

#define EndOfRule return pos

#endif
