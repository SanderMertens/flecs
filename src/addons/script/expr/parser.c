/**
 * @file addons/script/expr/parser.c
 * @brief Script expression parser.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"
#include "../parser.h"

/* From https://en.cppreference.com/w/c/language/operator_precedence */

static int flecs_expr_precedence[] = {
    [EcsTokParenOpen] = 1,
    [EcsTokMember] = 1,
    [EcsTokBracketOpen] = 1,
    [EcsTokNot] = 2,
    [EcsTokMul] = 3,
    [EcsTokDiv] = 3,
    [EcsTokMod] = 3,
    [EcsTokAdd] = 4,
    [EcsTokSub] = 4,
    [EcsTokShiftLeft] = 5,
    [EcsTokShiftRight] = 5,
    [EcsTokGt] = 6,
    [EcsTokGtEq] = 6,
    [EcsTokLt] = 6,
    [EcsTokLtEq] = 6,
    [EcsTokEq] = 7,
    [EcsTokNeq] = 7,
    [EcsTokBitwiseAnd] = 8,
    [EcsTokBitwiseOr] = 10,
    [EcsTokAnd] = 11,
    [EcsTokOr] = 12,
};

static
const char* flecs_script_parse_expr(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_token_kind_t left_oper,
    ecs_expr_node_t **out);

static
const char* flecs_script_parse_lhs(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_tokenizer_t *tokenizer,
    ecs_script_token_kind_t left_oper,
    ecs_expr_node_t **out);

static
bool flecs_has_precedence(
    ecs_script_token_kind_t first,
    ecs_script_token_kind_t second)
{
    if (!flecs_expr_precedence[first]) {
        return false;
    }
    return flecs_expr_precedence[first] <= flecs_expr_precedence[second];
}

static
const char* flecs_script_parse_rhs(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_tokenizer_t *tokenizer,
    ecs_expr_node_t *left,
    ecs_script_token_kind_t left_oper,
    ecs_expr_node_t **out)
{
    const char *last_pos = pos;

    do {
        TokenFramePush();

        last_pos = pos;

        LookAhead(
            case EcsTokAdd:
            case EcsTokSub:
            case EcsTokMul:
            case EcsTokDiv:
            case EcsTokMod:
            case EcsTokBitwiseOr:
            case EcsTokBitwiseAnd:
            case EcsTokEq:
            case EcsTokNeq:
            case EcsTokGt:
            case EcsTokGtEq:
            case EcsTokLt:
            case EcsTokLtEq:
            case EcsTokAnd:
            case EcsTokOr:
            case EcsTokShiftLeft:
            case EcsTokShiftRight: 
            case EcsTokBracketOpen:
            case EcsTokMember:
            {
                ecs_script_token_kind_t oper = lookahead_token.kind;

                /* Only consume more tokens if operator has precedence */
                if (flecs_has_precedence(left_oper, oper)) {
                    break;
                }

                /* Consume lookahead token */
                pos = lookahead;

                switch(oper) {
                case EcsTokBracketOpen: {
                    ecs_expr_element_t *result = flecs_expr_element(parser);
                    result->left = *out;

                    pos = flecs_script_parse_lhs(
                        parser, pos, tokenizer, 0, &result->index);
                    if (!pos) {
                        goto error;
                    }

                    Parse_1(']', {
                        *out = (ecs_expr_node_t*)result;
                        break;
                    });

                    break;
                }

                case EcsTokMember: {
                    ecs_expr_member_t *result = flecs_expr_member(parser);
                    result->left = *out;

                    Parse_1(EcsTokIdentifier, {
                        result->member_name = Token(1);
                        *out = (ecs_expr_node_t*)result;
                        break;
                    });

                    break;
                }

                default: {
                    ecs_expr_binary_t *result = flecs_expr_binary(parser);
                    result->left = *out;
                    result->operator = oper;

                    pos = flecs_script_parse_lhs(parser, pos, tokenizer,
                        result->operator, &result->right);
                    if (!pos) {
                        goto error;
                    }
                    *out = (ecs_expr_node_t*)result;
                    break;
                }
                };

                /* Ensures lookahead tokens in token buffer don't get overwritten */
                parser->token_keep = parser->token_cur;

                break;
            }
        )

        TokenFramePop();
    } while (pos != last_pos);

    return pos;
error:
    return NULL;
}

static
const char* flecs_script_parse_lhs(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_tokenizer_t *tokenizer,
    ecs_script_token_kind_t left_oper,
    ecs_expr_node_t **out)
{
    TokenFramePush();

    Parse(
        case EcsTokNumber: {
            const char *expr = Token(0);
            if (strchr(expr, '.') || strchr(expr, 'e')) {
                *out = (ecs_expr_node_t*)flecs_expr_float(parser, atof(expr));
            } else if (expr[0] == '-') {
                *out = (ecs_expr_node_t*)flecs_expr_int(parser, atoll(expr));
            } else {
                *out = (ecs_expr_node_t*)flecs_expr_uint(parser, atoll(expr));
            }
            break;
        }

        case EcsTokString: {
            *out = (ecs_expr_node_t*)flecs_expr_string(parser, Token(0));
            break;
        }

        case EcsTokIdentifier: {
            const char *expr = Token(0);
            if (expr[0] == '$') {
                *out = (ecs_expr_node_t*)flecs_expr_variable(parser, &expr[1]);
            } else if (!ecs_os_strcmp(expr, "true")) {
                *out = (ecs_expr_node_t*)flecs_expr_bool(parser, true);
            } else if (!ecs_os_strcmp(expr, "false")) {
                *out = (ecs_expr_node_t*)flecs_expr_bool(parser, false);
            } else {
                *out = (ecs_expr_node_t*)flecs_expr_identifier(parser, expr);
            }
            break;
        }

        case EcsTokParenOpen: {
            pos = flecs_script_parse_expr(parser, pos, 0, out);
            Parse_1(EcsTokParenClose, {
                break;
            })
            break;
        }

        case EcsTokNot: {
            ecs_expr_unary_t *unary = flecs_expr_unary(parser);
            pos = flecs_script_parse_expr(parser, pos, EcsTokNot, &unary->expr);
            unary->operator = EcsTokNot;
            *out = (ecs_expr_node_t*)unary;
            break;
        }

        case EcsTokSub: {
            ecs_expr_binary_t *binary = flecs_expr_binary(parser);
            pos = flecs_script_parse_expr(parser, pos, 0, &binary->right);
            binary->left = (ecs_expr_node_t*)flecs_expr_int(parser, -1);
            binary->operator = EcsTokMul;
            *out = (ecs_expr_node_t*)binary;
            break;
        }
    )

    TokenFramePop();

    /* Parse right-hand side of expression if there is one */
    return flecs_script_parse_rhs(
        parser, pos, tokenizer, *out, left_oper, out);
error:
    return NULL;
}

static
const char* flecs_script_parse_expr(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_token_kind_t left_oper,
    ecs_expr_node_t **out)
{
    ParserBegin;

    pos = flecs_script_parse_lhs(parser, pos, tokenizer, left_oper, out);

    EndOfRule;

    ParserEnd;
}

ecs_expr_node_t* ecs_script_parse_expr(
    ecs_world_t *world,
    ecs_script_t *script,
    const char *name,
    const char *expr) 
{
    if (!script) {
        script = flecs_script_new(world);
    }

    ecs_script_parser_t parser = {
        .script = flecs_script_impl(script),
        .scope = flecs_script_impl(script)->root,
        .significant_newline = false
    };

    ecs_script_impl_t *impl = flecs_script_impl(script);

    impl->token_buffer_size = ecs_os_strlen(expr) * 2 + 1;
    impl->token_buffer = flecs_alloc(
        &impl->allocator, impl->token_buffer_size);
    parser.token_cur = impl->token_buffer;

    ecs_expr_node_t *out = NULL;

    const char *result = flecs_script_parse_expr(&parser, expr, 0, &out);
    if (!result) {
        goto error;
    }

    if (flecs_script_expr_visit_type(script, out)) {
        goto error;
    }

    if (flecs_script_expr_visit_fold(script, &out)) {
        goto error;
    }

    return out;
error:
    return NULL;
}

FLECS_API
const char* ecs_script_expr_run(
    ecs_world_t *world,
    const char *expr,
    ecs_value_t *value,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_script_t *script = flecs_script_new(world);

    ecs_script_parser_t parser = {
        .script = flecs_script_impl(script),
        .scope = flecs_script_impl(script)->root,
        .significant_newline = false
    };

    ecs_script_impl_t *impl = flecs_script_impl(script);

    impl->token_buffer_size = ecs_os_strlen(expr) * 2 + 1;
    impl->token_buffer = flecs_alloc(
        &impl->allocator, impl->token_buffer_size);
    parser.token_cur = impl->token_buffer;

    ecs_expr_node_t *out = NULL;

    const char *result = flecs_script_parse_expr(&parser, expr, 0, &out);
    if (!result) {
        goto error;
    }

    if (flecs_script_expr_visit_type(script, out)) {
        goto error;
    }

    // printf("%s\n", ecs_script_expr_to_str(world, out));

    if (flecs_script_expr_visit_fold(script, &out)) {
        goto error;
    }

    // printf("%s\n", ecs_script_expr_to_str(world, out));

    if (!value->type) {
        value->type = out->type;
    }

    if (value->type && !value->ptr) {
        value->ptr = ecs_value_new(world, value->type);
    }

    ecs_assert(value->type != 0 && value->ptr != NULL, ECS_INVALID_OPERATION, 
        "failed to allocate storage for expression result");

    if (out->kind == EcsExprValue) {
        if (value->type == out->type) {
            // Output value is same as expression, copy value
            ecs_value_copy(world, value->type, value->ptr, 
                ((ecs_expr_val_t*)out)->ptr);
        } else {
            // Cast value to desired output type
            ecs_meta_cursor_t cur = ecs_meta_cursor(
                script->world, value->type, value->ptr);

            ecs_value_t expr_result = {
                .type = out->type,
                .ptr = ((ecs_expr_val_t*)out)->ptr
            };

            if (ecs_meta_set_value(&cur, &expr_result)) {
                goto error;
            }
        }
    } else {
        ecs_abort(ECS_UNSUPPORTED, "can't evaluate dynamic expressions yet");
    }

    return result;
error:
    return NULL;
}

#endif
