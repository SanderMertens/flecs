/**
 * @file addons/script/parser.c
 * @brief Script grammar parser.
 */

#include "flecs.h"

#include "../parser/grammar.h"

#ifdef FLECS_SCRIPT
#include "script.h"

#define EcsTokEndOfStatement\
    case ';':\
    case '\n':\
    case '\0'

/* Parse scope (statements inside {}) */
static
const char* flecs_script_scope(
    ecs_parser_t *parser,
    ecs_script_scope_t *scope,
    const char *pos)
{
    ParserBegin;

    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pos[-1] == '{', ECS_INTERNAL_ERROR, NULL);

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

        pos = flecs_script_stmt(parser, pos);
        if (!pos) {
            goto error;
        }
    )

scope_close:
    parser->scope = prev;

    ecs_assert(pos[-1] == '}', ECS_INTERNAL_ERROR, NULL);
    return pos;

    ParserEnd;
}

/* Parse comma expression (expressions separated by ',') */
static
const char* flecs_script_comma_expr(
    ecs_parser_t *parser,
    const char *pos,
    bool is_base_list)
{
    ParserBegin;

    Loop(
        LookAhead(
            case '\n':
                pos = lookahead;
                continue;

            case EcsTokIdentifier:
                LookAhead_Keep();

                if (is_base_list) {
                    flecs_script_insert_pair_tag(parser, "IsA", Token(0));
                } else {
                    flecs_script_insert_entity(parser, Token(0));
                }

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
static
const char* flecs_script_with_expr(
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
                    EndOfRule;
                )
            )

            if (Token(0)[0] == '$') {
                ecs_script_var_component_t *var = 
                    flecs_script_insert_var_component(parser, &Token(0)[1]);
                var->node.kind = EcsAstWithVar;
            } else {
                ecs_script_tag_t *tag =
                    flecs_script_insert_tag(parser, Token(0));
                tag->node.kind = EcsAstWithTag;
            }

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
static
const char* flecs_script_with(
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

        Parse(
            case ',': {
                has_next = true;
                break;
            }
            case '{': {
                return flecs_script_scope(parser, with->scope, pos);
            }
        )
    } while (has_next);

    ParserEnd;
}

/* Parenthesis expression */
static
const char* flecs_script_paren_expr(
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
                return flecs_script_scope(parser, entity->scope, pos);
            }
        )
    )

    ParserEnd;
}

/* Parse a single statement */
static
const char* flecs_script_if_stmt(
    ecs_parser_t *parser,
    const char *pos)
{
    ParserBegin;

    // if expr
    Expr('\0',
        // if expr {
        Parse_1('{', {
            ecs_script_if_t *stmt = flecs_script_insert_if(parser);
            stmt->expr = EXPR;
            pos = flecs_script_scope(parser, stmt->if_true, pos);
            if (!pos) {
                goto error;
            }

            // if expr { } else
            LookAhead_1(EcsTokKeywordElse, 
                pos = lookahead;

                Parse(
                    // if expr { } else if
                    case EcsTokKeywordIf: {
                        Scope(stmt->if_false, 
                            return flecs_script_if_stmt(parser, pos);
                        )
                    }

                    // if expr { } else\n if
                    case EcsTokNewline: {
                        Parse_1(EcsTokKeywordIf,
                            Scope(stmt->if_false, 
                                return flecs_script_if_stmt(parser, pos);
                            )
                        )
                    }

                    // if expr { } else {
                    case '{': {
                        return flecs_script_scope(parser, stmt->if_false, pos);
                    }
                )
            )

            EndOfRule;
        });
    )

    ParserEnd;
}

static
const char* flecs_script_parse_var(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer,
    ecs_script_node_kind_t kind)
{
    int token_offset = kind != EcsAstExportConst ? 0 : 1;

    Parse_1(EcsTokIdentifier,
        ecs_script_var_node_t *var = flecs_script_insert_var(
            parser, Token(1 + token_offset));
        var->node.kind = kind;

        bool is_prop = kind == EcsAstProp;

        Parse(
            // const color =
            case '=': {
                // const color = Color :
                LookAhead_2(EcsTokIdentifier, ':',
                    pos = lookahead;

                    var->type = Token(3 + token_offset);

                    {
                        // Position: {
                        LookAhead_1('{',
                            pos = lookahead;
                            Expr('}', {
                                var->expr = EXPR;
                                EndOfRule; 
                            })
                        )                        
                    }

                    {
                        // Position: [
                        LookAhead_1('[',
                            pos = lookahead;
                            Expr(']', {
                                var->expr = EXPR;
                                EndOfRule; 
                            })
                        )                        
                    }

                    // const color = Color: expr\n
                    Initializer('\n',
                        var->expr = INITIALIZER;
                        EndOfRule;
                    )
                )

                // const PI = expr\n
                Expr('\n',
                    Warning("'%s var = expr' syntax is deprecated"
                        ", use '%s var: expr' instead", 
                            is_prop ? "prop" : "const",
                            is_prop ? "prop" : "const");
                    var->expr = EXPR;
                    EndOfRule;
                )
            }

            case ':': {
                // const PI: expr\n
                Expr('\n',
                    var->expr = EXPR;
                    EndOfRule;
                )
            }
        )
    )

error:
    return NULL;
}

static
const char* flecs_script_parse_const(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer)
{
    return flecs_script_parse_var(parser, pos, tokenizer, EcsAstConst);
}

static
const char* flecs_script_parse_export_const(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer)
{
    return flecs_script_parse_var(parser, pos, tokenizer, EcsAstExportConst);
}

static
const char* flecs_script_parse_prop(
    ecs_parser_t *parser,
    const char *pos,
    ecs_tokenizer_t *tokenizer)
{
    return flecs_script_parse_var(parser, pos, tokenizer, EcsAstProp);
}

/* Parse a single statement */
const char* flecs_script_stmt(
    ecs_parser_t *parser,
    const char *pos) 
{
    ParserBegin;

    bool name_is_expr_0 = false;

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
        case EcsTokKeywordConst:      goto const_var;
        case EcsTokKeywordExport:     goto export_var;
        case EcsTokKeywordIf:         goto if_stmt;
        case EcsTokKeywordFor:        goto for_stmt;
        EcsTokEndOfStatement:         EndOfRule;
    );

anonymous_entity: {
    return flecs_script_scope(parser, 
        flecs_script_insert_entity(parser, "_")->scope, pos);
}

string_name:
    /* If this is an interpolated string, we need to evaluate it as expression
     * at evaluation time. Otherwise we can just use the string as name. The 
     * latter is useful if an entity name contains special characters that are
     * not allowed in identifier tokens. */
    if (flecs_string_is_interpolated(Token(0))) {
        name_is_expr_0 = true;
    }

identifier: {
    // enterprise } (end of scope)
    LookAhead_1('}',
        goto insert_tag;
    )

    Parse(
        // enterprise {
        case '{': {
            return flecs_script_scope(parser, 
                flecs_script_insert_entity(parser, Token(0))->scope, pos);
        }

        // Red,
        case ',': {
            if (name_is_expr_0) {
                Error("expression not allowed as entity name here");
            }

            flecs_script_insert_entity(parser, Token(0));
            pos = flecs_script_comma_expr(parser, pos, false);
            EndOfRule;
        }

        // Npc\n
        EcsTokEndOfStatement: {
            // Npc\n{
            LookAhead_1('{',
                pos = lookahead;
                return flecs_script_scope(parser, 
                    flecs_script_insert_entity(parser, Token(0))->scope, pos);
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

        // x =
        case '=': {
            goto identifier_assign;
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
    if (Token(0)[0] == '$') {
        if (!flecs_script_insert_var_component(parser, &Token(0)[1])) {
            Error(
                "invalid context for variable component '%s': must be "
                    "part of entity", tokenizer->tokens[0].value);
        }
    } else {
        if (!flecs_script_insert_tag(parser, Token(0))) {
            Error(
                "invalid context for tag '%s': must be part of entity", 
                tokenizer->tokens[0].value);
        }
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
    // using flecs.meta\n
    Parse_2(EcsTokIdentifier, '\n',
        flecs_script_insert_module(parser, Token(1));
        EndOfRule;
    )
}

// template
template_stmt: {
    // template SpaceShip
    Parse_1(EcsTokIdentifier, 
        ecs_script_template_node_t *template = flecs_script_insert_template(
            parser, Token(1));

        Parse(
            // template SpaceShip {
            case '{':
                return flecs_script_scope(parser, template->scope, pos);

            // template SpaceShip\n
            EcsTokEndOfStatement:
                EndOfRule;
        )
    )
}

// prop
prop_var: {
    // prop color = Color:
    return flecs_script_parse_prop(parser, pos, tokenizer);
}

// export
export_var: {
    // export const
    Parse_1(EcsTokKeywordConst,
        return flecs_script_parse_export_const(parser, pos, tokenizer);
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
    // for i
    Parse_2(EcsTokIdentifier, EcsTokKeywordIn, {
        Expr(0, {
            ecs_expr_node_t *from = EXPR;
            Parse_1(EcsTokRange, {
                Expr(0, {
                    ecs_expr_node_t *to = EXPR;
                    ecs_script_for_range_t *stmt = 
                        flecs_script_insert_for_range(parser);
                    stmt->loop_var = Token(1);
                    stmt->from = from;
                    stmt->to = to;

                    Parse_1('{', {
                        return flecs_script_scope(parser, stmt->scope, pos);
                    });
                });
            });
        });

    });
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
    // // (Likes, Apples)
    // Parse_4(EcsTokIdentifier, ',', EcsTokIdentifier, ')',
    //     goto pair;
    // )
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

            // (Eats, Apples): {
            Parse_1('{', {
                    // (Eats, Apples): { expr }
                    Initializer('}',
                        ecs_script_component_t *comp = 
                            flecs_script_insert_pair_component(
                                parser, Token(1), Token(3));
                        comp->expr = INITIALIZER;
                        EndOfRule;
                    )
                }
            )
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

    // enterprise : SpaceShip
    Parse_1(EcsTokIdentifier, {
        ecs_script_entity_t *entity = flecs_script_insert_entity(
            parser, Token(0));

        Scope(entity->scope, 
            flecs_script_insert_pair_tag(parser, "IsA", Token(2));

            LookAhead_1(',', {
                pos = lookahead;
                pos = flecs_script_comma_expr(parser, pos, true);
            })
        )

        Parse(
            // enterprise : SpaceShip {
            case '{':
                return flecs_script_scope(parser, entity->scope, pos);
        )
    })
}

// x =
identifier_assign: {
    ecs_script_entity_t *entity = flecs_script_insert_entity(
        parser, Token(0));

    // x = Position:
    LookAhead_2(EcsTokIdentifier, ':',
        pos = lookahead;

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

        // x = Position: {
        Parse_1('{', {
            // x = Position: {expr}
            Expr('}',
                Scope(entity->scope, 
                    ecs_script_component_t *comp = 
                        flecs_script_insert_component(parser, Token(2));
                    comp->expr = EXPR;
                )

                // x = Position: {expr}\n
                Parse(
                    EcsTokEndOfStatement:
                        EndOfRule;
                )
            )
        })
    )

    // x = f32\n
    Initializer('\n',
        Scope(entity->scope, 
            ecs_script_default_component_t *comp = 
                flecs_script_insert_default_component(parser);
            comp->expr = INITIALIZER;
        )
        
        EndOfRule;
    )
}

// Spaceship enterprise
identifier_string:
identifier_identifier: {
    ecs_script_entity_t *entity = flecs_script_insert_entity(
        parser, Token(1));
    entity->kind = Token(0);

    // Spaceship enterprise :
    LookAhead_1(':', 
        pos = lookahead;

        Parse_1(EcsTokIdentifier, {
            Scope(entity->scope, 
                flecs_script_insert_pair_tag(parser, "IsA", Token(3));

                LookAhead_1(',', {
                    pos = lookahead;
                    pos = flecs_script_comma_expr(parser, pos, true);
                })
            )

            goto identifier_identifier_x;
        })
    )

identifier_identifier_x:
    Parse(
        // Spaceship enterprise\n
        EcsTokEndOfStatement: {
            EndOfRule;
        }

        // Spaceship enterprise {
        case '{': {
            return flecs_script_scope(parser, entity->scope, pos);
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
                )

                return flecs_script_scope(parser, entity->scope, pos);
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

    ParserEnd;
}

/* Parse script */
ecs_script_t* ecs_script_parse(
    ecs_world_t *world,
    const char *name,
    const char *code,
    const ecs_script_eval_desc_t *desc,
    ecs_script_eval_result_t *result) 
{
    (void)desc; /* Will be used in future to expand type checking features */

    if (!code) {
        code = "";
    }

    if (result) {
        ecs_log_start_capture(true);
    }

    ecs_script_t *script = flecs_script_new(world);
    script->name = ecs_os_strdup(name);
    script->code = ecs_os_strdup(code);

    ecs_script_impl_t *impl = flecs_script_impl(script);

    ecs_parser_t parser = {
        .name = script->name,
        .code = script->code,
        .script = impl,
        .scope = impl->root,
        .significant_newline = true
    };

    /* Allocate a buffer that is able to store all parsed tokens. Multiply the
     * size of the script by two so that there is enough space to add \0
     * terminators and expression deliminators ('""') 
     * The token buffer will exist for as long as the script object exists, and
     * ensures that AST nodes don't need to do separate allocations for the data
     * they contain. */
    impl->token_buffer_size = ecs_os_strlen(code) * 2 + 1;
    impl->token_buffer = flecs_alloc_w_dbg_info(
        &impl->allocator, impl->token_buffer_size, "token buffer");
    parser.token_cur = impl->token_buffer;

    /* Start parsing code */
    const char *pos = script->code;

    do {
        pos = flecs_script_stmt(&parser, pos);
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
        ecs_log_stop_capture();
    }

    return script;
error:
    if (result) {
        result->error = ecs_log_stop_capture();
    }

    ecs_script_free(script);
    return NULL;
}

#endif
