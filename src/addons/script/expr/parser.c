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
const char* flecs_script_parse_lhs(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_tokenizer_t *tokenizer,
    ecs_script_token_kind_t left_oper,
    ecs_expr_node_t **out);

static
void flecs_script_parser_expr_free(
    ecs_script_parser_t *parser,
    ecs_expr_node_t *node)
{
    flecs_script_expr_visit_free(&parser->script->pub, node);
}

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
ecs_entity_t flecs_script_default_lookup(
    const ecs_world_t *world,
    const char *name,
    void *ctx)
{
    (void)ctx;
    return ecs_lookup(world, name);
}

const char* flecs_script_parse_initializer(
    ecs_script_parser_t *parser,
    const char *pos,
    char until,
    ecs_expr_initializer_t **node_out)
{
    bool first = true;

    ecs_expr_initializer_t *node = *node_out = flecs_expr_initializer(parser);
    ecs_allocator_t *a = &parser->script->allocator;

    do {
        ParserBegin;

        if (first) {
            /* End of initializer */
            LookAhead(
                case ')':
                case '}': {
                    if ((char)lookahead_token.kind != until) {
                        Error("expected '%c'", until);
                    }
                    node->node.kind = EcsExprEmptyInitializer;
                    EndOfRule;
                })

            first = false;
        }

        ecs_expr_initializer_element_t *elem = ecs_vec_append_t(
            a, &node->elements, ecs_expr_initializer_element_t);
        ecs_os_zeromem(elem);

        {
            /* Parse member name */
            LookAhead_2(EcsTokIdentifier, ':', {
                elem->member = Token(0);
                LookAhead_Keep();
                pos = lookahead;
                break;
            })
        }

        pos = flecs_script_parse_expr(parser, pos, 0, &elem->value);
        if (!pos) {
            goto error;
        }

        {
            /* Parse next element or end of initializer*/
            LookAhead(
                case ',': {
                    pos = lookahead;
                    break;
                }
                case '\n':
                case ')':
                case '}': {
                    if ((char)lookahead_token.kind != until) {
                        Error("expected '%c'", until);
                    }
                    EndOfRule;
                }
            )
        }
    } while (true);

    ParserEnd;
}

static
const char* flecs_script_parse_collection_initializer(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_expr_initializer_t **node_out)
{
    bool first = true;
    
    ecs_expr_initializer_t *node = *node_out = flecs_expr_initializer(parser);
    ecs_allocator_t *a = &parser->script->allocator;

    do {
        ParserBegin;

        if (first) {
            /* End of initializer */
            LookAhead_1(']', {
                pos = lookahead;
                node->node.kind = EcsExprEmptyInitializer;
                EndOfRule;
            })

            first = false;
        }

        ecs_expr_initializer_element_t *elem = ecs_vec_append_t(
            a, &node->elements, ecs_expr_initializer_element_t);
        ecs_os_zeromem(elem);

        pos = flecs_script_parse_expr(parser, pos, 0, &elem->value);
        if (!pos) {
            goto error;
        }

        {
            /* Parse next element or end of initializer*/
            LookAhead(
                case ',': {
                    pos = lookahead;
                    break;
                }
                case ']': {
                    EndOfRule;
                }
            )
        }
    } while (true);

    ParserEnd;
}

static
const char* flecs_script_parse_rhs(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_tokenizer_t *tokenizer,
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
            case EcsTokParenOpen:
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
                    
                    *out = (ecs_expr_node_t*)result;

                    pos = flecs_script_parse_lhs(
                        parser, pos, tokenizer, 0, &result->index);
                    if (!pos) {
                        goto error;
                    }

                    Parse_1(']', {
                        break;
                    });

                    break;
                }

                case EcsTokMember: {
                    Parse_1(EcsTokIdentifier, {
                        ecs_expr_member_t *result = flecs_expr_member(parser);
                        result->left = *out;
                        result->member_name = Token(1);
                        *out = (ecs_expr_node_t*)result;
                        break;
                    });

                    break;
                }

                case EcsTokParenOpen: {
                    Parse_1(EcsTokParenClose, { 
                        ecs_expr_function_t *result = flecs_expr_function(parser);
                        result->left = *out;
                        *out = (ecs_expr_node_t*)result;
                        break; 
                    });

                    break;
                }

                default: {
                    ecs_expr_binary_t *result = flecs_expr_binary(parser);
                    result->left = *out;
                    result->operator = oper;

                    *out = (ecs_expr_node_t*)result;

                    pos = flecs_script_parse_lhs(parser, pos, tokenizer,
                        result->operator, &result->right);
                    if (!pos) {
                        goto error;
                    }

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
                char *end;
                *out = (ecs_expr_node_t*)flecs_expr_int(parser, 
                    strtoll(expr, &end, 10));
            } else {
                char *end;
                *out = (ecs_expr_node_t*)flecs_expr_uint(parser, 
                    strtoull(expr, &end, 10));
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

        case EcsTokNot: {
            ecs_expr_unary_t *node = flecs_expr_unary(parser);
            pos = flecs_script_parse_expr(parser, pos, EcsTokNot, &node->expr);
            if (!pos) {
                flecs_script_parser_expr_free(parser, (ecs_expr_node_t*)node);
                goto error;
            }

            node->operator = EcsTokNot;
            *out = (ecs_expr_node_t*)node;
            break;
        }

        case EcsTokSub: {
            ecs_expr_binary_t *node = flecs_expr_binary(parser);
            pos = flecs_script_parse_expr(parser, pos, 0, &node->right);
            if (!pos) {
                flecs_script_parser_expr_free(parser, (ecs_expr_node_t*)node);
                goto error;
            }

            node->left = (ecs_expr_node_t*)flecs_expr_int(parser, -1);
            node->operator = EcsTokMul;
            *out = (ecs_expr_node_t*)node;
            break;
        }

        case '(': {
            pos = flecs_script_parse_expr(parser, pos, 0, out);
            if (!pos) {
                goto error;
            }

            Parse_1(')', {
                break;
            })

            break;
        }

        case '{': {
            ecs_expr_initializer_t *node = NULL;
            pos = flecs_script_parse_initializer(parser, pos, '}', &node);
            if (!pos) {
                flecs_script_parser_expr_free(parser, (ecs_expr_node_t*)node);
                goto error;
            }

            Parse_1('}', {
                break;
            })

            *out = (ecs_expr_node_t*)node;
            break;
        }

        case '[': {
            ecs_expr_initializer_t *node = NULL;
            pos = flecs_script_parse_collection_initializer(parser, pos, &node);
            if (!pos) {
                flecs_script_parser_expr_free(parser, (ecs_expr_node_t*)node);
                goto error;
            }

            node->is_collection = true;

            Parse_1(']', {
                break;
            })

            *out = (ecs_expr_node_t*)node;
            break;
        }
    )

    TokenFramePop();

    if (!pos[0]) {
        return pos;
    }

    /* Parse right-hand side of expression if there is one */
    return flecs_script_parse_rhs(parser, pos, tokenizer, left_oper, out);
error:
    return NULL;
}

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

ecs_script_t* ecs_script_expr_parse(
    ecs_world_t *world,
    const char *expr,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_script_expr_run_desc_t priv_desc = {0};
    if (desc) {
        priv_desc = *desc;
    }

    if (!priv_desc.lookup_action) {
        priv_desc.lookup_action = flecs_script_default_lookup;
    }

    ecs_script_t *script = flecs_script_new(world);
    ecs_script_impl_t *impl = flecs_script_impl(script);

    ecs_script_parser_t parser = {
        .script = impl,
        .scope = impl->root,
        .significant_newline = false
    };

    impl->token_buffer_size = ecs_os_strlen(expr) * 2 + 1;
    impl->token_buffer = flecs_alloc(
        &impl->allocator, impl->token_buffer_size);
    parser.token_cur = impl->token_buffer;

    const char *ptr = flecs_script_parse_expr(&parser, expr, 0, &impl->expr);
    if (!ptr) {
        goto error;
    }

    impl->next_token = ptr;

    if (flecs_script_expr_visit_type(script, impl->expr, &priv_desc)) {
        goto error;
    }

    // printf("%s\n", ecs_script_ast_to_str(script));

    if (!desc || !desc->disable_folding) {
        if (flecs_script_expr_visit_fold(script, &impl->expr, &priv_desc)) {
            goto error;
        }
    }

    // printf("%s\n", ecs_script_ast_to_str(script));

    return script;
error:
    ecs_script_free(script);
    return NULL;
}

int ecs_script_expr_eval(
    const ecs_script_t *script,
    ecs_value_t *value,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_script_impl_t *impl = flecs_script_impl(
        /* Safe, won't be writing to script */
        ECS_CONST_CAST(ecs_script_t*, script));
    ecs_assert(impl->expr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_expr_run_desc_t priv_desc = {0};
    if (desc) {
        priv_desc = *desc;
    }

    if (!priv_desc.lookup_action) {
        priv_desc.lookup_action = flecs_script_default_lookup;
    }

    if (flecs_script_expr_visit_eval(script, impl->expr, &priv_desc, value)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

FLECS_API
const char* ecs_script_expr_run(
    ecs_world_t *world,
    const char *expr,
    ecs_value_t *value,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_script_expr_run_desc_t priv_desc = {0};
    if (desc) {
        priv_desc = *desc;
    }
    
    if (!priv_desc.type) {
        priv_desc.type = value->type;
    } else if (desc && (value->type != desc->type)) {
        ecs_throw(ECS_INVALID_PARAMETER, 
            "type of value parameter does not match desc->type");
    }

    ecs_script_t *s = ecs_script_expr_parse(world, expr, &priv_desc);
    if (!s) {
        goto error;
    }

    if (ecs_script_expr_eval(s, value, &priv_desc)) {
        ecs_script_free(s);
        goto error;
    }

    const char *result = flecs_script_impl(s)->next_token;

    ecs_script_free(s);

    return result;
error:
    return NULL;
}

#endif
