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

    scope->open = pos - 1;

    ecs_script_scope_t *prev = parser->scope;
    parser->scope = scope;

    Loop(
        LookAhead(
            case EcsTokScopeClose:
                Consume();
                goto scope_close;
            case EcsTokEnd:
                Error("unexpected end of script");
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
    scope->node.end = pos;
    return pos;

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

static const char* flecs_script_entity_template(
    ecs_parser_t *parser,
    const char *pos)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_script_scope_t *parent = scope->parent;
    ecs_script_entity_t *entity = NULL;

    if (parent) {
        ecs_script_node_t **stmts = ecs_vec_first(&parent->stmts);
        int32_t i = ecs_vec_count(&parent->stmts);
        while (i --) {
            if (stmts[i]->kind == EcsAstEntity &&
                ((ecs_script_entity_t*)stmts[i])->scope == scope)
            {
                entity = (ecs_script_entity_t*)stmts[i];
                break;
            }
        }
    }

    if (!entity) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos - 1),
            "template without a name must be declared in an entity scope");
        return NULL;
    }

    if (!entity->name || entity->name_expr) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos - 1),
            "template without a name must be declared in the scope of an "
            "entity with a static name");
        return NULL;
    }

    if (entity->base_count > 1) {
        ecs_parser_error(parser->name, parser->code,
            flecs_parser_errpos(parser, pos - 1),
            "multiple template bases are not supported");
        return NULL;
    }

    parser->scope = parent;
    ecs_script_template_node_t *template = flecs_script_insert_template(
        parser, entity->name);
    parser->scope = scope;

    if (entity->base_count) {
        ecs_script_tag_t *base = ecs_vec_first_t(
            &scope->stmts, ecs_script_tag_t*)[0];
        template->base = base->id.second;
    }

    return flecs_script_scope(parser, template->scope, pos);
}

/* Parse base list (bases separated by ',') */
static const char* flecs_script_base_list(
    ecs_parser_t *parser,
    ecs_script_entity_t *entity,
    const char *base,
    const char *pos)
{
    ParserBegin;

    ecs_script_scope_t *prev = parser->scope;
    parser->scope = entity->scope;

    flecs_script_insert_pair_tag(parser, "IsA", base);
    entity->base_count ++;

    Loop(
        LookAhead_1(',',
            Consume();
            pos = flecs_parser_skip_newlines(parser, pos);

            LookAhead_1(EcsTokIdentifier,
                Consume();
                flecs_script_insert_pair_tag(parser, "IsA", Token(1));
                entity->base_count ++;
                continue;
            )
        )

        break;
    )

    parser->scope = prev;
    return pos;
error:
    parser->scope = prev;
    return NULL;
}

static const char* flecs_script_pair(
    ecs_parser_t *parser,
    const char *pos,
    const char **first,
    const char **second)
{
    ParserBegin;

    Parse(
        case EcsTokIdentifier:
        case EcsTokString:
            *first = Token(0);
            Parse_1(',',
                Parse(
                    case EcsTokIdentifier:
                    case EcsTokString:
                        *second = Token(2);
                        Parse_1(')',
                            EndOfRule;
                        )
                )
            )
    )

    ParserEnd;
}

/* Parse with expression (expression after 'with' keyword) */
static const char* flecs_script_with_expr(
    ecs_parser_t *parser,
    const char *pos)
{
    ParserBegin;

    const char *first = NULL, *second = NULL;

    Parse(
        // Position
        case EcsTokIdentifier: {
            first = Token(0);
            goto with_id;
        }

        // (Eats, Apples)
        case '(': {
            pos = flecs_script_pair(parser, pos, &first, &second);
            if (!pos) {
                goto error;
            }

            goto with_id;
        }
    )

with_id:
    // Position (
    // (Eats, Apples) (
    LookAhead_1('(',
        Consume();

        // Position ( expr )
        // (Eats, Apples) ( expr )
        Initializer(')',
            ecs_script_component_t *component =
                flecs_script_insert_pair_component(parser, first, second);
            component->node.kind = EcsAstWithComponent;
            component->expr = INITIALIZER;
            EndOfRule;
        )
    )

    ecs_script_tag_t *tag = flecs_script_insert_pair_tag(parser, first, second);
    tag->node.kind = EcsAstWithTag;
    EndOfRule;

    ParserEnd;
}

/* Parse with expression list (expression list after 'with' keyword) */
static const char* flecs_script_with(
    ecs_parser_t *parser,
    ecs_script_with_t *with,
    const char *pos)
{
    ParserBegin;

    Loop(
        Scope(with->expressions,
            pos = flecs_script_with_expr(parser, pos);
        )

        if (!pos) {
            goto error;
        }

        pos = flecs_parser_skip_newlines(parser, pos);

        Parse(
            case ',': {
                pos = flecs_parser_skip_newlines(parser, pos);
                continue;
            }
            case '{': {
                return flecs_script_scope(parser, with->scope, pos);
            }
        )
    )

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
        )

        // Position spaceship (expr) {
        // Position spaceship (expr)\n{
        LookAheadSkipNewlines_1('{',
            Consume();
            return flecs_script_entity_scope(parser, entity, pos);
        )

        EndOfRule;
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
    Expr(
        pos = flecs_parser_skip_newlines(parser, pos);

        // if expr {
        Parse_1('{', {
            ecs_script_if_t *stmt = flecs_script_insert_if(parser);
            stmt->expr = EXPR;
            pos = flecs_script_scope(parser, stmt->if_true, pos);
            if (!pos) {
                goto error;
            }

            // if expr { }\n else
            LookAheadSkipNewlines_1(EcsTokKeywordElse,
                // if expr { } else\n
                Consume();
                pos = flecs_parser_skip_newlines(parser, pos);

                Parse(
                    // if expr { } else if
                    case EcsTokKeywordIf: {
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

    Expr({
        stmt->expr = EXPR;

        pos = flecs_parser_skip_newlines(parser, pos);

        Parse(
            case EcsTokRange: {
                stmt->from = EXPR;
                stmt->expr = NULL;

                Expr({
                    stmt->to = EXPR;

                    pos = flecs_parser_skip_newlines(parser, pos);

                    Parse_1('{', {
                        return flecs_script_scope(parser, stmt->scope, pos);
                    })
                })
            }

            case '{': {
                if (EXPR->kind == EcsExprRange) {
                    ecs_expr_range_t *range = (ecs_expr_range_t*)EXPR;
                    stmt->from = range->from;
                    stmt->to = range->to;
                    range->from = NULL;
                    range->to = NULL;
                    stmt->expr = NULL;
                    flecs_expr_visit_free(&parser->script->pub, EXPR);
                }

                return flecs_script_scope(parser, stmt->scope, pos);
            }
        )
    })

    ParserEnd;
}

static const char* flecs_script_parse_var(
    ecs_parser_t *parser,
    const char *pos,
    ecs_script_node_kind_t kind)
{
    ParserBegin;

    Parse_1(EcsTokIdentifier,
        ecs_script_var_node_t *var = flecs_script_insert_var(
            parser, Token(0));
        var->node.kind = kind;

        bool is_prop = kind == EcsAstProp;
        bool is_mut = kind == EcsAstMut || kind == EcsAstExportMut;
        const char *kind_str = is_prop ? "prop" : (is_mut ? "mut" : "const");

        Parse(
            // const color =
            case '=': {
                LookAhead_1(EcsTokKeywordAwait,
                    Consume();
                    Expr(
                        var->is_await = true;
                        var->expr = EXPR;
                        EndOfRule;
                    )
                )

                // const color = Color :
                LookAhead_2(EcsTokIdentifier, ':',
                    Error("'%s %s = %s: ...' is invalid, did you mean "
                        "'%s %s: %s = ...' instead?",
                        kind_str, var->name, Token(2),
                        kind_str, var->name, Token(2));
                )

                // const PI = expr\n
                Expr(
                    var->expr = EXPR;
                    EndOfRule;
                )
            }

            case ':': {
                LookAhead_1(EcsTokKeywordTemplate,
                    if (!is_prop) {
                        Error("'%s %s: template ...' is invalid, template "
                            "types are only allowed for prop variables",
                            kind_str, var->name);
                    }

                    Consume();
                    var->type_is_template = true;

                    LookAhead_1(EcsTokIdentifier,
                        var->type = Token(3);
                        Consume();
                    )

                    LookAhead_2('[', ']',
                        Consume();
                        var->type_is_vector = true;
                    )

                    goto var_type;
                )

                LookAhead_1(EcsTokIdentifier,
                    Consume();

                    var->type = Token(2);

                    goto var_type;
                )

                goto var_type_error;
            }
        )

    var_type: {
        LookAhead(
            case '=': {
                Consume();

                LookAhead_1(EcsTokKeywordAwait,
                    Error("'%s %s: %s = await ...' is invalid, "
                        "await variables derive their type from the "
                        "awaited expression, use '%s %s = await ...' "
                        "instead",
                        kind_str, var->name, var->type,
                        kind_str, var->name);
                )

                LookAhead(
                    case '{':
                    case '[':
                    case EcsTokKeywordMatch:
                        Expr(
                            var->expr = EXPR;
                            EndOfRule;
                        )
                )

                LookAhead_2(EcsTokIdentifier, '(',
                    Expr(
                        var->expr = EXPR;
                        EndOfRule;
                    )
                )

                Initializer('\n',
                    var->expr = INITIALIZER;
                    EndOfRule;
                )
            }

            // prop color: Color\n
            EcsTokEndOfStatement:
            case EcsTokScopeClose: {
                if (is_prop) {
                    EndOfRule;
                }

                break;
            }
        )

    var_type_error:
        Error("expected type name followed by '=', did you mean "
            "'%s %s = ...' or '%s %s: type = ...' instead?",
            kind_str, var->name, kind_str, var->name);
    }
    )

error:
    return NULL;
}

static const char* flecs_script_fn_params(
    ecs_parser_t *parser,
    ecs_script_function_node_t *fn,
    const char *pos)
{
    ParserBegin;

    Loop(
        LookAhead(
            case EcsTokParenClose: {
                Consume();
                EndOfRule;
            }
            case EcsTokIdentifier: {
                Consume();

                Parse_2(EcsTokColon, EcsTokIdentifier, {
                    ecs_script_fn_param_t *p = ecs_vec_append_t(
                        &parser->script->allocator, &fn->params,
                        ecs_script_fn_param_t);
                    ecs_os_zeromem(p);
                    p->node = (ecs_script_node_t){ .pos = parser->pos };
                    p->sp = -1;
                    p->name = Token(0);
                    p->type = Token(2);

                    LookAhead_1(EcsTokComma,
                        Consume();
                        continue;
                    )

                    LookAhead_1(EcsTokParenClose,
                        Consume();
                        EndOfRule;
                    )

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
        pos = flecs_parser_skip_newlines(parser, pos);

        LookAhead(
            case EcsTokScopeClose: {
                Error("function body must end with an expression");
            }
            case EcsTokSemiColon: {
                Consume();
                continue;
            }
            case EcsTokKeywordConst: {
                parser->stmt_pos = pos;
                Consume();
                Scope(fn->body,
                    pos = flecs_script_parse_var(parser, pos, EcsAstConst);
                )
                if (!pos) {
                    goto error;
                }
                continue;
            }
        )
        break;
    )

    Expr(
        fn->return_expr = EXPR;
    )

    pos = flecs_parser_skip_newlines(parser, pos);

    LookAhead_1(EcsTokScopeClose,
        Consume();
        return pos;
    )

    Error("expected '}' after function body");
error:
    return NULL;
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
static const char* flecs_script_stmt_parse(
    ecs_parser_t *parser,
    const char *pos)
{
    ParserBegin;

    const char *component_first = NULL, *component_second = NULL;
    bool component_collection = false;
    bool component_partial = false;
    ecs_expr_node_t *component_index = NULL;

    parser->stmt_pos = NULL;

    LookAhead(
        EcsTokEndOfStatement:
            EndOfRule;
    )

    Parse(
        case EcsTokIdentifier:        goto identifier;
        case EcsTokString:            goto identifier;
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
        case EcsTokKeywordAsync:      goto async_stmt;
        case EcsTokKeywordWhile:      goto while_stmt;
    );

anonymous_entity: {
    return flecs_script_entity_scope(parser,
        flecs_script_insert_entity(parser, "_"), pos);
}

identifier: {
    LookAhead(
        // auto_override |
        case '|': {
            Consume();
            goto identifier_flag;
        }

        // Position:
        case ':': {
            Consume();
            goto identifier_colon;
        }

        // facade[
        case '[': {
            Consume();
            goto identifier_index;
        }

        // SpaceShip(
        case '(': {
            Consume();
            goto identifier_paren;
        }

        // value =
        case '=': {
            Consume();
            goto identifier_assign;
        }

        // Spaceship enterprise
        // Spaceship "enterprise"
        case EcsTokIdentifier:
        case EcsTokString: {
            Consume();
            goto identifier_identifier;
        }
    )

    // enterprise {
    // Npc\n{
    LookAheadSkipNewlines_1('{',
        Consume();
        return flecs_script_entity_scope(parser,
            flecs_script_insert_entity(parser, Token(0)), pos);
    )

    goto insert_tag;
}

// Npc
// facade[0]
insert_tag: {
    ecs_script_tag_t *tag = flecs_script_insert_tag(parser, Token(0));
    tag->id.index_expr = component_index;
    EndOfRule;
}

// @
annotation: {
    // @brief
    Parse_1(EcsTokIdentifier, {
        // @brief value
        ecs_token_t value;
        pos = flecs_tokenizer_line(parser, pos, &value, false);
        flecs_script_insert_annot(parser, Token(1), value.value);
        EndOfRule;
    })
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
        EndOfRule;
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
        EndOfRule;
    )
}

// template
template_stmt: {
    LookAheadSkipNewlines_1('{',
        Consume();
        return flecs_script_entity_template(parser, pos);
    )

    // template SpaceShip
    Parse_1(EcsTokIdentifier,
        ecs_script_template_node_t *template = flecs_script_insert_template(
            parser, Token(1));

        LookAhead_1(':',
            Consume();
            Loop(
                pos = flecs_parser_skip_newlines(parser, pos);

                Parse_1(EcsTokIdentifier,
                    if (!ecs_os_strcmp(Token(3), "parent")) {
                        if (template->parent) {
                            Error("duplicate parent constraint");
                        }

                        Parse_1(EcsTokIdentifier,
                            template->parent = Token(4);
                        )
                    } else {
                        if (template->base) {
                            Error("multiple template bases are not supported");
                        }

                        template->base = Token(3);
                    }

                    LookAhead_1(',',
                        Consume();
                        continue;
                    )
                )

                goto template_scope;
            )
        )

    template_scope:
        pos = flecs_parser_skip_newlines(parser, pos);

        // template SpaceShip {
        Parse_1('{',
            return flecs_script_scope(parser, template->scope, pos);
        )
    )
}

// prop
prop_var: {
    // prop color = Color:
    return flecs_script_parse_var(parser, pos, EcsAstProp);
}

// mut
mut_var: {
    return flecs_script_parse_var(parser, pos, EcsAstMut);
}

// export
export_var: {
    Parse(
        // export const
        case EcsTokKeywordConst:
            return flecs_script_parse_var(parser, pos, EcsAstExportConst);
        // export mut
        case EcsTokKeywordMut:
            return flecs_script_parse_var(parser, pos, EcsAstExportMut);
    )
}

// const
const_var: {
    // const color
    return flecs_script_parse_var(parser, pos, EcsAstConst);
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
            goto for_in_stmt;
        }

        case '(': {
            Loop(
                Parse_1(EcsTokIdentifier,
                    stmt->loop_vars[stmt->loop_var_count ++] = Token(2);
                )

                if (stmt->loop_var_count == 1) {
                    Parse_1(',', continue;)
                } else if (stmt->loop_var_count == 2) {
                    LookAhead_1(',',
                        Consume();
                        continue;
                    )
                }

                Parse_1(')', goto for_in_stmt;)
            )
        }
    )

for_in_stmt:
    Parse_1(EcsTokKeywordIn,
        return flecs_script_for_in(parser, pos, stmt);
    )
}

fn_stmt: {
    Parse_2(EcsTokIdentifier, '(', {
        ecs_script_function_node_t *fn = flecs_script_insert_function(
            parser, Token(1));

        bool significant_newline = parser->significant_newline;
        parser->significant_newline = false;
        pos = flecs_script_fn_params(parser, fn, pos);
        parser->significant_newline = significant_newline;
        if (!pos) {
            goto error;
        }

        pos = flecs_parser_skip_newlines(parser, pos);

        Parse_1(EcsTokArrow,
            pos = flecs_parser_skip_newlines(parser, pos);

            Parse_1(EcsTokIdentifier, {
                fn->return_type = Token(4);
                fn->return_type_node.pos = parser->pos;

                pos = flecs_parser_skip_newlines(parser, pos);

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
    EndOfRule;
}

await_stmt: {
    Expr({
        ecs_script_await_t *await = flecs_script_insert_await(parser);
        await->expr = EXPR;
        EndOfRule;
    })
}

// async
async_stmt: {
    // async {
    Parse_1('{', {
        ecs_script_async_t *stmt = flecs_script_insert_async(parser);
        bool async_scope = parser->async_scope;
        parser->async_scope = true;
        pos = flecs_script_scope(parser, stmt->scope, pos);
        parser->async_scope = async_scope;
        return pos;
    })
}

// while
while_stmt: {
    // while expr
    Expr(
        ecs_script_while_t *stmt = flecs_script_insert_while(parser);
        stmt->expr = EXPR;
        pos = flecs_parser_skip_newlines(parser, pos);

        // while expr {
        Parse_1('{', {
            return flecs_script_scope(parser, stmt->scope, pos);
        })
    )
}

// value =
// Position =
identifier_assign: {
    if (!parser->async_scope) {
        component_first = Token(0);
        component_partial = true;
        goto component_assign;
    }

    ecs_script_assign_t *stmt = flecs_script_insert_assign(parser, Token(0));

    // value = expr\n
    Expr({
        stmt->expr = EXPR;
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
            LookAheadSkipNewlines_1(EcsTokKeywordCatch,
                Consume();
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

    {
        ecs_token_t filename;
        pos = flecs_tokenizer_line(parser, pos, &filename, true);
        if (!filename.value[0]) {
            Error("expected filename after 'include'");
        }

        flecs_script_insert_include(parser, filename.value);
        EndOfRule;
    }
}

// (
paren: {
    pos = flecs_script_pair(
        parser, pos, &component_first, &component_second);
    if (!pos) {
        goto error;
    }

    // (Likes, Apples):
    LookAhead_1(':',
        Consume();
        goto component_expr;
    )

    // (Likes, Apples) =
    LookAhead_1('=',
        Consume();
        component_partial = true;
        goto component_assign;
    )

    // (IsA, Machine) {
    LookAheadSkipNewlines_1('{',
        Consume();
        ecs_script_pair_scope_t *ps = flecs_script_insert_pair_scope(
            parser, component_first, component_second);
        return flecs_script_scope(parser, ps->scope, pos);
    )

    // (Likes, Apples)
    flecs_script_insert_pair_tag(parser, component_first, component_second);
    EndOfRule;
}

identifier_flag: {
    if (ecs_os_strcmp(Token(0), "auto_override")) {
        Error("invalid flag '%s'", Token(0));
    }

    Parse(
        case '(':
            pos = flecs_script_pair(
                parser, pos, &component_first, &component_second);
            if (!pos) {
                goto error;
            }

            goto flagged_id;
        case EcsTokIdentifier:
            component_first = Token(2);
            goto flagged_id;
    )
}

flagged_id: {
    ecs_script_tag_t *tag = flecs_script_insert_pair_tag(
        parser, component_first, component_second);
    tag->id.flag = ECS_AUTO_OVERRIDE;

    // auto_override | Position: {
    LookAhead_1(':',
        Consume();
        LookAhead_1('{',
            goto component_expr;
        )

        Error("expected '{' after ':'");
    )

    EndOfRule;
}

// facade[0]
identifier_index: {
    component_first = Token(0);

    LookAhead(
        case '}':
        case ']':
        EcsTokEndOfStatement: {
            Error("expected index expression after '%s['",
                component_first);
        }
    )

    Expr(
        component_index = EXPR;
    )

    Parse_1(']',
        // facade[0]:
        LookAhead_1(':',
            Consume();
            goto component_expr;
        )

        goto insert_tag;
    )
}

// Position:
identifier_colon: {
    component_first = Token(0);
    LookAhead(
        case '[':
            component_collection = true;
            goto component_expr;
        case '{':
        case EcsTokKeywordMatch:
            goto component_expr;
    )

    bool is_inherit = tokenizer->tokens[0].kind == EcsTokString;
    if (!is_inherit) {
        LookAheadAny_2(case EcsTokIdentifier:, case '{': case ',':,
            is_inherit = Token(2)[0] != '$';
        )
    }

    if (!is_inherit) {
        goto component_expr;
    }

    // enterprise : SpaceShip
    Parse_1(EcsTokIdentifier, {
        ecs_script_entity_t *entity = flecs_script_insert_entity(
            parser, Token(0));
        if (!entity) {
            goto error;
        }

        if (!ecs_os_strcmp(Token(2), "Prefab")) {
            Warning("'%s : Prefab' creates a live instance, not a "
                "prefab; use 'prefab %s' instead", Token(0), Token(0));
        }

        pos = flecs_script_base_list(parser, entity, Token(2), pos);
        if (!pos) {
            goto error;
        }

        Parse(
            // enterprise : SpaceShip {
            case '{':
                return flecs_script_entity_scope(parser, entity, pos);
        )
    })
}

// Spaceship enterprise
identifier_identifier: {
    ecs_script_entity_t *entity = flecs_script_insert_entity(
        parser, Token(1));
    if (!entity) {
        goto error;
    }

    entity->kind = Token(0);

    // Spaceship enterprise :
    LookAhead_1(':', 
        Consume();

        Parse_1(EcsTokIdentifier, {
            pos = flecs_script_base_list(parser, entity, Token(3), pos);
            if (!pos) {
                goto error;
            }

            goto identifier_identifier_x;
        })
    )

identifier_identifier_x:
    // Spaceship enterprise(
    LookAhead_1('(',
        Consume();
        return flecs_script_paren_expr(parser, Token(0), entity, pos);
    )

    // Spaceship enterprise {
    // Spaceship enterprise\n{
    LookAheadSkipNewlines_1('{',
        Consume();
        return flecs_script_entity_scope(parser, entity, pos);
    )

    EndOfRule;
}

// SpaceShip(
identifier_paren: {
    return flecs_script_paren_expr(parser, Token(0),
        flecs_script_insert_entity(parser, NULL), pos);
}

component_assign: {
    LookAhead_1('[',
        component_collection = true;
    )

    goto component_expr;
}

component_expr: {
    Expr({
        ecs_script_component_t *comp = flecs_script_insert_pair_component(
            parser, component_first, component_second);
        comp->expr = EXPR;
        comp->is_collection = component_collection;
        comp->is_partial = component_partial;
        comp->id.index_expr = component_index;
        EndOfRule;
    })
}

    ParserEnd;
}

const char* flecs_script_stmt(
    ecs_parser_t *parser,
    const char *pos)
{
    ecs_script_scope_t *scope = parser->scope;
    int32_t first = scope ? ecs_vec_count(&scope->stmts) : 0;

    pos = flecs_script_stmt_parse(parser, pos);

    if (pos && scope) {
        const char *end = flecs_parser_stmt_end(parser, pos);
        ecs_script_node_t **stmts = ecs_vec_first_t(
            &scope->stmts, ecs_script_node_t*);
        int32_t i, count = ecs_vec_count(&scope->stmts);
        for (i = first; i < count; i ++) {
            if (!stmts[i]->end) {
                stmts[i]->end = end;
            }
        }
    }

    return pos;
}

/* Parse a single statement, and verify that it is followed by a statement
 * separator (';', newline, end of scope or end of script). */
const char* flecs_script_stmt_w_separator(
    ecs_parser_t *parser,
    const char *pos)
{
    pos = flecs_script_stmt(parser, pos);
    if (!pos) {
        return NULL;
    }

    ecs_token_t token;
    const char *old_token_cur = parser->token_cur;
    const char *lookahead = flecs_token(parser, pos, &token, true);
    flecs_token_restore(parser, old_token_cur);

    if (!lookahead) {
        return NULL;
    }

    switch(token.kind) {
    EcsTokEndOfStatement:
        return lookahead;
    case EcsTokScopeClose:
        return pos;
    default:
        break;
    }

    ecs_parser_error(parser->name, parser->code,
        flecs_parser_errpos(parser, lookahead - 1), "missing ; or newline");

    return NULL;
}

ecs_script_t* flecs_script_parse_init(
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

static void flecs_script_append_using(
    ecs_script_scope_t *scope,
    ecs_strbuf_t *buf)
{
    if (scope->parent) {
        flecs_script_append_using(scope->parent, buf);
    }

    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    for (i = 0; i < count; i ++) {
        if (stmts[i]->kind == EcsAstUsing) {
            ecs_script_using_t *using_node = (ecs_script_using_t*)stmts[i];
            ecs_strbuf_appendlit(buf, "using ");
            ecs_strbuf_appendstr(buf, using_node->name);
            ecs_strbuf_appendch(buf, ';');
        }
    }
}

ecs_script_t* flecs_script_parse_nested(
    const ecs_parser_t *outer,
    const char *code,
    const char **next)
{
    ecs_parser_t scan = *outer;
    scan.significant_newline = false;
    ecs_token_t token = { .kind = EcsTokUnknown };
    const char *end = code;
    for (int32_t depth = 1; depth && token.kind != EcsTokEnd;) {
        if (!(end = flecs_token(&scan, end, &token, true))) {
            return NULL;
        }

        depth += (token.kind == '{') - (token.kind == '}');
        scan.token_cur = outer->token_cur;
    }

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    flecs_script_append_using(outer->scope, &buf);

    const char *outer_code = outer->code;
    const char *outer_open = code - 1;
    const char *line_start = outer_code;
    const char *ptr;
    for (ptr = outer_code; ptr < outer_open; ptr ++) {
        if (ptr[0] == '\n') {
            ecs_strbuf_appendch(&buf, '\n');
            line_start = ptr + 1;
        }
    }

    int32_t column = flecs_ito(int32_t, outer_open - line_start);
    if (line_start == outer_code) {
        column -= ecs_strbuf_written(&buf);
    }

    for (; column > 0; column --) {
        ecs_strbuf_appendch(&buf, ' ');
    }

    int32_t open_offset = ecs_strbuf_written(&buf);
    ecs_strbuf_appendch(&buf, '{');
    ecs_strbuf_appendstrn(&buf, code, flecs_ito(int32_t, end - code));
    char *nested_code = ecs_strbuf_get(&buf);

    ecs_parser_t parser;
    ecs_script_t *script = flecs_script_parse_init(
        outer->script->pub.world, outer->name, nested_code, &parser);
    ecs_os_free(nested_code);
    ecs_script_impl_t *impl = flecs_script_impl(script);

    const char *open = &script->code[open_offset];
    const char *pos = flecs_parser_skip_newlines(&parser, script->code);
    while (pos < open) {
        pos = flecs_script_stmt_w_separator(&parser, pos);
        if (!pos) {
            goto error;
        }

        pos = flecs_parser_skip_newlines(&parser, pos);
    }

    ecs_assert(pos == open, ECS_INTERNAL_ERROR, NULL);
    pos = flecs_script_scope(&parser, impl->root, open + 1);
    if (!pos) {
        goto error;
    }

    *next = code + (pos - (open + 1));
    ECS_CONST_CAST(char*, open)[0] = ' ';
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

    if (desc && desc->skip_unknown) {
        impl->skip_unknown = true;
    }

    if (desc && desc->ir) {
        impl->ir_enabled = true;
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
