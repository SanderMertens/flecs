/**
 * @file addons/script/parser.h
 * @brief Flecs script parser.
 */

#ifndef FLECS_SCRIPT_PARSER_H
#define FLECS_SCRIPT_PARSER_H

#if defined(ECS_TARGET_CLANG)
/* Ignore unused enum constants in switch as it would blow up the parser code */
#pragma clang diagnostic ignored "-Wswitch-enum"
/* To allow for nested Parse statements */
#pragma clang diagnostic ignored "-Wshadow"
#endif

/* Definitions for parser functions */
#define ParserBegin\
    ecs_script_tokens_t token_stack = {0};\
    ecs_script_token_t *tokens = token_stack.tokens;\
    (void)tokens

#define ParserEnd\
    error:\
        return NULL

/* Get token */
#define Token(n) (tokens[n].value)

/* Error */
#define Error(parser, ...)\
    ecs_parser_error(parser->script->name, parser->script->code,\
        (pos - parser->script->code) - 1, __VA_ARGS__);\
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
            Error(parser, "empty expression");\
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
                Error(parser, "unexpected %s'%s'", \
                    flecs_script_token_kind_str(t->kind), t->value);\
            } else {\
                Error(parser, "unexpected %s", \
                    flecs_script_token_kind_str(t->kind));\
            }\
        }\
    }

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

/* Same as Parse, but doesn't error out if token is not in handled cases */
#define LookAhead(...)\
    const char *lookahead;\
    ecs_script_token_t lookahead_token;\
    if ((lookahead = flecs_script_token(parser, pos, &lookahead_token, true))) {\
        token_stack.tokens[token_stack.count ++] = lookahead_token;\
        switch(lookahead_token.kind) {\
            __VA_ARGS__\
        default:\
            token_stack.count --;\
            break;\
        }\
    }

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

#define Scope(s, ...) {\
        ecs_script_scope_t *old_scope = parser->scope;\
        parser->scope = s;\
        __VA_ARGS__\
        parser->scope = old_scope;\
    }

#define Loop(...)\
    int32_t token_stack_count = token_stack.count;\
    do {\
        token_stack.count = token_stack_count;\
        __VA_ARGS__\
    } while (true);

#define EndOfRule return pos

#endif
