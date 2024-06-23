/**
 * @file addons/script/parser.h
 * @brief Script grammar parser.
 * 
 * Macro utilities that facilitate a simple recursive descent parser.
 */

#ifndef FLECS_SCRIPT_PARSER_H
#define FLECS_SCRIPT_PARSER_H

#if defined(ECS_TARGET_CLANG)
/* Ignore unused enum constants in switch as it would blow up the parser code */
#pragma clang diagnostic ignored "-Wswitch-enum"
/* To allow for nested Parse statements */
#pragma clang diagnostic ignored "-Wshadow"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wshadow"
#endif

/* Create script & parser structs with static token buffer */
#define EcsParserFixedBuffer(w, script_name, expr, tokens, tokens_len)\
    ecs_script_impl_t script = {\
        .pub.world = ECS_CONST_CAST(ecs_world_t*, w),\
        .pub.name = script_name,\
        .pub.code = expr\
    };\
    ecs_script_parser_t parser = {\
        .script = flecs_script_impl(&script),\
        .pos = expr,\
        .token_cur = tokens\
    }

/* Definitions for parser functions */
#define ParserBegin\
    ecs_script_tokens_t token_stack = {0};\
    ecs_script_token_t *tokens = token_stack.tokens;\
    (void)tokens

#define ParserEnd\
        Error("unexpected end of rule (parser error)");\
    error:\
        return NULL

/* Get token */
#define Token(n) (tokens[n].value)

/* Error */
#define Error(...)\
    ecs_parser_error(parser->script->pub.name, parser->script->pub.code,\
        (pos - parser->script->pub.code) - 1, __VA_ARGS__);\
    goto error

/* Parse expression */
#define Expr(until, ...)\
    {\
        ecs_assert(token_stack.count < 256, ECS_INTERNAL_ERROR, NULL);\
        ecs_script_token_t *t = &token_stack.tokens[token_stack.count ++];\
        if (!(pos = flecs_script_expr(parser, pos, t, until))) {\
            goto error;\
        }\
        if (!t->value[0] && (until == '\n' || until == '{')) {\
            pos ++;\
            Error("empty expression");\
        }\
    }\
    Parse_1(until, __VA_ARGS__)

/* Parse token until character */
#define Until(until, ...)\
    {\
        ecs_assert(token_stack.count < 256, ECS_INTERNAL_ERROR, NULL);\
        ecs_script_token_t *t = &token_stack.tokens[token_stack.count ++];\
        if (!(pos = flecs_script_until(parser, pos, t, until))) {\
            goto error;\
        }\
    }\
    Parse_1(until, __VA_ARGS__)

/* Parse next token */
#define Parse(...)\
    {\
        ecs_assert(token_stack.count < 256, ECS_INTERNAL_ERROR, NULL);\
        ecs_script_token_t *t = &token_stack.tokens[token_stack.count ++];\
        if (!(pos = flecs_script_token(parser, pos, t, false))) {\
            goto error;\
        }\
        switch(t->kind) {\
            __VA_ARGS__\
        default:\
            if (t->value) {\
                Error("unexpected %s'%s'", \
                    flecs_script_token_kind_str(t->kind), t->value);\
            } else {\
                Error("unexpected %s", \
                    flecs_script_token_kind_str(t->kind));\
            }\
        }\
    }

/* Parse N consecutive tokens */
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

/* Same as Parse, but doesn't error out if token is not in handled cases */
#define LookAhead(...)\
    const char *lookahead;\
    ecs_script_token_t lookahead_token;\
    const char *old_lh_token_cur = parser->token_cur;\
    if ((lookahead = flecs_script_token(parser, pos, &lookahead_token, true))) {\
        token_stack.tokens[token_stack.count ++] = lookahead_token;\
        switch(lookahead_token.kind) {\
            __VA_ARGS__\
        default:\
            token_stack.count --;\
            break;\
        }\
        if (old_lh_token_cur > parser->token_keep) {\
            parser->token_cur = ECS_CONST_CAST(char*, old_lh_token_cur);\
        } else {\
            parser->token_cur = parser->token_keep;\
        }\
    }

/* Lookahead N consecutive tokens */
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
        pos = old_ptr;\
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
        pos = old_ptr;\
    )

/* Open scope */
#define Scope(s, ...) {\
        ecs_script_scope_t *old_scope = parser->scope;\
        parser->scope = s;\
        __VA_ARGS__\
        parser->scope = old_scope;\
    }

/* Parser loop */
#define Loop(...)\
    int32_t token_stack_count = token_stack.count;\
    do {\
        token_stack.count = token_stack_count;\
        __VA_ARGS__\
    } while (true);

#define EndOfRule return pos

#endif
