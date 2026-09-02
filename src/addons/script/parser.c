/**
 * @file addons/script/parser.c
 * @brief Script grammar parser.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT

#include "../parser/grammar.h"
#include "script.h"

#define EcsTokEndOfStatement\
    case ';':\
    case '\n':\
    case '\0'

static void flecs_script_initializer_set_full(
    ecs_expr_node_t *node)
{
    if (node->kind == EcsExprInitializer) {
        ((ecs_expr_initializer_t*)node)->is_partial = false;
    }
}

static const char* flecs_script_skip_newlines(
    ecs_parser_t *parser,
    const char *pos)
{
    const char *old_token_cur = parser->token_cur;
    const char *lookahead;
    ecs_token_t token;

    while ((lookahead = flecs_token(parser, pos, &token, true))) {
        if (token.kind != EcsTokNewline) {
            break;
        }

        pos = lookahead;
    }

    if (old_token_cur > parser->token_keep) {
        parser->token_cur = ECS_CONST_CAST(char*, old_token_cur);
    } else {
        parser->token_cur = parser->token_keep;
    }

    return pos;
}

/* Parse scope (statements inside {}) */
static const char* flecs_script_scope(
    ecs_parser_t *parser,
    ecs_script_scope_t *scope,
    const char *pos)
{
    ParserBegin;

    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pos[-1] == '{', ECS_INTERNAL_ERROR, NULL);

    if (parser->scope_depth >= ECS_PARSER_MAX_RECURSION_DEPTH) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos),
            "maximum scope nesting depth exceeded");
        return NULL;
    }

    parser->scope_depth ++;

    ecs_script_scope_t *prev = parser->scope;
    parser->scope = scope;

    Loop(
        LookAhead(
            case EcsTokScopeClose:
                pos = lookahead;
                goto scope_close;
            case EcsTokEnd:
                Error("unexpected end of script");
                goto error;
        )

        pos = flecs_script_stmt_w_separator(parser, pos);
        if (!pos) {
            goto error;
        }
    )

scope_close:
    parser->scope = prev;
    parser->scope_depth --;

    ecs_assert(pos[-1] == '}', ECS_INTERNAL_ERROR, NULL);
    return pos;

    Error("unexpected end of rule (parser error)");
error:
    parser->scope = prev;
    parser->scope_depth --;
    return NULL;
}

/* Parse scope of entity */
static const char* flecs_script_entity_scope(
    ecs_parser_t *parser,
    ecs_script_entity_t *entity,
    const char *pos)
{
    if (!entity) {
        return NULL;
    }

    return flecs_script_scope(parser, entity->scope, pos);
}

/* Parse base list (bases separated by ',') */
static const char* flecs_script_base_list(
    ecs_parser_t *parser,
    const char *pos)
{
    ParserBegin;

    Loop(
        LookAhead(
            case '\n':
                pos = lookahead;
                continue;

            case EcsTokIdentifier:
                LookAhead_Keep();

                flecs_script_insert_pair_tag(parser, "IsA", Token(0));

                LookAhead_1(',',
                    pos = lookahead;
                    continue;
                )
        )

        break;
    )

    return pos;
error:
    return NULL;
}

/* Parse with expression (expression after 'with' keyword) */
static const char* flecs_script_with_expr(
    ecs_parser_t *parser,
    const char *pos)
{
    ParserBegin;

    Parse(
        // Position
        case EcsTokIdentifier: {
            // Position (
            LookAhead_1('(',
                pos = lookahead;

                // Position ( expr )
                Initializer(')',
                    ecs_script_component_t *component =
                        flecs_script_insert_component(parser, Token(0));
                    component->node.kind = EcsAstWithComponent;
                    component->expr = INITIALIZER;
                    flecs_script_initializer_set_full(component->expr);
                    EndOfRule;
                )
            )

            ecs_script_tag_t *tag =
                flecs_script_insert_tag(parser, Token(0));
            tag->node.kind = EcsAstWithTag;

            EndOfRule;
        }

        // (
        case '(':
            // (Eats, Apples)
            Parse_4(EcsTokIdentifier, ',', EcsTokIdentifier, ')',
                // (Eats, Apples) ( expr
                LookAhead_1('(',
                    pos = lookahead;

                    // (Eats, Apples) ( expr )
                    Initializer(')',
                        ecs_script_component_t *component =
                            flecs_script_insert_pair_component(parser,
                                Token(1), Token(3));
                        component->node.kind = EcsAstWithComponent;
                        component->expr = INITIALIZER;
                        flecs_script_initializer_set_full(component->expr);
                        EndOfRule;
                    )
                )

                ecs_script_tag_t *tag = 
                    flecs_script_insert_pair_tag(parser, Token(1), Token(3));
                tag->node.kind = EcsAstWithTag;
                EndOfRule;
            )
    )

    ParserEnd;
}

/* Parse with expression list (expression list after 'with' keyword) */
static const char* flecs_script_with(
    ecs_parser_t *parser,
    ecs_script_with_t *with,
    const char *pos)
{
    ParserBegin;

    bool has_next;
    do {
        Scope(with->expressions, 
            pos = flecs_script_with_expr(parser, pos);
        )

        if (!pos) {
            goto error;
        }

    next:
        Parse(
            case ',': {
                has_next = true;
                break;
            }
            case EcsTokNewline: {
                goto next;
            }
            case '{': {
                return flecs_script_scope(parser, with->scope, pos);
            }
        )
    } while (has_next);

    ParserEnd;
}

/* Parenthesis expression */
static const char* flecs_script_paren_expr(
    ecs_parser_t *parser,
    const char *kind,
    ecs_script_entity_t *entity,
    const char *pos)
{
    ParserBegin;

    Initializer(')',
        entity->kind_w_expr = true;

        Scope(entity->scope,
            ecs_script_component_t *component =
                flecs_script_insert_component(parser, kind);
            component->expr = INITIALIZER;
            flecs_script_initializer_set_full(component->expr);
        )

        Parse(
            // Position spaceship(expr) }
            //   This can happen when used as new expression.
            case '}':
                pos --;

            // Position spaceship (expr)\n
            EcsTokEndOfStatement: {
                EndOfRule;
            }

            // Position spaceship (expr) {
            case '{': {
                return flecs_script_entity_scope(parser, entity, pos);
            }
        )
    )

    ParserEnd;
}

/* Parse a single statement */
static const char* flecs_script_if_stmt(
    ecs_parser_t *parser,
    const char *pos)
{
    ParserBegin;

    // if expr
    Expr('\0',
        pos = flecs_script_skip_newlines(parser, pos);

        // if expr {
        Parse_1('{', {
            ecs_script_if_t *stmt = flecs_script_insert_if(parser);
            stmt->expr = EXPR;
            pos = flecs_script_scope(parser, stmt->if_true, pos);
            if (!pos) {
                goto error;
            }

            // if expr { }\n else
            {
                const char *stmt_pos = pos;
                pos = flecs_script_skip_newlines(parser, pos);

                LookAhead_1(EcsTokKeywordElse,
                    // if expr { } else\n
                    pos = flecs_script_skip_newlines(parser, lookahead);

                    Parse(
                        // if expr { } else if
                        case EcsTokKeywordIf: {
                            if (parser->scope_depth >=
                                ECS_PARSER_MAX_RECURSION_DEPTH)
                            {
                                Error("maximum scope nesting depth exceeded");
                            }

                            parser->scope_depth ++;
                            Scope(stmt->if_false,
                                pos = flecs_script_if_stmt(parser, pos);
                            )
                            parser->scope_depth --;
                            if (!pos) {
                                goto error;
                            }
                            return pos;
                        }

                        // if expr { } else {
                        case '{': {
                            return flecs_script_scope(
                                parser, stmt->if_false, pos);
                        }
                    )
                )

                pos = stmt_pos;
            }

            EndOfRule;
        });
    )

    ParserEnd;
}

static const char* flecs_script_for_in(
    ecs_parser_t *parser,
    const char *pos,
    ecs_script_for_t *stmt)
{
    ParserBegin;

    LookAhead_1('[', {
        pos = lookahead;

        Expr('\0', {
            stmt->from = EXPR;

            Parse_1(EcsTokRange, {
                Expr('\0', {
                    stmt->to = EXPR;

                    Parse_1(']', {
                        pos = flecs_script_skip_newlines(parser, pos);

                        Parse_1('{', {
                            return flecs_script_scope(
                                parser, stmt->scope, pos);
                        })
                    })
                })
            })
        })
    })

    Expr('\0', {
        stmt->from = EXPR;

        pos = flecs_script_skip_newlines(parser, pos);

        Parse(
            case EcsTokRange: {
                Expr('\0', {
                    stmt->to = EXPR;

                    pos = flecs_script_skip_newlines(parser, pos);

                    Parse_1('{', {
                        return flecs_script_scope(parser, stmt->scope, pos);
                    })
                })
            }

            case '{': {
                stmt->expr = stmt->from;
                stmt->from = NULL;
                return flecs_script_scope(parser, stmt->scope, pos);
            }
        )
    })

    ParserEnd;
}

static const char* flecs_script_parse_var(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer,
    ecs_script_node_kind_t kind)
{
    bool is_export = kind == EcsAstExportConst || kind == EcsAstExportMut;
    int token_offset = !is_export ? 0 : 1;

    Parse_1(EcsTokIdentifier,
        ecs_script_var_node_t *var = flecs_script_insert_var(
            parser, Token(1 + token_offset));
        var->node.kind = kind;

        bool is_prop = kind == EcsAstProp;
        bool is_mut = kind == EcsAstMut || kind == EcsAstExportMut;
        const char *kind_str = is_prop ? "prop" : (is_mut ? "mut" : "const");

        Parse(
            // const color =
            case '=': {
                {
                    LookAhead_1(EcsTokKeywordAwait,
                        pos = lookahead;
                        Expr('\n',
                            var->is_await = true;
                            var->expr = EXPR;
                            EndOfRule;
                        )
                    )
                }

                {
                    // const color = Color :
                    LookAhead_2(EcsTokIdentifier, ':',
                        Error("'%s %s = %s: ...' is invalid, did you mean "
                            "'%s %s: %s = ...' instead?",
                            kind_str, var->name, Token(3 + token_offset),
                            kind_str, var->name, Token(3 + token_offset));
                    )
                }

                // const PI = expr\n
                Expr('\n',
                    var->expr = EXPR;
                    EndOfRule;
                )
            }

            case ':': {
                {
                    LookAhead_1(EcsTokKeywordTemplate,
                        if (!is_prop) {
                            Error("'%s %s: template ...' is invalid, template "
                                "types are only allowed for prop variables",
                                kind_str, var->name);
                        }

                        pos = lookahead;

                        Parse_1(EcsTokIdentifier,
                            var->type = Token(4 + token_offset);
                            var->type_is_template = true;
                            goto var_type;
                        )
                    )
                }

                LookAhead_1(EcsTokIdentifier,
                    pos = lookahead;

                    var->type = Token(3 + token_offset);

                    goto var_type;
                )

                Error("expected type name followed by '=', did you mean "
                    "'%s %s = ...' or '%s %s: type = ...' instead?",
                    kind_str, var->name, kind_str, var->name);
            }
        )

    var_type: {
        LookAhead(
            case '=': {
                pos = lookahead;

                {
                    LookAhead_1(EcsTokKeywordAwait,
                        Error("'%s %s: %s = await ...' is invalid, "
                            "await variables derive their type from the "
                            "awaited expression, use '%s %s = await ...' "
                            "instead",
                            kind_str, var->name, var->type,
                            kind_str, var->name);
                    )
                }

                {
                    LookAhead_1('{',
                        pos = lookahead;
                        Expr('}', {
                            var->expr = EXPR;
                            EndOfRule;
                        })
                    )
                }

                {
                    LookAhead_1('[',
                        pos = lookahead;
                        Expr(']', {
                            var->expr = EXPR;
                            EndOfRule;
                        })
                    )
                }

                {
                    LookAhead_1(EcsTokKeywordMatch,
                        Expr('\n',
                            var->expr = EXPR;
                            EndOfRule;
                        )
                    )
                }

                Initializer('\n',
                    var->expr = INITIALIZER;
                    EndOfRule;
                )
            }

            EcsTokEndOfStatement: {
                if (is_prop) {
                    pos = lookahead;
                    EndOfRule;
                }
                break;
            }

            case EcsTokScopeClose: {
                if (is_prop) {
                    EndOfRule;
                }
                break;
            }
        )

        Error("expected type name followed by '=', did you mean "
            "'%s %s = ...' or '%s %s: type = ...' instead?",
            kind_str, var->name, kind_str, var->name);
    }
    )

error:
    return NULL;
}

static const char* flecs_script_parse_const(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer)
{
    return flecs_script_parse_var(parser, pos, tokenizer, EcsAstConst);
}

static const char* flecs_script_parse_mut(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer)
{
    return flecs_script_parse_var(parser, pos, tokenizer, EcsAstMut);
}

static const char* flecs_script_parse_export_const(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer)
{
    return flecs_script_parse_var(parser, pos, tokenizer, EcsAstExportConst);
}

static const char* flecs_script_parse_export_mut(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer)
{
    return flecs_script_parse_var(parser, pos, tokenizer, EcsAstExportMut);
}

static const char* flecs_script_parse_prop(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer)
{
    return flecs_script_parse_var(parser, pos, tokenizer, EcsAstProp);
}

static const char* flecs_script_fn_params(
    ecs_parser_t *parser,
    ecs_script_function_node_t *fn,
    const char *pos)
{
    ParserBegin;

    bool prev_significant_newline = parser->significant_newline;
    parser->significant_newline = false;

    Loop(
        LookAhead(
            case EcsTokParenClose: {
                pos = lookahead;
                parser->significant_newline = prev_significant_newline;
                return pos;
            }
            case EcsTokIdentifier: {
                LookAhead_Keep();

                Parse_2(EcsTokColon, EcsTokIdentifier, {
                    ecs_script_fn_param_t *p = ecs_vec_append_t(
                        &parser->script->allocator, &fn->params,
                        ecs_script_fn_param_t);
                    ecs_os_zeromem(p);
                    p->node = (ecs_script_node_t){ .pos = parser->pos };
                    p->sp = -1;
                    p->name = Token(0);
                    p->type = Token(2);
                    parser->token_keep = parser->token_cur;

                    {
                        LookAhead_1(EcsTokComma,
                            pos = lookahead;
                            continue;
                        )
                    }

                    {
                        LookAhead_1(EcsTokParenClose,
                            pos = lookahead;
                            parser->significant_newline = prev_significant_newline;
                            return pos;
                        )
                    }

                    Error("expected ',' or ')' after parameter");
                })
            }
        )
        Error("expected parameter or ')'");
    )

    ParserEnd;
}

static const char* flecs_script_fn_body(
    ecs_parser_t *parser,
    ecs_script_function_node_t *fn,
    const char *pos)
{
    ParserBegin;

    Loop(
        LookAhead(
            case EcsTokNewline: {
                pos = lookahead;
                continue;
            }
            case EcsTokScopeClose: {
                Error("function body must end with an expression");
            }
            case EcsTokKeywordConst: {
                parser->stmt_pos = pos;
                pos = lookahead;
                Scope(fn->body,
                    pos = flecs_script_parse_const(parser, pos, tokenizer);
                )
                if (!pos) {
                    goto error;
                }
                parser->token_keep = parser->token_cur;
                continue;
            }
        )
        break;
    )

    parser->significant_newline = false;
    if (!(pos = flecs_script_parse_expr(parser, pos, 0, &fn->return_expr))) {
        parser->significant_newline = true;
        goto error;
    }
    parser->significant_newline = true;
    parser->token_keep = parser->token_cur;

    {
        Loop(
            LookAhead(
                case EcsTokNewline: {
                    pos = lookahead;
                    continue;
                }
                case EcsTokScopeClose: {
                    pos = lookahead;
                    return pos;
                }
            )
            Error("expected '}' after function body");
        )
    }

    ParserEnd;
}

static
int32_t flecs_script_last_stmt_kind(
    ecs_script_scope_t *scope)
{
    int32_t count = ecs_vec_count(&scope->stmts);
    if (!count) {
        return -1;
    }
    return ecs_vec_last_t(&scope->stmts, ecs_script_node_t*)[0]->kind;
}

/* Parse a single statement */
const char* flecs_script_stmt(
    ecs_parser_t *parser,
    const char *pos)
{
    ParserBegin;

    parser->stmt_pos = NULL;

    Parse(
        case EcsTokIdentifier:        goto identifier;
        case EcsTokString:            goto string_name;
        case '{':                     goto anonymous_entity;
        case '(':                     goto paren;
        case '@':                     goto annotation;
        case EcsTokKeywordWith:       goto with_stmt;
        case EcsTokKeywordModule:     goto module_stmt;
        case EcsTokKeywordUsing:      goto using_stmt;
        case EcsTokKeywordTemplate:   goto template_stmt;
        case EcsTokKeywordProp:       goto prop_var;
        case EcsTokKeywordMut:        goto mut_var;
        case EcsTokKeywordConst:      goto const_var;
        case EcsTokKeywordExport:     goto export_var;
        case EcsTokKeywordIf:         goto if_stmt;
        case EcsTokKeywordFor:        goto for_stmt;
        case EcsTokKeywordInclude:    goto include_stmt;
        case EcsTokKeywordFn:         goto fn_stmt;
        case EcsTokKeywordAwait:      goto await_stmt;
        case EcsTokKeywordTry:        goto try_stmt;
        case EcsTokKeywordContinue:   goto continue_stmt;
        EcsTokEndOfStatement:         EndOfRule;
    );

anonymous_entity: {
    return flecs_script_entity_scope(parser,
        flecs_script_insert_entity(parser, "_"), pos);
}

string_name:
    /* If this is an interpolated string, we need to evaluate it as expression
     * at evaluation time. Otherwise we can just use the string as name. The
     * latter is useful if an entity name contains special characters that are
     * not allowed in identifier tokens. */

identifier: {
    // enterprise } (end of scope)
    LookAhead_1('}',
        goto insert_tag;
    )

    Parse(
        // enterprise {
        case '{': {
            return flecs_script_entity_scope(parser,
                flecs_script_insert_entity(parser, Token(0)), pos);
        }

        // Npc\n
        EcsTokEndOfStatement: {
            // Npc\n{
            LookAhead_1('{',
                pos = lookahead;
                return flecs_script_entity_scope(parser,
                    flecs_script_insert_entity(parser, Token(0)), pos);
            )

            goto insert_tag;
        }

        // auto_override |
        case '|': {
            goto identifier_flag;
        }

        // Position:
        case ':': {
            goto identifier_colon;
        }

        // SpaceShip(
        case '(': {
            goto identifier_paren;
        }

        // Spaceship enterprise
        case EcsTokIdentifier: {
            goto identifier_identifier;
        }

        // Spaceship "enterprise"
        case EcsTokString: {
            goto identifier_string;
        }
    )
}

insert_tag: {
    if (!flecs_script_insert_tag(parser, Token(0))) {
        Error(
            "invalid context for tag '%s': must be part of entity",
            tokenizer->tokens[0].value);
    }

    EndOfRule;
}

// @
annotation: {
    // @brief
    Parse_1(EcsTokIdentifier,
        // $brief expr
        Until('\n', 
            flecs_script_insert_annot(parser, Token(1), Token(2));
            EndOfRule;
        )
    )
}

// with
with_stmt: {
    ecs_script_with_t *with = flecs_script_insert_with(parser);
    pos = flecs_script_with(parser, with, pos);
    EndOfRule;
}

// using
using_stmt: {
    if (parser->scope != parser->script->root) {
        Error("'using' must be declared in the root scope of a script");
    }

    {
        int32_t last = flecs_script_last_stmt_kind(parser->scope);
        if (last != -1 && last != EcsAstModule && last != EcsAstInclude &&
            last != EcsAstUsing)
        {
            Error("'using' must be declared before other statements");
        }
    }

    // using flecs.meta\n
    Parse_1(EcsTokIdentifier,
        flecs_script_insert_using(parser, Token(1));

        Parse(
            EcsTokEndOfStatement:
                EndOfRule;
        )
    )
}

// module
module_stmt: {
    if (parser->scope != parser->script->root) {
        Error("'module' must be declared in the root scope of a script");
    }

    if (ecs_vec_count(&parser->scope->stmts)) {
        Error("'module' must be the first statement of a script");
    }

    // module flecs.meta\n
    Parse_1(EcsTokIdentifier,
        flecs_script_insert_module(parser, Token(1));

        Parse(
            EcsTokEndOfStatement:
                EndOfRule;
        )
    )
}

// template
template_stmt: {
    // template SpaceShip
    Parse_1(EcsTokIdentifier, 
        ecs_script_template_node_t *template = flecs_script_insert_template(
            parser, Token(1));

        {
            LookAhead_1(':',
                pos = lookahead;
                Parse_1(EcsTokIdentifier,
                    template->base = Token(3);
                    goto template_scope;
                )
            )
        }

    template_scope:
        pos = flecs_script_skip_newlines(parser, pos);

        // template SpaceShip {
        Parse_1('{',
            return flecs_script_scope(parser, template->scope, pos);
        )
    )
}

// prop
prop_var: {
    // prop color = Color:
    return flecs_script_parse_prop(parser, pos, tokenizer);
}

// mut
mut_var: {
    return flecs_script_parse_mut(parser, pos, tokenizer);
}

// export
export_var: {
    Parse(
        // export const
        case EcsTokKeywordConst:
            return flecs_script_parse_export_const(parser, pos, tokenizer);
        // export mut
        case EcsTokKeywordMut:
            return flecs_script_parse_export_mut(parser, pos, tokenizer);
    )
}

// const
const_var: {
    // const color
    return flecs_script_parse_const(parser, pos, tokenizer);
}

// if
if_stmt: {
    return flecs_script_if_stmt(parser, pos);
}

// for
for_stmt: {
    ecs_script_for_t *stmt = flecs_script_insert_for(parser);

    Parse(
        case EcsTokIdentifier: {
            stmt->loop_vars[0] = Token(1);
            stmt->loop_var_count = 1;

            Parse_1(EcsTokKeywordIn, {
                return flecs_script_for_in(parser, pos, stmt);
            })
        }

        case '(': {
            Parse_2(EcsTokIdentifier, ',', {
                stmt->loop_vars[0] = Token(2);
                stmt->loop_var_count = 1;

                Parse_1(EcsTokIdentifier, {
                    stmt->loop_vars[1] = Token(4);
                    stmt->loop_var_count = 2;

                    Parse(
                        case ')': {
                            Parse_1(EcsTokKeywordIn, {
                                return flecs_script_for_in(parser, pos, stmt);
                            })
                        }

                        case ',': {
                            Parse_2(EcsTokIdentifier, ')', {
                                stmt->loop_vars[2] = Token(6);
                                stmt->loop_var_count = 3;

                                Parse_1(EcsTokKeywordIn, {
                                    return flecs_script_for_in(
                                        parser, pos, stmt);
                                })
                            })
                        }
                    )
                })
            })
        }
    )
}

fn_stmt: {
    Parse_2(EcsTokIdentifier, '(', {
        ecs_script_function_node_t *fn = flecs_script_insert_function(
            parser, Token(1));

        pos = flecs_script_fn_params(parser, fn, pos);
        if (!pos) {
            goto error;
        }

        pos = flecs_script_skip_newlines(parser, pos);

        Parse_1(EcsTokArrow,
            pos = flecs_script_skip_newlines(parser, pos);

            Parse_1(EcsTokIdentifier, {
                fn->return_type = Token(4);
                fn->return_type_node.pos = parser->pos;

                pos = flecs_script_skip_newlines(parser, pos);

                Parse_1('{', {
                    pos = flecs_script_fn_body(parser, fn, pos);
                    if (!pos) {
                        goto error;
                    }

                    EndOfRule;
                })
            })
        )
    })
}

continue_stmt: {
    flecs_script_insert_continue(parser);

    LookAhead_1('}',
        EndOfRule;
    )

    Parse(
        EcsTokEndOfStatement:
            EndOfRule;
    )
}

await_stmt: {
    Expr('\n', {
        ecs_script_await_t *await = flecs_script_insert_await(parser);
        await->expr = EXPR;
        EndOfRule;
    })
}

// try
try_stmt: {
    // try {
    Parse_1('{', {
        ecs_script_try_t *stmt = flecs_script_insert_try(parser);
        pos = flecs_script_scope(parser, stmt->try_scope, pos);
        if (!pos) {
            goto error;
        }

        Loop(
            // try { } catch
            // The catch keyword is not always tokenized as keyword ("catch("
            // lacks the whitespace boundary keywords require), so also accept
            // an identifier with the value "catch".
            LookAhead(
            case EcsTokIdentifier:
                if (ecs_os_strcmp(lookahead_token.value, "catch")) {
                    tokenizer->stack.count --;
                    break;
                }
                /* fall through */
            case EcsTokKeywordCatch: {
                pos = lookahead;
                ecs_script_catch_t *clause = flecs_script_try_add_catch(
                    parser, stmt);

                Parse(
                    // try { } catch {
                    case '{': {
                        pos = flecs_script_scope(parser, clause->scope, pos);
                        if (!pos) {
                            goto error;
                        }
                        continue;
                    }

                    // try { } catch(
                    case '(': {
                        // try { } catch(Error)
                        Parse_2(EcsTokIdentifier, ')', {
                            if (Token(4)[0] == '$') {
                                Error("variable not allowed as catch error");
                            }
                            clause->error = Token(4);
                            parser->token_keep = parser->token_cur;

                            // try { } catch(Error) {
                            Parse_1('{', {
                                pos = flecs_script_scope(
                                    parser, clause->scope, pos);
                                if (!pos) {
                                    goto error;
                                }
                                continue;
                            })
                        })
                    }
                )
            }
            )

            if (!ecs_vec_count(&stmt->catches)) {
                Error("expected 'catch' after 'try' block");
            }

            EndOfRule;
        )
    })
}

// include foo.flecs
include_stmt: {
    if (parser->scope != parser->script->root) {
        Error("'include' must be declared in the root scope of a script");
    }

    {
        int32_t last = flecs_script_last_stmt_kind(parser->scope);
        if (last != -1 && last != EcsAstModule && last != EcsAstInclude) {
            Error(
                "'include' must be declared before statements other than "
                "'module'");
        }
    }

    Until('\n',
        char *filename = ECS_CONST_CAST(char*, Token(1));
        if (filename) {
            char *p = filename;
            while (p[0]) {
                if (p[0] == '/' && (p[1] == '/' || p[1] == '*')) {
                    while (p > filename && isspace((unsigned char)p[-1])) {
                        p --;
                    }
                    p[0] = '\0';
                    break;
                }
                p ++;
            }
        }
        if (!filename || !filename[0]) {
            Error("expected filename after 'include'");
        }
        flecs_script_insert_include(parser, filename);
        EndOfRule;
    )
}

// (
paren: {
    Parse(
        case EcsTokIdentifier:
        case EcsTokString:
            Parse_1(',',
                Parse(
                    case EcsTokIdentifier:
                    case EcsTokString: 
                        Parse_1(')', 
                            goto pair;
                        )
                )
            )
    )
}

// (Likes, Apples)
pair: {
    // (Likes, Apples) } (end of scope)
    LookAhead_1('}',
        flecs_script_insert_pair_tag(parser, Token(1), Token(3));
        EndOfRule;
    )

    Parse(
        // (Likes, Apples)\n
        EcsTokEndOfStatement: {
            flecs_script_insert_pair_tag(parser, Token(1), Token(3));
            EndOfRule;
        }

        // (Eats, Apples):
        case ':': {
            // Use lookahead so that expression parser starts at "match"
            LookAhead_1(EcsTokKeywordMatch, {
                // (Eats, Apples): match expr
                Expr('\n', {
                    ecs_script_component_t *comp = 
                        flecs_script_insert_pair_component(
                            parser, Token(1), Token(3));
                    comp->expr = EXPR;
                    EndOfRule; 
                })
            })

            {
                // (Eats, Apples): {
                LookAhead_1('{', {
                    pos = lookahead;
                    // (Eats, Apples): { expr }
                    Initializer('}',
                        ecs_script_component_t *comp =
                            flecs_script_insert_pair_component(
                                parser, Token(1), Token(3));
                        comp->expr = INITIALIZER;
                        EndOfRule;
                    )
                })
            }

            // (Eats, Apples): expr
            Expr('\n', {
                ecs_script_component_t *comp =
                    flecs_script_insert_pair_component(
                        parser, Token(1), Token(3));
                comp->expr = EXPR;
                EndOfRule;
            })
        }

        // (IsA, Machine) {
        case '{': {
            ecs_script_pair_scope_t *ps = flecs_script_insert_pair_scope(
                parser, Token(1), Token(3));
            return flecs_script_scope(parser, ps->scope, pos);
        }
    )
}

// auto_override |
identifier_flag: {
    ecs_id_t flag;
    if (!ecs_os_strcmp(Token(0), "auto_override")) {
        flag = ECS_AUTO_OVERRIDE;
    } else {
        Error("invalid flag '%s'", Token(0));
    }

    Parse(
        // auto_override | (
        case '(':
            // auto_override | (Rel, Tgt)
            Parse_4(EcsTokIdentifier, ',', EcsTokIdentifier, ')',
                ecs_script_tag_t *tag = flecs_script_insert_pair_tag(
                    parser, Token(3), Token(5));
                tag->id.flag = flag;

                {
                    // auto_override | (Rel, Tgt) } (end of scope)
                    LookAhead_1('}',
                        EndOfRule;
                    )
                }

                Parse(
                    // auto_override | (Rel, Tgt)\n
                    EcsTokEndOfStatement: {
                        EndOfRule;
                    }

                    // auto_override | (Rel, Tgt):
                    case ':': {
                        Parse_1('{',
                            // auto_override | (Rel, Tgt): {expr}
                            Expr('}', {
                                ecs_script_component_t *comp = 
                                    flecs_script_insert_pair_component(
                                        parser, Token(3), Token(5));
                                comp->expr = EXPR;
                                EndOfRule; 
                            })
                        )
                    }
                )
            )

        // auto_override | Position
        case EcsTokIdentifier: {
            ecs_script_tag_t *tag = flecs_script_insert_tag(
                parser, Token(2));
            tag->id.flag = flag;

            {
                // auto_override | Position } (end of scope)
                LookAhead_1('}',
                    EndOfRule;
                )
            }

            Parse(
                // auto_override | Position\n
                EcsTokEndOfStatement: {
                    EndOfRule;
                }

                // auto_override | Position:
                case ':': {
                    Parse_1('{',
                        // auto_override | Position: {expr}
                        Expr('}', {
                            ecs_script_component_t *comp = 
                                flecs_script_insert_component(
                                    parser, Token(2));
                            comp->expr = EXPR;
                            EndOfRule; 
                        })
                    )
                }
            )
        }
    )
}

// Position:
identifier_colon: {
    {
        // Position: {
        LookAhead_1('{',
            pos = lookahead;
            goto component_expr_scope;
        )
    }

    {
        // Position: [
        LookAhead_1('[',
            pos = lookahead;
            goto component_expr_collection;
        )
    }

    {
        // Position: match
        LookAhead_1(EcsTokKeywordMatch,
            goto component_expr_match;
        )
    }

    bool is_inherit = tokenizer->tokens[0].kind == EcsTokString;
    int32_t colon_stack_count = tokenizer->stack.count;
    if (!is_inherit) {
        LookAhead_1(EcsTokIdentifier,
            if (lookahead_token.value[0] != '$') {
                const char *id_pos = pos;
                pos = lookahead;
                LookAhead(
                    case '{':
                        is_inherit = true;
                        break;
                    case ',':
                        is_inherit = true;
                        break;
                )
                pos = id_pos;
            }
        )
    }
    tokenizer->stack.count = colon_stack_count;

    if (!is_inherit) {
        goto component_expr_value;
    }

    // enterprise : SpaceShip
    Parse_1(EcsTokIdentifier, {
        ecs_script_entity_t *entity = flecs_script_insert_entity(
            parser, Token(0));
        if (!entity) {
            goto error;
        }

        Scope(entity->scope,
            flecs_script_insert_pair_tag(parser, "IsA", Token(2));

            LookAhead_1(',', {
                pos = lookahead;
                pos = flecs_script_base_list(parser, pos);
            })
        )

        Parse(
            // enterprise : SpaceShip {
            case '{':
                return flecs_script_entity_scope(parser, entity, pos);
        )
    })
}

// Spaceship enterprise
identifier_string:
identifier_identifier: {
    ecs_script_entity_t *entity = flecs_script_insert_entity(
        parser, Token(1));
    if (!entity) {
        goto error;
    }

    entity->kind = Token(0);

    // Spaceship enterprise :
    LookAhead_1(':', 
        pos = lookahead;

        Parse_1(EcsTokIdentifier, {
            Scope(entity->scope, 
                flecs_script_insert_pair_tag(parser, "IsA", Token(3));

                LookAhead_1(',', {
                    pos = lookahead;
                    pos = flecs_script_base_list(parser, pos);
                })
            )

            goto identifier_identifier_x;
        })
    )

identifier_identifier_x:
    {
        // Spaceship enterprise } (end of scope)
        LookAhead_1('}',
            EndOfRule;
        )
    }

    Parse(
        // Spaceship enterprise\n
        EcsTokEndOfStatement: {
            EndOfRule;
        }

        // Spaceship enterprise {
        case '{': {
            return flecs_script_entity_scope(parser, entity, pos);
        }

        // Spaceship enterprise(
        case '(': {
            return flecs_script_paren_expr(parser, Token(0), entity, pos);
        }
    )
}

// SpaceShip(
identifier_paren: {
    // SpaceShip()
    Initializer(')',
        Parse(
            // SpaceShip(expr) }
            //   This can happen when used as new expression.
            case '}':
                pos --;
            
            // SpaceShip(expr)\n
            EcsTokEndOfStatement: {
                ecs_script_entity_t *entity = flecs_script_insert_entity(
                    parser, NULL);

                Scope(entity->scope,
                    ecs_script_component_t *comp =
                        flecs_script_insert_component(parser, Token(0));
                    comp->expr = INITIALIZER;
                    flecs_script_initializer_set_full(comp->expr);
                )

                EndOfRule;
            }

            // SpaceShip(expr) {
            case '{': {
                ecs_script_entity_t *entity = flecs_script_insert_entity(
                    parser, NULL);

                Scope(entity->scope,
                    ecs_script_component_t *comp =
                        flecs_script_insert_component(parser, Token(0));
                    comp->expr = INITIALIZER;
                    flecs_script_initializer_set_full(comp->expr);
                )

                return flecs_script_entity_scope(parser, entity, pos);
            }
        )
    )
}

// Position: {
component_expr_scope: {

    // Position: {expr}
    Expr('}', {
        ecs_script_component_t *comp = flecs_script_insert_component(
            parser, Token(0));
        comp->expr = EXPR;
        EndOfRule; 
    })
}

// Points: [
component_expr_collection: {
    // Position: [expr]
    Expr(']', {
        ecs_script_component_t *comp = flecs_script_insert_component(
            parser, Token(0));
        comp->expr = EXPR;
        comp->is_collection = true;
        EndOfRule; 
    })
}

// Position: match
component_expr_match: {

    // Position: match expr
    Expr('\n', {
        ecs_script_component_t *comp = flecs_script_insert_component(
            parser, Token(0));
        comp->expr = EXPR;
        EndOfRule;
    })
}

// Position: expr
component_expr_value: {
    Expr('\n', {
        ecs_script_component_t *comp = flecs_script_insert_component(
            parser, Token(0));
        comp->expr = EXPR;
        EndOfRule;
    })
}

    ParserEnd;
}

/* Test if the statement that ended at pos already consumed its separator. The
 * tokenizer skips whitespace and comments that trail a token, so scan back over
 * them to find the last significant character. */
static bool flecs_script_stmt_is_terminated(
    const char *start,
    const char *pos)
{
    while (pos > start) {
        char c = pos[-1];

        if (c == '\n' || c == ';') {
            return true;
        }

        if (c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f') {
            pos --;
            continue;
        }

        if (c == '*' && (pos - 2) >= start && pos[-2] == '/') {
            pos -= 2;
            continue;
        }

        if (c == '/' && (pos - 3) >= start && pos[-2] == '*') {
            const char *p = pos - 3;
            while (p > start) {
                if (p[-1] == '/' && p[0] == '*') {
                    break;
                }
                p --;
            }

            if (p == start) {
                break;
            }

            pos = p - 1;
            continue;
        }

        break;
    }

    return false;
}

/* Parse a single statement, and verify that it is followed by a statement
 * separator (';', newline, end of scope or end of script). */
const char* flecs_script_stmt_w_separator(
    ecs_parser_t *parser,
    const char *pos)
{
    const char *start = pos;

    pos = flecs_script_stmt(parser, pos);
    if (!pos) {
        return NULL;
    }

    if (flecs_script_stmt_is_terminated(start, pos)) {
        return pos;
    }

    ecs_token_t token;
    const char *old_token_cur = parser->token_cur;
    const char *lookahead = flecs_token(parser, pos, &token, true);

    if (old_token_cur > parser->token_keep) {
        parser->token_cur = ECS_CONST_CAST(char*, old_token_cur);
    } else {
        parser->token_cur = parser->token_keep;
    }

    if (!lookahead) {
        flecs_token(parser, pos, &token, false);
        return NULL;
    }

    switch(token.kind) {
    EcsTokEndOfStatement:
    case EcsTokScopeClose:
        return pos;
    default:
        break;
    }

    ecs_parser_error(parser->name, parser->code,
        flecs_parser_errpos(parser, lookahead - 1), "missing ; or newline");

    return NULL;
}

static ecs_script_t* flecs_script_parse_init(
    ecs_world_t *world,
    const char *name,
    const char *code,
    ecs_parser_t *parser)
{
    if (!code) {
        code = "";
    }

    ecs_script_t *script = flecs_script_new(world);
    script->name = ecs_os_strdup(name);
    script->code = ecs_os_strdup(code);

    ecs_script_impl_t *impl = flecs_script_impl(script);

    *parser = (ecs_parser_t){
        .name = script->name,
        .code = script->code,
        .pos = script->code,
        .script = impl,
        .scope = impl->root,
        .significant_newline = true
    };

    /* Allocate a buffer that is able to store all parsed tokens. Multiply the
     * size of the script by two so that there is enough space to add \0
     * terminators and expression delimiters ('""')
     * The token buffer will exist for as long as the script object exists, and
     * ensures that AST nodes don't need to do separate allocations for the data
     * they contain. */
    impl->token_buffer_size = ecs_os_strlen(code) * 2 + 1;
    impl->token_buffer = flecs_alloc_w_dbg_info(
        &impl->allocator, impl->token_buffer_size, "token buffer");
    parser->token_cur = impl->token_buffer;
    parser->token_end = &impl->token_buffer[impl->token_buffer_size];

    return script;
}

ecs_script_t* flecs_script_parse_nested(
    ecs_world_t *world,
    const char *name,
    const char *using_code,
    const char *code,
    const char **next)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    if (using_code) {
        ecs_strbuf_appendstr(&buf, using_code);
    }
    int32_t open_offset = ecs_strbuf_written(&buf);
    ecs_strbuf_appendch(&buf, '{');
    ecs_strbuf_appendstr(&buf, code);
    char *nested_code = ecs_strbuf_get(&buf);

    ecs_parser_t parser;
    ecs_script_t *script = flecs_script_parse_init(
        world, name, nested_code, &parser);
    ecs_os_free(nested_code);
    ecs_script_impl_t *impl = flecs_script_impl(script);

    const char *open = &script->code[open_offset];
    const char *pos = script->code;
    while (pos < open) {
        pos = flecs_script_stmt_w_separator(&parser, pos);
        if (!pos) {
            goto error;
        }
    }

    ecs_assert(pos == open, ECS_INTERNAL_ERROR, NULL);
    pos = flecs_script_scope(&parser, impl->root, open + 1);
    if (!pos) {
        goto error;
    }

    *next = code + (pos - (open + 1));
    ECS_CONST_CAST(char*, open)[0] = '\n';
    ECS_CONST_CAST(char*, pos)[-1] = '\0';
    impl->token_remaining = parser.token_cur;
    return script;

error:
    ecs_script_free(script);
    return NULL;
}

ecs_script_t* ecs_script_parse(
    ecs_world_t *world,
    const char *name,
    const char *code,
    const ecs_script_eval_desc_t *desc,
    ecs_script_eval_result_t *result)
{
    if (result) {
        flecs_log_capture_push(true);
    }

    ecs_parser_t parser;
    ecs_script_t *script = flecs_script_parse_init(
        world, name, code, &parser);
    ecs_script_impl_t *impl = flecs_script_impl(script);

    if (desc && desc->lenient) {
        impl->lenient = true;
    }

    /* Start parsing code */
    const char *pos = script->code;

    do {
        pos = flecs_script_stmt_w_separator(&parser, pos);
        if (!pos) {
            /* NULL means error */
            goto error;
        }

        if (!pos[0]) {
            /* \0 means end of input */
            break;
        }
    } while (true);

    impl->token_remaining = parser.token_cur;

    if (result) {
        ecs_os_free(flecs_log_capture_pop());
    }

    return script;
error:
    if (result) {
        result->error = flecs_log_capture_pop();
        flecs_log_get_captured_error_pos(&result->line, &result->column);
    }

    ecs_script_free(script);
    return NULL;
}

#endif
