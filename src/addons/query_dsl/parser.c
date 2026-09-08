/**
 * @file addons/query_dsl/parser.c
 * @brief Query DSL parser.
 */

#include "flecs.h"

#ifdef FLECS_QUERY_DSL

#include "../parser/grammar.h"
#include "query_dsl.h"

typedef struct flecs_term_parser_t {
    ecs_parser_t *parser;
    const char *pos;
    ecs_token_t token;
} flecs_term_parser_t;

static bool flecs_term_next(flecs_term_parser_t *p) {
    p->pos = flecs_token(p->parser, p->pos, &p->token, false);
    return p->pos != NULL;
}

static bool flecs_term_identifier(flecs_term_parser_t *p) {
    return p->token.kind == EcsTokIdentifier ||
        p->token.kind == EcsTokNumber || p->token.kind == EcsTokMul;
}

static bool flecs_term_error(flecs_term_parser_t *p, const char *message) {
    ecs_parser_t *parser = p->parser;
    ecs_parser_error(parser->name, parser->code,
        flecs_parser_errpos(parser, p->pos ? p->pos - 1 : parser->pos),
        "%s", message);
    return false;
}

static ecs_entity_t flecs_query_parse_trav_flags(const char *tok) {
    if (!ecs_os_strcmp(tok, "self")) return EcsSelf;
    if (!ecs_os_strcmp(tok, "up")) return EcsUp;
    if (!ecs_os_strcmp(tok, "cascade")) return EcsCascade;
    if (!ecs_os_strcmp(tok, "desc")) return EcsDesc;
    return 0;
}

static bool flecs_term_trav(flecs_term_parser_t *p, ecs_term_ref_t *ref) {
    for (;;) {
        if (p->token.kind != EcsTokIdentifier) {
            return flecs_term_error(p, "expected traversal flag");
        }
        ref->id |= flecs_query_parse_trav_flags(p->token.value);
        if (!flecs_term_next(p)) return false;
        if (p->token.kind == '|') {
            if (!flecs_term_next(p)) return false;
            continue;
        }
        if (p->token.kind == EcsTokIdentifier) {
            p->parser->term->trav = ecs_lookup(p->parser->world, p->token.value);
            if (!p->parser->term->trav) {
                return flecs_term_error(p, "unresolved traversal relationship");
            }
            if (!flecs_term_next(p)) return false;
        }
        return true;
    }
}

static bool flecs_term_ref(flecs_term_parser_t *p, ecs_term_ref_t *ref) {
    if (p->token.kind == EcsTokIdentifier &&
        flecs_query_parse_trav_flags(p->token.value))
    {
        return flecs_term_trav(p, ref);
    }
    if (p->token.kind == '@') {
        p->parser->term->id = ECS_VALUE_PAIR;
        if (!flecs_term_next(p)) return false;
        if (p->token.kind == EcsTokMul) {
            ref->id = EcsWildcard;
        } else if (p->token.kind == EcsTokNumber) {
            ref->id = strtoul(p->token.value, NULL, 10);
        } else if (p->token.kind == EcsTokIdentifier) {
            ref->name = p->token.value;
        } else {
            return flecs_term_error(p, "expected value pair target");
        }
        return flecs_term_next(p);
    }
    if (!flecs_term_identifier(p)) {
        return flecs_term_error(p, "expected term argument");
    }
    ref->name = p->token.value;
    if (!flecs_term_next(p)) return false;
    if (p->token.kind == '|') {
        if (!flecs_term_next(p) || !flecs_term_trav(p, ref)) return false;
        if (p->token.kind == EcsTokIdentifier) {
            p->parser->term->trav = ecs_lookup(p->parser->world, p->token.value);
            if (!p->parser->term->trav) {
                return flecs_term_error(p, "unresolved traversal relationship");
            }
            return flecs_term_next(p);
        }
    }
    return true;
}

static bool flecs_term_args(flecs_term_parser_t *p, int32_t arg) {
    ecs_parser_t *parser = p->parser;
    for (;;) {
        ecs_term_ref_t *ref;
        if (!arg) {
            ref = &parser->term->src;
        } else if (arg == 1) {
            ref = &parser->term->second;
        } else {
            if (arg > FLECS_TERM_ARG_COUNT_MAX || !parser->extra_args) {
                return flecs_term_error(p, "too many arguments in term");
            }
            ref = &parser->extra_args[arg - 2];
        }
        if (!flecs_term_ref(p, ref)) return false;
        if (p->token.kind == ')') return flecs_term_next(p);
        if (p->token.kind != ',' && p->token.kind != EcsTokOr) {
            return flecs_term_error(p, "expected argument separator or ')'");
        }
        ecs_oper_kind_t oper = p->token.kind == ',' ? EcsAnd : EcsOr;
        if (arg > 1 && parser->extra_oper != oper) {
            return flecs_term_error(p, "cannot mix operators in extra term arguments");
        }
        parser->extra_oper = oper;
        arg ++;
        if (!flecs_term_next(p)) return false;
    }
}

static bool flecs_term_pair(flecs_term_parser_t *p) {
    if (!flecs_term_next(p) || !flecs_term_identifier(p)) {
        return flecs_term_error(p, "expected pair relationship");
    }
    p->parser->term->first.name = p->token.value;
    if (!flecs_term_next(p)) return false;
    if (p->token.kind == '|') {
        if (!flecs_term_next(p) || !flecs_term_trav(p, &p->parser->term->first)) {
            return false;
        }
    }
    if (p->token.kind != ',') return flecs_term_error(p, "expected ',' in pair");
    return flecs_term_next(p) && flecs_term_args(p, 1);
}

static const char* flecs_query_term_parse(
    ecs_parser_t *parser,
    const char *pos)
{
    flecs_term_parser_t p = { .parser = parser, .pos = pos };
    ecs_term_t *term = parser->term;
    bool inout = false, unary = false, neq = false;
    if (!flecs_term_next(&p)) return NULL;
    if (p.token.kind == '[') {
        inout = true;
        if (!flecs_term_next(&p) || p.token.kind != EcsTokIdentifier) goto unexpected;
        static const char *names[] = {"default", "none", "filter", "inout", "in", "out"};
        static const ecs_inout_kind_t kinds[] = {
            EcsInOutDefault, EcsInOutNone, EcsInOutFilter, EcsInOut, EcsIn, EcsOut
        };
        for (int32_t i = 0; i < 6; i ++) {
            if (!ecs_os_strcmp(p.token.value, names[i])) {
                term->inout = flecs_ito(int16_t, kinds[i]);
                break;
            }
        }
        if (!flecs_term_next(&p) || p.token.kind != ']' || !flecs_term_next(&p)) {
            goto unexpected;
        }
    }
    if (p.token.kind == '!' || p.token.kind == '?') {
        term->oper = p.token.kind == '!' ? EcsNot : EcsOptional;
        unary = true;
        if (!flecs_term_next(&p)) return NULL;
    }
    if (p.token.kind == '{' && (!inout || unary)) {
        term->first.id = EcsScopeOpen;
        term->src.id = EcsIsEntity;
        term->inout = EcsInOutNone;
        return p.pos;
    }
    if (!inout && !unary) {
        if (p.token.kind == EcsTokEnd || p.token.kind == EcsTokNewline) return p.pos;
        if (p.token.kind == '}') {
            term->first.id = EcsScopeClose;
            term->src.id = EcsIsEntity;
            term->inout = EcsInOutNone;
            const char *end = p.pos;
            if (!flecs_term_next(&p)) return NULL;
            return p.token.kind == ',' ? p.pos : end;
        }
    }
    if (flecs_term_identifier(&p) && !unary) {
        int16_t oper = 0;
        ecs_id_t flag = 0;
        const char *name = p.token.value;
        if (!ecs_os_strcmp(name, "and")) oper = EcsAndFrom;
        else if (!ecs_os_strcmp(name, "or")) oper = EcsOrFrom;
        else if (!ecs_os_strcmp(name, "not")) oper = EcsNotFrom;
        else if (!ecs_os_strcmp(name, "auto_override")) flag = ECS_AUTO_OVERRIDE;
        else if (!ecs_os_strcmp(name, "toggle")) flag = ECS_TOGGLE;
        if (oper || flag) {
            if (!flecs_term_next(&p) || p.token.kind != '|' || !flecs_term_next(&p)) {
                goto unexpected;
            }
            if (flecs_term_identifier(&p)) {
                if (oper) term->oper = oper;
                if (flag) term->id = flag;
            } else if (p.token.kind != '(') {
                goto unexpected;
            }
        }
    }
    if (p.token.kind == '(') {
        if (!flecs_term_pair(&p)) return NULL;
    } else if (flecs_term_identifier(&p)) {
        term->first.name = p.token.value;
        if (!flecs_term_next(&p)) return NULL;
        if (p.token.kind == EcsTokEq || p.token.kind == EcsTokNeq || p.token.kind == EcsTokMatch) {
            if (term->oper != EcsAnd) {
                flecs_term_error(&p, "cannot mix operator with equality expression");
                return NULL;
            }
            neq = p.token.kind == EcsTokNeq;
            ecs_entity_t pred = p.token.kind == EcsTokMatch ? EcsPredMatch : EcsPredEq;
            term->src = term->first;
            term->first = (ecs_term_ref_t){ .id = pred };
            if (!flecs_term_next(&p)) return NULL;
            if (!flecs_term_identifier(&p) && p.token.kind != EcsTokString) goto unexpected;
            term->second.name = p.token.value;
            if (p.token.kind == EcsTokString) {
                term->second.id = EcsIsName;
                if (pred == EcsPredMatch && term->second.name[0] == '!') {
                    term->second.name ++;
                    term->oper = EcsNot;
                }
            }
            if (!flecs_term_next(&p)) return NULL;
        } else {
            bool trav = p.token.kind == '|';
            if (trav && (!flecs_term_next(&p) || !flecs_term_trav(&p, &term->first))) return NULL;
            if (p.token.kind == '(') {
                if (!flecs_term_next(&p)) return NULL;
                if (!trav && p.token.kind == ')') {
                    term->src.id = EcsIsEntity;
                    if (!flecs_term_next(&p)) return NULL;
                } else if (!flecs_term_args(&p, 0)) {
                    return NULL;
                }
            }
        }
    } else {
        goto unexpected;
    }
    switch (p.token.kind) {
    case '}': p.pos --; break;
    case EcsTokOr:
        if (term->oper != EcsAnd || neq) {
            flecs_term_error(&p, "cannot mix operators in || expression");
            return NULL;
        }
        term->oper = EcsOr;
        break;
    case ',': case EcsTokNewline: case EcsTokEnd: break;
    default: goto unexpected;
    }
    if (neq) term->oper = EcsNot;
    return p.pos;
unexpected:
    flecs_term_error(&p, "unexpected token in term");
    return NULL;
}

int flecs_terms_parse(
    ecs_world_t *world,
    const char *name,
    const char *code,
    char *token_buffer,
    ecs_term_t *terms,
    int32_t *term_count_out)
{
    if (!ecs_os_strcmp(code, "0")) {
        *term_count_out = 0;
        return 0;
    }

    ecs_parser_t parser = {
        .name = name,
        .code = code,
        .world = world,
        .pos = code,
        .merge_variable_members = true
    };

    parser.token_cur = token_buffer;

    int32_t term_count = 0;
    const char *ptr = code;
    ecs_term_ref_t extra_args[FLECS_TERM_ARG_COUNT_MAX];

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
    char *token_buffer,
    ecs_term_t *term)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(expr != NULL, ECS_INVALID_PARAMETER, "%s", name);
    ecs_assert(term != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_parser_t parser = {
        .name = name,
        .code = expr,
        .world = world,
        .token_cur = token_buffer
    };

    parser.term = term;

    return flecs_query_term_parse(&parser, expr);
}

const char* flecs_id_parse(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_id_t *id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(expr != NULL, ECS_INVALID_PARAMETER, "%s", name);
    ecs_assert(id != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_size_t token_buffer_size = ecs_os_strlen(expr) * 2 + 1;
    char *token_buffer = ecs_os_malloc(token_buffer_size);
    const char *ret = NULL;

    ecs_parser_t parser = {
        .name = name,
        .code = expr,
        .world = ECS_CONST_CAST(ecs_world_t*, world),  /* Safe, won't modify */
        .token_cur = token_buffer,
        .token_end = &token_buffer[token_buffer_size]
    };

    ecs_term_t term = {0};
    parser.term = &term;

    expr = flecs_scan_whitespace(&parser, expr);
    if (!ecs_os_strcmp(expr, "#0")) {
        *id = 0;
        ret = &expr[1];
        goto done;
    }

    const char *result = flecs_query_term_parse(&parser, expr);
    if (!result) {
        goto done;
    }

    ecs_query_validator_ctx_t ctx = {0};
    ctx.world = world;
    ctx.term = &term;

    if (flecs_term_finalize(world, &term, &ctx)) {
        goto done;
    }

    if (term.oper != EcsAnd) {
        ecs_parser_error(name, expr, (result - expr),
            "invalid operator for add expression");
        goto done;
    }

    if ((term.src.id & ~EcsTraverseFlags) != (EcsThis|EcsIsVariable)) {
        ecs_parser_error(name, expr, (result - expr),
            "invalid source for add expression (must be $this)");
        goto done;
    }

    *id = term.id;
    ret = result;
done:
    ecs_os_free(token_buffer);
    return ret;
}

static const char* flecs_query_arg_parse(
    ecs_parser_t *parser,
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

static const char* flecs_query_args_parse(
    ecs_parser_t *parser,
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
    ecs_parser_t parser = {
        .name = q_name,
        .code = expr,
        .world = q->real_world,
        .token_cur = token_buffer
    };

    return flecs_query_args_parse(&parser, q, it, expr);
error:
    return NULL;
}

#endif
