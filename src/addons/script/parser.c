/**
 * @file addons/plecs.c
 * @brief Plecs addon.
 */

#include "flecs.h"
#include "script.h"
#include "parser.h"

/* Parser implementation */

const char* flecs_script_stmt(
    ecs_script_parser_t *parser,
    const char *pos);

const char* flecs_script_scope(
    ecs_script_parser_t *parser,
    ecs_script_scope_t *scope,
    const char *pos)
{
    ParserBegin;

    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pos[-1] == '{', ECS_INTERNAL_ERROR, NULL);

    ecs_script_scope_t *prev = parser->scope;
    parser->scope = scope;

    do {
        LookAhead(
            case EcsTokScopeClose:
                pos = lookahead;
                goto scope_close;
            case EcsTokEnd:
                Error(parser, "unexpected end of script");
                goto error;
        )

        pos = flecs_script_stmt(parser, pos);
        if (!pos) {
            goto error;
        }
    } while(true);

scope_close:
    parser->scope = prev;

    ecs_assert(pos[-1] == '}', ECS_INTERNAL_ERROR, NULL);
    return pos;

    ParserEnd;
}

const char* flecs_script_with_expr(
    ecs_script_parser_t *parser,
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
                Expr(')',
                    ecs_script_component_t *component =
                        flecs_script_insert_component(parser, Token(0));
                    component->node.kind = EcsAstWithComponent;
                    component->expr = Token(2);
                    EndOfRule;
                )
            )

            ecs_script_tag_t *tag =
                flecs_script_insert_tag(parser, Token(0));
            tag->node.kind = EcsAstWithTag;
            EndOfRule;
        }
        
        case '$': {
            // $color
            Parse_1(EcsTokIdentifier,
                flecs_script_insert_with_var(parser, Token(1));
                EndOfRule;
            )
        }
    )

    ParserEnd;
}

const char* flecs_script_with(
    ecs_script_parser_t *parser,
    ecs_script_with_t *with,
    const char *pos)
{
    ParserBegin;

    bool has_next;
    do {
        has_next = false;

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

const char* flecs_script_stmt(
    ecs_script_parser_t *parser,
    const char *pos) 
{
    ParserBegin;

    Parse(
        case EcsTokIdentifier:        goto identifier;
        case '{':                     return flecs_script_scope(parser, parser->scope, pos);
        case '(':                     goto paren;
        case '$':                     goto variable;
        case '@':                     goto annotation;
        case EcsTokKeywordWith:       goto with;
        case EcsTokKeywordUsing:      goto using;
        case EcsTokKeywordAssembly:   goto assembly;
        case EcsTokKeywordProp:       goto prop_var;
        case EcsTokKeywordConst:      goto const_var;
        case EcsTokKeywordIf:         goto if_stmt;
        case '\n':                    EndOfRule;
        case '\0':                    printf("end\n"); EndOfRule;
    );

identifier: {
    Parse(
        // enterprise {
        case '{': {
            return flecs_script_scope(parser, 
                flecs_script_insert_entity(
                    parser, Token(0))->scope, pos);
        }

        // Red,
        case ',': {
            flecs_script_insert_entity(parser, Token(0));
            EndOfRule;
        }

        // Npc\n
        case '\n': {
            if (!flecs_script_insert_tag(parser, Token(0))) {
                Error(parser, 
                    "invalid context for tag '%s': must be part of entity", 
                    tokens[0].value);
            }
            EndOfRule;
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
    )
}

// $
variable: {
    Parse_1(EcsTokIdentifier,
        Parse(
            // $color\n
            case '\n': {
                flecs_script_insert_with_var(parser, Token(1));
                EndOfRule;
            }

            // $entity_name {
            case '{': {
                ecs_script_entity_t *entity = flecs_script_insert_entity(
                    parser, Token(1));
                entity->name_is_var = true;
                return flecs_script_scope(parser, entity->scope, pos);
            }
        )
    )
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
with: {
    ecs_script_with_t *with = flecs_script_insert_with(parser);
    pos = flecs_script_with(parser, with, pos);
    EndOfRule;
}

// using
using: {
    // using flecs.meta\n
    Parse_2(EcsTokIdentifier, '\n',
        flecs_script_insert_using(parser, Token(1));
        EndOfRule;
    )
}

// assembly
assembly: {
    // assembly SpaceShip {
    Parse_2(EcsTokIdentifier, '{',
        ecs_script_assembly_t *assembly = flecs_script_insert_assembly(
            parser, Token(1));
        return flecs_script_scope(parser, assembly->scope, pos);
    )
}

// prop
prop_var: {
    // prop color : Color =
    Parse_4(EcsTokIdentifier, ':', EcsTokIdentifier, '=',
        ecs_script_var_node_t *var = flecs_script_insert_var(
            parser, Token(1));
        var->node.kind = EcsAstProp;
        var->type = Token(3);

        // prop color : Color = {
        LookAhead_1('{',
            // prop color : Color = {expr}
            pos = lookahead;
            Expr('}',
                var->expr = Token(6);
                EndOfRule;
            )
        )

        // prop color : Color = expr\n
        Expr('\n',
            var->expr = Token(5);
            EndOfRule;
        )
    )
}

// const
const_var: {
    // const color
    Parse_1(EcsTokIdentifier,
        ecs_script_var_node_t *var = flecs_script_insert_var(
            parser, Token(1));
        var->node.kind = EcsAstConst;

        Parse(
            // const color :
            case ':': {
                // const color : Color =
                Parse_2(EcsTokIdentifier, '=',
                    var->type = Token(3);

                    // const color : Color = {
                    LookAhead_1('{',
                        // const color : Color = {expr}
                        pos = lookahead;
                        Expr('}',
                            var->expr = Token(6);
                            EndOfRule;
                        )
                    )

                    // const color : Color = expr\n
                    Expr('\n',
                        var->expr = Token(5);
                        EndOfRule;
                    )
                )
            }

            case '=': {
                // const PI = expr\n
                Expr('\n',
                    var->expr = Token(4);
                    EndOfRule;
                )
            }
        )
    )
}

// if
if_stmt: {
    // if expr {
    Expr('{',
        ecs_script_if_t *stmt = flecs_script_insert_if(parser);
        stmt->expr = Token(1);
        pos = flecs_script_scope(parser, stmt->if_true, pos);
        if (!pos) {
            goto error;
        }

        // if expr { } else
        LookAhead_1(EcsTokKeywordElse, 
            pos = lookahead;

            // if expr { } else {
            Parse_1('{',
                return flecs_script_scope(parser, stmt->if_false, pos);
            )
        )

        EndOfRule;
    )
}

// (
paren: {
    // (Likes, Apples)
    Parse_4(EcsTokIdentifier, ',', EcsTokIdentifier, ')',
        goto pair;
    )
}

// (Likes, Apples)
pair: {
    Parse(
        // (Likes, Apples)\n
        case '\n': {
            flecs_script_insert_pair_tag(parser, Token(1), Token(3));
            EndOfRule;
        }

        // (Eats, Apples):
        case ':': {
            // (Eats, Apples): {
            Parse_1('{',
                // (Eats, Apples): { expr }
                Expr('}',
                    ecs_script_component_t *comp = 
                        flecs_script_insert_pair_component(
                            parser, Token(1), Token(3));
                    comp->expr = Token(7);
                    EndOfRule;
                )
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
        // enterprise : SpaceShip {
        LookAhead_2(EcsTokIdentifier, '{', {
            pos = lookahead;

            ecs_script_entity_t *entity = flecs_script_insert_entity(
                parser, Token(0));

            printf(" -: %s %s %s %s\n", Token(0), Token(1), Token(2), Token(3));

            Scope(entity->scope, 
                flecs_script_insert_pair_tag(parser, "IsA", Token(2));
            )

            return flecs_script_scope(parser, entity->scope, pos);
        })
    }

    // x : f32
    Expr('\n',
        ecs_script_entity_t *entity = flecs_script_insert_entity(
            parser, Token(0));

        Scope(entity->scope, 
            ecs_script_default_component_t *comp = 
                flecs_script_insert_default_component(parser);
            comp->expr = Token(2);
        )
        
        EndOfRule;
    )
}

// Spaceship enterprise
identifier_identifier: {
    Parse(
        // Spaceship enterprise\n
        case '\n': {
            ecs_script_entity_t *entity = flecs_script_insert_entity(
                parser, Token(1));
            entity->kind = Token(0);
            EndOfRule;
        }

        // Spaceship enterprise {
        case '{': {
            ecs_script_entity_t *entity = flecs_script_insert_entity(
                parser, Token(1));
            entity->kind = Token(0);
            return flecs_script_scope(parser, entity->scope, pos);
        }

        // Spaceship enterprise(
        case '(': {
            goto component_expr_paren;
        }
    )
}

// SpaceShip(
identifier_paren: {
    // SpaceShip()
    Expr(')', 
        Parse(
            // SpaceShip(expr)\n
            case '\n': {
                ecs_script_entity_t *entity = flecs_script_insert_entity(
                    parser, NULL);

                Scope(entity->scope, 
                    ecs_script_component_t *comp = 
                        flecs_script_insert_component(parser, Token(0)); 
                    comp->expr = Token(2);
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
                    comp->expr = Token(2);
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
        comp->expr = Token(3);
        EndOfRule; 
    })
}

// Position spaceship (
component_expr_paren: {
    Parse(
        // Position spaceship ()
        case ')': {
            Parse(
                case '\n': {
                    ecs_script_entity_t *entity = flecs_script_insert_entity(
                        parser, Token(1));
                    entity->kind = Token(0);

                    Scope(entity->scope, 
                        flecs_script_insert_component(parser, Token(0)); )

                    EndOfRule;
                }
                case '{': {
                    ecs_script_entity_t *entity = flecs_script_insert_entity(
                        parser, Token(1));
                    entity->kind = Token(0);

                    Scope(entity->scope, 
                        flecs_script_insert_component(parser, Token(0)); )

                    return flecs_script_scope(parser, entity->scope, pos);
                }
            )
        }
    )
}

    ParserEnd;
}

ecs_script_t* ecs_script_parse(
    ecs_world_t *world,
    const char *name,
    const char *code) 
{
    ecs_script_t *script = flecs_script_new(world);
    script->name = name;
    script->code = code;

    ecs_script_parser_t parser = {
        .script = script,
        .scope = script->root,
    };

    script->token_buffer = ecs_os_malloc(ecs_os_strlen(code) * 2 + 1),
    parser.token_cur = script->token_buffer;

    const char *pos = code;

    do {
        pos = flecs_script_stmt(&parser, pos);
        if (!pos) {
            goto error;
        }

        if (!pos[0]) {
            break;
        }
    } while (true);

    return script;
error:
    ecs_os_free(script->token_buffer);
    return NULL;
}

ecs_script_t* ecs_script_parse_file(
    ecs_world_t *world,
    const char *filename) 
{
    char *code = flecs_load_from_file(filename);
    if (!code) {
        return NULL;
    }

    return ecs_script_parse(world, filename, code);
}
