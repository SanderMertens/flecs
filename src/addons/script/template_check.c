/**
 * @file addons/script/template_check.c
 * @brief Validate exclusive scope ownership for template declarations.
 *
 * Inside templates an entity, tag, component or pair may only be owned by a
 * single scope, so that when a conditional scope is no longer taken it is
 * unambiguous whether the objects it declared must be deleted/removed.
 * Declarations in mutually exclusive branches of the same if statement are
 * allowed, declarations in scopes that can be active at the same time are not.
 * Variables in pair elements are treated as wildcards, variables in entity
 * names as a sentinel value; overlaps that cannot be statically ruled out are
 * rejected. Within a single scope dynamic declarations may not overlap with
 * other declarations, as aliasing would cause an id change to remove an id
 * owned by the other declaration.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

typedef struct ecs_template_check_path_t {
    ecs_script_node_t *ctrl; /* if or for node */
    int8_t branch;           /* 0 = if_true/loop body, 1 = if_false */
} ecs_template_check_path_t;

typedef struct ecs_template_check_decl_t {
    ecs_script_node_t *node;
    char *epath;     /* sanitized path of entity the declaration applies to */
    char *first;     /* NULL = wildcard */
    char *second;    /* NULL = wildcard (pairs only) */
    bool is_pair;
    bool any;        /* variable component, matches any id */
    bool is_entity;
    bool dynamic;    /* name/id contains variables or expressions */
    int32_t path_offset;
    int32_t path_count;
} ecs_template_check_decl_t;

typedef struct ecs_template_check_ctx_t {
    ecs_parser_t *parser;
    ecs_vec_t decls;      /* vec<ecs_template_check_decl_t> */
    ecs_vec_t path;       /* vec<ecs_template_check_path_t>, current path */
    ecs_vec_t path_store; /* vec<ecs_template_check_path_t>, per decl */
} ecs_template_check_ctx_t;

typedef enum ecs_template_check_relation_t {
    EcsTemplateCheckSame,
    EcsTemplateCheckExclusive,
    EcsTemplateCheckNested,
    EcsTemplateCheckDivergent
} ecs_template_check_relation_t;

static
bool flecs_template_check_ident_char(
    char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || (ch == '_');
}

/* Replace variables and interpolated expressions in names with a sentinel so
 * names can be compared without knowing variable values. */
static
char* flecs_template_check_name_key(
    const char *name,
    bool *dynamic)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    const char *p = name;

    while (p[0]) {
        if (p[0] == '\\') {
            ecs_strbuf_appendch(&buf, p[0]);
            p ++;
            if (p[0]) {
                ecs_strbuf_appendch(&buf, p[0]);
                p ++;
            }
            continue;
        }

        if (p[0] == '$') {
            dynamic[0] = true;
            ecs_strbuf_appendch(&buf, '0');
            p ++;
            while (flecs_template_check_ident_char(p[0])) {
                p ++;
            }
            continue;
        }

        if (p[0] == '{') {
            dynamic[0] = true;
            ecs_strbuf_appendch(&buf, '0');
            int32_t depth = 1;
            p ++;
            while (p[0] && depth) {
                if (p[0] == '{') {
                    depth ++;
                } else if (p[0] == '}') {
                    depth --;
                }
                p ++;
            }
            continue;
        }

        ecs_strbuf_appendch(&buf, p[0]);
        p ++;
    }

    return ecs_strbuf_get(&buf);
}

static
char* flecs_template_check_id_elem(
    const char *str,
    ecs_expr_node_t *expr,
    bool *dynamic)
{
    if (expr) {
        dynamic[0] = true;
        return NULL;
    }

    if (!str) {
        return NULL;
    }

    if (str[0] == '$') {
        if (!ecs_os_strcmp(str, "$this")) {
            /* Resolves to the same entity for the lifetime of an instance */
            return ecs_os_strdup(str);
        }
        dynamic[0] = true;
        return NULL;
    }

    return ecs_os_strdup(str);
}

static
bool flecs_template_check_keys_overlap(
    const ecs_template_check_decl_t *a,
    const ecs_template_check_decl_t *b)
{
    if (a->is_entity != b->is_entity) {
        return false;
    }

    if (ecs_os_strcmp(a->epath, b->epath)) {
        return false;
    }

    if (a->is_entity) {
        return true;
    }

    if (a->any || b->any) {
        return true;
    }

    if (a->is_pair != b->is_pair) {
        return false;
    }

    if (a->first && b->first && ecs_os_strcmp(a->first, b->first)) {
        return false;
    }

    if (a->is_pair && a->second && b->second &&
        ecs_os_strcmp(a->second, b->second))
    {
        return false;
    }

    return true;
}

static
ecs_template_check_relation_t flecs_template_check_relation(
    ecs_template_check_ctx_t *ctx,
    const ecs_template_check_decl_t *a,
    const ecs_template_check_decl_t *b)
{
    ecs_template_check_path_t *store = ecs_vec_first(&ctx->path_store);
    int32_t k, count = a->path_count < b->path_count ?
        a->path_count : b->path_count;

    for (k = 0; k < count; k ++) {
        ecs_template_check_path_t *ea = &store[a->path_offset + k];
        ecs_template_check_path_t *eb = &store[b->path_offset + k];
        if (ea->ctrl == eb->ctrl) {
            if (ea->branch == eb->branch) {
                continue;
            }
            return EcsTemplateCheckExclusive;
        }
        return EcsTemplateCheckDivergent;
    }

    if (a->path_count == b->path_count) {
        return EcsTemplateCheckSame;
    }

    return EcsTemplateCheckNested;
}

static
void flecs_template_check_error(
    ecs_template_check_ctx_t *ctx,
    const ecs_template_check_decl_t *decl)
{
    ecs_parser_t *parser = ctx->parser;
    int64_t offset = 0;
    if (decl->node->pos) {
        offset = decl->node->pos - parser->code;
    }

    if (decl->is_entity) {
        ecs_parser_error(parser->name, parser->code, offset,
            "entity '%s' is already owned by an overlapping scope (move the "
            "declaration to a single scope or to exclusive if/else branches)",
            decl->epath);
    } else if (decl->any) {
        ecs_parser_error(parser->name, parser->code, offset,
            "variable component overlaps with another declaration in an "
            "overlapping scope");
    } else if (decl->is_pair) {
        ecs_parser_error(parser->name, parser->code, offset,
            "pair (%s, %s) is already owned by an overlapping scope (move the "
            "declaration to a single scope or to exclusive if/else branches)",
            decl->first ? decl->first : "*",
            decl->second ? decl->second : "*");
    } else {
        ecs_parser_error(parser->name, parser->code, offset,
            "'%s' is already owned by an overlapping scope (move the "
            "declaration to a single scope or to exclusive if/else branches)",
            decl->first ? decl->first : "*");
    }
}

static
void flecs_template_check_alias_error(
    ecs_template_check_ctx_t *ctx,
    const ecs_template_check_decl_t *decl)
{
    ecs_parser_t *parser = ctx->parser;
    int64_t offset = 0;
    if (decl->node->pos) {
        offset = decl->node->pos - parser->code;
    }

    if (decl->is_entity) {
        ecs_parser_error(parser->name, parser->code, offset,
            "entity '%s' can alias another declaration in the same scope "
            "(dynamic names may not overlap with other declarations)",
            decl->epath);
    } else if (decl->any) {
        ecs_parser_error(parser->name, parser->code, offset,
            "variable component can alias another declaration in the same "
            "scope");
    } else if (decl->is_pair) {
        ecs_parser_error(parser->name, parser->code, offset,
            "pair (%s, %s) can alias another declaration in the same scope "
            "(dynamic ids may not overlap with other declarations)",
            decl->first ? decl->first : "*",
            decl->second ? decl->second : "*");
    } else {
        ecs_parser_error(parser->name, parser->code, offset,
            "'%s' can alias another declaration in the same scope "
            "(dynamic ids may not overlap with other declarations)",
            decl->first ? decl->first : "*");
    }
}

static
int flecs_template_check_insert(
    ecs_template_check_ctx_t *ctx,
    ecs_template_check_decl_t *decl)
{
    int32_t path_count = ecs_vec_count(&ctx->path);
    decl->path_offset = ecs_vec_count(&ctx->path_store);
    decl->path_count = path_count;
    if (path_count) {
        ecs_vec_grow_t(NULL, &ctx->path_store,
            ecs_template_check_path_t, path_count);
        ecs_os_memcpy_n(ecs_vec_get_t(&ctx->path_store,
            ecs_template_check_path_t, decl->path_offset),
            ecs_vec_first(&ctx->path), ecs_template_check_path_t, path_count);
    }

    /* Store first so the context owns the decl strings, including when a
     * conflict is found. */
    ecs_vec_append_t(NULL, &ctx->decls, ecs_template_check_decl_t)[0] = *decl;

    int32_t i, count = ecs_vec_count(&ctx->decls) - 1;
    ecs_template_check_decl_t *decls = ecs_vec_first(&ctx->decls);
    decl = &decls[count];

    for (i = 0; i < count; i ++) {
        ecs_template_check_decl_t *prev = &decls[i];
        if (!flecs_template_check_keys_overlap(prev, decl)) {
            continue;
        }

        ecs_template_check_relation_t rel =
            flecs_template_check_relation(ctx, prev, decl);

        if (rel == EcsTemplateCheckExclusive) {
            continue;
        }

        if (rel == EcsTemplateCheckSame) {
            /* Static declarations in the same conditional region are always
             * active together, so overlap is unambiguous. When either side is
             * dynamic the declarations can alias, which would cause an id
             * change to remove an id owned by the other declaration. */
            if (prev->dynamic || decl->dynamic) {
                flecs_template_check_alias_error(ctx, decl);
                return -1;
            }
            continue;
        }

        if (rel == EcsTemplateCheckNested) {
            flecs_template_check_error(ctx, decl);
            return -1;
        }

        /* Divergent scopes (two different if statements or loops) can both be
         * active, which makes ownership ambiguous. */
        flecs_template_check_error(ctx, decl);
        return -1;
    }

    return 0;
}

static
int flecs_template_check_id(
    ecs_template_check_ctx_t *ctx,
    ecs_script_node_t *node,
    ecs_script_id_t *id,
    const char *epath)
{
    ecs_template_check_decl_t decl = {
        .node = node,
        .epath = ecs_os_strdup(epath),
        .is_pair = id->second != NULL || id->second_expr != NULL
    };

    decl.first = flecs_template_check_id_elem(
        id->first, id->first_expr, &decl.dynamic);
    decl.second = flecs_template_check_id_elem(
        id->second, id->second_expr, &decl.dynamic);

    return flecs_template_check_insert(ctx, &decl);
}

static
int flecs_template_check_scope(
    ecs_template_check_ctx_t *ctx,
    ecs_script_scope_t *scope,
    const char *epath)
{
    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);

    for (i = 0; i < count; i ++) {
        ecs_script_node_t *stmt = stmts[i];
        switch(stmt->kind) {
        case EcsAstEntity: {
            ecs_script_entity_t *node = (ecs_script_entity_t*)stmt;
            char *key;
            bool dynamic = false;

            if (!node->name) {
                /* Anonymous entities are always unique */
                key = flecs_asprintf("#%p", (void*)node);
            } else {
                key = flecs_template_check_name_key(node->name, &dynamic);
            }

            char *child_path = epath[0] ?
                flecs_asprintf("%s.%s", epath, key) : key;
            if (child_path != key) {
                ecs_os_free(key);
            }

            if (node->name) {
                ecs_template_check_decl_t decl = {
                    .node = stmt,
                    .epath = ecs_os_strdup(child_path),
                    .is_entity = true,
                    .dynamic = dynamic
                };

                if (flecs_template_check_insert(ctx, &decl)) {
                    ecs_os_free(child_path);
                    return -1;
                }
            }

            int result = flecs_template_check_scope(
                ctx, node->scope, child_path);
            ecs_os_free(child_path);
            if (result) {
                return -1;
            }
            break;
        }
        case EcsAstTag: {
            ecs_script_tag_t *node = (ecs_script_tag_t*)stmt;
            if (flecs_template_check_id(ctx, stmt, &node->id, epath)) {
                return -1;
            }
            break;
        }
        case EcsAstComponent: {
            ecs_script_component_t *node = (ecs_script_component_t*)stmt;
            if (flecs_template_check_id(ctx, stmt, &node->id, epath)) {
                return -1;
            }
            break;
        }
        case EcsAstDefaultComponent: {
            ecs_template_check_decl_t decl = {
                .node = stmt,
                .epath = ecs_os_strdup(epath),
                .first = ecs_os_strdup("#default")
            };

            if (flecs_template_check_insert(ctx, &decl)) {
                return -1;
            }
            break;
        }
        case EcsAstVarComponent: {
            ecs_template_check_decl_t decl = {
                .node = stmt,
                .epath = ecs_os_strdup(epath),
                .any = true,
                .dynamic = true
            };

            if (flecs_template_check_insert(ctx, &decl)) {
                return -1;
            }
            break;
        }
        case EcsAstIf: {
            ecs_script_if_t *node = (ecs_script_if_t*)stmt;
            ecs_template_check_path_t *elem = ecs_vec_append_t(
                NULL, &ctx->path, ecs_template_check_path_t);
            elem->ctrl = stmt;
            elem->branch = 0;

            if (flecs_template_check_scope(ctx, node->if_true, epath)) {
                return -1;
            }

            ecs_vec_last_t(&ctx->path, ecs_template_check_path_t)->branch = 1;

            if (flecs_template_check_scope(ctx, node->if_false, epath)) {
                return -1;
            }

            ecs_vec_remove_last(&ctx->path);
            break;
        }
        case EcsAstFor: {
            ecs_script_for_range_t *node = (ecs_script_for_range_t*)stmt;
            ecs_template_check_path_t *elem = ecs_vec_append_t(
                NULL, &ctx->path, ecs_template_check_path_t);
            elem->ctrl = stmt;
            elem->branch = 0;

            if (flecs_template_check_scope(ctx, node->scope, epath)) {
                return -1;
            }

            ecs_vec_remove_last(&ctx->path);
            break;
        }
        case EcsAstWith: {
            ecs_script_with_t *node = (ecs_script_with_t*)stmt;
            if (flecs_template_check_scope(ctx, node->scope, epath)) {
                return -1;
            }
            break;
        }
        case EcsAstPairScope: {
            ecs_script_pair_scope_t *node = (ecs_script_pair_scope_t*)stmt;
            if (flecs_template_check_scope(ctx, node->scope, epath)) {
                return -1;
            }
            break;
        }
        case EcsAstScope:
        case EcsAstWithVar:
        case EcsAstWithTag:
        case EcsAstWithComponent:
        case EcsAstUsing:
        case EcsAstModule:
        case EcsAstAnnotation:
        case EcsAstTemplate: /* Nested templates are validated separately */
        case EcsAstProp:
        case EcsAstConst:
        case EcsAstExportConst:
        case EcsAstInclude:
        case EcsAstFunction:
            break;
        }
    }

    return 0;
}

int flecs_script_template_check(
    ecs_parser_t *parser,
    ecs_script_template_node_t *node)
{
    ecs_template_check_ctx_t ctx = {
        .parser = parser
    };

    ecs_vec_init_t(NULL, &ctx.decls, ecs_template_check_decl_t, 0);
    ecs_vec_init_t(NULL, &ctx.path, ecs_template_check_path_t, 0);
    ecs_vec_init_t(NULL, &ctx.path_store, ecs_template_check_path_t, 0);

    int result = flecs_template_check_scope(&ctx, node->scope, "");

    int32_t i, count = ecs_vec_count(&ctx.decls);
    ecs_template_check_decl_t *decls = ecs_vec_first(&ctx.decls);
    for (i = 0; i < count; i ++) {
        ecs_os_free(decls[i].epath);
        ecs_os_free(decls[i].first);
        ecs_os_free(decls[i].second);
    }

    ecs_vec_fini_t(NULL, &ctx.decls, ecs_template_check_decl_t);
    ecs_vec_fini_t(NULL, &ctx.path, ecs_template_check_path_t);
    ecs_vec_fini_t(NULL, &ctx.path_store, ecs_template_check_path_t);

    return result;
}

#endif
