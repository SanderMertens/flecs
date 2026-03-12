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
/* Suppress warnings that would be impractical to address in macro-heavy parser */
#pragma clang diagnostic ignored "-Wswitch-enum"
/* Nested Parse macros intentionally shadow variables */
#pragma clang diagnostic ignored "-Wshadow"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wshadow"
#elif defined(ECS_TARGET_MSVC)
/* Nested Parse macros intentionally shadow variables */
#pragma warning(disable : 4456)
#endif

/* Initialize script and parser with a caller-provided token buffer */
#define EcsParserFixedBuffer(w, script_name, expr, tokens, tokens_len)\
    ecs_script_impl_t script = {\
        .pub.world = ECS_CONST_CAST(ecs_world_t*, w),\
        .pub.name = script_name,\
        .pub.code = expr\
    };\
    ecs_parser_t parser = {\
        .script = flecs_script_impl(&script),\
        .name = script_name,\
        .code = expr,\
        .pos = expr,\
        .token_cur = tokens,\
        .token_end = &(tokens)[tokens_len]\
    }

/* Begin a parser function: initializes the tokenizer */
#define ParserBegin\
    ecs_tokenizer_t _tokenizer;\
    ecs_os_zeromem(&_tokenizer);\
    _tokenizer.tokens = _tokenizer.stack.tokens;\
    ecs_tokenizer_t *tokenizer = &_tokenizer;

#define ParserEnd\
        Error("unexpected end of rule (parser error)");\
    error:\
        return NULL

/* Access value of the nth parsed token */
#define Token(n) (tokenizer->tokens[n].value)

/* Save/restore token stack position for recursive parser functions */
#define TokenFramePush() \
    tokenizer->tokens = &tokenizer->stack.tokens[tokenizer->stack.count];

#define TokenFramePop() \
    tokenizer->tokens = tokenizer->stack.tokens;

/* Report parser error and jump to error label */
#define Error(...)\
    ecs_parser_error(parser->name, parser->code,\
        (pos - parser->code) - 1, __VA_ARGS__);\
    goto error

/* Report parser warning (non-fatal) */
#define Warning(...)\
    ecs_parser_warning(parser->name, parser->code,\
        (pos - parser->code) - 1, __VA_ARGS__);\

/* Parse a script expression, stopping at the delimiter character */
#define Expr(until, ...)\
    {\
        ecs_expr_node_t *EXPR = NULL;\
        if (until == '}' || until == ']') {\
            pos --;\
            if (until == '}') {\
                ecs_assert(pos[0] == '{', ECS_INTERNAL_ERROR, NULL);\
            } else if (until == ']') {\
                ecs_assert(pos[0] == '[', ECS_INTERNAL_ERROR, NULL);\
            }\
        }\
        parser->significant_newline = false;\
        if (!(pos = flecs_script_parse_expr(parser, pos, 0, &EXPR))) {\
            goto error;\
        }\
        parser->significant_newline = true;\
        __VA_ARGS__\
    }

/* Parse an initializer list, stopping at the delimiter character */
#define Initializer(until, ...)\
    {\
        ecs_expr_node_t *INITIALIZER = NULL;\
        ecs_expr_initializer_t *_initializer = NULL;\
        if (until != '\n') {\
            parser->significant_newline = false;\
        }\
        if (!(pos = flecs_script_parse_initializer(\
            parser, pos, until, &_initializer))) \
        {\
            flecs_expr_visit_free(\
                &parser->script->pub, (ecs_expr_node_t*)_initializer);\
            goto error;\
        }\
        parser->significant_newline = true;\
        if (pos[0] != until) {\
            if (until != '\n' || pos[0] != '\0') {\
                Error("expected '%c'", until);\
            }\
            if (pos[0] == '\0') {\
                pos --;\
            }\
        }\
        INITIALIZER = (ecs_expr_node_t*)_initializer;\
        pos ++;\
        __VA_ARGS__\
    }

/* Parse raw text until the specified delimiter character */
#define Until(until, ...)\
    {\
        ecs_assert(tokenizer->stack.count < 256, ECS_INTERNAL_ERROR, NULL);\
        ecs_token_t *t = &tokenizer->stack.tokens[tokenizer->stack.count ++];\
        if (!(pos = flecs_tokenizer_until(parser, pos, t, until))) {\
            goto error;\
        }\
    }\
    Parse_1(until, __VA_ARGS__)

/* Parse next token and switch on its kind */
#define Parse(...)\
    {\
        ecs_assert(tokenizer->stack.count < 256, ECS_INTERNAL_ERROR, NULL);\
        ecs_token_t *t = &tokenizer->stack.tokens[tokenizer->stack.count ++];\
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

/* Parse N consecutive tokens with expected kinds */
#define Parse_1(tok, ...)\
    Parse(\
        case tok: {\
            __VA_ARGS__\
        }\
    )

#define Parse_2(tok1, tok2, ...)\
    Parse_1(tok1, \
        Parse(\
            case tok2: {\
                __VA_ARGS__\
            }\
        )\
    )

#define Parse_3(tok1, tok2, tok3, ...)\
    Parse_2(tok1, tok2, \
        Parse(\
            case tok3: {\
                __VA_ARGS__\
            }\
        )\
    )

#define Parse_4(tok1, tok2, tok3, tok4, ...)\
    Parse_3(tok1, tok2, tok3, \
        Parse(\
            case tok4: {\
                __VA_ARGS__\
            }\
        )\
    )

#define Parse_5(tok1, tok2, tok3, tok4, tok5, ...)\
    Parse_4(tok1, tok2, tok3, tok4, \
        Parse(\
            case tok5: {\
                __VA_ARGS__\
            }\
        )\
    )

#define LookAhead_Keep() \
    pos = lookahead;\
    parser->token_keep = parser->token_cur

/* Peek at next token without consuming it; falls through if unhandled */
#define LookAhead(...)\
    const char *lookahead;\
    ecs_token_t lookahead_token;\
    const char *old_lh_token_cur = parser->token_cur;\
    if ((lookahead = flecs_token(parser, pos, &lookahead_token, true))) {\
        tokenizer->stack.tokens[tokenizer->stack.count ++] = lookahead_token;\
        switch(lookahead_token.kind) {\
            __VA_ARGS__\
        default:\
            tokenizer->stack.count --;\
            break;\
        }\
        if (old_lh_token_cur > parser->token_keep) {\
            parser->token_cur = ECS_CONST_CAST(char*, old_lh_token_cur);\
        } else {\
            parser->token_cur = parser->token_keep;\
        }\
    } else {\
        if (flecs_token(parser, pos, &lookahead_token, false)) {\
            if (lookahead_token.value) {\
                Error("unexpected %s'%s'", \
                    flecs_token_kind_str(lookahead_token.kind), \
                    lookahead_token.value);\
            } else {\
                Error("unexpected %s", \
                    flecs_token_kind_str(lookahead_token.kind));\
            }\
        }\
        goto error;\
    }

/* Peek at N consecutive tokens without consuming them */
#define LookAhead_1(tok, ...)\
    LookAhead(\
        case tok: {\
            __VA_ARGS__\
        }\
    )

#define LookAhead_2(tok1, tok2, ...)\
    LookAhead_1(tok1, \
        const char *old_ptr = pos;\
        pos = lookahead;\
        LookAhead(\
            case tok2: {\
                __VA_ARGS__\
            }\
        )\
        if (pos != lookahead) {\
            pos = old_ptr;\
        }\
    )

#define LookAhead_3(tok1, tok2, tok3, ...)\
    LookAhead_2(tok1, tok2, \
        const char *old_ptr = pos;\
        pos = lookahead;\
        LookAhead(\
            case tok3: {\
                __VA_ARGS__\
            }\
        )\
        if (pos != lookahead) {\
            pos = old_ptr;\
        }\
    )

/* Execute body with a temporary parser scope, then restore */
#define Scope(s, ...) {\
        ecs_script_scope_t *old_scope = parser->scope;\
        parser->scope = s;\
        __VA_ARGS__\
        parser->scope = old_scope;\
    }

/* Repeatedly parse tokens, resetting token stack between iterations */
#define Loop(...)\
    int32_t token_stack_count = tokenizer->stack.count;\
    do {\
        tokenizer->stack.count = token_stack_count;\
        __VA_ARGS__\
    } while (true);

#define EndOfRule return pos

#endif
