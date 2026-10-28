/**
 * @file addons/script/expr/parser.c
 * @brief Script expression parser.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT

#include "../script.h"
#include "../../parser/grammar.h"
#include <errno.h>

/* From https://en.cppreference.com/w/c/language/operator_precedence */

static int flecs_expr_precedence[EcsTokLast] = {
    [EcsTokMember] = 1,
    [EcsTokBracketOpen] = 1,
    [EcsTokHasBracketOpen] = 1,
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

static const char* flecs_script_parse_lhs(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer,
    ecs_token_kind_t left_oper,
    ecs_expr_node_t **out);

static void flecs_script_parser_expr_free(
    ecs_parser_t *parser,
    ecs_expr_node_t *node)
{
    flecs_expr_visit_free(&parser->script->pub, node);
}

static bool flecs_has_precedence(
    ecs_token_kind_t first,
    ecs_token_kind_t second)
{
    if (!flecs_expr_precedence[first]) {
        return false;
    }

    return flecs_expr_precedence[first] <= flecs_expr_precedence[second];
}

static ecs_entity_t flecs_script_default_lookup(
    const ecs_world_t *world,
    const char *name,
    void *ctx)
{
    (void)ctx;
    return ecs_lookup(world, name);
}

static const char* flecs_script_parse_match_elems(
    ecs_parser_t *parser,
    const char *pos,
    ecs_expr_match_t *node)
{
    ecs_allocator_t *a = &parser->script->allocator;
    bool old_significant_newline = parser->significant_newline;
    parser->significant_newline = true;

    ecs_vec_init_t(NULL, &node->elements, ecs_expr_match_element_t, 0);

    do {
        ParserBegin;

        pos = flecs_parser_skip_newlines(parser, pos);

        LookAhead(
            case '}': {
                Consume();
                parser->significant_newline = old_significant_newline;
                EndOfRule;
            }
        )

        ecs_expr_match_element_t *elem = ecs_vec_append_t(
            a, &node->elements, ecs_expr_match_element_t);
        ecs_os_zeromem(elem);

        pos = flecs_script_parse_expr(parser, pos, 0, &elem->compare);
        if (!pos) {
            goto error;
        }

        Parse_1(':', {
            pos = flecs_script_parse_expr(parser, pos, 0, &elem->expr);
            if (!pos) {
                goto error;
            }

            LookAhead_1('}', {
                Consume();
                parser->significant_newline = old_significant_newline;
                EndOfRule;
            })

            Parse(
                case ';':
                case '\n':
                    break;
            )
        })

    } while (true);

    ParserEnd;
}

const char* flecs_script_parse_initializer(
    ecs_parser_t *parser,
    const char *pos,
    char until,
    ecs_expr_node_t **node_out)
{
    ecs_expr_initializer_t *node = flecs_expr_initializer(parser);
    *node_out = (ecs_expr_node_t*)node;
    ecs_allocator_t *a = &parser->script->allocator;

    do {
        ParserBegin;

        /* End of initializer */
        LookAhead(
            case ']':
            case ')':
            case '}': {
                Consume();
                if ((char)lookahead_token.kind != until) {
                    goto unexpected_end;
                }

                if (!ecs_vec_count(&node->elements)) {
                    node->node.kind = EcsExprEmptyInitializer;
                }

                EndOfRule;
            })

        ecs_expr_initializer_element_t *elem = ecs_vec_append_t(
            a, &node->elements, ecs_expr_initializer_element_t);
        ecs_os_zeromem(elem);

        if (until != ']') {
            LookAheadAny_2(case EcsTokIdentifier: EcsTokKeyword:, case ':':,
                elem->member = Token(0);
                Consume();
            )
        }

        int16_t old_elem_depth = parser->elem_depth;
        parser->elem_depth = flecs_ito(int16_t, parser->expr_depth + 1);
        pos = flecs_script_parse_expr(parser, pos, 0, &elem->value);
        parser->elem_depth = old_elem_depth;
        if (!pos) {
            goto error;
        }

        if (until == ']') {
            bool is_key = false;

            LookAhead_1(':', {
                Consume();
                is_key = true;
            })

            if (is_key) {
                elem->key = elem->value;
                elem->value = NULL;

                parser->elem_depth = flecs_ito(int16_t, parser->expr_depth + 1);
                pos = flecs_script_parse_expr(parser, pos, 0, &elem->value);
                parser->elem_depth = old_elem_depth;
                if (!pos) {
                    goto error;
                }
            }
        }

        if (until == ']' && !elem->key && ecs_vec_count(&node->elements) == 1) {
            bool is_range = false;

            LookAhead_1(EcsTokRange, {
                Consume();
                is_range = true;
            })

            /* [expr .. expr] is a range expression */
            if (is_range) {
                ecs_expr_range_t *range = flecs_expr_range(parser);
                range->from = elem->value;
                elem->value = NULL;
                ecs_vec_remove_last(&node->elements);
                flecs_script_parser_expr_free(parser, (ecs_expr_node_t*)node);
                *node_out = (ecs_expr_node_t*)range;

                pos = flecs_script_parse_expr(parser, pos, 0, &range->to);
                if (!pos) {
                    goto error;
                }

                Parse_1(']', {
                    EndOfRule;
                })
            }
        }

        {
            bool separated = false;

            /* Parse next element or end of initializer */
            LookAhead(
                case ',': {
                    Consume();
                    separated = true;
                    break;
                }

                case ']':
                case ')':
                case '}':
                    if ((char)lookahead_token.kind == until) {
                        Consume();
                        EndOfRule;
                    }

                    if (until == '\n' && lookahead_token.kind == '}') {
                        EndOfRule;
                    }

                    Consume();
                    goto unexpected_end;

                case ';':
                case '\n':
                case '\0':
                    if (until == '\n') {
                        EndOfRule;
                    }

                    if (lookahead_token.kind == '\0') {
                        Consume();
                        goto unexpected_end;
                    }

                    break;
            )

            if (!separated) {
                bool newline = false;
                const char *next = flecs_scan_whitespace_and_comment(
                    parser, pos, &newline);
                if (!next) {
                    goto error;
                }

                if (!newline) {
                    ecs_parser_error(parser->name, parser->code,
                        flecs_parser_errpos(parser, next),
                        until == '\n'
                            ? "expected ',' or newline"
                            : "expected ',' or newline between elements");
                    goto error;
                }
            }
        }
    } while (true);

unexpected_end:
    if (until == '\n') {
        Error("expected newline");
    }

    Error("expected '%c'", until);
error:
    flecs_script_parser_expr_free(parser, *node_out);
    *node_out = NULL;
    return NULL;
}

static const char* flecs_script_parse_function(
    ecs_parser_t *parser,
    const char *pos,
    ecs_expr_node_t **out) 
{
    ecs_expr_function_t *result = flecs_expr_function(parser);
    result->left = *out;

    ecs_expr_node_t *args = NULL;
    pos = flecs_script_parse_initializer(parser, pos, ')', &args);
    result->args = (ecs_expr_initializer_t*)args;
    if (!pos) {
        goto error;
    }

    *out = (ecs_expr_node_t*)result;

    return pos;
error:
    flecs_free_t(&parser->script->allocator, ecs_expr_function_t, result);
    return NULL;
}

static ecs_expr_node_t* flecs_script_parse_number(
    ecs_parser_t *parser,
    const char *expr,
    bool is_negative)
{
    int base = 10;
    if (expr[0] == '0' && (expr[1] == 'x' || expr[1] == 'X')) {
        base = 16;
    } else if (expr[0] == '0' && (expr[1] == 'b' || expr[1] == 'B')) {
        base = 2;
    }

    if (base == 10 &&
        (strchr(expr, '.') || strchr(expr, 'e') || strchr(expr, 'E')))
    {
        double v = atof(expr);
        return (ecs_expr_node_t*)flecs_expr_float(parser, is_negative ? -v : v);
    }

    errno = 0;
    uint64_t v = strtoull(base == 10 ? expr : expr + 2, NULL, base);
    if (errno == ERANGE) {
        return NULL;
    }

    if (!is_negative) {
        return (ecs_expr_node_t*)flecs_expr_uint(parser, v);
    }

    if (v > (uint64_t)INT64_MAX + 1) {
        return NULL;
    }

    return (ecs_expr_node_t*)flecs_expr_int(parser, (int64_t)(0 - v));
}

static const char* flecs_script_parse_rhs(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer,
    ecs_token_kind_t left_oper,
    ecs_expr_node_t **out)
{
    const char *last_pos = pos;

    do {
        TokenFramePush();

        last_pos = pos;

        bool elem_end = false;
        if (!parser->significant_newline &&
            parser->elem_depth == parser->expr_depth)
        {
            if (!flecs_scan_whitespace_and_comment(parser, pos, &elem_end)) {
                goto error;
            }
        }

        const char *old_token_cur = parser->token_cur;
        ecs_token_t lookahead_token;
        const char *lookahead = flecs_token(
            parser, pos, &lookahead_token, true);
        flecs_token_restore(parser, old_token_cur);
        if (!lookahead) {
            goto error;
        }

        ecs_token_kind_t oper = lookahead_token.kind;
        bool is_oper = oper != EcsTokNot && flecs_expr_precedence[oper];

        if (elem_end && (oper == EcsTokBracketOpen ||
            (oper == EcsTokSub && isdigit((unsigned char)lookahead[0]))))
        {
            is_oper = false;
        }

        if (is_oper && !flecs_has_precedence(left_oper, oper)) {
            pos = lookahead;

            switch(oper) {
            case EcsTokBracketOpen: {
                ecs_expr_element_t *result = flecs_expr_element(parser);
                result->left = *out;
                
                *out = (ecs_expr_node_t*)result;

                pos = flecs_script_parse_expr(
                    parser, pos, 0, &result->index);
                if (!pos) {
                    goto error;
                }

                Parse_1(']', {});

                break;
            }

            case EcsTokHasBracketOpen: {
                ecs_expr_has_t *result = flecs_expr_has(parser);
                result->left = *out;

                *out = (ecs_expr_node_t*)result;

                bool is_pair = false;

                LookAhead_1('(', {
                    Consume();
                    is_pair = true;
                })

                Parse_1(EcsTokIdentifier, {
                    result->first = (ecs_expr_node_t*)
                        flecs_expr_identifier(parser, t->value);
                });

                if (is_pair) {
                    Parse_1(',', {});

                    Parse_1(EcsTokIdentifier, {
                        result->second = (ecs_expr_node_t*)
                            flecs_expr_identifier(parser, t->value);
                    });

                    Parse_1(')', {});
                }

                Parse_1(']', {});

                break;
            }

            case EcsTokMember: {
                Parse(
                case EcsTokIdentifier:
                EcsTokKeyword: {
                    ecs_expr_member_t *result = flecs_expr_member(parser);
                    result->left = *out;
                    result->member_name = Token(0);
                    *out = (ecs_expr_node_t*)result;

                    if (pos[0] == '(') {
                        pos = flecs_script_parse_function(
                            parser, pos + 1, out);
                        if (!pos) {
                            goto error;
                        }
                    }

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
        }

        TokenFramePop();
    } while (pos != last_pos);

    return pos;
error:
    return NULL;
}

static const char* flecs_script_parse_lhs(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer,
    ecs_token_kind_t left_oper,
    ecs_expr_node_t **out)
{
    TokenFramePush();

    bool can_have_rhs = true;

    Parse(
        case EcsTokNumber: {
            *out = flecs_script_parse_number(parser, Token(0), false);
            if (!*out) {
                Error("integer literal '%s' is out of range", Token(0));
            }

            break;
        }

        case EcsTokChar: {
            *out = (ecs_expr_node_t*)flecs_expr_char(parser, Token(0));
            break;
        }

        case EcsTokString: {
            if (flecs_string_is_interpolated(Token(0))) {
                *out = (ecs_expr_node_t*)flecs_expr_interpolated_string(
                    parser, Token(0));
            } else {
                *out = (ecs_expr_node_t*)flecs_expr_string(parser, Token(0));
            }

            break;
        }

        case EcsTokIdentifier: {
            const char *expr = Token(0);
            if (expr[0] == '$') {
                *out = (ecs_expr_node_t*)flecs_expr_variable(parser, &expr[1]);
            } else if (!ecs_os_strcmp(expr, "this")) {
                *out = (ecs_expr_node_t*)flecs_expr_variable(parser, expr);
            } else if (!ecs_os_strcmp(expr, "true")) {
                *out = (ecs_expr_node_t*)flecs_expr_bool(parser, true);
            } else if (!ecs_os_strcmp(expr, "false")) {
                *out = (ecs_expr_node_t*)flecs_expr_bool(parser, false);
            } else {
                char *last_elem = strrchr(expr, '.');
                if (last_elem && last_elem[1] == '$') {
                    /* Scoped global variable */
                    ecs_expr_variable_t *v = flecs_expr_variable(parser, expr);
                    ecs_os_memmove(&last_elem[1], &last_elem[2],
                        ecs_os_strlen(&last_elem[2]) + 1);
                    v->node.kind = EcsExprGlobalVariable;
                    *out = (ecs_expr_node_t*)v;
                } else {
                    /* Entity identifier */
                    *out = (ecs_expr_node_t*)flecs_expr_identifier(parser, expr);
                }
            }

            if (pos[0] == '(') {
                pos = flecs_script_parse_function(parser, pos + 1, out);
                if (!pos) {
                    goto error;
                }
            }

            break;
        }

        case EcsTokNot:
        case EcsTokSub: {
            ecs_token_kind_t oper = t->kind;
            if (oper == EcsTokSub && isdigit((unsigned char)pos[0])) {
                Parse_1(EcsTokNumber, {
                    *out = flecs_script_parse_number(parser, Token(1), true);
                    if (!*out) {
                        Error("integer literal '-%s' is out of range",
                            Token(1));
                    }
                })
                break;
            }

            ecs_expr_unary_t *node = flecs_expr_unary(parser);
            pos = flecs_script_parse_expr(parser, pos, EcsTokNot, &node->expr);
            if (!pos) {
                flecs_script_parser_expr_free(parser, (ecs_expr_node_t*)node);
                goto error;
            }

            node->operator = oper;
            *out = (ecs_expr_node_t*)node;
            break;
        }

        case EcsTokKeywordMatch: {
            ecs_expr_match_t *node = flecs_expr_match(parser);
            *out = (ecs_expr_node_t*)node;

            pos = flecs_script_parse_expr(parser, pos, 0, &node->expr);
            if (!pos) {
                goto error;
            }

            Parse_1('{', {
                pos = flecs_script_parse_match_elems(parser, pos, node);
                if (!pos) {
                    goto error;
                }
            })

            can_have_rhs = false;

            break;
        }

        case EcsTokKeywordNew: {
            ecs_expr_new_t *node = flecs_expr_new(parser);
            *out = (ecs_expr_node_t*)node;

            int32_t stmt_count = ecs_vec_count(&parser->scope->stmts);

            bool old_significant_newline = parser->significant_newline;
            const char *old_stmt_pos = parser->stmt_pos;
            parser->significant_newline = true;
            pos = flecs_script_stmt(parser, pos);
            parser->significant_newline = old_significant_newline;
            parser->stmt_pos = old_stmt_pos;
            if (!pos) {
                goto error;
            }

            if ((ecs_vec_count(&parser->scope->stmts) - stmt_count) != 1) {
                Error("expected entity statement after new");
            }

            ecs_script_node_t *script_node = ecs_vec_last_t(
                &parser->scope->stmts, ecs_script_node_t*)[0];
            if (script_node->kind != EcsAstEntity) {
                Error("expected entity statement after new");
            }

            ecs_vec_remove_last(&parser->scope->stmts);

            node->entity = (ecs_script_entity_t*)script_node;

            can_have_rhs = false;

            break;
        }

        case EcsTokKeywordScript: {
            ecs_expr_script_t *node = flecs_expr_script(parser);
            *out = (ecs_expr_node_t*)node;

            Parse_1('{', {
                node->script = flecs_script_parse_nested(parser, pos, &pos);
                if (!node->script) {
                    goto error;
                }
            })

            can_have_rhs = false;
            break;
        }

        case '(': {
            pos = flecs_script_parse_expr(parser, pos, 0, out);
            if (!pos) {
                goto error;
            }

            Parse_1(')', {})

            break;
        }

        case '{':
        case '[': {
            char until = tokenizer->tokens[0].kind == '{' ? '}' : ']';
            ecs_expr_node_t *node = NULL;
            pos = flecs_script_parse_initializer(parser, pos, until, &node);
            if (!pos) {
                goto error;
            }

            if (until == ']' && (node->kind == EcsExprInitializer ||
                node->kind == EcsExprEmptyInitializer))
            {
                ((ecs_expr_initializer_t*)node)->is_collection = true;
            }

            *out = node;
            can_have_rhs = until == ']';
            break;
        }
    )

    TokenFramePop();

    /* Return if this was end of expression, or if the parsed expression cannot
     * have a right-hand side. */
    if (!pos[0] || !can_have_rhs) {
        return pos;
    }

    /* Parse right-hand side of expression if there is one */
    return flecs_script_parse_rhs(parser, pos, tokenizer, left_oper, out);
error:
    return NULL;
}

const char* flecs_script_parse_expr(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_kind_t left_oper,
    ecs_expr_node_t **out)
{
    if (parser->expr_depth >= ECS_PARSER_MAX_RECURSION_DEPTH) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos),
            "maximum expression nesting depth exceeded");
        return NULL;
    }

    parser->expr_depth ++;

    ParserBegin;

    pos = flecs_script_parse_lhs(parser, pos, tokenizer, left_oper, out);
    if (!pos) {
        parser->expr_depth --;
        if (out && *out) {
            flecs_script_parser_expr_free(parser, *out);
            *out = NULL;
        }

        return NULL;
    }

    parser->expr_depth --;

    if (out && *out && !(*out)->end) {
        (*out)->end = flecs_parser_stmt_end(parser, pos);
    }

    return pos;
}

ecs_script_t* ecs_expr_parse(
    ecs_world_t *world,
    const char *expr,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_eval_desc_t priv_desc = {0};
    if (desc) {
        priv_desc = *desc;
    }

    if (!priv_desc.lookup_action) {
        priv_desc.lookup_action = flecs_script_default_lookup;
    }

    ecs_parser_t parser;
    ecs_script_t *script = flecs_script_parse_init(world, NULL, expr, &parser);
    ecs_script_impl_t *impl = flecs_script_impl(script);
    parser.significant_newline = false;

    const char *ptr = flecs_script_parse_expr(
        &parser, script->code, 0, &impl->expr);
    if (!ptr) {
        goto error;
    }

    impl->next_token = &expr[ptr - script->code];
    impl->token_remaining = parser.token_cur;

    if (flecs_expr_visit_type(script, &impl->expr, &priv_desc)) {
        goto error;
    }

    if (!desc || !desc->disable_folding) {
        if (flecs_expr_visit_fold(script, &impl->expr, &priv_desc)) {
            goto error;
        }
    }

    return script;
error:
    ecs_script_free(script);
    return NULL;
}

int ecs_expr_eval(
    const ecs_script_t *script,
    ecs_value_t *value,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_script_impl_t *impl = flecs_script_impl(
        /* Safe, won't be writing to script */
        ECS_CONST_CAST(ecs_script_t*, script));
    ecs_assert(impl->expr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_expr_eval_desc_t priv_desc = {0};
    if (desc) {
        priv_desc = *desc;
    }

    if (!priv_desc.lookup_action) {
        priv_desc.lookup_action = flecs_script_default_lookup;
    }

    flecs_script_runtime_error_reset(
        flecs_script_runtime_get(script->world));

    if (flecs_expr_visit_eval(script, impl->expr, &priv_desc, value)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

const char* ecs_expr_run(
    ecs_world_t *world,
    const char *expr,
    ecs_value_t *value,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_eval_desc_t priv_desc = {0};
    if (desc) {
        priv_desc = *desc;
    }
    
    if (!priv_desc.type) {
        priv_desc.type = value->type;
    } else if (desc && (value->type != desc->type)) {
        ecs_throw(ECS_INVALID_PARAMETER, 
            "type of value parameter does not match desc->type");
    }

    ecs_script_t *s = ecs_expr_parse(world, expr, &priv_desc);
    if (!s) {
        goto error;
    }

    if (ecs_expr_eval(s, value, &priv_desc)) {
        ecs_script_free(s);
        goto error;
    }

    const char *result = flecs_script_impl(s)->next_token;

    ecs_script_free(s);

    return result;
error:
    return NULL;
}

char* ecs_script_string_interpolate(
    ecs_world_t *world,
    const char *str,
    const ecs_script_vars_t *vars)
{
    if (!flecs_string_is_interpolated(str)) {
        char *result = ecs_os_strdup(str);
        if (!flecs_string_escape(result)) {
            ecs_os_free(result);
            return NULL;
        }

        return result;
    }

    char *expr = flecs_asprintf("\"%s\"", str);

    ecs_expr_eval_desc_t desc = { .vars = vars };
    char *r = NULL;
    if (!ecs_expr_run(world, expr, &ecs_value_ptr(ecs_string_t, &r), &desc)) {
        ecs_os_free(expr);
        return NULL;
    }

    ecs_os_free(expr);

    return r;
}

#endif
