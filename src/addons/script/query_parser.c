/**
 * @file addons/script/query_parser.c
 * @brief Script grammar parser.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"
#include "parser.h"

#define EcsTokTermIdentifier\
    EcsTokIdentifier:\
    case EcsTokNumber:\
    case EcsTokMul

#define EcsTokEndOfTerm\
    '}':\
        pos --; /* Give token back to parser */\
    case EcsTokOr:\
        if (t->kind == EcsTokOr) {\
            if (parser->term->oper != EcsAnd) {\
                Error("cannot mix operators in || expression");\
            }\
            parser->term->oper = EcsOr;\
        }\
    case ',':\
    case '\n':\
    case '\0'

// $this ==
static
const char* flecs_term_parse_equality_pred(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_entity_t pred) 
{
    ParserBegin;

    if (parser->term->oper != EcsAnd) {
        Error("cannot mix operator with equality expression");
    }

    parser->term->src = parser->term->first;
    parser->term->first = (ecs_term_ref_t){0};
    parser->term->first.id = pred;

    Parse(
        // $this == foo
        //          ^
        case EcsTokTermIdentifier: {
            parser->term->second.name = Token(0);
            Parse( case EcsTokEndOfTerm: EndOfRule; )
        }

        // $this == "foo"
        //          ^
        case EcsTokString: {
            parser->term->second.name = Token(0);
            parser->term->second.id = EcsIsName;

            if (pred == EcsPredMatch) {
                if (Token(0)[0] == '!') {
                    /* If match expression starts with !, set Not operator. The
                     * reason the ! is embedded in the expression is because 
                     * there is only a single match (~=) operator. */
                    parser->term->second.name ++;
                    parser->term->oper = EcsNot;
                }
            }

            Parse( 
                case EcsTokEndOfTerm:
                    EndOfRule; 
            )
        }
    )

    ParserEnd;
}

static
ecs_entity_t flecs_query_parse_trav_flags(
    const char *tok)
{
         if (!ecs_os_strcmp(tok, "self"))    return EcsSelf;
    else if (!ecs_os_strcmp(tok, "up"))      return EcsUp;
    else if (!ecs_os_strcmp(tok, "cascade")) return EcsCascade;
    else if (!ecs_os_strcmp(tok, "desc"))    return EcsDesc;
    else return 0;
}

static
const char* flecs_term_parse_trav(
    ecs_script_parser_t *parser,
    ecs_term_ref_t *ref,
    const char *pos) 
{
    ParserBegin;

    Loop(
        // self
        Parse_1(EcsTokIdentifier,
            ref->id |= flecs_query_parse_trav_flags(Token(0));

            LookAhead(
                // self|
                case '|':
                    pos = lookahead;
                    continue;

                // self IsA
                case EcsTokIdentifier:
                    pos = lookahead;
                    parser->term->trav = ecs_lookup(
                        parser->script->pub.world, Token(1));
                    if (!parser->term->trav) {
                        Error(
                            "unresolved traversal relationship '%s'", Token(1));
                        goto error;
                    }

                    EndOfRule;
            )

            EndOfRule;
        )
    )

    ParserEnd;
}

// Position(
static
const char* flecs_term_parse_arg(
    ecs_script_parser_t *parser,
    const char *pos,
    int32_t arg)
{
    ParserBegin;

    ecs_term_ref_t *ref = NULL;

    // Position(src
    if (arg == 0) {
        ref = &parser->term->src;

    // Position(src, tgt
    } else if (arg == 1) {
        ref = &parser->term->second;
    } else {
        if (arg > FLECS_TERM_ARG_COUNT_MAX) {
            Error("too many arguments in term");
        }
        ref = &parser->extra_args[arg - 2];
    }

    bool is_trav_flag = false;

    LookAhead_1(EcsTokIdentifier, 
        is_trav_flag = flecs_query_parse_trav_flags(Token(0)) != 0;
    )

    if (is_trav_flag) {
        // Position(self|up
        //          ^
        pos = flecs_term_parse_trav(parser, ref, pos);
        if (!pos) {
            goto error;
        }
    } else {
        // Position(src
        //          ^
        Parse(
            case EcsTokTermIdentifier: {
                ref->name = Token(0);

                // Position(src|
                //          ^
                LookAhead_1('|',
                    pos = lookahead;
                    pos = flecs_term_parse_trav(parser, ref, pos);
                    if (!pos) {
                        goto error;
                    }

                    // Position(src|up IsA
                    //          ^
                    LookAhead_1(EcsTokIdentifier,
                        pos = lookahead;
                        parser->term->trav = ecs_lookup(
                            parser->script->pub.world, Token(1));
                        if (!parser->term->trav) {
                            Error(
                                "unresolved trav identifier '%s'", Token(1));
                        }
                    )
                )

                break;
            }
        )
    }

    Parse(
        // Position(src,
        //          ^
        case ',':
            if ((arg > 1) && parser->extra_oper != EcsAnd) {
                Error("cannot mix operators in extra term arguments");
            }
            parser->extra_oper = EcsAnd;
            return flecs_term_parse_arg(parser, pos, arg + 1);

        // Position(src, second ||
        //          ^
        case EcsTokOr:
            if ((arg > 1) && parser->extra_oper != EcsOr) {
                Error("cannot mix operators in extra term arguments");
            }
            parser->extra_oper = EcsOr;
            return flecs_term_parse_arg(parser, pos, arg + 1);

        // Position(src)
        //          ^
        case ')':
            Parse(
                case EcsTokEndOfTerm:
                    EndOfRule;
            )
    )

    ParserEnd;
}

// Position
static
const char* flecs_term_parse_id(
    ecs_script_parser_t *parser,
    const char *pos) 
{
    ParserBegin;

    Parse(
        case EcsTokEq:
            return flecs_term_parse_equality_pred(
                parser, pos, EcsPredEq);
        case EcsTokNeq: {
            const char *ret = flecs_term_parse_equality_pred(
                parser, pos, EcsPredEq);
            if (ret) {
                parser->term->oper = EcsNot;
            }
            return ret;
        }
        case EcsTokMatch:
            return flecs_term_parse_equality_pred(
                parser, pos, EcsPredMatch);

        // Position|
        case '|': {
            pos = flecs_term_parse_trav(parser, &parser->term->first, pos);
            if (!pos) {
                goto error;
            }

            // Position|self(
            Parse(
                case '(':
                    return flecs_term_parse_arg(parser, pos, 0);
                case EcsTokEndOfTerm:
                    EndOfRule;
            )
        }

        // Position(
        case '(': {
            // Position()
            LookAhead_1(')',
                pos = lookahead;
                parser->term->src.id = EcsIsEntity;

                Parse(
                    case EcsTokEndOfTerm:
                        EndOfRule;
                )
            )

            return flecs_term_parse_arg(parser, pos, 0);
        }

        case EcsTokEndOfTerm: 
            EndOfRule;
    )

    ParserEnd;
}

// (
static const char* flecs_term_parse_pair(
    ecs_script_parser_t *parser,
    const char *pos)
{
    ParserBegin;

    // (Position
    //  ^
    Parse(
        case EcsTokTermIdentifier: {
            parser->term->first.name = Token(0);

            LookAhead_1('|',
                // (Position|self
                pos = lookahead;
                pos = flecs_term_parse_trav(
                    parser, &parser->term->first, pos);
                if (!pos) {
                    goto error;
                }
            )

            // (Position,
            Parse_1(',',
                return flecs_term_parse_arg(parser, pos, 1);
            )
        }
    )

    ParserEnd;
}

// AND
static
const char* flecs_term_parse_flags(
    ecs_script_parser_t *parser,
    const char *token_0,
    const char *pos) 
{
    ecs_assert(token_0 != NULL, ECS_INTERNAL_ERROR, NULL);

    ParserBegin;

    ecs_id_t flag = 0;
    int16_t oper = 0;
    ecs_term_t *term = parser->term;

    // AND
    if      (!ecs_os_strcmp(token_0, "and"))      oper = EcsAndFrom;
    else if (!ecs_os_strcmp(token_0, "or"))       oper = EcsOrFrom;
    else if (!ecs_os_strcmp(token_0, "not"))      oper = EcsNotFrom;
    else if (!ecs_os_strcmp(token_0, "auto_override")) flag = ECS_AUTO_OVERRIDE;
    else if (!ecs_os_strcmp(token_0, "toggle"))   flag = ECS_TOGGLE;
    else {
        // Position
        term->first.name = token_0;
        return flecs_term_parse_id(parser, pos);
    }

    if (oper || flag) {
        // and |
        //     ^
        Parse_1('|', 
            Parse(
                // and | Position
                //     ^
                case EcsTokTermIdentifier: { 
                    if (oper) {
                        term->oper = oper;
                    } else if (flag) {
                        term->id = flag;
                    }

                    term->first.name = Token(1);

                    return flecs_term_parse_id(parser, pos);
                }

                // and | (
                //     ^
                case '(': {
                    return flecs_term_parse_pair(parser, pos);
                }
            )
        )
    }

    ParserEnd;
}

// !
static
const char* flecs_term_parse_unary(
    ecs_script_parser_t *parser,
    const char *pos)
{
    ParserBegin;

    Parse(
        // !(
        case '(': {
            return flecs_term_parse_pair(parser, pos);
        }

        // !{
        case '{': {
            parser->term->first.id = EcsScopeOpen;
            parser->term->src.id = EcsIsEntity;
            parser->term->inout = EcsInOutNone;
            EndOfRule;
        }

        // !Position
        //  ^
        case EcsTokTermIdentifier: {
            parser->term->first.name = Token(0);
            return flecs_term_parse_id(parser, pos);
        }
    )

    ParserEnd;
}

// [
static
const char* flecs_term_parse_inout(
    ecs_script_parser_t *parser,
    const char *pos)
{
    ParserBegin;

    ecs_term_t *term = parser->term;

    // [inout]
    //  ^
    Parse_2(EcsTokIdentifier, ']',
        if      (!ecs_os_strcmp(Token(0), "default")) term->inout = EcsInOutDefault;
        else if (!ecs_os_strcmp(Token(0), "none"))    term->inout = EcsInOutNone;
        else if (!ecs_os_strcmp(Token(0), "filter"))  term->inout = EcsInOutFilter;
        else if (!ecs_os_strcmp(Token(0), "inout"))   term->inout = EcsInOut;
        else if (!ecs_os_strcmp(Token(0), "in"))      term->inout = EcsIn;
        else if (!ecs_os_strcmp(Token(0), "out"))     term->inout = EcsOut;

        Parse(
            // [inout] Position
            //  ^
            case EcsTokTermIdentifier: { 
                return flecs_term_parse_flags(parser, Token(2), pos);
            }

            // [inout] !Position
            //  ^
            case '!':
                term->oper = EcsNot;
                return flecs_term_parse_unary(parser, pos);
            case '?':
                term->oper = EcsOptional;
                return flecs_term_parse_unary(parser, pos);

            // [inout] (
            //  ^
            case '(': {
                return flecs_term_parse_pair(parser, pos);
            }
        )
    )

    ParserEnd;
}

static
const char* flecs_query_term_parse(
    ecs_script_parser_t *parser,
    const char *pos) 
{
    ParserBegin;

    Parse(
        case '[':
            return flecs_term_parse_inout(parser, pos);
        case EcsTokTermIdentifier:   
            return flecs_term_parse_flags(parser, Token(0), pos);
        case '(':
            return flecs_term_parse_pair(parser, pos);
        case '!':
            parser->term->oper = EcsNot;
            return flecs_term_parse_unary(parser, pos);
        case '?':
            parser->term->oper = EcsOptional;
            return flecs_term_parse_unary(parser, pos);
        case '{':
            parser->term->first.id = EcsScopeOpen;
            parser->term->src.id = EcsIsEntity;
            parser->term->inout = EcsInOutNone;
            EndOfRule;
        case '}':
            parser->term->first.id = EcsScopeClose;
            parser->term->src.id = EcsIsEntity;
            parser->term->inout = EcsInOutNone;
            LookAhead_1(',',
                pos = lookahead;
            )
            EndOfRule;
        case '\n':\
        case '\0':
            EndOfRule;
    );

    ParserEnd;
}

int flecs_terms_parse(
    ecs_script_t *script,
    ecs_term_t *terms,
    int32_t *term_count_out)
{
    if (!ecs_os_strcmp(script->code, "0")) {
        *term_count_out = 0;
        return 0;
    }

    ecs_script_parser_t parser = {
        .script = flecs_script_impl(script),
        .pos = script->code
    };

    parser.token_cur = flecs_script_impl(script)->token_buffer;

    int32_t term_count = 0;
    const char *ptr = script->code;
    ecs_term_ref_t extra_args[FLECS_TERM_ARG_COUNT_MAX];
    ecs_os_memset_n(extra_args, 0, ecs_term_ref_t, 
        FLECS_TERM_ARG_COUNT_MAX);

    parser.extra_args = extra_args;
    parser.extra_oper = 0;

    do {
        if (term_count == FLECS_TERM_COUNT_MAX) {
            ecs_err("max number of terms (%d) reached, increase "
                "FLECS_TERM_COUNT_MAX to support more",
                FLECS_TERM_COUNT_MAX);
            goto error;
        }

        /* Parse next term */
        ecs_term_t *term = &terms[term_count];
        parser.term = term;
        ecs_os_memset_t(term, 0, ecs_term_t);
        ecs_os_memset_n(extra_args, 0, ecs_term_ref_t, FLECS_TERM_ARG_COUNT_MAX);
        parser.extra_oper = 0;

        ptr = flecs_query_term_parse(&parser, ptr);
        if (!ptr) {
            /* Parser error */
            goto error;
        }

        if (!ecs_term_is_initialized(term)) {
            /* Last term parsed */
            break;
        }

        term_count ++;

        /* Unpack terms with more than two args into multiple terms so that:
         *   Rel(X, Y, Z)
         *  becomes:
         *   Rel(X, Y), Rel(Y, Z) */
        int32_t arg = 0;
        while (ecs_term_ref_is_set(&extra_args[arg ++])) {
            ecs_assert(arg <= FLECS_TERM_ARG_COUNT_MAX, 
                ECS_INTERNAL_ERROR, NULL);

            if (term_count == FLECS_TERM_COUNT_MAX) {
                ecs_err("max number of terms (%d) reached, increase "
                    "FLECS_TERM_COUNT_MAX to support more",
                    FLECS_TERM_COUNT_MAX);
                goto error;
            }

            term = &terms[term_count ++];
            *term = term[-1];

            if (parser.extra_oper == EcsAnd) {
                term->src = term[-1].second;
                term->second = extra_args[arg - 1];
            } else if (parser.extra_oper == EcsOr) {
                term->src = term[-1].src;
                term->second = extra_args[arg - 1];
                term[-1].oper = EcsOr;
            }

            if (term->first.name != NULL) {
                term->first.name = term->first.name;
            }
    
            if (term->src.name != NULL) {
                term->src.name = term->src.name;
            }
        }

        if (arg) {
            ecs_os_memset_n(extra_args, 0, ecs_term_ref_t, 
                FLECS_TERM_ARG_COUNT_MAX);
        }
    } while (ptr[0]);

    (*term_count_out) += term_count;

    return 0;
error:
    return -1;
}

const char* flecs_term_parse(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_term_t *term,
    char *token_buffer)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(expr != NULL, ECS_INVALID_PARAMETER, name);
    ecs_assert(term != NULL, ECS_INVALID_PARAMETER, NULL);

    EcsParserFixedBuffer(world, name, expr, token_buffer, 256);
    parser.term = term;

    const char *result = flecs_query_term_parse(&parser, expr);
    if (!result) {
        return NULL;
    }

    ecs_os_memset_t(term, 0, ecs_term_t);

    return flecs_query_term_parse(&parser, expr);
}

const char* flecs_id_parse(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_id_t *id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(expr != NULL, ECS_INVALID_PARAMETER, name);
    ecs_assert(id != NULL, ECS_INVALID_PARAMETER, NULL);

    char token_buffer[256];
    ecs_term_t term = {0};
    EcsParserFixedBuffer(world, name, expr, token_buffer, 256);
    parser.term = &term;

    expr = flecs_scan_whitespace(&parser, expr);
    if (!ecs_os_strcmp(expr, "#0")) {
        *id = 0;
        return &expr[1];
    }

    const char *result = flecs_query_term_parse(&parser, expr);
    if (!result) {
        return NULL;
    }

    if (ecs_term_finalize(world, &term)) {
        return NULL;
    }

    if (!ecs_id_is_valid(world, term.id)) {
        ecs_parser_error(name, expr, (result - expr), 
            "invalid term for add expression");
        return NULL;
    }

    if (term.oper != EcsAnd) {
        ecs_parser_error(name, expr, (result - expr), 
            "invalid operator for add expression");
        return NULL;
    }

    if ((term.src.id & ~EcsTraverseFlags) != (EcsThis|EcsIsVariable)) {
        ecs_parser_error(name, expr, (result - expr), 
            "invalid source for add expression (must be $this)");
        return NULL;
    }

    *id = term.id;
    
    return result;
}

static
const char* flecs_query_arg_parse(
    ecs_script_parser_t *parser,
    ecs_query_t *q,
    ecs_iter_t *it,
    const char *pos)
{
    ParserBegin;

    Parse_3(EcsTokIdentifier, ':', EcsTokIdentifier, {
        int var = ecs_query_find_var(q, Token(0));
        if (var == -1) {
            Error("unknown variable '%s'", Token(0));
        }

        ecs_entity_t val = ecs_lookup(q->world, Token(2));
        if (!val) {
            Error("unresolved entity '%s'", Token(2));
        }

        ecs_iter_set_var(it, var, val);

        EndOfRule;
    })

    ParserEnd;
}

static
const char* flecs_query_args_parse(
    ecs_script_parser_t *parser,
    ecs_query_t *q,
    ecs_iter_t *it,
    const char *pos)
{
    ParserBegin;

    bool has_paren = false;
    LookAhead(
        case '\0':
            pos = lookahead;
            EndOfRule;
        case '(': {
            pos = lookahead;
            has_paren = true;
            LookAhead_1(')',
                pos = lookahead;
                EndOfRule;
            )
        }
    )

    Loop(
        pos = flecs_query_arg_parse(parser, q, it, pos);
        if (!pos) {
            goto error;
        }

        Parse(
            case ',':
                continue;
            case '\0':
                EndOfRule;
            case ')':
                if (!has_paren) {
                    Error("unexpected ')' without opening '(')");
                }
                EndOfRule;
        )
    )

    ParserEnd;
}

const char* ecs_query_args_parse(
    ecs_query_t *q,
    ecs_iter_t *it,
    const char *expr)
{
    flecs_poly_assert(q, ecs_query_t);
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(expr != NULL, ECS_INVALID_PARAMETER, NULL);

    const char *q_name = q->entity ? ecs_get_name(q->world, q->entity) : NULL;
    if (ecs_os_strlen(expr) > 512) {
        ecs_parser_error(q_name, expr, 0, "query argument expression too long");
        return NULL;
    }

    char token_buffer[1024];
    EcsParserFixedBuffer(q->world, q_name, expr, token_buffer, 256);
    return flecs_query_args_parse(&parser, q, it, expr);
error:
    return NULL;
}

#endif
